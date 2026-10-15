#include <dwc/dwc_main.h>
#include <dwc/R4QK01/dwc_match_internal.h>
#include <dwc/dwc_common.h>
#include <dwc/dwc_error.h>
#include <dwc/dwc_friend.h>
#include <dwc/dwc_friend_internal.h>
#include <dwc/dwc_report.h>
#include <dwc/dwc_memfunc.h>
#include <gamespy/GP/gp.h>
#include <gamespy/common/gsPlatformSocket.h>
#include <gamespy/gt2/gt2.h>
#include <gamespy/gstats/gstats.h>
#include <gamespy/common/gsAvailable.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DWCstControl
{
    GT2Socket gt2Socket;
    GT2ConnectionCallbacks gt2Callbacks;
    int gt2SendBufSize;
    int gt2RecvBufSize;
    GPConnection gpObj;
    DWCUserData* userdata;
    DWCState state;
    DWCState lastState;
    u8 _02C;
    u8 ownCloseFlag;
    u8 _02E[2];
    u16 playerName[DWC_MAX_PLAYER_NAME];
    int profileID;
    const char* gameName;
    const char* secretKey;
    DWCLoginCallback loginCallback;
    void* loginParam;
    DWCUpdateServersCallback updateServersCallback;
    void* updateServersParam;
    DWCMatchedSCCallback matchedCallback;
    void* matchedParam;
    DWCConnectionClosedCallback closedCallback;
    void* closedParam;
    DWCLoginControl logcnt;
    DWCFriendControl friendcnt;
    DWCMatchControl matchcnt;
    DWCTransportInfo transinfo;
} DWCControl;

typedef struct DWCConnectionInfo
{
    u8 index;
    u8 aid;
    u16 reserve;
    int profileID;
} DWCConnectionInfo;

static GT2Connection stGt2ConnectionList[32];
static DWCConnectionInfo stConnectionInfoList[32];
static u8 stAidList[32];
static DWCControl* stpDwcCnt;
static int stLastSocketError;

void DWCi_MatchedCallback(DWCError, BOOL, BOOL, BOOL, int, void*);

static void DWCi_SetState(DWCState state);
static void DWCi_ClearGT2ConnectionList(void);

static GPResult DWCi_HandleGPError(GPResult result);
static void DWCi_GPErrorCallback(GPConnection*, GPErrorArg*, void*);
static void DWCi_GPRecvBuddyMessageCallback(GPConnection*, GPRecvBuddyMessageArg*, void*);
void DWCi_GPRecvBuddyRequestCallback(GPConnection*, GPRecvBuddyRequestArg*, void*);
BOOL DWCi_GPRecvBuddyAuthCallback(GPConnection*, GPRecvBuddyMessageArg*, void*);
void DWCi_GPRecvBuddyStatusCallback(GPConnection*, GPRecvBuddyStatusArg*, void*);
void DWCi_MatchProcess(BOOL fullSpec);
void DWCi_TransportProcess(void);

void DWCi_MatchInit(DWCMatchControl*, GPConnection*, GT2Socket*, GT2ConnectionCallbacks*,
    const char*, const char*, const DWCFriendData*, int);
void DWCi_ShutdownMatch(void);
void DWCi_ShutdownTransport(void);
void DWCi_InitGPProcessCount(void);
void DWCi_GT2ConnectedCallback(GT2Connection, GT2Result, GT2Byte*, int);
void DWCi_RecvCallback(GT2Connection, GT2Byte*, int, GT2Bool);
void DWCi_PingCallback(GT2Connection, int);
void fn_80455E60(GT2Connection, GT2CloseReason, BOOL, u8);
void fn_80451FF0(void);
void fn_80452000(void);
void fn_804711D0(void* destination, const void* source, u32 size);
static void DWCi_GT2ReceivedCallback(GT2Connection, GT2Byte*, int, GT2Bool);
static void DWCi_GT2ClosedCallback(GT2Connection, GT2CloseReason);
static void DWCi_GT2PingCallback(GT2Connection, int);
static void DWCi_LoginCallback(DWCError, int, void*);

