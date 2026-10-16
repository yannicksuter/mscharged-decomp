#include <dwc/dwc_base64.h>
#include <dwc/dwc_common.h>
#include <dwc/dwc_nonport.h>
#include <dwc/dwc_error.h>
#include <dwc/dwc_friend.h>
#include <dwc/dwc_login.h>
#include <dwc/dwc_main.h>
#include <dwc/R4QK01/dwc_match_internal.h>
#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_report.h>
#include <gamespy/GP/gp.h>
#include <gamespy/gt2/gt2.h>
#include <gamespy/qr2/qr2.h>
#include <gamespy/serverbrowsing/sb_serverbrowsing.h>
#include <revolution/os/OSTime.h>
#include <revolution/so.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    DWC_MATCH_DNS_ERROR_RETRY_MAX = 1,
    DWC_NUM_QR2_MATCH_KEY = 9,
    DWC_SB_UPDATE_REQUEST_TIMEOUT = 30000,
    DWC_MATCH_COMMAND_ADD_MESSAGE_MAX = 512
};

static DWCGameMatchKeyData stGameMatchKeys[DWC_QR2_GAME_RESERVED_KEYS];
static void DWCi_QR2ServerKeyCallback(int, qr2_buffer_t, void*);
static void DWCi_QR2PlayerKeyCallback(int, int, qr2_buffer_t, void*);
static void DWCi_QR2TeamKeyCallback(int, int, qr2_buffer_t, void*);
static void DWCi_QR2KeyListCallback(qr2_key_type, qr2_keybuffer_t, void*);
static int DWCi_QR2CountCallback(qr2_key_type, void*);
static void DWCi_QR2AddErrorCallback(qr2_error_t, gsi_char*, void*);
static void DWCi_QR2PublicAddrCallback(unsigned int, unsigned short, void*);
void DWCi_QR2NatnegCallback(int, void*);
void DWCi_QR2ClientMsgCallback(gsi_char*, int, void*);
static qr2_error_t DWCi_HandleQR2Error(qr2_error_t);
BOOL fn_80456F10(void);

static DWCMatchControl* stpMatchCnt;
static char* stpAddFilter;
int dwcMeshRetryLimit = 5;
int dwcServerSearchRatio = -2;
extern int lbl_80606524;
extern BOOL lbl_80606528;
extern int lbl_8060652C;
extern BOOL lbl_80606530;

static DWCMatchControl* DWCi_GetMatchCnt(void)
{
    return stpMatchCnt;
}
static OSTime stStateChangedTick;

void DWCi_SetMatchCommonParam(u8 matchType, u8 numEntry,
    DWCMatchedSCCallback callback, void* param);
BOOL fn_80454BB0(void);
GPResult DWCi_SetGPStatus(int status, const char* statusString, const char* locationString);
static GPResult DWCi_HandleGPError(GPResult result);
qr2_error_t DWCi_QR2Startup(int profileID);
void DWCi_SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void* instance);
SBError DWCi_SBUpdateAsync(int profileID);
static SBError DWCi_HandleSBError(SBError result);
int DWCi_SendResvCommand(int profileID, BOOL delay);
int DWCi_GetFriendListIndex(int profileID);


static inline void DWCi_EnterMatchState(DWCMatchState state)
{
    DWCi_SetMatchStatus(state);
}
static BOOL DWCi_IsMatchLocked(void);
GPResult DWCi_GPSetServerStatus(int status);
static void DWCi_SetHostState(u8 state);
static void DWCi_SetGroupID(u32 groupID);
void DWCi_StopMatching(DWCError error, int errorCode);
static int DWCi_GetDefaultMatchFilter(char* filter, int profileID, u8 numEntry, u8 matchType, u8 hostState);
static void DWCi_PrependNode(const DWCNodeInfo* node);

static u32 DWCi_GenerateGroupID(void)
{
    return 100000000 + DWCi_GetMathRand32(10000000);
}

typedef enum
{
    DWC_MATCH_RESET_ALL = 0,
    DWC_MATCH_RESET_RESTART,
    DWC_MATCH_RESET_CONTINUE,
    DWC_MATCH_RESET_NUM
} DWCMatchResetLevel;

void DWCi_ResetMatchParam(DWCMatchResetLevel level);

void DWCi_FreeNegotiateList(void)
{
    if (lbl_8060652C == 0)
        NNFreeNegotiateList();
    else
        lbl_80606530 = TRUE;
}

void DWCi_ClearNodeList(void)
{
    memset(&DWCi_GetMatchCnt()->nodeList, 0, sizeof(DWCi_GetMatchCnt()->nodeList));
}

static void DWCi_ClearGameMatchKeys(void)
{
    int i;
    for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++)
        if (stGameMatchKeys[i].keyStr != NULL)
            DWC_Free(DWC_ALLOCTYPE_BASE, stGameMatchKeys[i].keyStr, 0);
    memset(stGameMatchKeys, 0, sizeof(stGameMatchKeys));
}

static u8 DWCi_GetNewMatchKey(void)
{
    u8 i;

    for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++)
    {
        if (!stGameMatchKeys[i].keyID)
        {
            return (u8)(DWC_QR2_GAME_KEY_START + i);
        }
    }

    return 0;
}

u8 DWC_AddMatchKeyInt(u8 keyID, const char* keyString, const int* valueSrc)
{
    int index;

    if (!keyString || !valueSrc)
    {
        return 0;
    }

    if (keyID >= DWC_QR2_GAME_KEY_START
        && stGameMatchKeys[keyID - DWC_QR2_GAME_KEY_START].keyID)
    {
        if (stGameMatchKeys[keyID - DWC_QR2_GAME_KEY_START].keyStr != NULL
            && strcmp(stGameMatchKeys[keyID - DWC_QR2_GAME_KEY_START].keyStr,
                   keyString)
                != 0)
        {
            return 0;
        }
    }
    else
    {
        keyID = DWCi_GetNewMatchKey();
        if (!keyID)
        {
            return 0;
        }
    }

    index = keyID - DWC_QR2_GAME_KEY_START;
    stGameMatchKeys[index].keyID = keyID;
    stGameMatchKeys[index].isStr = 0;
    stGameMatchKeys[index].pad = 0;

    if (stGameMatchKeys[index].keyStr)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, stGameMatchKeys[index].keyStr, 0);
    }

    stGameMatchKeys[index].keyStr
        = DWC_Alloc(DWC_ALLOCTYPE_BASE, strlen(keyString) + 1);
    if (!stGameMatchKeys[index].keyStr)
    {
        DWCi_StopMatching(DWC_ERROR_FATAL,
            DWC_ECODE_SEQ_MATCH + DWC_ECODE_TYPE_ALLOC);
        return 0;
    }

    strcpy(stGameMatchKeys[index].keyStr, keyString);

    DWC_Printf(DWC_REPORTFLAG_DEBUG,
        "DWC_AddMatchKeyInt: key='%s', value=%d\n", keyString, *valueSrc);

    stGameMatchKeys[index].value = valueSrc;

    qr2_register_keyA(keyID, stGameMatchKeys[index].keyStr);

    return keyID;
}

int DWC_GetMatchIntValue(int index, const char* keyString, int idefault)
{
    SBServer server;

    if (stpMatchCnt == NULL || DWCi_IsError())
    {
        return idefault;
    }
    server = ServerBrowserGetServer(stpMatchCnt->sbObj, index);
    if (server != NULL)
    {
        return SBServerGetIntValueA(server, keyString, idefault);
    }
    return idefault;
}