void DWC_InitFriendsMatch(DWCFriendsMatchControl* dwccnt,
    DWCUserData* userdata, int productID, const char* gameName,
    const char* secretKey, int sendBufSize, int recvBufSize,
    DWCFriendData friendList[], int friendListLen)
{
    u32 cpySize;

    DWC_Printf(4, "!!DWC_InitFriendsMatch() was called!!\n");

    stpDwcCnt = DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCControl));
    memset(stpDwcCnt, 0, sizeof(DWCControl));

    DWC_ClearError();

    stpDwcCnt->gt2Socket = NULL;
    stpDwcCnt->gt2Callbacks.connected = DWCi_GT2ConnectedCallback;
    stpDwcCnt->gt2Callbacks.received = DWCi_GT2ReceivedCallback;
    stpDwcCnt->gt2Callbacks.closed = DWCi_GT2ClosedCallback;
    stpDwcCnt->gt2Callbacks.ping = DWCi_GT2PingCallback;
    stpDwcCnt->gt2SendBufSize = sendBufSize ? sendBufSize : 8 * 1024;
    stpDwcCnt->gt2RecvBufSize = recvBufSize ? recvBufSize : 8 * 1024;

    stpDwcCnt->gpObj = NULL;
    stpDwcCnt->userdata = userdata;

    stpDwcCnt->state = DWC_STATE_INIT;
    stpDwcCnt->lastState = DWC_STATE_INIT;
    stpDwcCnt->ownCloseFlag = FALSE;
    stpDwcCnt->profileID = 0;
    stpDwcCnt->gameName = gcd_gamename;
    stpDwcCnt->secretKey = gcd_secret_key;

    stpDwcCnt->loginCallback = NULL;
    stpDwcCnt->loginParam = NULL;
    stpDwcCnt->updateServersCallback = NULL;
    stpDwcCnt->updateServersParam = NULL;
    stpDwcCnt->matchedCallback = NULL;
    stpDwcCnt->matchedParam = NULL;
    stpDwcCnt->closedCallback = NULL;
    stpDwcCnt->closedParam = NULL;

    DWCi_ClearGT2ConnectionList();

    DWCi_LoginInit(&stpDwcCnt->logcnt, userdata, &stpDwcCnt->gpObj,
        productID, userdata->gamecode, stpDwcCnt->playerName,
        DWCi_LoginCallback, NULL);

    DWCi_FriendInit(&stpDwcCnt->friendcnt, &stpDwcCnt->gpObj,
        stpDwcCnt->playerName, friendList, friendListLen);

    DWCi_MatchInit(&stpDwcCnt->matchcnt, &stpDwcCnt->gpObj,
        &stpDwcCnt->gt2Socket, &stpDwcCnt->gt2Callbacks,
        gcd_gamename, gcd_secret_key, friendList, friendListLen);

    DWCi_InitTransport(&stpDwcCnt->transinfo);

    cpySize = strlen(secretKey) < sizeof(gcd_secret_key)
        ? strlen(secretKey)
        : sizeof(gcd_secret_key) - 1;
    fn_804711D0(gcd_secret_key, secretKey, cpySize);
    gcd_secret_key[cpySize] = '\0';
}