void DWCi_MatchInit(DWCMatchControl* matchcnt, GPConnection* pGpObj,
    GT2Socket* pGt2Socket, GT2ConnectionCallbacks* gt2Callbacks,
    const char* gameName, const char* secretKey,
    const DWCFriendData friendList[], int friendListLen)
{
    stpMatchCnt = matchcnt;
    stStateChangedTick = OSGetTime();
    DWCi_GetMatchCnt()->pGpObj = pGpObj;
    DWCi_GetMatchCnt()->pGt2Socket = pGt2Socket;
    DWCi_GetMatchCnt()->gt2Callbacks = gt2Callbacks;
    DWCi_GetMatchCnt()->qr2Obj = NULL;
    DWCi_GetMatchCnt()->qr2IP = 0;
    DWCi_GetMatchCnt()->qr2Port = 0;
    DWCi_GetMatchCnt()->sbObj = NULL;
    DWCi_EnterMatchState(DWC_MATCH_STATE_INIT);
    DWCi_GetMatchCnt()->_019 = 0;
    DWCi_GetMatchCnt()->profileID = 0;
    DWCi_GetMatchCnt()->gameName = gameName;
    DWCi_GetMatchCnt()->secretKey = secretKey;
    DWCi_GetMatchCnt()->friendList = friendList;
    DWCi_GetMatchCnt()->friendListLen = friendListLen;
    memset(DWCi_GetMatchCnt()->friendIdxList, 0, sizeof(DWCi_GetMatchCnt()->friendIdxList));
    DWCi_GetMatchCnt()->friendIdxListLen = 0;
    DWCi_GetMatchCnt()->matchedCallback = NULL;
    DWCi_GetMatchCnt()->matchedParam = NULL;
    DWCi_GetMatchCnt()->evalCallback = NULL;
    DWCi_GetMatchCnt()->evalParam = NULL;
    DWCi_GetMatchCnt()->stopSCCallback = NULL;
    DWCi_GetMatchCnt()->serverDownBaseBitmap = 0;
    DWCi_GetMatchCnt()->groupID = 0;
    DWCi_GetMatchCnt()->_8C4 = 0;
    DWCi_GetMatchCnt()->_8C8 = 0;
    DWCi_GetMatchCnt()->qr2MatchType = 0;
    DWCi_GetMatchCnt()->hostState = 0;
    DWCi_GetMatchCnt()->sbUpdateRequestTick = 0;
    DWCi_GetMatchCnt()->_8F8 = 0;
    DWCi_GetMatchCnt()->_908 = 1;
    DWCi_GetMatchCnt()->closeAllRequested = 0;
    DWCi_GetMatchCnt()->_8E6 = 0;
    DWC_Printf(4, "ServerBrowserLimitUpdate timeout reset.(%s)\n", "DWCi_MatchInit");
    DWCi_GetMatchCnt()->topology = 0;
    DWCi_ClearNodeList();
    memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
    DWCi_ClearGameMatchKeys();
    DWCi_ResetMatchParam(DWC_MATCH_RESET_ALL);
}

void DWCi_ConnectToAnybodyAsync(int topology, u8 numEntry, const char* addFilter,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCEvalPlayerCallback evalCallback, void* evalParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam)
{
    char filter[256];
    unsigned long addFilterLenMax;
    if (stpAddFilter)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }
    if (addFilter)
    {
        addFilterLenMax = 256 - DWCi_GetDefaultMatchFilter(filter, -1, 32, 3, 3) - strlen(" and ()");
        stpAddFilter = DWC_Alloc(DWC_ALLOCTYPE_BASE, addFilterLenMax);
        if (!stpAddFilter)
        {
            DWCi_StopMatching(DWC_ERROR_FATAL, -80001);
            return;
        }
        DWCi_Np_CPUCopyFast(stpAddFilter, addFilter, addFilterLenMax);
        stpAddFilter[addFilterLenMax - 1] = '\0';
    }
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_ANYBODY, numEntry, matchedCallback, matchedParam);
    DWCi_GetMatchCnt()->newClientCallback = newClientCallback;
    DWCi_GetMatchCnt()->newClientParam = newClientParam;
    DWCi_GetMatchCnt()->evalCallback = evalCallback;
    DWCi_GetMatchCnt()->evalParam = evalParam;
    DWCi_GetMatchCnt()->topology = topology;
    DWCi_GetMatchCnt()->reservationCallback = reservationCallback;
    if (reservationData)
        memcpy(DWCi_GetMatchCnt()->reservationData, reservationData, sizeof(DWCi_GetMatchCnt()->reservationData));
    else
        memset(DWCi_GetMatchCnt()->reservationData, 0, sizeof(DWCi_GetMatchCnt()->reservationData));
    DWCi_GetMatchCnt()->reservationParam = reservationParam;
    DWCi_SetMatchStatus(DWC_MATCH_STATE_SEARCH_OWN);
    if (!DWCi_GetMatchCnt()->sbObj)
        DWCi_GetMatchCnt()->sbObj = ServerBrowserNewA(DWCi_GetMatchCnt()->gameName, DWCi_GetMatchCnt()->gameName,
            DWCi_GetMatchCnt()->secretKey, 0, 20, QVERSION_QR2, SBFalse, DWCi_SBCallback, NULL);
    if (!DWCi_GetMatchCnt()->sbObj)
        if (DWCi_HandleSBError(sbe_allocerror))
            return;
    DWCi_GPSetServerStatus(DWC_STATUS_MATCH_ANYBODY);
    if (!DWCi_GetMatchCnt()->qr2Obj)
        DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID);
    else
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    if (DWCi_HandleSBError(DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->profileID)))
        return;
}

void DWCi_SetupGameServer(int topology, u8 maxEntry,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam)
{
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_SC_SV, maxEntry, matchedCallback, matchedParam);
    DWCi_GetMatchCnt()->newClientCallback = newClientCallback;
    DWCi_GetMatchCnt()->newClientParam = newClientParam;
    DWCi_GetMatchCnt()->topology = topology;
    DWCi_GetMatchCnt()->reservationCallback = reservationCallback;
    if (reservationData)
        memcpy(DWCi_GetMatchCnt()->reservationData, reservationData, sizeof(DWCi_GetMatchCnt()->reservationData));
    else
        memset(DWCi_GetMatchCnt()->reservationData, 0, sizeof(DWCi_GetMatchCnt()->reservationData));
    DWCi_GetMatchCnt()->reservationParam = reservationParam;
    DWCi_EnterMatchState(DWC_MATCH_STATE_SEARCH_OWN);
    if (DWCi_HandleGPError(DWCi_GPSetServerStatus(DWC_STATUS_MATCH_SC_SV)))
        return;
    DWCi_SetHostState(2);
    DWCi_GetMatchCnt()->_8F4 = 1;
    if (!DWCi_GetMatchCnt()->qr2Obj)
        DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID);
    DWCi_SetGroupID(DWCi_GetMatchCnt()->profileID + DWCi_GenerateGroupID());
    if (!DWCi_GetMatchCnt()->sbObj)
        DWCi_GetMatchCnt()->sbObj = ServerBrowserNewA(DWCi_GetMatchCnt()->gameName, DWCi_GetMatchCnt()->gameName,
            DWCi_GetMatchCnt()->secretKey, 0, 20, QVERSION_QR2, SBFalse, DWCi_SBCallback, NULL);
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    if (DWCi_HandleSBError(DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->profileID)))
        return;
}

void DWCi_ConnectToGameServerAsync(int topology, int serverPid,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam)
{
    DWCNodeInfo savedServer;
    DWCNodeInfo savedSelf;
    u8 savedResvRetryCount;
    int result;
    if (DWCi_GetMatchCnt()->_8C8)
    {
        savedServer = DWCi_GetMatchCnt()->serverNode;
        savedSelf = *DWCi_GetNodeByProfileID(DWCi_GetMatchCnt()->profileID);
        savedResvRetryCount = DWCi_GetMatchCnt()->scResvRetryCount;
    }
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_SC_CL, 0, matchedCallback, matchedParam);
    DWCi_GetMatchCnt()->topology = topology;
    if (!DWCi_GetMatchCnt()->_8C8)
    {
        DWCNodeInfo node;
        memset(&node, 0, sizeof(node));
        memcpy(&DWCi_GetMatchCnt()->serverNode, &node, sizeof(node));
        DWCi_GetMatchCnt()->serverNode.aid = 0xFF;
        DWCi_GetMatchCnt()->serverNode.profileID = serverPid;
    }
    else
    {
        DWCi_GetMatchCnt()->serverNode = savedServer;
        DWCi_PrependNode(&savedSelf);
        DWCi_Np_CPUCopyFast(DWCi_GetMatchCnt()->reservationData, savedSelf.reservationData, sizeof(savedSelf.reservationData));
        DWCi_GetMatchCnt()->scResvRetryCount = savedResvRetryCount;
    }
    DWCi_GetMatchCnt()->newClientCallback = newClientCallback;
    DWCi_GetMatchCnt()->newClientParam = newClientParam;
    DWCi_GetMatchCnt()->reservationCallback = reservationCallback;
    if (reservationData)
        memcpy(DWCi_GetMatchCnt()->reservationData, reservationData, sizeof(DWCi_GetMatchCnt()->reservationData));
    else
        memset(DWCi_GetMatchCnt()->reservationData, 0, sizeof(DWCi_GetMatchCnt()->reservationData));
    DWCi_GetMatchCnt()->reservationParam = reservationParam;
    DWCi_GetMatchCnt()->qr2IsReserved = 1;
    DWCi_GetMatchCnt()->qr2Reservation = DWCi_GetMatchCnt()->profileID;
    DWCi_SetHostState(1);
    if (!DWCi_GetMatchCnt()->_8C8)
        DWCi_SetMatchStatus(DWC_MATCH_STATE_SEARCH_OWN);
    if (!DWCi_GetMatchCnt()->sbObj)
        DWCi_GetMatchCnt()->sbObj = ServerBrowserNewA(DWCi_GetMatchCnt()->gameName, DWCi_GetMatchCnt()->gameName,
            DWCi_GetMatchCnt()->secretKey, 0, 20, QVERSION_QR2, SBFalse, DWCi_SBCallback, NULL);
    if (!DWCi_GetMatchCnt()->sbObj)
        if (DWCi_HandleSBError(sbe_allocerror))
            return;
    if (DWCi_HandleGPError(DWCi_GPSetServerStatus(DWC_STATUS_MATCH_SC_CL)))
        return;
    if (!DWCi_GetMatchCnt()->qr2Obj)
    {
        if (DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID))
            return;
    }
    else
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    if (!DWCi_GetMatchCnt()->_8C8)
    {
        if (DWCi_HandleSBError(DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->profileID)))
            return;
    }
    else
    {
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
        result = DWCi_SendResvCommand(serverPid, FALSE);
        if (DWCi_GetMatchCnt()->qr2MatchType == 0)
            result = DWCi_HandleSBError(result);
        else
            result = DWCi_HandleGPError(result);
        if (result)
            return;
    }
}

void DWCi_SetMatchStatus(DWCMatchState state)
{
    static const char* names[] = {
        "DWC_MATCH_STATE_INIT",
        "DWC_MATCH_STATE_CL_WAITING",
        "DWC_MATCH_STATE_CL_SEARCH_HOST",
        "DWC_MATCH_STATE_CL_WAIT_RESV",
        "DWC_MATCH_STATE_CL_SEARCH_EVAL_HOST",
        "DWC_MATCH_STATE_CL_NN",
        "DWC_MATCH_STATE_CL_GT2",
        "DWC_MATCH_STATE_CL_CANCEL_SYN",
        "DWC_MATCH_STATE_CL_SYN",
        "DWC_MATCH_STATE_CL_SVDOWN_1",
        "DWC_MATCH_STATE_CL_SVDOWN_2",
        "DWC_MATCH_STATE_CL_SVDOWN_3",
        "DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST",
        "DWC_MATCH_STATE_SV_WAITING",
        "DWC_MATCH_STATE_SV_OWN_NN",
        "DWC_MATCH_STATE_SV_OWN_GT2",
        "DWC_MATCH_STATE_SV_WAIT_CL_LINK",
        "DWC_MATCH_STATE_SV_CANCEL_SYN",
        "DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT",
        "DWC_MATCH_STATE_SV_SYN",
        "DWC_MATCH_STATE_SV_SYN_WAIT",
        "DWC_MATCH_STATE_WAIT_CLOSE",
        "DWC_MATCH_STATE_SEARCH_OWN",
        "DWC_MATCH_STATE_SV_SYN_CLOSE_WAIT",
    };

    int oldState = DWCi_GetMatchCnt()->state;
    if (oldState != state)
    {
        OSTime lastTick = stStateChangedTick;
        int milliseconds;
        int seconds;
        stStateChangedTick = OSGetTime();
        milliseconds = OSTicksToMilliseconds(stStateChangedTick - lastTick);
        seconds = milliseconds / 1000;
        // Preserve the retail diagnostic's seconds subtraction and rounding.
        milliseconds = (milliseconds - seconds + 50) / 100;
        DWC_Printf(1, "status changed %s(%d.%d sec) -> %s\n", names[oldState],
            seconds, milliseconds, names[state]);
    }
    DWCi_GetMatchCnt()->state = state;
}

static BOOL DWCi_IsMatchLocked(void)
{
    if (!DWCi_GetMatchCnt()->_908)
        return FALSE;
    if (fn_80454BB0()
        && (DWCi_GetMatchCnt()->qr2MatchType == 1 || DWCi_GetMatchCnt()->qr2MatchType == 3 || DWCi_GetMatchCnt()->qr2MatchType == 2)
        && !DWCi_GetMatchCnt()->_8F4)
        return TRUE;
    return FALSE;
}

GPResult DWCi_GPSetServerStatus(int status)
{
    char value[12];
    char string[38];
    int count;
    snprintf(value, sizeof(value), "%u", DWCi_GetMatchCnt()->qr2NumEntry + 1);
    DWC_SetCommonKeyValueString("SCM", value, string, '/');
    count = DWCi_GetMatchCnt()->nodeList.count;
    if (DWCi_GetMatchCnt()->qr2MatchType == 2 && !count)
        count = 1;
    snprintf(value, sizeof(value), "%u", count);
    DWC_AddCommonKeyValueString("SCN", value, string, '/');
    snprintf(value, sizeof(value), "%u", DWC_MATCHING_VERSION);
    DWC_AddCommonKeyValueString("VER", value, string, '/');
    snprintf(value, sizeof(value), "%u", DWCi_IsMatchLocked() ? TRUE : FALSE);
    DWC_AddCommonKeyValueString("LCK", value, string, '/');
    return DWCi_SetGPStatus(status, string, NULL);
}

static void DWCi_SetHostState(u8 state)
{
    DWCi_GetMatchCnt()->hostState = state;
    if (DWCi_GetMatchCnt()->qr2Obj)
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
}

static void DWCi_SetGroupID(u32 groupID)
{
    DWC_Printf(1, "Create New Group ID:%u\n", groupID);
    DWCi_GetMatchCnt()->groupID = groupID;
}

void DWCi_StopMatching(DWCError error, int errorCode)
{
    if (!stpMatchCnt || !error)
        return;
    stpMatchCnt->closeState = 1;
    gt2CloseAllConnectionsHard(*stpMatchCnt->pGt2Socket);
    stpMatchCnt->closeState = 0;
    DWCi_SetError(error, errorCode);
    DWCi_GPSetServerStatus(DWC_STATUS_ONLINE);
    {
        DWCMatchControl* control = stpMatchCnt;
        BOOL self = control->cbEventPid ? FALSE : TRUE;
        int index = DWCi_GetFriendListIndex(control->cbEventPid);
        control->matchedCallback(error, FALSE, self,
            control->matchType == 2 ? TRUE : FALSE, index, control->matchedParam);
    }
    DWCi_CloseMatching();
}