void DWC_ShutdownFriendsMatch(void)
{
    DWC_Printf(4,
        "!!DWC_ShutdownFriendsMatch() was called!! stpDwcCnt = 0x%x\n",
        stpDwcCnt);

    if (!stpDwcCnt)
    {
        return;
    }

    if (stpDwcCnt->matchcnt.qr2Obj)
    {
        qr2_shutdown(stpDwcCnt->matchcnt.qr2Obj);
        stpDwcCnt->matchcnt.qr2Obj = NULL;
    }
    stpDwcCnt->matchcnt.qr2ShutdownFlag = 0;

    if (stpDwcCnt->matchcnt.sbObj)
    {
        ServerBrowserFree(stpDwcCnt->matchcnt.sbObj);
        stpDwcCnt->matchcnt.sbObj = NULL;
    }

    DWCi_FreeNegotiateList();

    fn_80451FF0();
    CloseStatsConnection();
    fn_80452000();

    if (stpDwcCnt->gpObj)
    {
        (void)gpSetCallback(
            &stpDwcCnt->gpObj, GP_ERROR, (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_MESSAGE,
            (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_REQUEST,
            (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_STATUS,
            (GPCallback)NULL, NULL);
        (void)gpProcess(&stpDwcCnt->gpObj);
        gpDestroy(&stpDwcCnt->gpObj);
        stpDwcCnt->gpObj = NULL;
    }

    DWCi_ShutdownLogin();
    DWCi_ShutdownFriend();
    DWCi_ShutdownMatch();
    DWCi_ShutdownTransport();

    if (stpDwcCnt->gt2Socket)
    {
        gt2CloseSocket(stpDwcCnt->gt2Socket);
        stpDwcCnt->gt2Socket = NULL;
    }

    DWC_Free(DWC_ALLOCTYPE_BASE, stpDwcCnt, 0);
    stpDwcCnt = NULL;
}

void DWC_ProcessFriendsMatch(void)
{
    GSIACResult acResult;
    GPResult gpResult;

    if (!stpDwcCnt || stpDwcCnt->state == DWC_STATE_INIT || DWCi_IsError())
    {
        return;
    }

    switch (stpDwcCnt->state)
    {
    case DWC_STATE_AVAILABLE_CHECK:
        acResult = GSIAvailableCheckThink();
        switch (acResult)
        {
        case GSIACAvailable:
            DWC_Printf(DWC_REPORTFLAG_ACHECK,
                "Confirmed the backend of GameSpy server.\n");

            gpResult = gpInitialize(
                &stpDwcCnt->gpObj, stpDwcCnt->logcnt.productID, 16, 11);
            if (DWCi_HandleGPError(gpResult))
            {
                return;
            }

            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_ERROR,
                (GPCallback)DWCi_GPErrorCallback, NULL);
            if (DWCi_HandleGPError(gpResult))
            {
                return;
            }

            gpResult = gpSetCallback(&stpDwcCnt->gpObj,
                GP_RECV_BUDDY_MESSAGE,
                (GPCallback)DWCi_GPRecvBuddyMessageCallback, NULL);
            if (DWCi_HandleGPError(gpResult))
            {
                return;
            }

            gpResult = gpSetCallback(&stpDwcCnt->gpObj,
                GP_RECV_BUDDY_AUTH,
                (GPCallback)DWCi_GPRecvBuddyAuthCallback, NULL);
            if (DWCi_HandleGPError(gpResult))
            {
                return;
            }

            gpResult = gpSetCallback(&stpDwcCnt->gpObj,
                GP_RECV_BUDDY_REQUEST,
                (GPCallback)DWCi_GPRecvBuddyRequestCallback, NULL);
            if (DWCi_HandleGPError(gpResult))
            {
                return;
            }

            gpResult = gpSetCallback(&stpDwcCnt->gpObj,
                GP_RECV_BUDDY_STATUS,
                (GPCallback)DWCi_GPRecvBuddyStatusCallback, NULL);
            if (DWCi_HandleGPError(gpResult))
            {
                return;
            }

            DWCi_SetState(DWC_STATE_LOGIN);

            if (!DWCi_LoginAsync())
            {
                DWCi_StopLogin(DWC_ERROR_AUTH_ANY, -20100);
            }
            break;
        case GSIACUnavailable:
            DWCi_StopLogin(DWC_ERROR_AUTH_OUT_OF_SERVICE, -20110);
            return;
        case GSIACTemporarilyUnavailable:
            DWCi_StopLogin(DWC_ERROR_AUTH_STOP_SERVICE, -20101);
            return;
        default:
            break;
        }
        break;

    case DWC_STATE_LOGIN:
        DWCi_LoginProcess();
        break;

    case DWC_STATE_ONLINE:
        DWCi_FriendProcess();
        DWCi_MatchProcess(FALSE);
        break;

    case DWC_STATE_MATCHING:
        DWCi_MatchProcess(TRUE);
        DWCi_FriendProcess();
        break;

    case DWC_STATE_CONNECTED:
        DWCi_TransportProcess();
        DWCi_FriendProcess();

        if (stpDwcCnt->matchcnt.matchType == DWC_MATCH_TYPE_SC_SV
            || stpDwcCnt->matchcnt.matchType == DWC_MATCH_TYPE_SC_CL)
        {
            DWCi_MatchProcess(TRUE);
        }
        else if (stpDwcCnt->gt2Socket)
        {
            DWCi_MatchProcess(FALSE);
        }
        break;

    default:
        break;
    }

    if (stpDwcCnt->matchcnt.qr2ShutdownFlag == 1)
    {
        if (stpDwcCnt->matchcnt.qr2Obj != NULL)
        {
            qr2_shutdown(stpDwcCnt->matchcnt.qr2Obj);
            stpDwcCnt->matchcnt.qr2Obj = NULL;
        }
        stpDwcCnt->matchcnt.qr2ShutdownFlag = 0;
    }
}


#define MULTIPLIER -1664117991
static int StringHash(const char* s, int numbuckets)
{
    goa_uint32 hashcode = 0;
    while (*s != 0)
    {
        hashcode = (goa_uint32)((int)hashcode * MULTIPLIER + tolower(*s));
        s++;
    }
    return (int)(hashcode % numbuckets);
}

static int DWCi_GetMasterServerIndex(const char* gameName)
{
    return StringHash(gameName, 20);
}

static inline int DWCi_CacheDNSQueries(const char* gameName)
{
    char hostname[256];
    if (gethostbyname("gpcm.gs.nintendowifi.net") == NULL) goto error;
    if (gethostbyname("gpsp.gs.nintendowifi.net") == NULL) goto error;
    if (gethostbyname("gamestats.gs.nintendowifi.net") == NULL) goto error;
    if (gethostbyname("gamestats2.gs.nintendowifi.net") == NULL) goto error;

    if (gameName)
    {
        sprintf(hostname, "%s.available.gs.nintendowifi.net", gameName);
        if (gethostbyname(hostname) == NULL) goto error;
        sprintf(hostname, "%s.natneg1.gs.nintendowifi.net", gameName);
        if (gethostbyname(hostname) == NULL) goto error;
        sprintf(hostname, "%s.natneg2.gs.nintendowifi.net", gameName);
        if (gethostbyname(hostname) == NULL) goto error;
        sprintf(hostname, "%s.natneg3.gs.nintendowifi.net", gameName);
        if (gethostbyname(hostname) == NULL) goto error;
        sprintf(hostname, "%s.master.gs.nintendowifi.net", gameName);
        if (gethostbyname(hostname) == NULL) goto error;
        sprintf(hostname, "%s.gamestats.gs.nintendowifi.net", gameName);
        if (gethostbyname(hostname) == NULL) goto error;
        sprintf(hostname, "%s.gamestats2.gs.nintendowifi.net", gameName);
        if (gethostbyname(hostname) == NULL) goto error;
        sprintf(hostname, "%s.ms%d.gs.nintendowifi.net", gameName,
            DWCi_GetMasterServerIndex(gameName));
        if (gethostbyname(hostname) == NULL) goto error;
    }

    return 0;

error:
    DWC_Printf(DWC_REPORTFLAG_WARNING, "Failed to cache DNS query.\n");
    return -1;
}

BOOL DWC_LoginAsync(const u16* ingamesn, const char* reserved,
    DWCLoginCallback callback, void* param)
{
    u32 len;
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_LoginAsync() was called!!\n");
    if (ingamesn == NULL)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "ingamesn is NULL!!\n");
        return FALSE;
    }
    if (DWCi_IsError() || stpDwcCnt->state != DWC_STATE_INIT)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }
    stpDwcCnt->loginCallback = callback;
    stpDwcCnt->loginParam = param;
    if (!ingamesn || ingamesn[0] == 0)
    {
        len = 0;
    }
    else
    {
        memset(stpDwcCnt->playerName, 0, sizeof(stpDwcCnt->playerName));
        len = DWCi_WStrLen(ingamesn) <= DWC_MAX_PLAYER_NAME - 1
            ? DWCi_WStrLen(ingamesn) : DWC_MAX_PLAYER_NAME - 1;
        fn_804711D0(stpDwcCnt->playerName, ingamesn, len * 2);
    }
    stpDwcCnt->playerName[len] = 0;
    DWCi_CacheDNSQueries(gcd_gamename);
    DWCi_SetState(DWC_STATE_AVAILABLE_CHECK);
    GSIStartAvailableCheckA(stpDwcCnt->gameName);
    return TRUE;
}