static int DWCi_GetDefaultMatchFilter(char* filter, int profileID, u8 numEntry, u8 matchType, u8 hostState)
{
    char hostStateFilter[256];
    if (hostState == 3)
        snprintf(hostStateFilter, sizeof(hostStateFilter), "(%s = %u or %s = %u)", "dwc_hoststate", 0, "dwc_hoststate", 2);
    else
        snprintf(hostStateFilter, sizeof(hostStateFilter), "%s = %u", DWC_QR2_HOST_STATE_KEY_STR, hostState);
    snprintf(filter, 256,
        "%s = %d and %s != %u and maxplayers = %d and numplayers < %d and %s = %d and %s = %u and %s",
        "dwc_mver", DWC_MATCHING_VERSION, "dwc_pid", profileID, numEntry, numEntry, "dwc_mtype", matchType,
        "dwc_suspend", 0, hostStateFilter);
    return strlen(filter);
}

void DWCi_ClearQR2Key(void)
{
    if (DWC_GetState() == DWC_STATE_ONLINE)
    {
        DWCi_GetMatchCnt()->qr2NumEntry = 0;
        DWCi_GetMatchCnt()->groupID = 0;
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_ClearQR2Key ignored.\n");
    }
}

DWCNodeInfo* DWCi_GetNodeByProfileID(int profileID)
{
    int i;
    for (i = 0; i < DWCi_GetMatchCnt()->nodeList.count; i++)
        if (DWCi_GetMatchCnt()->nodeList.nodes[i].profileID == profileID)
            return &DWCi_GetMatchCnt()->nodeList.nodes[i];
    return NULL;
}

static void DWCi_PrependNode(const DWCNodeInfo* node)
{
    int i;
    if (DWCi_GetMatchCnt()->nodeList.count == 32)
        return;
    for (i = DWCi_GetMatchCnt()->nodeList.count; i > 0; i--)
        DWCi_GetMatchCnt()->nodeList.nodes[i] = DWCi_GetMatchCnt()->nodeList.nodes[i - 1];
    DWCi_GetMatchCnt()->nodeList.nodes[0] = *node;
    DWCi_GetMatchCnt()->nodeList.count++;
}

static const int stEvalRate[DWC_SB_UPDATE_MAX_SERVERS] = { 3, 3, 2, 2, 1, 1 };
void SBServerAddIntKeyValue(SBServer server, const char* keyname, int value);

int DWCi_EvaluateServers(int sort)
{
    int eval;
    int deleteFlag = FALSE;
    int localDelete;
    int profileID;
    int i;
    SBServer server;

    for (i = 0; i < ServerBrowserCount(DWCi_GetMatchCnt()->sbObj); i++)
    {
        server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, i);

        if (DWCi_GetMatchCnt()->qr2MatchType == 0)
        {
            profileID = SBServerGetIntValueA(server, "dwc_pid", 0);
            localDelete = FALSE;

            if (DWCi_GetNodeByProfileID(profileID) != NULL)
            {
                ServerBrowserRemoveServer(DWCi_GetMatchCnt()->sbObj, server);
                localDelete = TRUE;
                i--;
            }
            if (localDelete)
            {
                continue;
            }
        }

        if (DWCi_GetMatchCnt()->evalCallback != NULL)
        {
            eval = DWCi_GetMatchCnt()->evalCallback(
                i, DWCi_GetMatchCnt()->evalParam);

            if (eval > 0)
            {
                if (eval > 0x800000 - 1)
                {
                    eval = 0x800000 - 1;
                }
                SBServerAddIntKeyValue(server, "dwc_eval",
                    (eval << 8) | DWCi_GetMathRand32(0x100));
            }
            else
            {
                ServerBrowserRemoveServer(DWCi_GetMatchCnt()->sbObj, server);
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                    "Deleted server [%d] (eval point is %d).\n", i, eval);
                deleteFlag = TRUE;
                i--;
            }
        }
        else
        {
            SBServerAddIntKeyValue(server, "dwc_eval", DWCi_GetMathRand32(0x80));
        }
    }

    if (sort != 0 && ServerBrowserCount(DWCi_GetMatchCnt()->sbObj) != 0)
    {
        ServerBrowserSortA(DWCi_GetMatchCnt()->sbObj, SBFalse, "dwc_eval",
            sbcm_int);
    }

    if (deleteFlag && ServerBrowserCount(DWCi_GetMatchCnt()->sbObj) == 0)
    {
        return 0;
    }
    return 1;
}

void DWCi_RandomizeServers(void)
{
    u32 rand;
    int maxEval = 0;
    int total = 0;
    int i;
    int cumulative[6];

    if (ServerBrowserCount(stpMatchCnt->sbObj) <= 1)
    {
        return;
    }

    for (i = 0; i < ServerBrowserCount(stpMatchCnt->sbObj); i++)
    {
        int value = SBServerGetIntValueA(
            ServerBrowserGetServer(stpMatchCnt->sbObj, i), "dwc_eval", -1);

        if (value > maxEval)
        {
            maxEval = value;
        }
        total += stEvalRate[i];
    }

    rand = DWCi_GetMathRand32(0x64);
    for (i = 0; i < ServerBrowserCount(stpMatchCnt->sbObj); i++)
    {
        if (i == ServerBrowserCount(stpMatchCnt->sbObj) - 1)
        {
            cumulative[i] = 100;
            break;
        }
        cumulative[i] = stEvalRate[i] * 100 / total
            + (i > 0 ? cumulative[i - 1] : 0);
        if (rand < cumulative[i])
        {
            break;
        }
    }

    DWC_Printf(0x40, "Server[%d] is selected (%d/100: rand %d)\n", i,
        cumulative[i], rand);
    if (maxEval < 0x7FFFFFFF)
    {
        maxEval++;
    }
    SBServerAddIntKeyValue(ServerBrowserGetServer(stpMatchCnt->sbObj, i),
        "dwc_eval", maxEval);
    ServerBrowserSortA(stpMatchCnt->sbObj, SBFalse, "dwc_eval", sbcm_int);
}

static GPResult DWCi_HandleGPError(GPResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GP_NO_ERROR)
    {
        return GP_NO_ERROR;
    }

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, GP error %d\n", result);

    switch (result)
    {
    case GP_MEMORY_ERROR:
        dwcError = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GP_PARAMETER_ERROR:
        dwcError = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    case GP_NETWORK_ERROR:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GP_SERVER_ERROR:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    errorCode += DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_GP;

    DWCi_StopMatching(dwcError, errorCode);

    return result;
}

static SBError DWCi_HandleSBError(SBError error)
{
    int errorCode;
    DWCError dwcError;

    if (error == sbe_noerror)
    {
        return sbe_noerror;
    }

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, SB error %d\n", error);

    switch (error)
    {
    case sbe_socketerror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case sbe_dnserror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case sbe_connecterror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    case sbe_dataerror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DATA;
        break;
    case sbe_allocerror:
        dwcError = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case sbe_paramerror:
        dwcError = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    }

    errorCode += DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_SB;

    DWCi_StopMatching(dwcError, errorCode);

    return error;
}

BOOL DWCi_IsBuddyMatchLocked(const GPBuddyStatus* status)
{
    BOOL locked = FALSE;
    char value[4] = { 0 };
    if (DWC_GetCommonValueString("LCK", value, status->statusString, '/') > 0)
    {
        locked = (u8)strtoul(value, NULL, 10);
    }
    return locked;
}

qr2_error_t DWCi_QR2Startup(int profileID)
{
    int i;
    qr2_error_t qr2Error = e_qrnoerror;

    if (DWCi_GetMatchCnt()->qr2Obj)
    {
        DWC_Printf(
            DWC_REPORTFLAG_MATCH_NN, "QR2 is already set up.\n");
        return e_qrnoerror;
    }

    for (i = 0; i < DWC_MATCH_DNS_ERROR_RETRY_MAX; i++)
    {
        qr2Error = qr2_init_socketA(&DWCi_GetMatchCnt()->qr2Obj,
            gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
            gt2GetLocalPort(*DWCi_GetMatchCnt()->pGt2Socket),
            DWCi_GetMatchCnt()->gameName, DWCi_GetMatchCnt()->secretKey, 1, 1,
            DWCi_QR2ServerKeyCallback, DWCi_QR2PlayerKeyCallback,
            DWCi_QR2TeamKeyCallback, DWCi_QR2KeyListCallback,
            DWCi_QR2CountCallback, DWCi_QR2AddErrorCallback, NULL);
        if (!qr2Error)
        {
            break;
        }

        qr2_shutdown(DWCi_GetMatchCnt()->qr2Obj);
        DWCi_GetMatchCnt()->qr2Obj = NULL;

        if ((qr2Error != e_qrdnserror)
            || (i == DWC_MATCH_DNS_ERROR_RETRY_MAX - 1))
        {
            DWCi_HandleQR2Error(qr2Error);
            return qr2Error;
        }
    }

    DWCi_GetMatchCnt()->qr2IP = 0;
    DWCi_GetMatchCnt()->qr2Port = 0;

    qr2_register_publicaddress_callback(
        DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2PublicAddrCallback);
    qr2_register_natneg_callback(
        DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2NatnegCallback);
    qr2_register_clientmessage_callback(
        DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2ClientMsgCallback);
    qr2_register_key(DWC_QR2_PID_KEY, DWC_QR2_PID_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_TYPE_KEY, DWC_QR2_MATCH_TYPE_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_VER_KEY, DWC_QR2_MATCH_VER_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_EVAL_KEY, DWC_QR2_MATCH_EVAL_KEY_STR);
    qr2_register_key(DWC_QR2_GROUP_ID_KEY, DWC_QR2_GROUP_ID_KEY_STR);
    qr2_register_key(DWC_QR2_HOST_STATE_KEY, DWC_QR2_HOST_STATE_KEY_STR);
    qr2_register_key(DWC_QR2_SUSPEND_KEY, DWC_QR2_SUSPEND_KEY_STR);
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

    return qr2Error;
}

static qr2_error_t DWCi_HandleQR2Error(qr2_error_t error)
{
    int errorCode;
    DWCError dwcError;

    if (error == e_qrnoerror)
    {
        return e_qrnoerror;
    }

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, QR2 error %d\n", error);

    switch (error)
    {
    case e_qrwsockerror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case e_qrbinderror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_BIND;
        break;
    case e_qrdnserror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case e_qrconnerror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_PEER;
        break;
    case e_qrnochallengeerror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    switch (DWC_GetState())
    {
    case DWC_STATE_LOGIN:
        errorCode += DWC_ECODE_SEQ_LOGIN + DWC_ECODE_GS_QR2;
        DWCi_StopLogin(dwcError, errorCode);
        break;
    case DWC_STATE_MATCHING:
        errorCode += DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_QR2;
        DWCi_StopMatching(dwcError, errorCode);
        break;
    default:
        errorCode += DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_QR2;
        DWCi_SetError(dwcError, errorCode);
        break;
    }

    return error;
}

static inline int DWCi_SelectSearchHostState(void)
{
    int ratio = dwcServerSearchRatio;
    if (ratio == -2)
    {
        int hostState = 3;
        if (DWCi_GetMatchCnt()->searchServerNext == 1)
            hostState = 2;
        DWCi_GetMatchCnt()->searchServerNext = 0;
        return hostState;
    }
    if (ratio == -1)
    {
        int hostState = 0;
        if (DWCi_GetMatchCnt()->searchServerNext)
            hostState = 2;
        DWCi_GetMatchCnt()->searchServerNext ^= 1;
        return hostState;
    }
    else
    {
        int choice = DWCi_GetMathRand32(100);
        int hostState = 0;
        if (choice < ratio)
            hostState = 2;
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
            "ServerSearchRatio=%d %% (next search->%d)\n", ratio, hostState);
        return hostState;
    }
}

SBError DWCi_SBUpdateAsync(int profileID)
{
    char filter[MAX_FILTER_LEN];
    u8 basicFields[DWC_QR2_RESERVED_KEYS + DWC_QR2_GAME_RESERVED_KEYS];
    int len;
    int numBasicFields;
    int i;
    SBBool async = SBTrue;
    SBError sbError = sbe_noerror;

    numBasicFields = DWC_NUM_QR2_MATCH_KEY;
    basicFields[0] = NUMPLAYERS_KEY;
    basicFields[1] = MAXPLAYERS_KEY;
    basicFields[2] = DWC_QR2_PID_KEY;
    basicFields[3] = DWC_QR2_MATCH_TYPE_KEY;
    basicFields[4] = DWC_QR2_MATCH_VER_KEY;
    basicFields[5] = DWC_QR2_MATCH_EVAL_KEY;
    basicFields[6] = DWC_QR2_GROUP_ID_KEY;
    basicFields[7] = DWC_QR2_HOST_STATE_KEY;
    basicFields[8] = DWC_QR2_SUSPEND_KEY;

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY
        || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
    {
        for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++)
        {
            if (stGameMatchKeys[i].keyID)
            {
                basicFields[numBasicFields] = stGameMatchKeys[i].keyID;
                numBasicFields++;
            }
        }
    }
    switch (DWCi_GetMatchCnt()->state)
    {
    case DWC_MATCH_STATE_CL_SEARCH_HOST:
        len = DWCi_GetDefaultMatchFilter(filter,
            DWCi_GetMatchCnt()->profileID, DWCi_GetMatchCnt()->qr2NumEntry,
            DWCi_GetMatchCnt()->qr2MatchType, DWCi_SelectSearchHostState());
        if (stpAddFilter)
        {
            snprintf(filter + len, sizeof(filter) - len, " and (%s)", stpAddFilter);
            filter[sizeof(filter) - 1] = '\0';
        }
        DWCi_GetMatchCnt()->_8DC++;
        break;
    case DWC_MATCH_STATE_SEARCH_OWN:
    case DWC_MATCH_STATE_CL_WAIT_RESV:
    case DWC_MATCH_STATE_CL_SEARCH_EVAL_HOST:
        snprintf(filter, sizeof(filter), "%s = %u", DWC_QR2_PID_KEY_STR, profileID);
        DWCi_GetMatchCnt()->reqProfileID = profileID;
        break;
    case DWC_MATCH_STATE_CL_SVDOWN_1:
    case DWC_MATCH_STATE_CL_SVDOWN_2:
    case DWC_MATCH_STATE_CL_SVDOWN_3:
        snprintf(filter, sizeof(filter), "%s = %u", DWC_QR2_PID_KEY_STR, profileID);
        async = SBFalse;
        break;
    case DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST:
        snprintf(filter, sizeof(filter), "(%s = %u) and (%s = %u) and (%s != %u)",
            DWC_QR2_GROUP_ID_KEY_STR, DWCi_GetMatchCnt()->groupID,
            DWC_QR2_HOST_STATE_KEY_STR, 2, DWC_QR2_PID_KEY_STR, DWCi_GetMatchCnt()->profileID);
        break;
    default:
        DWC_Printf(DWC_REPORTFLAG_ERROR,
            "---DWCi_SBUpdateAsync() illegal state %d.\n", DWCi_GetMatchCnt()->state);
        break;
    }
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "ServerBrowserFilter : %s\n", filter);
    ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
    for (i = 0; i < DWC_MATCH_DNS_ERROR_RETRY_MAX; i++)
    {
        sbError = ServerBrowserLimitUpdate(DWCi_GetMatchCnt()->sbObj, async,
            SBFalse, basicFields, numBasicFields, filter, DWC_SB_UPDATE_MAX_SERVERS);
        if (!sbError || sbError != sbe_dnserror)
            break;
        DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "Retry\n");
    }
    if (sbError == sbe_noerror)
    {
        OSTime timeout = OSGetTime()
            + OSMillisecondsToTicks((u64)DWC_SB_UPDATE_REQUEST_TIMEOUT);
        DWCi_GetMatchCnt()->sbUpdateRequestTick = timeout;
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
            "ServerBrowserLimitUpdate timeout set.(%s)\n", "DWCi_SBUpdateAsync");
    }
    return sbError;
}