BOOL fn_80450660(void);

static void DWCi_UpdateServersCallback(int error, BOOL isChanged, void* param)
{
    stpDwcCnt->updateServersCallback(error, isChanged, stpDwcCnt->updateServersParam);
}

BOOL DWC_UpdateServersAsync(const char* playerName,
    DWCUpdateServersCallback updateCallback, void* updateParam,
    DWCFriendStatusCallback statusCallback, void* statusParam,
    DWCDeleteFriendListCallback deleteCallback, void* deleteParam)
{
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_UpdateServersAsync() was called!!\n");
    if (DWCi_IsError() || stpDwcCnt->state < DWC_STATE_ONLINE || fn_80450660())
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }
    stpDwcCnt->updateServersCallback = updateCallback;
    stpDwcCnt->updateServersParam = updateParam;
    DWCi_UpdateServersAsync(stpDwcCnt->logcnt.authToken,
        stpDwcCnt->logcnt.partnerChallenge, DWCi_UpdateServersCallback, NULL,
        statusCallback, statusParam, deleteCallback, deleteParam);
    return TRUE;
}

BOOL DWC_ConnectToAnybodyAsync(int topology, u8 numEntry, const char* addFilter,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCEvalPlayerCallback evalCallback, void* evalParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam)
{
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_ConnectToAnybodyAsync() was called!!\n");
    if (DWCi_IsError() || stpDwcCnt->state != DWC_STATE_ONLINE)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }
    DWCi_ClearGT2ConnectionList();
    stpDwcCnt->matchedCallback = matchedCallback;
    stpDwcCnt->matchedParam = matchedParam;
    DWCi_SetState(DWC_STATE_MATCHING);
    stpDwcCnt->matchcnt.qr2MatchType = DWC_MATCH_TYPE_ANYBODY;
    stpDwcCnt->matchcnt._8C8 = 0;
    DWCi_ConnectToAnybodyAsync(topology, (u8)(numEntry - 1), addFilter,
        DWCi_MatchedCallback, NULL, newClientCallback, newClientParam,
        evalCallback, evalParam, reservationCallback, reservationData, reservationParam);
    return TRUE;
}

BOOL DWC_SetupGameServer(int topology, u8 maxEntry,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam)
{
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_SetupGameServer() was called!!\n");
    if (DWCi_IsError() || stpDwcCnt->state != DWC_STATE_ONLINE)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }
    DWCi_ClearGT2ConnectionList();
    stpDwcCnt->matchedCallback = matchedCallback;
    stpDwcCnt->matchedParam = matchedParam;
    DWCi_SetState(DWC_STATE_MATCHING);
    stpDwcCnt->matchcnt.qr2MatchType = DWC_MATCH_TYPE_SC_SV;
    stpDwcCnt->matchcnt._8C8 = 0;
    DWCi_SetupGameServer(topology, (u8)(maxEntry - 1), DWCi_MatchedCallback, NULL,
        newClientCallback, newClientParam, reservationCallback, reservationData, reservationParam);
    return TRUE;
}

BOOL DWC_ConnectToGameServerAsync(int topology, int serverIndex,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam)
{
    int profileID;
    int buddyIdx = -1;
    GPBuddyStatus status;
    DWCError error;
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_ConnectToGameServerAsync() was called!!\n");
    if (DWCi_IsError() || stpDwcCnt->state != DWC_STATE_ONLINE)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }
    DWCi_ClearGT2ConnectionList();
    stpDwcCnt->matchedCallback = matchedCallback;
    stpDwcCnt->matchedParam = matchedParam;
    DWCi_SetState(DWC_STATE_MATCHING);
    if (!(profileID = DWCi_GetProfileIDFromList(serverIndex)) || !gpIsBuddy(&stpDwcCnt->gpObj, profileID))
    {
        DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is not buddy.\n", profileID);
        error = DWC_ERROR_NOT_FRIEND_SERVER;
        goto error;
    }
    gpGetBuddyIndex(&stpDwcCnt->gpObj, profileID, &buddyIdx);
    gpGetBuddyStatus(&stpDwcCnt->gpObj, buddyIdx, &status);
    if (status.status != DWC_STATUS_MATCH_SC_SV)
    {
        DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is not game server.\n", profileID);
        error = DWC_ERROR_NOT_FRIEND_SERVER;
        goto error;
    }
    {
        char valueStr[4];
        u8 maxEntry;
        u8 numEntry;
        valueStr[0] = '0';
        if (DWCi_IsBuddyMatchLocked(&status))
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is locked.\n", profileID);
            error = DWC_ERROR_MATCH_LOCKED;
            goto error;
        }
        DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_MAX, valueStr, status.statusString, '/');
        maxEntry = (u8)strtoul(valueStr, NULL, 10);
        DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_NUM, valueStr, status.statusString, '/');
        numEntry = (u8)strtoul(valueStr, NULL, 10);
        if (numEntry == maxEntry)
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is fully occupied.\n", profileID);
            error = DWC_ERROR_SERVER_FULL;
            goto error;
        }
    }
    stpDwcCnt->matchcnt.qr2MatchType = DWC_MATCH_TYPE_SC_CL;
    stpDwcCnt->matchcnt._8C8 = 0;
    DWCi_ConnectToGameServerAsync(topology, profileID, DWCi_MatchedCallback, NULL,
        newClientCallback, newClientParam, reservationCallback, reservationData, reservationParam);
    return TRUE;