void fn_80470900(u8, u8, const void*, int, BOOL);

static BOOL DWCi_CanSendGT2MatchCommand(int profileID)
{
    if (!DWCi_GetGT2ConnectionByProfileID(profileID, TRUE))
        return FALSE;
    return DWCi_GetNodeByProfileID(profileID) ? TRUE : FALSE;
}

static inline void DWCi_SendGT2MsgCommand(u8 command, int profileID, DWCNodeInfo* node,
    const u32 data[], int len)
{
    DWCSBMessage senddata;
    if (data && len)
        DWCi_Np_CPUCopyFast(senddata.data, data, sizeof(u32) * len);
    else
        len = 0;
    strcpy((char*)&senddata.header, DWC_SB_COMMAND_STRING);
    senddata.header.version = DWCi_HtoLEl(DWC_MATCHING_VERSION);
    senddata.header.command = command;
    senddata.header.size = (u8)(sizeof(u32) * len);
    senddata.header.qr2Port = DWCi_HtoLEs(DWCi_GetMatchCnt()->qr2Port);
    senddata.header.qr2IP = DWCi_GetMatchCnt()->qr2IP;
    senddata.header.profileID = DWCi_HtoLEl(DWCi_GetMatchCnt()->profileID);
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
        "<GT2> SEND-0x%02x -> [--------:-----] [pid=%u]\n", command, profileID);
    fn_80470900(DWC_GetMyAID(), node->aid, &senddata,
        sizeof(DWCSBMessageHeader) + senddata.header.size, FALSE);
}

static inline SBError DWCi_SendSBMsgCommand(
    u8 command, u32 ip, u16 port, const u32 data[], int len)
{
    int i;
    DWCSBMessage senddata;
    SBError sbError;
    if (data && len)
        DWCi_Np_CPUCopyFast(senddata.data, data, sizeof(u32) * len);
    else
        len = 0;
    strcpy((char*)&senddata.header, DWC_SB_COMMAND_STRING);
    senddata.header.version = DWCi_HtoLEl(DWC_MATCHING_VERSION);
    senddata.header.command = command;
    senddata.header.size = (u8)(sizeof(u32) * len);
    senddata.header.qr2Port = DWCi_HtoLEs(DWCi_GetMatchCnt()->qr2Port);
    senddata.header.qr2IP = DWCi_GetMatchCnt()->qr2IP;
    senddata.header.profileID = DWCi_HtoLEl(DWCi_GetMatchCnt()->profileID);
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
        "<SB> SEND-0x%02x -> [%08x:%d] [pid=--------]\n", command, ip, port);
    for (i = 0; i < DWC_MATCH_DNS_ERROR_RETRY_MAX; i++)
    {
        sbError = ServerBrowserSendMessageToServerA(DWCi_GetMatchCnt()->sbObj,
            gt2AddressToString(ip, 0, NULL), port, (gsi_char*)&senddata,
            (int)(sizeof(DWCSBMessageHeader) + senddata.header.size));
        if (!sbError)
            break;
        else if (sbError != sbe_dnserror)
            break;
    }
    return sbError;
}

static GPResult DWCi_SendGPBuddyMsgCommand(GPConnection* connection,
    u8 command, int profileID, const char* message)
{
    char tmpStr[DWC_MATCH_COMMAND_ADD_MESSAGE_MAX];
    u32 len, len2;
    GPResult result;
    len = (u32)snprintf(tmpStr, sizeof(tmpStr), "%s%dv%s",
        DWC_GP_COMMAND_STRING, DWC_MATCHING_VERSION, DWC_GP_COMMAND_MATCH_STRING);
    tmpStr[len] = (char)command;
    tmpStr[len + 1] = '\0';
    if (message)
    {
        len2 = strlen(message);
        DWCi_Np_CPUCopyFast(&tmpStr[len + 1], message, len2);
        tmpStr[len + 1 + len2] = '\0';
    }
    result = gpSendBuddyMessage(connection, profileID, tmpStr);
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
        "<GP> SEND-0x%02x -> [--------:-----] [pid=%u]\n", command, profileID);
    return result;
}

int DWCi_SendMatchCommand(u8 command, int profileID, u32 ip, u16 port,
    const u32 data[], int len)
{
    char message[DWC_MATCH_COMMAND_ADD_MESSAGE_MAX];
    int msgLen;
    int result;
    if (DWCi_CanSendGT2MatchCommand(profileID))
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "SEND GT2 MSG TO PID:%u\n", profileID);
        DWCi_SendGT2MsgCommand(command, profileID, DWCi_GetNodeByProfileID(profileID), data, len);
        result = 0;
    }
    else if (gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID))
    {
        msgLen = 0;
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "SEND BUDDY MSG TO PID:%u\n", profileID);
        if (data && len)
            msgLen = DWC_Base64Encode((const char*)data, sizeof(u32) * len,
                message, sizeof(message));
        message[msgLen] = '\0';
        result = DWCi_SendGPBuddyMsgCommand(DWCi_GetMatchCnt()->pGpObj,
            command, profileID, message);
    }
    else
    {
        if (!ip)
        {
            DWCNodeInfo* node = DWCi_GetNodeByProfileID(profileID);
            ip = node->publicIP;
            port = node->publicPort;
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "FROM BM but peer is not buddy.\n");
        }
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "SEND SB MSG TO IP/PORT:(%08x:%d)\n", ip, port);
        result = DWCi_SendSBMsgCommand(command, ip, port, data, len);
    }
    if (command == DWC_MATCH_COMMAND_RESV_OK
        || command == DWC_MATCH_COMMAND_TELL_ADDR
        || command == DWC_MATCH_COMMAND_LINK_CLS_REQ
        || command == DWC_MATCH_COMMAND_LINK_CLS_SUC)
    {
        DWCi_GetMatchCnt()->cmdCnt.command = command;
        DWCi_GetMatchCnt()->cmdCnt.port = port;
        DWCi_GetMatchCnt()->cmdCnt.ip = ip;
        DWCi_GetMatchCnt()->cmdCnt.profileID = profileID;
        DWCi_GetMatchCnt()->cmdCnt.len = len;
        {
            OSTime sendTime = OSGetTime();
            DWCi_GetMatchCnt()->cmdCnt.sendTime = sendTime;
        }
        if (data && len)
            DWCi_Np_CPUCopyFast(DWCi_GetMatchCnt()->cmdCnt.data, data, sizeof(u32) * len);
    }
    return result;
}

static int DWCi_GetHostState(void)
{
    return DWCi_GetMatchCnt()->hostState;
}

static const char* DWCi_QR2KeyName(int keyid)
{
    switch (keyid)
    {
    case NUMPLAYERS_KEY: return "NUMPLAYERS";
    case MAXPLAYERS_KEY: return "MAXPLAYERS";
    case DWC_QR2_PID_KEY: return "PID";
    case DWC_QR2_MATCH_TYPE_KEY: return "TYPE";
    case DWC_QR2_MATCH_VER_KEY: return "VER";
    case DWC_QR2_MATCH_EVAL_KEY: return "EVAL";
    case DWC_QR2_GROUP_ID_KEY: return "GROUPID";
    case DWC_QR2_HOST_STATE_KEY: return "HOST";
    case DWC_QR2_SUSPEND_KEY: return "SUSPEND";
    default: return "Unknown";
    }
}

static void DWCi_QR2ServerKeyCallback(int keyid, qr2_buffer_t outbuf, void* userdata)
{
    int index;

    switch (keyid)
    {
    case NUMPLAYERS_KEY:
    {
        int count = DWCi_GetMatchCnt()->nodeList.count;
        if (--count < 0)
            count = 0;
        qr2_buffer_add_int(outbuf, count);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), count);
        break;
    }
    case MAXPLAYERS_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2NumEntry);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), DWCi_GetMatchCnt()->qr2NumEntry);
        break;
    case DWC_QR2_PID_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->profileID);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), DWCi_GetMatchCnt()->profileID);
        break;
    case DWC_QR2_MATCH_TYPE_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2MatchType);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), DWCi_GetMatchCnt()->qr2MatchType);
        break;
    case DWC_QR2_MATCH_VER_KEY:
        qr2_buffer_add_int(outbuf, DWC_MATCHING_VERSION);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), DWC_MATCHING_VERSION);
        break;
    case DWC_QR2_MATCH_EVAL_KEY:
        qr2_buffer_add_int(outbuf, 1);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), 1);
        break;
    case DWC_QR2_GROUP_ID_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetHostState() == 2 ? DWCi_GetMatchCnt()->groupID : 0);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), DWCi_GetHostState() == 2 ? DWCi_GetMatchCnt()->groupID : 0);
        break;
    case DWC_QR2_HOST_STATE_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->hostState);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), DWCi_GetMatchCnt()->hostState);
        break;
    case DWC_QR2_SUSPEND_KEY:
    {
        BOOL suspended = fn_80456F10();
        qr2_buffer_add_int(outbuf, suspended);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
            "QR2, Received ServerKeyReq : keyID %d(%s) - %d\n",
            keyid, DWCi_QR2KeyName(keyid), suspended);
        break;
    }
    default:
        index = keyid - DWC_QR2_GAME_KEY_START;
        if ((index >= 0 && index < DWC_QR2_GAME_RESERVED_KEYS)
            && stGameMatchKeys[index].keyID)
        {
            if (stGameMatchKeys[index].isStr)
            {
                qr2_buffer_addA(
                    outbuf, (char*)stGameMatchKeys[index].value);
            }
            else
            {
                qr2_buffer_add_int(
                    outbuf, *(const int*)stGameMatchKeys[index].value);
            }
        }
        break;
    }
}

static void DWCi_QR2PlayerKeyCallback(
    int keyid, int index, qr2_buffer_t outbuf, void* userdata)
{
}

static void DWCi_QR2TeamKeyCallback(
    int keyid, int index, qr2_buffer_t outbuf, void* userdata)
{
}

static void DWCi_QR2KeyListCallback(
    qr2_key_type keytype, qr2_keybuffer_t keybuffer, void* userdata)
{
    int i;

    switch (keytype)
    {
    case key_server:
        qr2_keybuffer_add(keybuffer, NUMPLAYERS_KEY);
        qr2_keybuffer_add(keybuffer, MAXPLAYERS_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_PID_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_TYPE_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_VER_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_EVAL_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_GROUP_ID_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_HOST_STATE_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_SUSPEND_KEY);

        for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++)
        {
            if (stGameMatchKeys[i].keyID)
            {
                qr2_keybuffer_add(keybuffer, stGameMatchKeys[i].keyID);
            }
        }
        break;
    case key_player:
        break;
    case key_team:
        break;
    }

    DWC_Printf(DWC_REPORTFLAG_QR2_REQ,
        "QR2, Received KeyListReq : keytype %d\n", keytype);
}

static int DWCi_QR2CountCallback(qr2_key_type keytype, void* userdata)
{
    return 0;
}

static void DWCi_QR2AddErrorCallback(
    qr2_error_t error, gsi_char* errmsg, void* userdata)
{
    DWC_Printf(2, "QR2 Failed query addition to master server %d\n", error);
    DWC_Printf(2, "%s\n", errmsg);
    DWCi_HandleQR2Error(error);
}

static void DWCi_QR2PublicAddrCallback(
    unsigned int ip, unsigned short port, void* userdata)
{
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Got my query IP %s & port %d.\n", inet_ntoa(*(struct in_addr*)&ip), port);
    stpMatchCnt->qr2IP = ip;
    stpMatchCnt->qr2Port = port;
}


void DWCi_SBPrintServerData(SBServer server)
{
    int i;

    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "SBServerGetPrivateAddress     = %s\n",
        SBServerGetPrivateAddress(server));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "SBServerGetPrivateInetAddress = %x\n",
        SBServerGetPrivateInetAddress(server));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "SBServerGetPrivateQueryPort   = %d\n",
        SBServerGetPrivateQueryPort(server));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "SBServerGetPublicAddress      = %s\n",
        SBServerGetPublicAddress(server));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "SBServerGetPublicInetAddres   = %x\n",
        SBServerGetPublicInetAddress(server));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "SBServerGetPublicQueryPort    = %d\n",
        SBServerGetPublicQueryPort(server));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "SBServerHasPrivateAddress     = %d\n",
        SBServerHasPrivateAddress(server));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "numplayers  = %d\n",
        SBServerGetIntValueA(server, "numplayers", -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "maxplayers  = %d\n",
        SBServerGetIntValueA(server, "maxplayers", -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s     = %u\n", "dwc_pid",
        SBServerGetIntValueA(server, "dwc_pid", 0));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n", "dwc_mtype",
        SBServerGetIntValueA(server, "dwc_mtype", -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n", "dwc_mver",
        SBServerGetIntValueA(server, "dwc_mver", -1));

    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n", DWC_QR2_GROUP_ID_KEY_STR,
        SBServerGetIntValueA(server, DWC_QR2_GROUP_ID_KEY_STR, -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n", DWC_QR2_HOST_STATE_KEY_STR,
        SBServerGetIntValueA(server, DWC_QR2_HOST_STATE_KEY_STR, -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n", DWC_QR2_SUSPEND_KEY_STR,
        SBServerGetIntValueA(server, DWC_QR2_SUSPEND_KEY_STR, -1));

    for (i = 0; i < 154; i++)
    {
        if (stGameMatchKeys[i].keyID != 0)
        {
            if (stGameMatchKeys[i].isStr != 0)
            {
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s  = %s\n", stGameMatchKeys[i].keyStr,
                    SBServerGetStringValueA(server,
                        stGameMatchKeys[i].keyStr, "NONE"));
            }
            else
            {
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s  = %d\n", stGameMatchKeys[i].keyStr,
                    SBServerGetIntValueA(server, stGameMatchKeys[i].keyStr,
                        -1));
            }
        }
    }
}