error:
    DWCi_SetError(error, 0);
    stpDwcCnt->matchedCallback(error, FALSE, TRUE, FALSE, 0, stpDwcCnt->matchedParam);
    if (stpDwcCnt != NULL && stpDwcCnt->state == DWC_STATE_MATCHING)
    {
        DWCi_SetState(DWC_STATE_ONLINE);
        DWCi_GPSetServerStatus(DWC_STATUS_ONLINE);
    }
    return TRUE;
}


int DWC_CloseAllConnectionsHard(void)
{
    BOOL hasConnection = FALSE;
    int i;
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
        "!!DWC_CloseAllConnectionsHard() was called!!\n");
    stpDwcCnt->matchcnt.closeAllRequested = 1;
    if (DWC_CancelMatching())
        return 1;
    if (!stpDwcCnt || DWCi_IsError()
        || (stpDwcCnt->state != DWC_STATE_MATCHING
            && stpDwcCnt->state != DWC_STATE_CONNECTED))
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }
    for (i = 0; i < 32; i++)
    {
        if (stGt2ConnectionList[i] != NULL)
        {
            hasConnection = TRUE;
            break;
        }
    }
    if (!hasConnection)
    {
        DWCi_GetNodeCount();
        DWC_Printf(DWC_REPORTFLAG_TRANSPORT, "Closed 0 connection.\n");
        DWCi_GPSetServerStatus(DWC_STATUS_ONLINE);
        DWCi_FreeNegotiateList();
        DWCi_SetState(DWC_STATE_ONLINE);
        DWCi_ClearQR2Key();
        DWCi_ClearNodeList();
        DWCi_SetMatchStatus(DWC_MATCH_STATE_INIT);
        DWCi_CloseMatching();
        return 1;
    }
    stpDwcCnt->ownCloseFlag = TRUE;
    gt2CloseAllConnectionsHard(stpDwcCnt->gt2Socket);
    while (DWCi_GetNodeCount() > 1)
    {
        for (i = 0; i < DWCi_GetNodeCount(); i++)
        {
            DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
            if (node->profileID != stpDwcCnt->profileID)
                fn_80455E60(NULL, GT2LocalClose, TRUE, node->aid);
        }
    }
    stpDwcCnt->ownCloseFlag = FALSE;
    DWCi_ClearNodeList();
    DWCi_SetMatchStatus(DWC_MATCH_STATE_INIT);
    DWCi_CloseMatching();
    if (DWC_GetState() != DWC_STATE_ONLINE)
    {
        DWCi_FreeNegotiateList();
        DWCi_SetState(DWC_STATE_ONLINE);
        DWCi_ClearQR2Key();
    }
    return 0;
}

static void DWCi_SetState(DWCState state)
{
    const char* names[] = {
        "DWC_STATE_INIT", "DWC_STATE_AVAILABLE_CHECK", "DWC_STATE_LOGIN",
        "DWC_STATE_ONLINE", "DWC_STATE_MATCHING", "DWC_STATE_CONNECTED"
    };
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "dwc state changed %s -> %s\n", names[stpDwcCnt->state], names[state]);
    stpDwcCnt->lastState = stpDwcCnt->state;
    stpDwcCnt->state = state;
}

static void DWCi_ClearGT2ConnectionList(void)
{
    memset(stGt2ConnectionList, 0, sizeof(stGt2ConnectionList));
    memset(stConnectionInfoList, 0, sizeof(stConnectionInfoList));
}

static GT2Result DWCi_HandleGT2Error(GT2Result result);
static void DWCi_GT2SocketErrorCallback(GT2Socket socket);
void DWCi_GT2ConnectAttemptCallback(GT2Socket socket, GT2Connection connection,
    unsigned int ip, unsigned short port, int latency, GT2Byte* message, int len);
GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket,
    unsigned int ip, unsigned short port, GT2Byte* message, int len);

GT2Result DWCi_GT2Startup(void)
{
    u16 port;
    GT2Result result;

    if (stpDwcCnt->gt2Socket != NULL)
    {
        DWC_Printf(8, "gt2Socket is already made.\n");
        return GT2Success;
    }

    port = DWCi_GetMathRand32(0x4000) + 0xC000;
    DWC_Printf(0x40, "--- Private port = %d ---\n", port);
    result = gt2CreateSocket(&stpDwcCnt->gt2Socket,
        gt2AddressToString(0, port, NULL),
        stpDwcCnt->gt2SendBufSize,
        stpDwcCnt->gt2RecvBufSize,
        DWCi_GT2SocketErrorCallback);
    if (DWCi_HandleGT2Error(result))
    {
        return result;
    }
    gt2Listen(stpDwcCnt->gt2Socket, DWCi_GT2ConnectAttemptCallback);
    gt2SetUnrecognizedMessageCallback(
        stpDwcCnt->gt2Socket, DWCi_GT2UnrecognizedMessageCallback);
    return result;
}