void DWCi_ResetMatchParam(DWCMatchResetLevel level)
{
    DWCi_GetMatchCnt()->gt2ConnectCount = 0;
    DWCi_GetMatchCnt()->nnRecvCount = 0;
    DWCi_GetMatchCnt()->nnCookieRand = (u16)DWCi_GetMathRand32(0x10000);
    DWCi_GetMatchCnt()->nnLastCookie = 0;
    DWCi_GetMatchCnt()->nnFailedTime = 0;
    DWCi_GetMatchCnt()->nnFinishTime = 0;
    DWCi_GetMatchCnt()->clLinkProgress = 0;
    DWCi_GetMatchCnt()->cancelState = 0;
    DWCi_GetMatchCnt()->scResvRetryCount = 0;
    DWCi_GetMatchCnt()->synResendCount = 0;
    DWCi_GetMatchCnt()->cancelSynResendCount = 0;
    DWCi_GetMatchCnt()->resvWaitCount = 0;
    DWCi_GetMatchCnt()->closeState = 0;
    DWCi_GetMatchCnt()->cancelBaseLatency = 0;
    DWCi_GetMatchCnt()->lastSynSent = 0;
    DWCi_GetMatchCnt()->closedTime = 0;
    memset(&DWCi_GetMatchCnt()->cmdCnt, 0, sizeof(DWCi_GetMatchCnt()->cmdCnt));
    DWCi_GetMatchCnt()->_6C0 = 0;
    DWCi_GetMatchCnt()->_6C4 = 0;
    memset(DWCi_GetMatchCnt()->_6C8, 0, sizeof(DWCi_GetMatchCnt()->_6C8));

    if (level == DWC_MATCH_RESET_CONTINUE)
    {
        if (DWCi_GetHostState() == 2)
            DWCi_EnterMatchState(DWC_MATCH_STATE_SV_WAITING);
        else if (DWCi_GetHostState() == 1)
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
        else
            DWC_Printf(8, "DWCi_ResetMatchParam: Host is Free!\n");
    }
    else
    {
        DWCi_GetMatchCnt()->qr2IsReserved = 0;
        DWCi_GetMatchCnt()->qr2Reservation = 0;
        DWCi_GetMatchCnt()->sbUpdateFlag = 0;
        DWCi_GetMatchCnt()->baseLatency = 0;
        DWCi_GetMatchCnt()->friendCount = 0;
        DWCi_GetMatchCnt()->cmdResendFlag = 0;
        DWCi_GetMatchCnt()->cmdResendTick = 0;
        DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
        DWCi_GetMatchCnt()->cmdTimeoutStartTick = 0;
        DWCi_GetMatchCnt()->synAckBit = 0;
        DWCi_GetMatchCnt()->reqProfileID = 0;
        memset(&DWCi_GetMatchCnt()->nnInfo, 0, sizeof(DWCNNInfo));
        DWCi_GetMatchCnt()->stopSCCallback = NULL;
        DWCi_GetMatchCnt()->serverDownBaseBitmap = 0;
        DWCi_GetMatchCnt()->_8C4 = 0;
        DWCi_GetMatchCnt()->searchServerNext = 1;
        DWCi_GetMatchCnt()->_8DC = 0;
        DWCi_GetMatchCnt()->hostState = 0;
        DWCi_GetMatchCnt()->matchingComplete = 0;
        DWCi_GetMatchCnt()->reservationCallback = NULL;
        DWCi_GetMatchCnt()->reservationParam = NULL;
        DWCi_ClearNodeList();
        memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
        DWCi_GetMatchCnt()->_690 = 0;
        if (level == DWC_MATCH_RESET_RESTART)
        {
            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY
                || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SEARCH_OWN);
        }
        else
        {
            DWCi_GetMatchCnt()->matchType = 0;
            DWCi_GetMatchCnt()->qr2NumEntry = 0;
            DWCi_GetMatchCnt()->qr2ShutdownFlag = 0;
            DWCi_GetMatchCnt()->cbEventPid = 0;
            DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
            DWCi_GetMatchCnt()->clWaitTime = 0;
            DWCi_GetMatchCnt()->newClientCallback = NULL;
            DWCi_GetMatchCnt()->newClientParam = NULL;
            memset(DWCi_GetMatchCnt()->reservationData, 0, sizeof(DWCi_GetMatchCnt()->reservationData));
        }
    }
}

void DWCi_SetMatchCommonParam(u8 matchType, u8 numEntry,
    DWCMatchedSCCallback callback, void* param)
{
    DWCi_ResetMatchParam(DWC_MATCH_RESET_ALL);
    DWCi_GetMatchCnt()->matchType = matchType;
    DWCi_GetMatchCnt()->qr2NumEntry = numEntry;
    DWCi_GetMatchCnt()->matchedCallback = callback;
    DWCi_GetMatchCnt()->matchedParam = param;
    DWCi_GetMatchCnt()->_721 = 0;
    DWCi_GetMatchCnt()->_02C = 0xFF;
    DWCi_GetMatchCnt()->_030 = 0;
    DWCi_GetMatchCnt()->hostState = 0;
    DWCi_GetMatchCnt()->searchServerNext = 1;
    DWCi_GetMatchCnt()->_8DC = 0;
    DWCi_GetMatchCnt()->qr2NumEntry = numEntry;
    DWCi_GetMatchCnt()->_01C = 0;
    DWCi_GetMatchCnt()->_028 = 0;
    DWCi_GetMatchCnt()->_020 = 0;
    DWCi_GetMatchCnt()->_04C = 0;
    DWCi_GetMatchCnt()->_8F4 = 1;
    DWCi_GetMatchCnt()->_900 = 0;
    DWCi_GetMatchCnt()->_75C = 0;
}

int DWCi_GetNodeCount(void)
{
    return DWCi_GetMatchCnt()->nodeList.count;
}

DWCNodeInfo* DWCi_GetNodeByIndex(int index)
{
    if (index < DWCi_GetMatchCnt()->nodeList.count)
        return &DWCi_GetMatchCnt()->nodeList.nodes[index];
    return NULL;
}

DWCNodeInfo* DWCi_GetNodeByAID(u8 aid)
{
    int i;
    for (i = 0; i < DWCi_GetMatchCnt()->nodeList.count; i++)
        if (DWCi_GetMatchCnt()->nodeList.nodes[i].aid == aid)
            return &DWCi_GetMatchCnt()->nodeList.nodes[i];
    return NULL;
}

void DWCi_CloseMatching(void)
{
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, " Close Match....\n");
    if (stpMatchCnt == NULL)
        return;
    if (stpMatchCnt->sbObj != NULL)
    {
        if (lbl_80606524 == 0)
        {
            ServerBrowserFree(stpMatchCnt->sbObj);
            stpMatchCnt->sbObj = NULL;
        }
        else
            lbl_80606528 = TRUE;
    }
    DWCi_FreeNegotiateList();
    DWCi_EnterMatchState(DWC_MATCH_STATE_INIT);
    if (stpAddFilter != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }
    DWCi_ClearGameMatchKeys();
    stpMatchCnt->qr2ShutdownFlag = TRUE;
    DWCi_ClearNodeList();
    memset(&stpMatchCnt->serverNode, 0, sizeof(stpMatchCnt->serverNode));
    stpMatchCnt->_8F4 = 1;
    DWCi_GPSetServerStatus(DWC_STATUS_ONLINE);
}

BOOL DWCi_IsShutdownMatch(void)
{
    return stpMatchCnt == NULL;
}

DWCNodeInfo* DWCi_GetFirstNode(void)
{
    if (DWCi_GetMatchCnt()->nodeList.count >= 1)
        return &DWCi_GetMatchCnt()->nodeList.nodes[0];
    return NULL;
}

BOOL fn_8046F180(void)
{
    return DWCi_GetMatchCnt()->topology != 1;
}

BOOL fn_8046F8A0(void)
{
    return DWCi_GetMatchCnt()->topology == 2;
}

BOOL DWCi_IsBackupMine(void)
{
    DWCNodeInfo* node = DWCi_GetNodeByIndex(0);
    if (node == NULL)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO,
            "*** DWCi_IsBackupMine. ignored(state=%d)\n",
            stpMatchCnt ? stpMatchCnt->state : 0);
        return FALSE;
    }
    return node->profileID == DWCi_GetMatchCnt()->profileID;
}

BOOL DWCi_RemoveNodeByAID(u8 aid)
{
    int i;
    if (DWCi_GetMatchCnt()->nodeList.count == 0)
        return FALSE;
    for (i = 0; i < DWCi_GetMatchCnt()->nodeList.count; i++)
        if (DWCi_GetMatchCnt()->nodeList.nodes[i].aid == aid)
            break;
    if (i == DWCi_GetMatchCnt()->nodeList.count)
        return FALSE;
    for (; i < DWCi_GetMatchCnt()->nodeList.count - 1; i++)
        DWCi_GetMatchCnt()->nodeList.nodes[i] = DWCi_GetMatchCnt()->nodeList.nodes[i + 1];
    DWCi_GetMatchCnt()->nodeList.count--;
    return TRUE;
}