static GT2Result DWCi_HandleGT2Error(GT2Result result)
{
    int error;
    int errorCode;

    if (result == GT2Success)
    {
        return FALSE;
    }
    DWC_Printf(2, "Main, GT2 error %d\n", result);
    switch (result)
    {
    case GT2OutOfMemory:
        error = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GT2Rejected:
    case GT2DuplicateAddress:
        error = DWC_ERROR_NONE;
        errorCode = 0;
        result = FALSE;
        break;
    case GT2NetworkError:
        error = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GT2AddressError:
        error = DWC_ERROR_NETWORK;
        errorCode = -30;
        break;
    case GT2TimedOut:
        error = DWC_ERROR_NETWORK;
        errorCode = -70;
        break;
    case GT2NegotiationError:
        error = DWC_ERROR_NETWORK;
        errorCode = -80;
        break;
    }
    if (error != 0)
    {
        DWCi_StopLogin(error,
            DWC_ECODE_SEQ_LOGIN + DWC_ECODE_GS_GT2 + errorCode);
    }
    return result;
}

static void DWCi_GT2SocketErrorCallback(GT2Socket socket)
{
    stLastSocketError = GOAGetLastError(gt2GetSocketSOCKET(socket));

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Socket fatal error! (%d)\n",
        stLastSocketError);

    DWCi_SetError(DWC_ERROR_FATAL,
        DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2
            + DWC_ECODE_TYPE_SO_SOCKET);
    stpDwcCnt->gt2Socket = NULL;
}



static BOOL DWCi_HasConnectionList(void)
{
    return stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV
        || stpDwcCnt->matchcnt.matchingComplete;
}

int DWC_GetNumConnectionHost(void)
{
    if (!stpDwcCnt || !DWCi_HasConnectionList())
        return 0;
    return DWCi_GetNodeCount();
}

u8 DWC_GetMyAID(void)
{
    DWCNodeInfo* node;
    u8 aid;
    if (!stpDwcCnt || !DWCi_HasConnectionList())
        return 0xFF;
    node = DWCi_GetNodeByProfileID(stpDwcCnt->profileID);
    if (node == NULL)
        aid = 0xFF;
    else
        aid = node->aid;
    return aid;
}

int DWC_GetAIDList(u8** aidList)
{
    int i;
    if (!stpDwcCnt || !DWCi_HasConnectionList())
        return 0;
    memset(stAidList, 0, sizeof(stAidList));
    for (i = 0; i < DWCi_GetNodeCount(); i++)
        stAidList[i] = DWCi_GetNodeByIndex(i)->aid;
    *aidList = stAidList;
    return DWCi_GetNodeCount();
}

static u32 DWCi_GetAIDBitmapFromList(u8* aidList, int aidListLen)
{
    u32 bitmap = 0;
    int i;
    for (i = 0; i < aidListLen; i++)
        bitmap |= 1u << aidList[i];
    return bitmap;
}

u32 DWC_GetAIDBitmap(void)
{
    u8* aidList;
    int numHost;
    if (!stpDwcCnt || !DWCi_HasConnectionList())
        return 0;
    numHost = DWC_GetAIDList(&aidList);
    return DWCi_GetAIDBitmapFromList(aidList, numHost);
}

BOOL DWC_IsValidAID(u8 aid)
{
    if (!stpDwcCnt)
        return FALSE;
    if (aid == DWC_GetMyAID())
        return FALSE;
    return DWCi_GetNodeByAID(aid) != NULL;
}

static inline u8 DWC_GetServerAID(void)
{
    DWCNodeInfo* node;
    u8 aid;
    if (!stpDwcCnt
        || stpDwcCnt->matchcnt.state == DWC_MATCH_STATE_CL_SVDOWN_1
        || stpDwcCnt->matchcnt.state == DWC_MATCH_STATE_CL_SVDOWN_2
        || stpDwcCnt->matchcnt.state == DWC_MATCH_STATE_CL_SVDOWN_3
        || stpDwcCnt->matchcnt.state == DWC_MATCH_STATE_INIT
        || !stpDwcCnt->matchcnt.matchingComplete
        || !stpDwcCnt->matchcnt.hostState)
    {
        aid = 0xFF;
    }
    else
    {
        node = DWCi_GetNodeByIndex(0);
        if (node == NULL)
        {
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                "*** DWC_GetServerAID first node is NULL!!! ***\n");
            aid = 0xFF;
        }
        else
        {
            aid = node->aid;
        }
    }
    return aid;
}

BOOL fn_80454BB0(void)
{
    if (!stpDwcCnt)
        return FALSE;
    if (DWC_GetServerAID() == 0xFF)
        return FALSE;
    return stpDwcCnt->matchcnt.hostState == 2;
}

u8 fn_804568A0(void)
{
    u8 aid = DWC_GetServerAID();
    if (aid == 0xFF)
    {
        DWCNodeInfo* node = DWCi_GetNodeByIndex(0);
        if (node != NULL)
            aid = node->aid;
    }
    return aid;
}

DWCState DWC_GetState(void)
{
    if (stpDwcCnt != NULL)
    {
        return stpDwcCnt->state;
    }
    return DWC_STATE_INIT;
}

GT2Connection DWCi_GetGT2Connection(u8 aid)
{
    int i;

    if (stpDwcCnt == NULL)
    {
        return NULL;
    }
    for (i = 0; i < 32; i++)
    {
        if (stGt2ConnectionList[i] != NULL && ((DWCConnectionInfo*)gt2GetConnectionData(stGt2ConnectionList[i]))->aid == aid)
        {
            return stGt2ConnectionList[i];
        }
    }
    return NULL;
}

u8 DWCi_GetConnectionAID(GT2Connection connection)
{
    DWCConnectionInfo* info = (DWCConnectionInfo*)gt2GetConnectionData(connection);
    return info != NULL ? info->aid : 0xFF;
}

int DWCi_GT2GetConnectionListIdx(void)
{
    int i;

    for (i = 0; i < 32; i++)
    {
        if (stGt2ConnectionList[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

GT2Connection* DWCi_GetGT2ConnectionByIdx(int index)
{
    return &stGt2ConnectionList[index];
}

GT2Connection* DWCi_GetGT2ConnectionByProfileID(int profileID, int numHost)
{
    int i;
    for (i = 0; i < DWC_MAX_CONNECTIONS; i++)
    {
        if (stGt2ConnectionList[i] != NULL
            && ((DWCConnectionInfo*)gt2GetConnectionData(stGt2ConnectionList[i]))->profileID == profileID)
        {
            return &stGt2ConnectionList[i];
        }
    }
    return NULL;
}

DWCConnectionInfo* DWCi_GetConnectionInfoByIdx(int index)
{
    return &stConnectionInfoList[index];
}

BOOL DWCi_IsValidAID(u8 aid)
{
    int i;

    for (i = 0; i < 32; i++)
    {
        if (stGt2ConnectionList[i] != NULL && ((DWCConnectionInfo*)gt2GetConnectionData(stGt2ConnectionList[i]))->aid == aid)
        {
            return TRUE;
        }
    }
    return FALSE;
}


static GPResult DWCi_HandleGPError(GPResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GP_NO_ERROR)
    {
        return GP_NO_ERROR;
    }

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Main, GP error %d\n", result);

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

    switch (stpDwcCnt->state)
    {
    case DWC_STATE_AVAILABLE_CHECK:
        errorCode += DWC_ECODE_SEQ_LOGIN + DWC_ECODE_GS_GP;
        DWCi_StopLogin(dwcError, errorCode);
        break;
    case DWC_STATE_LOGIN:
        errorCode += DWC_ECODE_SEQ_LOGIN + DWC_ECODE_GS_GP;
        if (stpDwcCnt->logcnt.state < DWC_LOGIN_STATE_REMOTE_AUTH)
        {
            DWCi_StopLogin(dwcError, errorCode);
        }
        else
        {
            DWC_Printf(
                DWC_REPORTFLAG_ERROR, "Not handle an error here.\n");
        }
        break;
    case DWC_STATE_MATCHING:
        errorCode += DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_GP;
        DWCi_StopMatching(dwcError, errorCode);
        break;
    default:
        errorCode += DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GP;
        break;
    }

    DWCi_StopFriendProcess(dwcError, errorCode);
    return result;
}


static void DWCi_GPErrorCallback(
    GPConnection* pconnection, GPErrorArg* arg, void* param)
{
    gsi_char* errorCodeString;
    gsi_char* resultString;
    GPResult gpResult;

#ifdef DWC_IGNORE_GP_ERROR_ALREADY_BUDDY
    if (arg->errorCode == GP_ADDBUDDY_ALREADY_BUDDY
        || arg->errorCode == GP_BM_NOT_BUDDY
        || arg->errorCode == GP_DELBUDDY_NOT_BUDDY)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
            "Ignore GP ALLREADY_BUDDY or NOT_BUDDY %d.\n",
            arg->errorCode);
        return;
    }
#endif

#define RESULT(x) case x: resultString = #x; break;
    switch (arg->result)
    {
        RESULT(GP_NO_ERROR)
        RESULT(GP_MEMORY_ERROR)
        RESULT(GP_PARAMETER_ERROR)
        RESULT(GP_NETWORK_ERROR)
        RESULT(GP_SERVER_ERROR)
    default:
        resultString = "Unknown result!\n";
    }

#define ERRORCODE(x) case x: errorCodeString = #x; break;
    switch (arg->errorCode)
    {
        ERRORCODE(GP_GENERAL)
        ERRORCODE(GP_PARSE)
        ERRORCODE(GP_NOT_LOGGED_IN)
        ERRORCODE(GP_BAD_SESSKEY)
        ERRORCODE(GP_DATABASE)
        ERRORCODE(GP_NETWORK)
        ERRORCODE(GP_FORCED_DISCONNECT)
        ERRORCODE(GP_CONNECTION_CLOSED)
        ERRORCODE(GP_LOGIN)
        ERRORCODE(GP_LOGIN_TIMEOUT)
        ERRORCODE(GP_LOGIN_BAD_NICK)
        ERRORCODE(GP_LOGIN_BAD_EMAIL)
        ERRORCODE(GP_LOGIN_BAD_PASSWORD)
        ERRORCODE(GP_LOGIN_BAD_PROFILE)
        ERRORCODE(GP_LOGIN_PROFILE_DELETED)
        ERRORCODE(GP_LOGIN_CONNECTION_FAILED)
        ERRORCODE(GP_LOGIN_SERVER_AUTH_FAILED)
        ERRORCODE(GP_NEWUSER)
        ERRORCODE(GP_NEWUSER_BAD_NICK)
        ERRORCODE(GP_NEWUSER_BAD_PASSWORD)
        ERRORCODE(GP_UPDATEUI)
        ERRORCODE(GP_UPDATEUI_BAD_EMAIL)
        ERRORCODE(GP_NEWPROFILE)
        ERRORCODE(GP_NEWPROFILE_BAD_NICK)
        ERRORCODE(GP_NEWPROFILE_BAD_OLD_NICK)
        ERRORCODE(GP_UPDATEPRO)
        ERRORCODE(GP_UPDATEPRO_BAD_NICK)
        ERRORCODE(GP_ADDBUDDY)
        ERRORCODE(GP_ADDBUDDY_BAD_FROM)
        ERRORCODE(GP_ADDBUDDY_BAD_NEW)
        ERRORCODE(GP_ADDBUDDY_ALREADY_BUDDY)
        ERRORCODE(GP_AUTHADD)
        ERRORCODE(GP_AUTHADD_BAD_FROM)
        ERRORCODE(GP_AUTHADD_BAD_SIG)
        ERRORCODE(GP_STATUS)
        ERRORCODE(GP_BM)
        ERRORCODE(GP_BM_NOT_BUDDY)
        ERRORCODE(GP_GETPROFILE)
        ERRORCODE(GP_GETPROFILE_BAD_PROFILE)
        ERRORCODE(GP_DELBUDDY)
        ERRORCODE(GP_DELBUDDY_NOT_BUDDY)
        ERRORCODE(GP_DELPROFILE)
        ERRORCODE(GP_DELPROFILE_LAST_PROFILE)
        ERRORCODE(GP_SEARCH)
        ERRORCODE(GP_SEARCH_CONNECTION_FAILED)
    default:
        errorCodeString = "Unknown error code!\n";
    }

    if (arg->fatal)
    {
        DWC_Printf(DWC_REPORTFLAG_ERROR, "FATAL ERROR\n");
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_ERROR, "ERROR\n");
    }

    DWC_Printf(DWC_REPORTFLAG_ERROR, "RESULT: %s (%d)\n", resultString,
        arg->result);
    DWC_Printf(DWC_REPORTFLAG_ERROR, "ERROR CODE: %s (0x%X)\n",
        errorCodeString, arg->errorCode);
    DWC_Printf(
        DWC_REPORTFLAG_ERROR, "ERROR STRING: %s\n", arg->errorString);

    gpResult = GP_NETWORK_ERROR;
    (void)DWCi_HandleGPError(gpResult);
}

static void DWCi_GPRecvBuddyMessageCallback(
    GPConnection* pconnection, GPRecvBuddyMessageArg* arg, void* param)
{
    char version[12] = { 0, };
    char* message = arg->message;
    u32 len;

    if (memcmp(message, DWC_GP_COMMAND_STRING,
            strlen(DWC_GP_COMMAND_STRING)))
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING,
            "Received undefined buddy message. '%s'\n", message);
        return;
    }

    message += strlen(DWC_GP_COMMAND_STRING);
    len = (u32)(strchr(message, 'v') - message);
    strncpy(version, message, len);
    if (len > 10
        || strtoul(version, NULL, 10) != DWC_MATCHING_VERSION)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING,
            "Received different version buddy message command. '%s'\n",
            message);
        return;
    }

    message += len + 1;
    if (memcmp(message, DWC_GP_COMMAND_MATCH_STRING,
            strlen(DWC_GP_COMMAND_MATCH_STRING)) == 0)
    {
        if (stpDwcCnt->state == DWC_STATE_MATCHING
            || (stpDwcCnt->state == DWC_STATE_CONNECTED
                && (stpDwcCnt->matchcnt.matchType
                        == DWC_MATCH_TYPE_SC_SV
                    || stpDwcCnt->matchcnt.matchType
                        == DWC_MATCH_TYPE_SC_CL)))
        {
            message += strlen(DWC_GP_COMMAND_MATCH_STRING);
            DWCi_MatchGPRecvBuddyMsgCallback(
                pconnection, arg->profile, message);
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                "Ignore delayed GP matching command.\n");
        }
    }
}

BOOL DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback,
    void* param)
{
    if (stpDwcCnt == NULL)
    {
        return FALSE;
    }
    stpDwcCnt->closedCallback = callback;
    stpDwcCnt->closedParam = param;
    return TRUE;
}


int DWC_CloseConnectionHard(u8 aid)
{
    GT2Connection connection;
    DWCNodeInfo* node;
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
        "!!DWCi_CloseConnectionHard() was called!! aid = %d.\n", aid);
    if (!stpDwcCnt || DWCi_IsError()
        || (stpDwcCnt->state != DWC_STATE_MATCHING
            && stpDwcCnt->state != DWC_STATE_CONNECTED))
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }
    connection = DWCi_GetGT2Connection(aid);
    if (connection == NULL)
    {
        node = DWCi_GetNodeByAID(aid);
        if (node == NULL)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "No connection!\n");
            return -2;
        }
        fn_80455E60(NULL, GT2LocalClose, TRUE, node->aid);
        return 0;
    }
    gt2CloseConnectionHard(connection);
    return 0;
}

static void DWCi_LoginCallback(
    DWCError error, int profileID, void* param)
{
    if (error == 0)
    {
        stpDwcCnt->profileID = profileID;
        DWCi_SetState(DWC_STATE_ONLINE);
        DWCi_InitGPProcessCount();
    }
    else
    {
        DWCi_SetState(DWC_STATE_INIT);
    }
    if (stpDwcCnt->loginCallback != NULL)
    {
        stpDwcCnt->loginCallback(error, profileID, stpDwcCnt->loginParam);
    }
}


static void DWCi_GT2ReceivedCallback(GT2Connection connection, GT2Byte* message, int len, GT2Bool reliable)
{
    DWCi_RecvCallback(connection, message, len, reliable);
}

static void DWCi_GT2ClosedCallback(GT2Connection connection, GT2CloseReason reason)
{
    fn_80455E60(connection, reason, FALSE, 0xFF);
}

static void DWCi_GT2PingCallback(GT2Connection connection, int latency)
{
    DWC_Printf(DWC_REPORTFLAG_TRANSPORT, "Ping: %dms\n", latency);
    DWCi_PingCallback(connection, latency);
}
