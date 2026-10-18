#include <dwc/dwc_base64.h>
#include <dwc/dwc_common.h>
#include <dwc/dwc_nonport.h>
#include <dwc/dwc_error.h>
#include <dwc/dwc_friend.h>
#include <dwc/dwc_login.h>
#include <dwc/dwc_main.h>
#include <dwc/R4QK01/dwc_main_internal.h>
#include <dwc/R4QK01/dwc_match_internal.h>
#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_report.h>
#include <gamespy/GP/gp.h>
#include <gamespy/gt2/gt2.h>
#include <gamespy/natneg/natneg.h>
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
    DWC_MATCH_COMMAND_ADD_MESSAGE_MAX = 512,
    DWC_MATCH_CANCEL_STATE_INIT = 0,
    DWC_MATCH_CANCEL_STATE_EXEC,
    DWC_MATCH_CLOSE_STATE_TIMEOUT = 1
};

static DWCGameMatchKeyData stGameMatchKeys[DWC_QR2_GAME_RESERVED_KEYS];
static OSTime stStateChangedTick;
static DWCMatchControl* stpMatchCnt;
static BOOL s_needNNFree;
static int s_nnProcessing;
static BOOL s_needSbFree;
static int s_sbCallbackLevel;
static char* stpAddFilter;

int dwcMeshRetryLimit = 5;
int dwcServerSearchRatio = -2;

BOOL fn_80454BB0(void);
GPResult DWCi_SetGPStatus(int status, const char* statusString, const char* locationString);
int DWCi_GetFriendListIndex(int profileID);

typedef enum
{
    DWC_MATCH_RESET_ALL = 0,
    DWC_MATCH_RESET_RESTART,
    DWC_MATCH_RESET_CONTINUE,
    DWC_MATCH_RESET_NUM
} DWCMatchResetLevel;


typedef enum
{
    DWC_PP_CONNECTION_SV_CONNECT = 0,
    DWC_PP_CONNECTION_CL_GT2_CONNECT,
    DWC_PP_CONNECTION_CL_GT2_ACCEPT,
    DWC_PP_CONNECTION_SYN_FINISH,
    DWC_PP_CONNECTION_NUM
} DWCPostProcessConnection;

static const int stEvalRate[DWC_SB_UPDATE_MAX_SERVERS] = { 3, 3, 2, 2, 1, 1 };
void SBServerAddIntKeyValue(SBServer server, const char* keyname, int value);

void fn_80470900(u8, u8, const void*, int, BOOL);

BOOL fn_8046FEB0(u16 type, u8 aid, const void* buffer, int size);

void fn_804708C0(u8 aid);

static DWCMatchControl* DWCi_GetMatchCnt(void);
static inline void DWCi_EnterMatchState(DWCMatchState state);
static u32 DWCi_GenerateGroupID(void);
static void DWCi_ClearGameMatchKeys(void);
static u8 DWCi_GetNewMatchKey(void);
BOOL DWC_IsValidCancelMatching(void);
static BOOL DWCi_IsReadyToSuspend(void);
static int DWCi_HandleMatchCommandError(int error);
static void DWCi_StopResendingMatchCommand(void);
static u8 DWCi_GetNodeAIDFromProfileID(int profileID);
static BOOL DWCi_CancelPendingServer(int profileID);
static void DWCi_AutoSuspendComplete(void);
static void DWCi_SendStateChanged(qr2_t object);
static DWCNodeInfo* DWCi_GetLocalNode(void);
static BOOL DWCi_IsMatchLocked(void);
static void DWCi_SetHostState(u8 state);
static void DWCi_SetGroupID(u32 groupID);
static int DWCi_GetDefaultMatchFilter(char* filter, int profileID, u8 numEntry, u8 matchType, u8 hostState);
static void DWCi_PrependNode(const DWCNodeInfo* node);
static inline int DWCi_SelectSearchHostState(void);
static BOOL DWCi_CanSendGT2MatchCommand(int profileID);
static inline void DWCi_SendGT2MsgCommand(u8 command, int profileID, DWCNodeInfo* node,
    const u32 data[], int len);
static inline SBError DWCi_SendSBMsgCommand(
    u8 command, u32 ip, u16 port, const u32 data[], int len);
static GPResult DWCi_SendGPBuddyMsgCommand(GPConnection* connection,
    u8 command, int profileID, const char* message);
static int DWCi_GetHostState(void);
static const char* DWCi_QR2KeyName(int keyid);
static DWCMatchState DWCi_GetMatchStatus(void);
static void DWCi_CancelMatchNegotiation(void);
static void DWCi_StartSuspendRequest(BOOL suspend,
    DWCSuspendCallback callback, void* param);
static void DWCi_CloseUnreportedConnections(u32 bitmap);
static int IsPrivateAddr(u32 addr_in_netorder);
static NegotiateError DWCi_DoNatNegotiationAsync(DWCNNInfo* nnInfo);
static NegotiateError DWCi_HandleNNError(NegotiateError error);
static NegotiateResult DWCi_HandleNNResult(NegotiateResult result);
static BOOL DWCi_ProcessNNFailure(BOOL ignoreError);
static DWCNodeInfo* DWCi_GetPendingNode(void);
static void DWCi_AbortMeshNegotiation(void);
static int DWCi_MakeReservationCommand(u32* data);
static BOOL DWCi_IsFriendByIdxList(int profileID);
static BOOL DWCi_IsMatchCapacityFull(int numConnection, u8 numEntry);
static BOOL DWCi_IsSameGT2Connection(GT2Connection left, GT2Connection right);
static BOOL DWCi_IsMeshConnectionPending(int profileID);
static u32 DWCi_GetUnusedAID(void);
static void DWCi_CloseAllConnectionsByTimeout(void);
static void DWCi_RestartFromTimeout(void);
static BOOL DWCi_SendCancelMatchSynCommand(int profileID, u8 command);
static BOOL DWCi_CloseCancelHostAsync(int profileID);
static int DWCi_WriteNodePacket(u32* data, int index, const DWCNodeInfo* node);
static void DWCi_SendTypeMatchAck(void);
static u32 DWCi_GetClientAIDBitmap(BOOL includePending);
static void DWCi_FinishCancelMatching(void);
static int DWCi_CancelReservation(void);
static int DWCi_InvalidateReservation(void);
static void DWCi_AppendNode(const DWCNodeInfo* node);
static BOOL DWCi_HasOnlyLocalNode(void);
static int DWCi_GetClientLinkProgress(void);
static u8 DWCi_GetClientLinkIndex(u32 nodeCount, int progress);
static void DWCi_ResetNodePacketReceiveTicks(void);
static void DWCi_ReadNodePacket(DWCNodeInfo* node, const u32* data);
static BOOL DWCi_CheckDWCServer(SBServer server);
static void DWCi_SetServerNode(SBServer server);
static void DWCi_ConnectToGroupServer(SBServer server);
static const char* DWCi_GetReservationDenyReason(u8 reason);
void DWCi_FreeNegotiateList(void);
BOOL DWC_RegisterMatchStatus(void);
BOOL DWC_CancelMatching(void);
u8 DWC_AddMatchKeyInt(u8 keyID, const char* keyString, const int* valueSrc);
u8 DWC_AddMatchKeyString(u8 keyID, const char* keyString,
    const char* valueSrc);
int DWC_GetMatchIntValue(int index, const char* keyString, int idefault);
BOOL DWC_RequestSuspendMatchAsync(BOOL suspend,
    DWCSuspendCallback callback, void* param);
BOOL fn_80456F10(void);
void DWCi_MatchInit(DWCMatchControl* matchcnt, GPConnection* pGpObj,
    GT2Socket* pGt2Socket, GT2ConnectionCallbacks* gt2Callbacks,
    const char* gameName, const char* secretKey,
    const DWCFriendData friendList[], int friendListLen);
qr2_error_t DWCi_QR2Startup(int profileID);
void DWCi_ConnectToAnybodyAsync(int topology, u8 numEntry, const char* addFilter,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCEvalPlayerCallback evalCallback, void* evalParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam);
void DWCi_SetupGameServer(int topology, u8 maxEntry,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam);
void DWCi_ConnectToGameServerAsync(int topology, int serverPid,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCNewClientCallback newClientCallback, void* newClientParam,
    DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam);
void DWCi_MatchProcess(BOOL fullSpec);
GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket, unsigned int ip,
    unsigned short port, GT2Byte* message, int len);
void DWCi_GT2ConnectAttemptCallback(GT2Socket socket,
    GT2Connection connection, unsigned int ip, unsigned short port,
    int latency, GT2Byte* message, int len);
void DWCi_GT2ConnectedCallback(GT2Connection connection, GT2Result result,
    GT2Byte* message, int len);
void DWCi_MatchGPRecvBuddyMsgCallback(GPConnection* connection, u32 profileID, char* message);
void DWCi_StopMatching(DWCError error, int errorCode);
void DWCi_ClearQR2Key(void);
void fn_8045C740(u8 aid, u16 type, u8* data);
void fn_8045C7B0(u8 aid, u16 type, u8* data);
void fn_8045DBB0(const u32* data);
BOOL DWCi_ProcessMatchClosing(DWCError error, int errorCode, int profileID);
void DWCi_ProcessMatchSCClosing(int profileID);
GPResult DWCi_GPSetServerStatus(int status);
void DWCi_ShutdownMatch(void);
BOOL DWCi_IsShutdownMatch(void);
void DWCi_ResetMatchParam(DWCMatchResetLevel level);
void DWCi_SetMatchCommonParam(u8 matchType, u8 numEntry,
    DWCMatchedSCCallback callback, void* param);
void DWCi_CloseMatching(void);
SBError DWCi_SBUpdateAsync(int profileID);
NegotiateError fn_8045F330(int isQR2, int cookie, DWCNodeInfo* node);
int DWCi_SendMatchCommand(u8 command, int profileID, u32 ip, u16 port,
    const u32 data[], int len);
BOOL fn_8045FB60(u8 command, int srcPid, u32 srcIP, u16 srcPort, const u32* data, int len);
u32 fn_80462850(int profileID, u32 qr2IP, u16 qr2Port, u32 matchType,
    BOOL priorFlag, u32 topology, const void* reservationData, BOOL mesh);
u32 fn_80462B30(int profileID, u32 qr2IP, u16 qr2Port, u32 matchType,
    BOOL priorFlag, u32 topology, const void* reservationData, BOOL mesh);
int fn_80463010(int profileID, u32 ip, u16 port);
int DWCi_SendResvCommand(int profileID, BOOL delay);
int DWCi_SendResvCommandToFriend(BOOL delay, BOOL init, int resendPid);
BOOL DWCi_RetryReserving(int resendPid);
BOOL DWCi_CancelPreConnectedServerProcess(int clientPid, int level);
BOOL DWCi_CancelPreConnectedClientProcess(int serverPid);
void DWCi_PostProcessConnection(DWCPostProcessConnection type);
void fn_804653E0(void);
BOOL fn_80465AB0(void);
BOOL fn_80465D80(void);
void fn_80466160(void);
void DWCi_RestartFromCancel(DWCMatchResetLevel level);
BOOL fn_80466EA0(u32 bitmap);
void fn_804671A0(u8 aid, u16 type);
BOOL fn_80467570(void);
BOOL fn_80467AC0(int profileID, u8 command, u32 data);
BOOL fn_804681D0(void);
static GPResult DWCi_HandleGPError(GPResult result);
static SBError DWCi_HandleSBError(SBError error);
static qr2_error_t DWCi_HandleQR2Error(qr2_error_t error);
GT2Result fn_80468E40(GT2Result error);
void DWCi_SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void* instance);
void DWCi_SBPrintServerData(SBServer server);
int DWCi_EvaluateServers(int sort);
void DWCi_RandomizeServers(void);
static void DWCi_QR2ServerKeyCallback(int keyid, qr2_buffer_t outbuf, void* userdata);
static void DWCi_QR2PlayerKeyCallback(
    int keyid, int index, qr2_buffer_t outbuf, void* userdata);
static void DWCi_QR2TeamKeyCallback(
    int keyid, int index, qr2_buffer_t outbuf, void* userdata);
static void DWCi_QR2KeyListCallback(
    qr2_key_type keytype, qr2_keybuffer_t keybuffer, void* userdata);
static int DWCi_QR2CountCallback(qr2_key_type keytype, void* userdata);
static void DWCi_QR2AddErrorCallback(
    qr2_error_t error, gsi_char* errmsg, void* userdata);
static void DWCi_QR2PublicAddrCallback(
    unsigned int ip, unsigned short port, void* userdata);
void DWCi_QR2NatnegCallback(int cookie, void* userdata);
void fn_8046C590(const void* data, u32 len);
void DWCi_QR2ClientMsgCallback(
    gsi_char* data, int len, void* userdata);
static void DWCi_NNProgressCallback(NegotiateState state, void* userdata);
void DWCi_NNCompletedCallback(NegotiateResult result, SOCKET gamesocket,
    struct sockaddr_in* remoteaddr, void* userdata);
BOOL fn_8046DB10(void);
BOOL DWCi_IsBackupMine(void);
void fn_8046DCC0(int newGroup);
int fn_8046E040(BOOL force);
BOOL fn_8046E250(u8 aid, BOOL suspend, int reply);
BOOL fn_8046E730(void);
static BOOL DWCi_RequestSuspendMatch(BOOL suspend,
    DWCSuspendCallback callback, void* param);
BOOL fn_8046EC30(void);
BOOL fn_8046ED90(BOOL suspend);
BOOL fn_8046F060(void);
void DWCi_SetMatchStatus(DWCMatchState state);
BOOL fn_8046F180(void);
void DWCi_ClearNodeList(void);
int DWCi_GetNodeCount(void);
BOOL DWCi_RemoveNodeByAID(u8 aid);
BOOL fn_8046F2C0(int profileID);
DWCNodeInfo* DWCi_GetFirstNode(void);
DWCNodeInfo* DWCi_GetNodeByIndex(int index);
DWCNodeInfo* DWCi_GetNodeByProfileID(int profileID);
DWCNodeInfo* DWCi_GetNodeByAID(u8 aid);
OSTime fn_8046F4A0(void);
int fn_8046F650(void);
BOOL fn_8046F8A0(void);
BOOL DWCi_IsBuddyMatchLocked(const GPBuddyStatus* status);
void fn_8046F920(int profileID);
void fn_8046F930(void);
static const char* DWCi_GetMatchStateName(DWCMatchState state);

static DWCMatchControl* DWCi_GetMatchCnt(void)
{
    return stpMatchCnt;
}

static inline void DWCi_EnterMatchState(DWCMatchState state)
{
    DWCi_SetMatchStatus(state);
}

static u32 DWCi_GenerateGroupID(void)
{
    DWCMatchControl* control = DWCi_GetMatchCnt();
    u32 randomID = DWCi_GetMathRand32(10000000);
    int profileID = control->profileID;
    u32 groupID;
    randomID += 100000000;
    groupID = profileID + randomID;
    return groupID;
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

BOOL DWC_IsValidCancelMatching(void)
{
    if (DWCi_IsError())
        return FALSE;
    if (!DWCi_GetMatchCnt() || DWC_GetState() != DWC_STATE_MATCHING
        || DWCi_GetMatchCnt()->cancelState != 0)
        return FALSE;
    return TRUE;
}

static BOOL DWCi_IsReadyToSuspend(void)
{
    if (fn_80454BB0())
    {
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_WAITING
            || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SEARCH_OWN)
            return TRUE;
        return FALSE;
    }
    return TRUE;
}

static int DWCi_HandleMatchCommandError(int error)
{
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY)
        return DWCi_HandleSBError((SBError)error);
    return DWCi_HandleGPError((GPResult)error);
}

static void DWCi_StopResendingMatchCommand(void)
{
    DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;
    DWCi_GetMatchCnt()->cmdCnt.count = 0;
}

static u8 DWCi_GetNodeAIDFromProfileID(int profileID)
{
    DWCNodeInfo* node = DWCi_GetNodeByProfileID(profileID);
    u8 aid;
    if (node == NULL)
        aid = 0xFF;
    else
        aid = node->aid;
    return aid;
}

static BOOL DWCi_CancelPendingServer(int profileID)
{
    DWCMatchResetLevel level = DWC_MATCH_RESET_CONTINUE;
    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY
            || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
        && !DWCi_GetMatchCnt()->matchingComplete)
        level = DWC_MATCH_RESET_RESTART;
    return DWCi_CancelPreConnectedServerProcess(profileID, level);
}

static void DWCi_AutoSuspendComplete(void)
{
    if (DWCi_GetMatchCnt()->_02C == 0xFF)
        return;
    if (DWCi_GetMatchCnt()->_02C == 1)
        DWCi_GetMatchCnt()->_01C = DWC_GetAIDBitmap();
    else
        DWCi_GetMatchCnt()->_01C = 0;
    DWC_Printf(DWC_REPORTFLAG_INFO,
        "DWCi_AutoSuspendComplete - suspend is %s.\n",
        DWCi_GetMatchCnt()->_02C ? "TRUE" : "FALSE");
    if (DWCi_GetMatchCnt()->suspendCallback)
        DWCi_GetMatchCnt()->suspendCallback(DWC_SUSPEND_SUCCESS,
            DWCi_GetMatchCnt()->_02C, DWCi_GetMatchCnt()->suspendParam);
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    DWCi_GetMatchCnt()->_02C = 0xFF;
    DWCi_GetMatchCnt()->_04C = 0;
}

static void DWCi_SendStateChanged(qr2_t object)
{
    if (!object)
        return;
    qr2_think(object);
    if (object->userstatechangerequested)
        return;
    // Polling retains this match-type read even when no notification is pending.
    (void)DWCi_GetMatchCnt()->matchType;
}

static DWCNodeInfo* DWCi_GetLocalNode(void)
{
    return DWCi_GetNodeByProfileID(DWCi_GetMatchCnt()->profileID);
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

static DWCMatchState DWCi_GetMatchStatus(void)
{
    if (DWCi_GetMatchCnt())
        return DWCi_GetMatchCnt()->state;
    return DWC_MATCH_STATE_INIT;
}

static void DWCi_CancelMatchNegotiation(void)
{
    if (DWCi_GetMatchCnt()->nnInfo.cookie)
    {
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
        DWCi_GetMatchCnt()->nnRecvCount = 0;
        DWCi_GetMatchCnt()->nnLastCookie = 0;
        DWCi_GetMatchCnt()->nnFailedTime = 0;
    }
}

static void DWCi_StartSuspendRequest(BOOL suspend,
    DWCSuspendCallback callback, void* param)
{
    DWCi_GetMatchCnt()->_030 = 0;
    DWCi_GetMatchCnt()->_02C = suspend;
    DWCi_GetMatchCnt()->suspendCallback = callback;
    DWCi_GetMatchCnt()->suspendParam = param;
    DWCi_GetMatchCnt()->_020 = 0;
    DWCi_GetMatchCnt()->suspendStartTick = DWCi_Np_GetTick();
    DWCi_GetMatchCnt()->_04C = TRUE;
    DWCi_GetMatchCnt()->_048 = FALSE;
}

static void DWCi_CloseUnreportedConnections(u32 bitmap)
{
    u32 localBitmap;
    u32 deadBitmap;
    u32 aid;
    localBitmap = DWC_GetAIDBitmap();
    deadBitmap = localBitmap & (localBitmap ^ bitmap);
    if (deadBitmap)
    {
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
            "check dead aid: dead aidBitmap is %08x.\n", deadBitmap);
        for (aid = 0; aid < 32; aid++)
            if (deadBitmap & (1 << (u8)aid))
                DWCi_CloseConnectionHard((u8)aid);
    }
}

static int IsPrivateAddr(u32 addr_in_netorder)
{
    u32 address = SONtoHl(addr_in_netorder);
    u32 network = address >> 24;
    u32 subnet = (address >> 16) & 0xFF;
    if (network == 10)
        return TRUE;
    if (network == 172 && 16 <= subnet && subnet <= 31)
        return TRUE;
    if (network == 192 && subnet == 168)
        return TRUE;
    return FALSE;
}

static NegotiateError DWCi_DoNatNegotiationAsync(DWCNNInfo* nnInfo)
{
    SBError sbError;
    NegotiateError nnError;
    int i;

    if (!nnInfo->isQR2)
    {
        sbError = ServerBrowserSendNatNegotiateCookieToServer(
            DWCi_GetMatchCnt()->sbObj,
            gt2AddressToString(nnInfo->ip, 0, NULL), nnInfo->port,
            nnInfo->cookie);
        if (DWCi_HandleSBError(sbError))
            return ne_socketerror;
        DWC_Printf(0x40, "Send NN cookie = %x.\n", nnInfo->cookie);
    }

    for (i = 0; i < DWC_MATCH_DNS_ERROR_RETRY_MAX; ++i)
    {
        nnError = NNBeginNegotiationWithSocket(
            gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
            nnInfo->cookie, nnInfo->isQR2, DWCi_NNProgressCallback,
            DWCi_NNCompletedCallback, nnInfo);
        if (nnError == ne_noerror || nnError != ne_dnserror)
            break;
        DWC_Printf(4,
            " dns error occurs when NatNegotiation begin... retry\n");
    }

    return nnError;
}

static NegotiateError DWCi_HandleNNError(NegotiateError error)
{
    int errorCode;
    DWCError dwcError;

    if (error == ne_noerror)
    {
        return ne_noerror;
    }

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, NN error %d\n", error);

    switch (error)
    {
    case ne_allocerror:
        dwcError = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case ne_socketerror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case ne_dnserror:
        dwcError = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    }

    errorCode += DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_NN;

    DWCi_StopMatching(dwcError, errorCode);

    return error;
}

static NegotiateResult DWCi_HandleNNResult(NegotiateResult result)
{
    DWCError type;
    int code;

    if (result == 0)
    {
        return 0;
    }
    DWC_Printf(8, "Match, NN result %d\n", result);
    switch (result)
    {
    case nr_deadbeatpartner:
        return 1;
    case nr_inittimeout:
        return 2;
    case nr_pingtimeout:
        return nr_pingtimeout;
    default:
        type = DWC_ERROR_NETWORK;
        code = -9;
        break;
    }
    if (type != 0)
    {
        DWCi_StopMatching(type,
            code + DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_NN);
    }
    return result;
}

static BOOL DWCi_ProcessNNFailure(BOOL ignoreError)
{
    if (ignoreError)
    {
        return TRUE;
    }

    if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL)
    {
        DWCi_GetMatchCnt()->nnFailureCount++;

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN failure %d/%d.\n", DWCi_GetMatchCnt()->nnFailureCount, DWC_MATCH_NN_FAILURE_MAX);
    }

    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
        || DWCi_GetMatchCnt()->_8C8
        || (DWCi_GetMatchCnt()->nnFailureCount
            >= DWC_MATCH_NN_FAILURE_MAX))
    {
        DWCi_StopMatching(DWC_ERROR_NETWORK,
            DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_NN
                + DWC_ECODE_TYPE_MUCH_FAILURE);
        return FALSE;
    }

    return TRUE;
}

static DWCNodeInfo* DWCi_GetPendingNode(void)
{
    return DWCi_GetNodeByProfileID(DWCi_GetMatchCnt()->serverNode.profileID);
}

static void DWCi_AbortMeshNegotiation(void)
{
    DWCi_GetPendingNode()->meshRetries++;
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Mesh NN Count++.\n");
    fn_8046F650();
    DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAITING);
}

static int DWCi_MakeReservationCommand(u32* data)
{
    int length = 0;
    DWCNodeInfo* node = DWCi_GetLocalNode();
    data[length++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->qr2MatchType);
    data[length++] = node->publicIP;
    data[length++] = DWCi_HtoLEl(node->publicPort);
    data[length++] = node->privateIP;
    data[length++] = DWCi_HtoLEl(node->privatePort);
    data[length++] = DWCi_HtoLEl(node->hasPrivateAddress);
    data[length++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->_8C8);
    data[length++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->topology);
    data[length++] = *(const u32*)DWCi_GetMatchCnt()->reservationData;
    DWCi_GetMatchCnt()->reservationRequestID = DWCi_GetMathRand32(0xFFFFFFFE) + 1;
    data[length++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->reservationRequestID);
    data[length++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->meshConnecting ? TRUE : FALSE);
    return length;
}

static BOOL DWCi_IsFriendByIdxList(int profileID)
{
    int i;
    if (!DWCi_GetMatchCnt()->friendList)
        return FALSE;
    for (i = 0; i < DWCi_GetMatchCnt()->friendIdxListLen; i++)
    {
        int friendID = DWCi_GetProfileIDFromList(DWCi_GetMatchCnt()->friendIdxList[i]);
        if (friendID > 0 && friendID == profileID)
            return TRUE;
    }
    return FALSE;
}

static BOOL DWCi_IsMatchCapacityFull(int numConnection, u8 numEntry)
{
    return numEntry + 1 == numConnection;
}

static BOOL DWCi_IsSameGT2Connection(GT2Connection left, GT2Connection right)
{
    return left == right;
}

static BOOL DWCi_IsMeshConnectionPending(int profileID)
{
    DWCNodeInfo* node = DWCi_GetNodeByProfileID(profileID);
    if (!node)
        return FALSE;
    if (node == DWCi_GetLocalNode())
        return FALSE;
    return DWCi_IsSameGT2Connection(NULL, DWCi_GetGT2Connection(node->aid));
}

static u32 DWCi_GetUnusedAID(void)
{
    u32 aid;
    int i;
    if (DWCi_GetNodeCount() == DWC_MAX_CONNECTIONS)
        return 0xFF;
    for (aid = 0; aid < DWC_MAX_CONNECTIONS; aid++)
    {
        for (i = 0; i < DWCi_GetNodeCount(); i++)
            if ((u8)aid == DWCi_GetMatchCnt()->nodeList.nodes[i].aid)
                break;
        if (i == DWCi_GetNodeCount())
            return aid;
    }
    return 0xFF;
}

static void DWCi_CloseAllConnectionsByTimeout(void)
{
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;
    gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
    DWCi_GetMatchCnt()->closeState = 0;
}

static void DWCi_RestartFromTimeout(void)
{
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV
        || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "DWCi_RestartFromTimeout() shouldn't be called.\n");
        return;
    }
    DWCi_CloseAllConnectionsByTimeout();
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Closed all connections and restart matching.\n");
    DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
}

static BOOL DWCi_SendCancelMatchSynCommand(int profileID, u8 command)
{
    u32 senddata;
    int len;
    int result;
    DWCNodeInfo* node;
    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Sent CANCEL SYN %d command to %u.\n",
        command - DWC_MATCH_COMMAND_CANCEL_SYN, profileID);
    if (command == DWC_MATCH_COMMAND_CANCEL_SYN)
    {
        senddata = DWCi_HtoLEl(DWCi_GetMatchCnt()->cbEventPid);
        len = 1;
    }
    else
        len = 0;
    node = DWCi_GetNodeByProfileID(profileID);
    if (!node)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "DWCi_NodeInfoList_GetNodeInfoForProfileID is NULL\n");
        return FALSE;
    }
    result = DWCi_SendMatchCommand(command, profileID,
        node->publicIP, node->publicPort, &senddata, len);
    if (DWCi_HandleMatchCommandError(result))
        return FALSE;
    DWCi_GetMatchCnt()->lastCancelSynSent = DWCi_Np_GetTick();
    return TRUE;
}

static BOOL DWCi_CloseCancelHostAsync(int profileID)
{
    GT2Connection* connection;
    DWCi_GetMatchCnt()->cbEventPid = profileID;
    connection = DWCi_GetGT2ConnectionByProfileID(profileID, FALSE);
    if (connection)
    {
        DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;
        gt2CloseConnectionHard(*connection);
        DWCi_GetMatchCnt()->closeState = 0;
        fn_8046F2C0(profileID);
        return TRUE;
    }
    fn_8046F2C0(profileID);
    return FALSE;
}

static int DWCi_WriteNodePacket(u32* data, int index, const DWCNodeInfo* node)
{
    data[index++] = DWCi_HtoLEl((node->hasPrivateAddress << 16) | node->aid);
    data[index++] = DWCi_HtoLEl(node->profileID);
    data[index++] = node->publicIP;
    data[index++] = node->privateIP;
    data[index++] = DWCi_HtoLEl((node->publicPort << 16) | node->privatePort);
    data[index++] = *(const u32*)node->reservationData;
    return index;
}

static void DWCi_SendTypeMatchAck(void)
{
    int i;
    switch (DWCi_GetMatchCnt()->hostState)
    {
    case 2:
        break;
    default:
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "[WARN] DWCi_SendTypeMatchAck: I'm not server!!\n");
        return;
    }
    for (i = 1; i < DWCi_GetNodeCount(); i++)
        fn_804671A0(DWCi_GetNodeByIndex(i)->aid, DWC_SEND_TYPE_MATCH_ACK);
    if (DWCi_GetMatchCnt()->serverNode.profileID)
        fn_804671A0(DWCi_GetMatchCnt()->serverNode.aid, DWC_SEND_TYPE_MATCH_ACK);
    DWCi_EnterMatchState(DWC_MATCH_STATE_SV_SYN_WAIT);
    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Wait max latency %d msec.\n", DWCi_GetMatchCnt()->baseLatency);
}

static u32 DWCi_GetClientAIDBitmap(BOOL includePending)
{
    u32 bitmap = 0;
    int i;
    int count = DWCi_GetNodeCount();
    for (i = 1; i < count; i++)
        bitmap |= 1 << DWCi_GetNodeByIndex(i)->aid;
    if (includePending)
        bitmap |= 1 << DWCi_GetMatchCnt()->serverNode.aid;
    return bitmap;
}

static void DWCi_FinishCancelMatching(void)
{
    GPResult result = DWCi_GPSetServerStatus(DWC_STATUS_ONLINE);
    if (DWCi_HandleGPError(result))
        return;
    DWCi_CloseMatching();
    DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE, TRUE,
        DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
        DWCi_GetMatchCnt()->cbEventPid ? TRUE : DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_SV,
        DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
        DWCi_GetMatchCnt()->matchedParam);
    DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_INIT;
}

static int DWCi_CancelReservation(void)
{
    int result;
    if (!DWCi_GetMatchCnt()->serverNode.profileID)
        return 0;
    result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_RESV_CANCEL,
        DWCi_GetMatchCnt()->serverNode.profileID,
        DWCi_GetMatchCnt()->serverNode.publicIP, DWCi_GetMatchCnt()->serverNode.publicPort, NULL, 0);
    DWCi_GetMatchCnt()->reqProfileID = 0;
    return result;
}

static int DWCi_InvalidateReservation(void)
{
    u32 senddata = 0;
    int result = 0;
    if (DWCi_GetMatchCnt()->qr2IsReserved && DWCi_GetMatchCnt()->qr2Reservation
        && DWCi_GetMatchCnt()->qr2Reservation != DWCi_GetMatchCnt()->profileID)
    {
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_CANCEL,
            DWCi_GetMatchCnt()->qr2Reservation, DWCi_GetMatchCnt()->serverNode.publicIP,
            DWCi_GetMatchCnt()->serverNode.publicPort, &senddata, 1);
        DWCi_GetMatchCnt()->qr2IsReserved = FALSE;
        DWCi_GetMatchCnt()->qr2Reservation = 0;
        memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
    }
    return result;
}

static void DWCi_AppendNode(const DWCNodeInfo* node)
{
    if (DWCi_GetMatchCnt()->nodeList.count == DWC_MAX_CONNECTIONS)
        return;
    DWCi_GetMatchCnt()->nodeList.nodes[DWCi_GetMatchCnt()->nodeList.count] = *node;
    DWCi_GetMatchCnt()->nodeList.count++;
}

static BOOL DWCi_HasOnlyLocalNode(void)
{
    if (DWCi_GetNodeCount() == 1)
        return TRUE;
    return FALSE;
}

static int DWCi_GetClientLinkProgress(void)
{
    return DWCi_GetMatchCnt()->clLinkProgress;
}

static u8 DWCi_GetClientLinkIndex(u32 nodeCount, int progress)
{
    return nodeCount - 1 - progress;
}

static void DWCi_ResetNodePacketReceiveTicks(void)
{
    int i;
    for (i = 0; i < DWCi_GetNodeCount(); i++)
        fn_804708C0(DWCi_GetNodeByIndex(i)->aid);
}

static void DWCi_ReadNodePacket(DWCNodeInfo* node, const u32* data)
{
    memset(node, 0, sizeof(*node));
    node->aid = DWCi_LEtoHl(data[0]);
    node->hasPrivateAddress = DWCi_LEtoHl(data[0]) >> 16;
    node->profileID = DWCi_LEtoHl(data[1]);
    node->publicIP = data[2];
    node->privateIP = data[3];
    node->publicPort = DWCi_LEtoHl(data[4]) >> 16;
    node->privatePort = DWCi_LEtoHl(data[4]);
    memcpy(node->reservationData, data + 5, sizeof(node->reservationData));
}

static BOOL DWCi_CheckDWCServer(SBServer server)
{
    if (SBServerGetIntValueA(server, "numplayers", -1) == -1)
        return FALSE;
    if (SBServerGetIntValueA(server, "maxplayers", -1) == -1)
        return FALSE;
    if (SBServerGetIntValueA(server, DWC_QR2_MATCH_TYPE_KEY_STR, -1) == -1)
        return FALSE;
    if (SBServerGetIntValueA(server, DWC_QR2_MATCH_VER_KEY_STR, -1) == -1)
        return FALSE;
    return SBServerGetIntValueA(server, DWC_QR2_PID_KEY_STR, 0);
}

static void DWCi_SetServerNode(SBServer server)
{
    memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
    DWCi_GetMatchCnt()->serverNode.profileID = SBServerGetIntValueA(server, DWC_QR2_PID_KEY_STR, 0);
    DWCi_GetMatchCnt()->serverNode.aid = 0xFF;
    DWCi_GetMatchCnt()->serverNode.publicIP = SBServerGetPublicInetAddress(server);
    DWCi_GetMatchCnt()->serverNode.publicPort = SBServerGetPublicQueryPort(server);
    DWCi_GetMatchCnt()->serverNode.privateIP = SBServerGetPrivateInetAddress(server);
    DWCi_GetMatchCnt()->serverNode.privatePort = SBServerGetPrivateQueryPort(server);
    DWCi_GetMatchCnt()->serverNode.hasPrivateAddress = SBServerHasPrivateAddress(server);
}

static void DWCi_ConnectToGroupServer(SBServer server)
{
    DWCMatchControl* control = DWCi_GetMatchCnt();
    int topology = control->topology;
    int profileID = SBServerGetIntValueA(server, DWC_QR2_PID_KEY_STR, 0);
    DWCi_ConnectToGameServerAsync(topology, profileID,
        control->matchedCallback, control->matchedParam,
        control->newClientCallback, control->newClientParam,
        control->reservationCallback, control->reservationData,
        control->reservationParam);
}

static const char* DWCi_GetReservationDenyReason(u8 reason)
{
    switch (reason)
    {
    case 16: return "Game server is fully occupied.";
    case 17: return "This Domain is already closed.";
    case 18: return "The condition was not satisfied.";
    case 19: return "This Domain is already locked.";
    case 20: return "It tried to go to the client for the reservation.";
    case 21: return "It is a reservation to the friend who doesn't exist in the list.";
    case 22: return "It was rejected by the attempt callback.";
    case 23: return "The reservation came from a different other host.";
    case 24: return "Illegal mesh reservation.";
    default: return "";
    }
}

void DWCi_FreeNegotiateList(void)
{
    if (s_nnProcessing == 0)
        NNFreeNegotiateList();
    else
        s_needNNFree = TRUE;
}

BOOL DWC_RegisterMatchStatus(void)
{
    DWC_Printf(4, "!!DWC_RegisterMatchStatus() was called!!\n");

    if (DWCi_GetMatchCnt() == NULL || DWCi_GetMatchCnt()->profileID == 0
        || DWCi_IsError())
    {
        DWC_Printf(4, "But ignored.\n");
        return FALSE;
    }

    if (DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL DWC_CancelMatching(void)
{
    BOOL result;
    if (DWC_IsValidCancelMatching())
    {
        fn_80466160();
        result = TRUE;
    }
    else
    {
        if (!DWCi_GetMatchCnt()->closeAllRequested)
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Now unable to cancel.\n");
        result = FALSE;
    }
    DWCi_GetMatchCnt()->closeAllRequested = 0;
    return result;
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

u8 DWC_AddMatchKeyString(u8 keyID, const char* keyString,
    const char* valueSrc)
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
    stGameMatchKeys[index].isStr = 1;
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
        "DWC_AddMatchKeyString: key='%s' value='%s'\n", keyString, valueSrc);

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

BOOL DWC_RequestSuspendMatchAsync(BOOL suspend,
    DWCSuspendCallback callback, void* param)
{
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
        "!!DWC_RequestSuspendMatchAsync(%s) was called!!\n", suspend ? "TRUE" : "FALSE");
    return DWCi_RequestSuspendMatch(suspend, callback, param);
}

BOOL fn_80456F10(void)
{
    int suspend = -1;
    u32 bitmap;
    if (!DWCi_GetMatchCnt())
        return FALSE;
    if (!DWCi_GetMatchCnt()->hostState)
        return FALSE;
    bitmap = DWCi_GetMatchCnt()->_01C;
    bitmap &= DWC_GetAIDBitmap();
    if (!DWCi_IsReadyToSuspend())
        bitmap &= ~(1 << DWC_GetMyAID());
    if ((bitmap ^ DWC_GetAIDBitmap()) == 0)
        suspend = TRUE;
    else if ((bitmap & DWC_GetAIDBitmap()) == 0)
        suspend = FALSE;
    if (suspend != -1)
        DWCi_GetMatchCnt()->_028 = suspend == TRUE;
    return DWCi_GetMatchCnt()->_028;
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
    DWCi_GetMatchCnt()->serverDownAckBitmap = 0;
    DWCi_GetMatchCnt()->serverDownBaseBitmap = 0;
    DWCi_GetMatchCnt()->groupID = 0;
    DWCi_GetMatchCnt()->_8C4 = 0;
    DWCi_GetMatchCnt()->_8C8 = 0;
    DWCi_GetMatchCnt()->qr2MatchType = 0;
    DWCi_GetMatchCnt()->hostState = 0;
    DWCi_GetMatchCnt()->sbUpdateRequestTick = 0;
    DWCi_GetMatchCnt()->serverPollingUID = 0;
    DWCi_GetMatchCnt()->_908 = 1;
    DWCi_GetMatchCnt()->closeAllRequested = 0;
    DWCi_GetMatchCnt()->_8E6 = 0;
    DWC_Printf(4, "ServerBrowserLimitUpdate timeout reset.(%s)\n", __FUNCTION__);
    DWCi_GetMatchCnt()->topology = 0;
    DWCi_ClearNodeList();
    memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
    DWCi_ClearGameMatchKeys();
    DWCi_ResetMatchParam(DWC_MATCH_RESET_ALL);
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
    fn_8046F930();
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

    return qr2Error;
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
    DWCi_SetGroupID(DWCi_GenerateGroupID());
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
        // Retail initializes this local descriptor without subsequently using it.
        memset(&node, 0, sizeof(node));
        memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
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

void DWCi_MatchProcess(BOOL fullSpec)
{
    u32 sbInterval;
    int result;
    SBError sbError;

    if (!DWCi_GetMatchCnt() || DWCi_IsError())
        return;
    if (!fullSpec)
    {
        if (DWCi_GetMatchCnt()->qr2Obj)
            qr2_think(DWCi_GetMatchCnt()->qr2Obj);
        if (DWCi_GetMatchCnt()->pGt2Socket)
            gt2Think(*DWCi_GetMatchCnt()->pGt2Socket);
        return;
    }
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_INIT)
        return;

    switch (DWCi_GetMatchCnt()->state)
    {
    case DWC_MATCH_STATE_CL_WAIT_RESV:
        if (DWCi_GetMatchCnt()->cmdTimeoutTime > 0
            && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->cmdTimeoutStartTick)
                > DWCi_GetMatchCnt()->cmdTimeoutTime)
        {
            DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
            if (DWCi_GetMatchCnt()->_8C8)
            {
                if (DWCi_GetMatchCnt()->_8E6)
                    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                        "GroupID Timeout: wait server response %d/%d.\n",
                        DWCi_GetMatchCnt()->scResvRetryCount, DWCi_GetMatchCnt()->_8E6);
                DWCi_GetMatchCnt()->scResvRetryCount++;
                if (DWCi_GetMatchCnt()->scResvRetryCount > DWCi_GetMatchCnt()->_8E6)
                {
                    DWCi_StopMatching((DWCError)24, -80432);
                    return;
                }
                DWCi_RetryReserving(0);
                return;
            }
            if (fn_8046F650() == 1)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Mesh NN resv timed out.\n");
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
                break;
            }
            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
            {
                if (DWCi_GetMatchCnt()->_8E6)
                    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                        "Timeout: wait server response %d/%d.\n",
                        DWCi_GetMatchCnt()->scResvRetryCount, DWCi_GetMatchCnt()->_8E6);
                DWCi_GetMatchCnt()->scResvRetryCount++;
                if (DWCi_GetMatchCnt()->scResvRetryCount > DWCi_GetMatchCnt()->_8E6)
                {
                    DWCi_StopMatching((DWCError)24, -80430);
                    return;
                }
                result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->serverNode.profileID, FALSE);
                if (DWCi_HandleMatchCommandError(result))
                    return;
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                    "<FRIEND> NN resv(with %u) timed out. Try next server.\n",
                    DWCi_GetMatchCnt()->serverNode.profileID);
                memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                if (!DWCi_RetryReserving(0))
                    return;
            }
            else
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                    "NN resv(with %u) timed out. Try next server.\n",
                    DWCi_GetMatchCnt()->serverNode.profileID);
                memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                if (!DWCi_RetryReserving(0))
                    return;
            }
        }
        if (DWCi_GetMatchCnt()->cmdResendFlag
            && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->cmdResendTick)
                >= DWC_MATCH_CMD_RESEND_INTERVAL_MSEC)
        {
            result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->serverNode.profileID, FALSE);
            if (DWCi_HandleMatchCommandError(result))
                return;
        }
        break;

    case DWC_MATCH_STATE_CL_SEARCH_HOST:
    case DWC_MATCH_STATE_CL_SEARCH_EVAL_HOST:
    case DWC_MATCH_STATE_SEARCH_OWN:
        if (DWCi_GetMatchCnt()->sbUpdateFlag <= 0)
            break;
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_HOST)
        {
            if (DWCi_GetMatchCnt()->sbUpdateFlag == 1 || DWCi_GetMatchCnt()->_8DC <= 1)
                sbInterval = 4000;
            else
                sbInterval = 12000;
        }
        else if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SEARCH_OWN)
            sbInterval = 4000;
        else
            sbInterval = DWCi_GetMatchCnt()->sbUpdateFlag == 1 ? 4000 : 12000;
        if (OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->sbUpdateTick) > sbInterval)
        {
            sbError = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->reqProfileID);
            if (DWCi_HandleSBError(sbError))
                return;
            DWCi_GetMatchCnt()->sbUpdateFlag = 0;
        }
        break;

    case DWC_MATCH_STATE_CL_GT2:
        if (DWCi_GetMatchCnt()->nnFinishTime)
        {
            if (OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->nnFinishTime)
                > DWC_WAIT_GT2_CONNECT_TIMEOUT)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout: wait gt2Connect().\n");
                DWCi_GetMatchCnt()->nnFinishTime = 0;
                if (fn_8046F650() == 1)
                {
                    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Mesh NN resv timed out.\n");
                    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
                    break;
                }
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->serverNode.profileID))
                {
                    memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                    return;
                }
            }
        }
        else if (DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_TELL_ADDR
            && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->cmdCnt.sendTime)
                > DWC_MATCH_CMD_RTT_TIMEOUT)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "RTT Timeout with DWC_MATCH_STATE_CL_GT2.\n");
            if (fn_8046F650() == 1)
            {
                DWCi_StopResendingMatchCommand();
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Stop resending command %d.\n", DWC_MATCH_COMMAND_TELL_ADDR);
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Mesh NN resv timed out.\n");
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
                break;
            }
            DWCi_GetMatchCnt()->cmdCnt.count++;
            if (DWCi_GetMatchCnt()->cmdCnt.count > DWC_MATCH_CMD_RETRY_MAX)
            {
                DWCi_StopResendingMatchCommand();
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Stop resending command %d.\n", DWC_MATCH_COMMAND_TELL_ADDR);
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->serverNode.profileID))
                {
                    memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                    return;
                }
                break;
            }
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_TELL_ADDR,
                DWCi_GetMatchCnt()->cmdCnt.profileID, DWCi_GetMatchCnt()->cmdCnt.ip,
                DWCi_GetMatchCnt()->cmdCnt.port, DWCi_GetMatchCnt()->cmdCnt.data,
                DWCi_GetMatchCnt()->cmdCnt.len);
            if (DWCi_HandleMatchCommandError(result))
                return;
        }
        break;

    case DWC_MATCH_STATE_SV_OWN_NN:
        if (DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_RESV_OK
            && ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY
                    && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->cmdCnt.sendTime)
                        > DWC_MATCH_RESV_KEEP_TIME_ANYBODY)
                || (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_ANYBODY
                    && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->cmdCnt.sendTime)
                        > DWC_MATCH_RESV_KEEP_TIME_FRIEND)))
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Reservation timeout. Cancel reservation.\n");
            DWCi_StopResendingMatchCommand();
            if (fn_8046F650() == 1)
            {
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
                break;
            }
            if (!DWCi_CancelPendingServer(DWCi_GetMatchCnt()->serverNode.profileID))
                return;
        }
        break;

    case DWC_MATCH_STATE_SV_WAIT_CL_LINK:
        if (DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_LINK_CLS_REQ
            && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->cmdCnt.sendTime)
                > DWC_MATCH_LINK_CLS_TIMEOUT)
        {
            DWCi_GetMatchCnt()->cmdCnt.count++;
            if (DWCi_GetMatchCnt()->cmdCnt.count > 0)
            {
                DWCi_StopResendingMatchCommand();
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Wait clients connecting timeout.\n");
                if (!DWCi_CancelPendingServer(DWCi_GetMatchCnt()->serverNode.profileID))
                    return;
            }
        }
        break;

    case DWC_MATCH_STATE_CL_WAITING:
        if (DWC_GetState() == DWC_STATE_MATCHING && DWCi_GetMatchCnt()->_757
            && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->clWaitTime)
                > DWC_MATCH_CL_WAIT_TIMEOUT)
        {
            DWCNodeInfo* server = &DWCi_GetMatchCnt()->serverNode;
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "No data from server %d/%d.\n",
                DWCi_GetMatchCnt()->clWaitTimeoutCount, DWC_MATCH_CMD_RETRY_MAX);
            if (DWCi_GetMatchCnt()->clWaitTimeoutCount >= DWC_MATCH_CMD_RETRY_MAX)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout: Connection to server was shut down.\n");
                if (!DWCi_CancelPreConnectedClientProcess(server->profileID))
                    return;
            }
            else
            {
                result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_CL_WAIT_POLL,
                    server->profileID, server->publicIP, server->publicPort, NULL, 0);
                if (DWCi_HandleMatchCommandError(result))
                    return;
                DWCi_GetMatchCnt()->clWaitTimeoutCount++;
                DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick()
                    - DWCi_Np_MilliSecondsToTicks(DWC_MATCH_CL_WAIT_TIMEOUT - DWC_MATCH_CMD_RTT_TIMEOUT);
            }
        }
        break;

    case DWC_MATCH_STATE_CL_SVDOWN_1:
    {
        DWCMatchControl* control;
        fn_8046F650();
        control = DWCi_GetMatchCnt();
        control->serverDownAckBitmap = 0;
        control->backupRetryCount = 0;
        control->_8CD = 0;
        if (!DWCi_IsBackupMine()
            && !DWCi_GetGT2Connection(DWCi_GetNodeByIndex(0)->aid))
        {
            DWC_Printf(DWC_REPORTFLAG_INFO,
                "[shutdown myself] It doesn't have the GT2 connection with next server.\n");
            DWCi_StopMatching((DWCError)24, -80431);
            return;
        }
        control->serverDownAckBitmap |= 1 << DWCi_GetNodeAIDFromProfileID(control->profileID);
        if (DWCi_IsBackupMine())
        {
            if (DWCi_GetMatchCnt()->topology == 1)
            {
                DWCi_StopMatching((DWCError)24, -80431);
                break;
            }
            DWC_Printf(DWC_REPORTFLAG_INFO, "Reserve: I am Next Server!!\n");
            fn_8046DB10();
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SVDOWN_2);
            qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
            DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
            DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick();
        }
        else
        {
            qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SVDOWN_2);
            DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
            DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick();
        }
        break;
    }

    case DWC_MATCH_STATE_CL_SVDOWN_2:
        if (DWCi_IsBackupMine())
        {
            if (OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->clWaitTime) > 5000)
            {
                fn_8046DB10();
                if (++DWCi_GetMatchCnt()->backupRetryCount > 5)
                {
                    u32 missing = ~(DWCi_GetMatchCnt()->serverDownAckBitmap
                        & DWCi_GetMatchCnt()->serverDownBaseBitmap);
                    int i;
                    for (i = 0; i < DWCi_GetNodeCount(); i++)
                    {
                        DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
                        if (missing & (1 << node->aid))
                        {
                            DWC_Printf(DWC_REPORTFLAG_INFO,
                                "**** [SV Changing] shutdown no response host(PID:%u aid:%d)\n",
                                node->profileID, node->aid);
                            DWCi_CloseConnectionHard(node->aid);
                        }
                    }
                    DWCi_GetMatchCnt()->serverDownAckBitmap = DWCi_GetMatchCnt()->serverDownBaseBitmap;
                }
            }
            if ((DWCi_GetMatchCnt()->serverDownAckBitmap ^ DWCi_GetMatchCnt()->serverDownBaseBitmap) == 0)
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SVDOWN_3);
        }
        else if (OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->clWaitTime) > 60000)
        {
            DWC_Printf(DWC_REPORTFLAG_INFO, "Backup message timed out.\n");
            DWCi_StopMatching((DWCError)24, -80431);
        }
        break;

    case DWC_MATCH_STATE_CL_SVDOWN_3:
        if (DWCi_IsBackupMine())
        {
            if (DWCi_GetMatchCnt()->topology == 1)
            {
                DWCi_StopMatching((DWCError)24, -80431);
                break;
            }
            DWC_Printf(DWC_REPORTFLAG_INFO, "Server Down Detected!!\n");
            fn_8046DCC0(0);
            DWC_Printf(DWC_REPORTFLAG_INFO, "I am New Server!!\n");
            DWCi_GetMatchCnt()->_8D0 = 0;
            DWCi_ResetNodePacketReceiveTicks();
            DWCi_AutoSuspendComplete();
            qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
        }
        else if (OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->clWaitTime) > 20000)
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
        break;

    case DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST:
        if (DWCi_GetMatchCnt()->sbUpdateFlag > 0
            && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->sbUpdateTick) > 8000)
        {
            if (++DWCi_GetMatchCnt()->_8C4 > 15)
            {
                DWCi_StopMatching((DWCError)24, -80432);
                break;
            }
            DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "Retry search host by groupID[%d/%d].\n",
                DWCi_GetMatchCnt()->_8C4, 15);
            if (DWCi_HandleSBError(DWCi_SBUpdateAsync(0)))
                return;
            DWCi_GetMatchCnt()->sbUpdateFlag = 0;
        }
        break;
    default:
        break;
    }

    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN
            || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_NN)
        && DWCi_GetMatchCnt()->nnFailedTime
        && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->nnFailedTime) > DWC_WAIT_NN_RETRY_TIMEOUT)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout : wait NN retry.\n");
        DWCi_NNCompletedCallback(nr_deadbeatpartner, NULL, NULL, &DWCi_GetMatchCnt()->nnInfo);
    }

    if (DWCi_GetMatchCnt()->topology == 0 && !DWCi_GetMatchCnt()->meshConnecting
        && DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING
        && !DWCi_GetMatchCnt()->newClientNode.profileID && DWCi_GetNodeCount() > 2)
    {
        int i;
        for (i = 1; i < DWCi_GetNodeCount(); i++)
        {
            DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
            if (DWCi_GetLocalNode() == node
                || DWCi_GetGT2Connection(node->aid))
                continue;
            if (!node->connectionDeadline)
                node->connectionDeadline = fn_8046F4A0();
            if (DWCi_Np_GetTick() > node->connectionDeadline && node->meshRetries <= dwcMeshRetryLimit)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** TRY mesh create: aid(%d)\n", node->aid);
                node->connectionDeadline = fn_8046F4A0();
                DWCi_GetMatchCnt()->serverNode = *node;
                DWCi_GetMatchCnt()->meshConnecting = 1;
                result = DWCi_SendResvCommand(node->profileID, FALSE);
                if (DWCi_HandleMatchCommandError(result))
                {
                    DWCi_GetMatchCnt()->meshConnecting = 0;
                    memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                    break;
                }
                DWCi_GetMatchCnt()->meshState = 0;
                memset(&DWCi_GetMatchCnt()->meshNode, 0, sizeof(DWCi_GetMatchCnt()->meshNode));
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
                break;
            }
        }
    }

    if (DWCi_GetMatchCnt()->sbObj)
    {
        s_sbCallbackLevel = 0;
        s_needSbFree = FALSE;
        ServerBrowserThink(DWCi_GetMatchCnt()->sbObj);
        if (s_needSbFree)
        {
            ServerBrowserFree(DWCi_GetMatchCnt()->sbObj);
            DWCi_GetMatchCnt()->sbObj = NULL;
        }
        if (DWCi_GetMatchCnt()->sbObj
            && ServerBrowserState(DWCi_GetMatchCnt()->sbObj) != sb_disconnected
            && DWCi_GetMatchCnt()->sbUpdateRequestTick
            && DWCi_Np_GetTick() > DWCi_GetMatchCnt()->sbUpdateRequestTick)
        {
            DWCi_StopMatching(DWC_ERROR_NETWORK,
                DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_SB + DWC_ECODE_TYPE_NETWORK);
            DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "ServerBrowserLimitUpdate timeout.\n");
        }
    }
    DWCi_SendStateChanged(DWCi_GetMatchCnt()->qr2Obj);
    s_needNNFree = FALSE;
    s_nnProcessing = 1;
    NNThink();
    s_nnProcessing = 0;
    if (s_needNNFree == TRUE)
    {
        s_needNNFree = FALSE;
        NNFreeNegotiateList();
    }
    if (DWCi_GetMatchCnt()->pGt2Socket)
        gt2Think(*DWCi_GetMatchCnt()->pGt2Socket);
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_WAIT_CLOSE
        && OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->closedTime)
            > (DWC_MATCH_CMD_RTT_TIMEOUT >> 1))
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "RTT Timeout with DWCi_MatchProcess.\n");
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout : Wait prior profileID.\n");
        DWCi_GetMatchCnt()->qr2IsReserved = 0;
        DWCi_GetMatchCnt()->qr2Reservation = 0;
        DWCi_GetMatchCnt()->closeState = 0;
        DWCi_RestartFromCancel(1);
    }
    fn_8046EC30();
    fn_8046E040(0);
    if (!fn_80467570())
        return;
    if (!fn_804681D0())
        return;
}

GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket, unsigned int ip,
    unsigned short port, GT2Byte* message, int len)
{
    struct sockaddr_in saddr;

    if (!len || !message)
    {
        return GT2False;
    }

    DWCi_Np_CpuClear32(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.addr = ip;
    saddr.sin_port = SOHtoNs(port);

    if ((message[0] == QR_MAGIC_1 && message[1] == QR_MAGIC_2)
        || message[0] == '\\')
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
            "GT2 Unrecognized : Received QR2 data.\n");
        if (DWCi_GetMatchCnt()->qr2Obj)
        {
            qr2_parse_queryA(DWCi_GetMatchCnt()->qr2Obj, (char*)message, len,
                (struct sockaddr*)&saddr);
        }
        else
        {
            DWC_Printf(
                DWC_REPORTFLAG_MATCH_NN, "  ignore qr2 message.\n");
        }
    }
    else if (!memcmp(message, NNMagicData, NATNEG_MAGIC_LEN))
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
            "GT2 Unrecognized : Received NN data.\n");
        NNProcessData((char*)message, len, &saddr);
    }
    else if (message[0] == 0xFE && message[1] == 0xFE)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
            "GT2 Unrecognized : Not Connected gt2 data.\n");
        return GT2False;
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING,
            "GT2 Unrecognized : Received unrecognized data.\n");
        return GT2False;
    }

    return GT2True;
}

void DWCi_GT2ConnectAttemptCallback(GT2Socket socket,
    GT2Connection connection, unsigned int ip, unsigned short port,
    int latency, GT2Byte* message, int len)
{
    int index;
    GT2Bool result;
    GT2Connection* pGt2Con;
    DWCConnectionInfo* pConInfo;
    int profileID = 0;

    if (DWCi_GetMatchCnt() && DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_NN)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
            "DWCi_GT2ConnectAttemptCallback in DWC_MATCH_STATE_CL_NN\n");
        return;
    }
    if (!DWCi_GetMatchCnt() || DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_GT2
        || DWCi_GetMatchCnt()->cancelState != DWC_MATCH_CANCEL_STATE_INIT)
    {
        gt2Reject(connection, (const GT2Byte*)"Init state", -1);
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Reject was called : Init state\n");
        return;
    }
    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
        "Got DWCi_GT2ConnectAttemptCallback latency(%d) msg(%s)\n", latency, message);
    if (message && len)
        profileID = strtoul((const char*)message, NULL, 10);
    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "remote PID = %d\n", profileID);
    index = DWCi_GT2GetConnectionListIdx();
    if (index == -1)
    {
        gt2Reject(connection, (const GT2Byte*)"Server full", -1);
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Reject was called : Server full\n");
        if (fn_8046F650() == 1)
        {
            DWCi_StopResendingMatchCommand();
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Mesh stop.\n");
            DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAITING);
            return;
        }
        DWCi_StopMatching(DWC_ERROR_NETWORK,
            DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_CONN_OVER);
        return;
    }
    if (ip != DWCi_GetMatchCnt()->serverNode.connectionIP
        || SONtoHs(port) != DWCi_GetMatchCnt()->serverNode.connectionPort)
    {
        if (profileID == DWCi_GetMatchCnt()->serverNode.profileID)
        {
            DWC_Printf(DWC_REPORTFLAG_WARNING, "gt2Connect() came before NN complete.\n");
            DWCi_GetMatchCnt()->serverNode.connectionIP = ip;
            DWCi_GetMatchCnt()->serverNode.connectionPort = SONtoHs(port);
        }
        else
        {
            gt2Reject(connection, (const GT2Byte*)"Unknown connect attempt", -1);
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                "gt2Reject was called : Unknown connect attempt from %s\n",
                gt2AddressToString(ip, port, NULL));
            return;
        }
    }
    DWCi_GetMatchCnt()->nnFinishTime = 0;
    result = gt2Accept(connection, DWCi_GetMatchCnt()->gt2Callbacks);
    if (result == GT2False)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "Unexpected failure to gt2Accept.\n");
        DWCi_StopMatching(DWC_ERROR_NETWORK, DWC_ECODE_SEQ_MATCH + DWC_ECODE_TYPE_OTHER);
        return;
    }
    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
        "Accepted connection from %s (latency %d)\n",
        gt2AddressToString(ip, port, NULL), latency);
    DWCi_StopResendingMatchCommand();
    if (!DWCi_GetMatchCnt()->meshConnecting && DWCi_GetNodeCount() == 1)
        DWCi_GetMatchCnt()->baseLatency = (u16)(((latency >> 1) < 0xFFFF) ? (latency >> 1) : 0xFFFF);
    pGt2Con = DWCi_GetGT2ConnectionByIdx(index);
    pConInfo = DWCi_GetConnectionInfoByIdx(index);
    *pGt2Con = connection;
    pConInfo->index = (u8)index;
    pConInfo->aid = DWCi_GetMatchCnt()->serverNode.aid;
    pConInfo->profileID = DWCi_GetMatchCnt()->serverNode.profileID;
    fn_804708C0(pConInfo->aid);
    gt2SetConnectionData(connection, pConInfo);
    DWCi_PostProcessConnection(DWC_PP_CONNECTION_CL_GT2_ACCEPT);
}

void DWCi_GT2ConnectedCallback(GT2Connection connection, GT2Result result,
    GT2Byte* message, int len)
{
    char pidStr[24];
    int index;
    GT2Connection* pGt2Con;
    GT2Result gt2Result;
    DWCConnectionInfo* pConInfo;

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
        "DWCi_GT2ConnectedCallback(port=%d,result=%d,msg=[%s])\n",
        gt2GetRemotePort(connection), result, message ? (char*)message : "");
    if (!DWCi_GetMatchCnt()
        || (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_GT2
            && DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_GT2))
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
            "gt2ConnectedCallback: Already cancel(port=%d,result=%d,msg=[%s]).\n",
            gt2GetRemotePort(connection), result, message ? (char*)message : "");
        if (DWCi_GetMatchCnt()->meshState == DWC_MATCH_STATE_SV_OWN_GT2)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                "gt2ConnectedCallback: mesh creating.. go next process.\n");
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                "@@@@@ gt2ConnectedCallback: but ignored.\n");
            return;
        }
    }
    if (result != GT2Success)
    {
        if (!message)
            message = (GT2Byte*)"";
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "GT2 connect failed %d: %s\n", result, message);
        if (fn_8046F650() == 1)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Mesh NN resv timed out.\n");
            DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAITING);
            return;
        }
        if (DWCi_GetMatchCnt()->meshState == DWC_MATCH_STATE_SV_OWN_GT2)
        {
            DWCi_GetMatchCnt()->meshState = DWC_MATCH_STATE_INIT;
            memset(&DWCi_GetMatchCnt()->meshNode, 0, sizeof(DWCi_GetMatchCnt()->meshNode));
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Mesh gt2 failed.\n");
            return;
        }
        if (result == GT2DuplicateAddress)
        {
            return;
        }
        else if (result == GT2TimedOut)
        {
            DWCi_GetMatchCnt()->gt2ConnectCount++;
            if (DWCi_GetMatchCnt()->gt2ConnectCount > DWC_MATCH_CMD_RETRY_MAX)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Connect() retry over.\n");
                DWCi_GetMatchCnt()->gt2ConnectCount = 0;
                DWCi_CancelPendingServer(DWCi_GetMatchCnt()->serverNode.profileID);
                return;
            }
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Retry to gt2Connect.\n");
            snprintf(pidStr, sizeof(pidStr), "%u", DWCi_GetMatchCnt()->profileID);
            gt2Result = gt2Connect(*DWCi_GetMatchCnt()->pGt2Socket, NULL,
                gt2AddressToString(DWCi_GetMatchCnt()->serverNode.connectionIP,
                    DWCi_GetMatchCnt()->serverNode.connectionPort, NULL),
                (GT2Byte*)pidStr, -1, DWC_GT2_CONNECT_TIMEOUT,
                DWCi_GetMatchCnt()->gt2Callbacks, GT2False);
            if (gt2Result == GT2OutOfMemory)
            {
                fn_80468E40(gt2Result);
                return;
            }
            else if (gt2Result != GT2Success)
            {
                if (!DWCi_CancelPendingServer(DWCi_GetMatchCnt()->serverNode.profileID))
                    return;
            }
        }
        else if (!DWCi_CancelPendingServer(DWCi_GetMatchCnt()->serverNode.profileID))
        {
            return;
        }
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "GT2 connected.\n");
        index = DWCi_GT2GetConnectionListIdx();
        if (index == -1)
        {
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                "Don't continue matching without closing connections!!\n");
            DWCi_StopMatching(DWC_ERROR_NETWORK,
                DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_CONN_OVER);
            return;
        }
        pGt2Con = DWCi_GetGT2ConnectionByIdx(index);
        pConInfo = DWCi_GetConnectionInfoByIdx(index);
        *pGt2Con = connection;
        pConInfo->index = (u8)index;
        if (DWCi_GetMatchCnt()->meshState != DWC_MATCH_STATE_SV_OWN_GT2)
        {
            pConInfo->aid = DWCi_GetMatchCnt()->serverNode.aid;
            pConInfo->profileID = DWCi_GetMatchCnt()->serverNode.profileID;
        }
        else
        {
            pConInfo->aid = DWCi_GetMatchCnt()->meshNode.aid;
            pConInfo->profileID = DWCi_GetMatchCnt()->meshNode.profileID;
        }
        fn_804708C0(pConInfo->aid);
        gt2SetConnectionData(connection, pConInfo);
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2
            || DWCi_GetMatchCnt()->meshState == DWC_MATCH_STATE_SV_OWN_GT2)
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SV_CONNECT);
        else
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_CL_GT2_CONNECT);
    }
}

void DWCi_MatchGPRecvBuddyMsgCallback(GPConnection* connection, u32 profileID, char* message)
{
    u32 data[DWC_MATCH_COMMAND_ADD_MESSAGE_MAX / sizeof(u32)];
    int length;
    int words;
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
        "<GP> RECV-0x%02x <- [--------:-----] [pid=%u]\n", (u8)message[0], profileID);
    length = strlen(message) - 1;
    if (!length)
        words = 0;
    else
    {
        words = DWC_Base64Decode(message + 1, length, (char*)data, sizeof(data));
        if (words == -1 || words % 4 != 0 || words > DWC_MATCH_COMMAND_ADD_MESSAGE_MAX)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore illegal data(%d,%d).\n", length, words);
            return;
        }
        words /= 4;
    }
    fn_8045FB60((u8)message[0], profileID, 0, 0, data, words);
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

void fn_8045C740(u8 aid, u16 type, u8* data)
{
    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
        "Received SYN %d packet from aid %d.\n", type - DWC_SEND_TYPE_MATCH_SYN, aid);
    fn_8045C7B0(aid, type, data);
}

typedef struct DWCNodePacketView
{
    const u32* words;
} DWCNodePacketView;

static DWCNodePacketView DWCi_GetMatchAckNodeView(const u32* packet, int count, int index)
{
    DWCNodePacketView view;
    view.words = packet + (count - 1 - index) * 6 + 1;
    return view;
}

void fn_8045C7B0(u8 aid, u16 type, u8* data)
{
    int i;
    int count;
    u32* packet = (u32*)data;
    switch (type)
    {
    case DWC_SEND_TYPE_MATCH_SYN:
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING)
            return;
        if (DWCi_LEtoHl(packet[0]) == 1)
        {
            DWCi_GetMatchCnt()->cbEventPid = 0;
            DWCi_GetMatchCnt()->newClientNode.profileID = 0;
            DWCi_GetMatchCnt()->serverPollingUID = DWCi_LEtoHl(packet[1]);
            if (DWCi_GetMatchCnt()->topology == 2)
                DWCi_PrependNode(&DWCi_GetMatchCnt()->serverNode);
        }
        else
        {
            DWCNodeInfo node;
            DWCi_ReadNodePacket(&node, packet + 1);
            DWCi_GetMatchCnt()->newClientNode = node;
            DWCi_GetMatchCnt()->serverPollingUID = DWCi_LEtoHl(packet[7]);
            DWCi_GetMatchCnt()->cbEventPid = node.profileID;
        }
        DWCi_GetMatchCnt()->clWaitTime = DWCi_GetNodeCount() == 1 ? 90000 : DWCi_GetNodeCount() * 12000;
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "SYN wait time = %d\n", DWCi_GetMatchCnt()->clWaitTime);
        DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_SYN);
        fn_804671A0(aid, DWC_SEND_TYPE_MATCH_SYN_ACK);
        break;
    case DWC_SEND_TYPE_MATCH_SYN_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN)
        {
            u16 latency = (u16)(((const u8*)packet)[0] | (((const u8*)packet)[1] << 8));
            u32 bitmap;
            DWCi_GetMatchCnt()->synAckBit |= 1 << aid;
            if (latency > DWCi_GetMatchCnt()->baseLatency)
                DWCi_GetMatchCnt()->baseLatency = latency;
            bitmap = DWCi_GetClientAIDBitmap(TRUE);
            if (DWCi_GetMatchCnt()->synAckBit == bitmap)
            {
                DWCi_GetMatchCnt()->serverPollingUID = DWCi_GetMathRand32(0xFFFFFFFE) + 1;
                DWCi_SendTypeMatchAck();
                DWCi_GetMatchCnt()->synResendCount = 0;
                DWCi_GetMatchCnt()->lastSynSent = DWCi_Np_GetTick();
            }
        }
        else
            fn_804671A0(aid, DWC_SEND_TYPE_MATCH_ACK);
        break;
    case DWC_SEND_TYPE_MATCH_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SYN)
        {
            count = DWCi_LEtoHl(packet[0]);
            if (count)
            {
                for (i = 0; i < count; i++)
                {
                    DWCNodeInfo node;
                    DWCNodePacketView view = DWCi_GetMatchAckNodeView(packet, count, i);
                    data = (u8*)view.words;
                    DWCi_ReadNodePacket(&node, (const u32*)data);
                    DWCi_PrependNode(&node);
                }
                DWCi_GetMatchCnt()->serverPollingUID = DWCi_LEtoHl((packet + count * 6)[1]);
                DWCi_PrependNode(&DWCi_GetMatchCnt()->serverNode);
                memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
            }
            else
                DWCi_GetMatchCnt()->serverPollingUID = DWCi_LEtoHl(packet[1]);
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SYN_FINISH);
        }
        break;
    }
}

void fn_8045DBB0(const u32* data)
{
    u32 profileID;
    u32 bitmap;
    u32 pollingID;
    DWCNodeInfo* server;
    profileID = DWCi_LEtoHl(data[0]);
    bitmap = DWCi_LEtoHl(data[1]);
    pollingID = DWCi_LEtoHl(data[2]);
    DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
        "Received Polling packet from pid %d(%08x).\n", profileID, bitmap);
    if (fn_80454BB0())
    {
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "I'm a server. ignore.\n");
        return;
    }
    if (pollingID != DWCi_GetMatchCnt()->serverPollingUID)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "ignore serverPollingUID(%d).\n", pollingID);
        return;
    }
    server = DWCi_GetFirstNode();
    if (!server || server->profileID != profileID)
    {
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "command from wrong server. ignore.\n");
        return;
    }
    DWCi_CloseUnreportedConnections(bitmap);
}

BOOL DWCi_ProcessMatchClosing(DWCError error, int errorCode, int profileID)
{
    (void)profileID;
    if (DWC_GetState() != DWC_STATE_MATCHING)
        return FALSE;
    if (DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_SV)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_ProcessMatchClosing: SC_SV.\n");
        return TRUE;
    }
    if (error)
    {
        DWCi_StopMatching(error, errorCode + DWC_ECODE_SEQ_MATCH);
        return TRUE;
    }
    if (DWCi_GetMatchCnt()->cancelState == DWC_MATCH_CANCEL_STATE_EXEC
        || DWCi_GetMatchCnt()->closeState == DWC_MATCH_CLOSE_STATE_TIMEOUT)
        return TRUE;
    if (DWCi_GetMatchCnt()->nnInfo.cookie)
    {
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }
    if (DWCi_GetNodeCount() > 1)
    {
        if (!DWCi_GetMatchCnt()->closeState)
        {
            DWCi_GetMatchCnt()->closeState = 2;
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        }
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
        DWCi_StopMatching((DWCError)24, DWC_ECODE_SEQ_MATCH + DWC_ECODE_TYPE_SC_CL_FAIL);
    else if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Wait prior profileID.\n");
        DWCi_EnterMatchState(DWC_MATCH_STATE_WAIT_CLOSE);
        DWCi_GetMatchCnt()->closedTime = DWCi_Np_GetTick();
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Restart matching immediately.\n");
        DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
    }
    return TRUE;
}

void DWCi_ProcessMatchSCClosing(int profileID)
{
    if (DWCi_GetMatchCnt()->closeState != DWC_MATCH_CLOSE_STATE_TIMEOUT)
        DWCi_CancelPendingServer(profileID);
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

void DWCi_ShutdownMatch(void)
{
    stpMatchCnt = NULL;
    stStateChangedTick = OSGetTime();
    if (stpAddFilter != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }
    DWCi_ClearGameMatchKeys();
}

BOOL DWCi_IsShutdownMatch(void)
{
    return stpMatchCnt == NULL;
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
    DWCi_GetMatchCnt()->meshConnecting = 0;
    DWCi_GetMatchCnt()->meshState = 0;
    memset(&DWCi_GetMatchCnt()->meshNode, 0, sizeof(DWCi_GetMatchCnt()->meshNode));

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
        DWCi_GetMatchCnt()->serverDownAckBitmap = 0;
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
        DWCi_GetMatchCnt()->newClientNode.profileID = 0;
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
    DWCi_GetMatchCnt()->nnFailureCount = 0;
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
    DWCi_GetMatchCnt()->suspendRetryInterval = 0;
    DWCi_GetMatchCnt()->reservationRequestID = 0;
}

void DWCi_CloseMatching(void)
{
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, " Close Match....\n");
    if (stpMatchCnt == NULL)
        return;
    if (stpMatchCnt->sbObj != NULL)
    {
        if (s_sbCallbackLevel == 0)
        {
            ServerBrowserFree(stpMatchCnt->sbObj);
            stpMatchCnt->sbObj = NULL;
        }
        else
            s_needSbFree = TRUE;
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
            "ServerBrowserLimitUpdate timeout set.(%s)\n", __FUNCTION__);
    }
    return sbError;
}

NegotiateError fn_8045F330(int isQR2, int cookie, DWCNodeInfo* node)
{
    u32 ip;
    u32 senddata[2];
    int result;
    BOOL nnValid;
    NegotiateError nnError = ne_noerror;
    if (!isQR2)
    {
        cookie = (DWCi_GetMatchCnt()->profileID & 0xFFFF)
            | (DWCi_GetMatchCnt()->nnCookieRand << 16);
        if (node->hasPrivateAddress)
        {
            if (node->publicIP == ServerBrowserGetMyPublicIPAddr(DWCi_GetMatchCnt()->sbObj))
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                    "Server[%u] is behind same NAT as me.\n", node->profileID);
                node->connectionIP = node->privateIP;
                node->connectionPort = node->privatePort;
                nnValid = FALSE;
            }
            else
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                    "Server[%u] is behind NAT.\n", node->profileID);
                nnValid = TRUE;
            }
        }
        else
        {
            ip = SOGetHostID();
            if (IsPrivateAddr(ip))
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                    "Server[%u] is not behind NAT. But I'm behind NAT.\n", node->profileID);
                nnValid = TRUE;
            }
            else
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                    "Both I and Server[%u] are not behind NAT.\n", node->profileID);
                node->connectionIP = node->publicIP;
                node->connectionPort = node->publicPort;
                nnValid = FALSE;
            }
        }
        if (nnValid)
        {
            DWCi_GetMatchCnt()->nnCookieRand = (u16)DWCi_GetMathRand32(0x10000);
            DWCi_GetMatchCnt()->nnInfo.cookie = cookie;
        }
        else
        {
            senddata[0] = (u32)SOGetHostID();
            senddata[1] = DWCi_HtoLEl(gt2GetLocalPort(*DWCi_GetMatchCnt()->pGt2Socket));
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_TELL_ADDR,
                node->profileID, node->publicIP, node->publicPort, senddata, 2);
            DWCi_GetMatchCnt()->cmdCnt.count = 0;
            if (result)
                return ne_socketerror;
            DWCi_GetMatchCnt()->nnInfo.cookie = 0;
        }
        DWCi_GetMatchCnt()->nnInfo.isQR2 = 0;
        DWCi_GetMatchCnt()->nnInfo.retryCount = 0;
        DWCi_GetMatchCnt()->nnInfo.port = node->publicPort;
        DWCi_GetMatchCnt()->nnInfo.ip = node->publicIP;
    }
    else
    {
        DWCi_GetMatchCnt()->nnInfo.isQR2 = 1;
        DWCi_GetMatchCnt()->nnInfo.retryCount = 0;
        DWCi_GetMatchCnt()->nnInfo.port = 0;
        DWCi_GetMatchCnt()->nnInfo.ip = 0;
        DWCi_GetMatchCnt()->nnInfo.cookie = cookie;
        nnValid = TRUE;
    }
    if (nnValid)
        nnError = DWCi_DoNatNegotiationAsync(&DWCi_GetMatchCnt()->nnInfo);
    else
    {
        DWCi_NNCompletedCallback(nr_success,
            gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket), NULL,
            &DWCi_GetMatchCnt()->nnInfo);
        DWCi_GetMatchCnt()->nnFinishTime = 0;
    }
    return nnError;
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

BOOL fn_8045FB60(u8 command, int srcPid, u32 srcIP, u16 srcPort, const u32* data, int len)
{
    u16 port;
    u32 senddata[DWC_MAX_MATCH_IDX_LIST + 1];
    int sendlen = 0;
    int result;

    if (!DWCi_GetMatchCnt() || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_INIT)
        return TRUE;
    if (DWC_GetState() == DWC_STATE_MATCHING
        && DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING
        && DWCi_GetNodeCount() >= 1
        && DWCi_GetMatchCnt()->nodeList.nodes[0].profileID == srcPid)
    {
        DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
        DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick();
    }

    switch (command)
    {
    case DWC_MATCH_COMMAND_RESERVATION:
    {
        u32 matchType = DWCi_LEtoHl(data[0]);
        DWCNodeInfo node;
        BOOL prior;
        u32 topology;
        u32 requestID;
        BOOL mesh;
        u32 reply;
        int reservationResult;
        memset(&node, 0, sizeof(node));
        node.aid = 0xFF;
        node.profileID = srcPid;
        node.publicIP = data[1];
        node.publicPort = DWCi_LEtoHl(data[2]);
        node.privateIP = data[3];
        node.privatePort = DWCi_LEtoHl(data[4]);
        node.hasPrivateAddress = DWCi_LEtoHl(data[5]);
        prior = DWCi_LEtoHl(data[6]);
        topology = DWCi_LEtoHl(data[7]);
        memcpy(node.reservationData, &data[8], sizeof(node.reservationData));
        requestID = DWCi_LEtoHl(data[9]);
        mesh = FALSE;
        if (len >= 11)
            mesh = DWCi_LEtoHl(data[10]) == 1;
        reply = fn_80462850(node.profileID, node.publicIP, node.publicPort,
            matchType, prior ? TRUE : FALSE, topology, node.reservationData, mesh);
        if ((u8)reply == DWC_MATCH_COMMAND_RESV_OK)
        {
            DWCNodeInfo* local;
            memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
            DWCi_GetMatchCnt()->serverNode = node;
            reservationResult = fn_80463010(srcPid, srcIP, srcPort);
            if (DWCi_HandleMatchCommandError(reservationResult))
                return FALSE;
            if (DWCi_GetMatchCnt()->meshConnecting)
                DWCi_GetMatchCnt()->serverNode.aid =
                    DWCi_GetNodeByProfileID(DWCi_GetMatchCnt()->serverNode.profileID)->aid;
            if (!DWCi_GetMatchCnt()->meshConnecting
                && DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_SV
                && DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN
                && ((DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_ANYBODY
                        && DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_FRIEND)
                    || DWCi_GetMatchCnt()->matchingComplete)
                && DWCi_GetMatchCnt()->newClientCallback)
                DWCi_GetMatchCnt()->newClientCallback(DWCi_GetFriendListIndex(srcPid),
                    DWCi_GetMatchCnt()->newClientParam);
            local = DWCi_GetLocalNode();
            senddata[sendlen++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->qr2NumEntry);
            senddata[sendlen++] = DWCi_HtoLEl(local->aid);
            senddata[sendlen++] = DWCi_HtoLEl(local->profileID);
            senddata[sendlen++] = local->publicIP;
            senddata[sendlen++] = DWCi_HtoLEl(local->publicPort);
            senddata[sendlen++] = local->privateIP;
            senddata[sendlen++] = DWCi_HtoLEl(local->privatePort);
            senddata[sendlen++] = DWCi_HtoLEl(local->hasPrivateAddress);
            senddata[sendlen++] = *(const u32*)local->reservationData;
            senddata[sendlen++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->groupID);
            senddata[sendlen++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverNode.aid);
            senddata[sendlen++] = DWCi_HtoLEl(requestID);
            senddata[sendlen++] = DWCi_HtoLEl(DWCi_GetNodeCount());
            senddata[sendlen++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->meshConnecting ? TRUE : FALSE);
            DWC_Printf(DWC_REPORTFLAG_INFO, "Send <DWC_MATCH_COMMAND_RESV_OK> to: [%u]\n", node.profileID);
            DWC_Printf(DWC_REPORTFLAG_INFO, "Send Server AID(%d)\n", local->aid);
            DWC_Printf(DWC_REPORTFLAG_INFO, "Send Group ID(%d)\n", DWCi_GetMatchCnt()->groupID);
            DWC_Printf(DWC_REPORTFLAG_INFO, "Send Max Entry(%d)\n", DWCi_GetMatchCnt()->qr2NumEntry);
            DWC_Printf(DWC_REPORTFLAG_INFO, "new client aid: [%u]\n", DWCi_GetMatchCnt()->serverNode.aid);
            DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_OWN_NN);
        }
        else if ((u8)reply == DWC_MATCH_COMMAND_RESV_DENY)
        {
            DWC_Printf(DWC_REPORTFLAG_INFO, "send RESV_DENY. %s\n",
                DWCi_GetReservationDenyReason(DWCi_GetMatchCnt()->_8E4));
            senddata[0] = DWCi_HtoLEl(DWCi_GetMatchCnt()->_8E4);
            sendlen = 1;
        }
        if ((u8)reply != DWC_MATCH_COMMAND_DUMMY)
        {
            reservationResult = DWCi_SendMatchCommand(reply, node.profileID,
                node.publicIP, node.publicPort, senddata, sendlen);
            if (DWCi_HandleMatchCommandError(reservationResult))
                return FALSE;
        }
        break;
    }
    case DWC_MATCH_COMMAND_RESV_OK:
    {
        u32 numEntry = DWCi_LEtoHl(data[0]);
        DWCNodeInfo node;
        DWCNodeInfo* local = NULL;
        u32 groupID;
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "****** Ignore NN reservation ****** \n");
            break;
        }
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Succeeded NN reservation.\n");
        if (srcPid != DWCi_GetMatchCnt()->reqProfileID)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "****** Ignore RESV_OK. from wrong server(%d). ****** \n");
            break;
        }
        if (srcPid != DWCi_LEtoHl(data[2]))
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "****** Ignore RESV_OK. wrong server info. ****** \n");
            break;
        }
        if (DWCi_GetMatchCnt()->reservationRequestID != DWCi_LEtoHl(data[11]))
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "****** Ignore RESV_OK. wrong resvCheckValue. ****** \n");
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "****** myvalue:%u  wrong value:%u. ****** \n",
                DWCi_GetMatchCnt()->reservationRequestID, DWCi_LEtoHl(data[11]));
            break;
        }
        if (len >= 14)
        {
            BOOL mesh = DWCi_LEtoHl(data[13]) == 1;
            if (mesh != DWCi_GetMatchCnt()->meshConnecting)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "****** Ignore RESV_OK. wrong meshReserve. ****** \n");
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "****** my(%u) reserved:%s target(%u) reserved:%s. ****** \n",
                    DWCi_GetMatchCnt()->profileID, DWCi_GetMatchCnt()->meshConnecting ? "TRUE" : "FALSE",
                    DWCi_LEtoHl(data[2]), mesh ? "TRUE" : "FALSE");
                break;
            }
        }
        DWCi_GetMatchCnt()->resvWaitCount = 0;
        DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
        DWCi_GetMatchCnt()->cmdResendFlag = FALSE;
        memset(&node, 0, sizeof(node));
        node.aid = DWCi_LEtoHl(data[1]);
        node.profileID = DWCi_LEtoHl(data[2]);
        node.publicIP = data[3];
        node.publicPort = DWCi_LEtoHl(data[4]);
        node.privateIP = data[5];
        node.privatePort = DWCi_LEtoHl(data[6]);
        node.hasPrivateAddress = DWCi_LEtoHl(data[7]);
        memcpy(node.reservationData, &data[8], sizeof(node.reservationData));
        groupID = DWCi_LEtoHl(data[9]);
        if (!DWCi_GetMatchCnt()->meshConnecting)
        {
            local = DWCi_GetLocalNode();
            local->aid = DWCi_LEtoHl(data[10]);
        }
        DWCi_GetMatchCnt()->serverNode = node;
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Server IP:%x, port:%d\n", node.publicIP, node.privatePort);
        if (DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_CL)
        {
            DWCi_GetMatchCnt()->groupID = groupID;
            DWCi_GetMatchCnt()->qr2NumEntry = numEntry;
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Recv Server AID:%d\n", node.aid);
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Recv GroupID:%u.\n", DWCi_GetMatchCnt()->groupID);
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Recv Max Entry:%d.\n", DWCi_GetMatchCnt()->qr2NumEntry);
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Recv User Data: 0[%d] 1[%d] 2[%d] 3[%d].\n",
                DWCi_GetMatchCnt()->serverNode.reservationData[0], DWCi_GetMatchCnt()->serverNode.reservationData[1],
                DWCi_GetMatchCnt()->serverNode.reservationData[2], DWCi_GetMatchCnt()->serverNode.reservationData[3]);
            if (local)
                DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "My AID:%d.\n", local->aid);
        }
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND
            && DWCi_GetMatchCnt()->evalCallback && !DWCi_GetMatchCnt()->meshConnecting
            && !DWCi_GetMatchCnt()->_8C8)
        {
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_EVAL_HOST);
            if (DWCi_HandleSBError(DWCi_SBUpdateAsync(srcPid)))
                return FALSE;
        }
        else
        {
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
            result = fn_8045F330(FALSE, 0, &DWCi_GetMatchCnt()->serverNode);
            if (DWCi_HandleNNError(result))
                return FALSE;
        }
        break;
    }
    case DWC_MATCH_COMMAND_RESV_DENY:
    {
        u32 reason;
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV
            || srcPid != DWCi_GetMatchCnt()->reqProfileID)
            break;
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Reservation was denied by %u.\n", srcPid);
        reason = DWCi_LEtoHl(data[0]);
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "%s\n", DWCi_GetReservationDenyReason(reason));
        if (fn_8046F650() == TRUE)
        {
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
            break;
        }
        if (DWCi_GetMatchCnt()->_8C8 || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
        {
            DWCError error = (DWCError)23;
            if (reason == DWC_MATCH_RESV_DENY_REASON_SV_FULL)
                error = DWC_ERROR_SERVER_FULL;
            DWCi_StopMatching(error, 0);
            return FALSE;
        }
        return DWCi_RetryReserving(DWCi_GetMatchCnt()->reqProfileID);
    }
    case DWC_MATCH_COMMAND_RESV_WAIT:
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV)
            break;
        if (fn_8046F650() == TRUE)
        {
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
            break;
        }
        if (DWCi_GetMatchCnt()->_8C8)
        {
            if (srcPid == DWCi_GetMatchCnt()->reqProfileID
                && gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, srcPid))
            {
                DWCi_GetMatchCnt()->_8E6 = 1;
                DWCi_GetMatchCnt()->scResvRetryCount = 0;
                DWCi_GetMatchCnt()->cmdTimeoutTime = 12000;
                DWCi_GetMatchCnt()->cmdTimeoutStartTick = DWCi_Np_GetTick();
            }
            break;
        }
        if (srcPid == DWCi_GetMatchCnt()->reqProfileID)
        {
            DWCi_GetMatchCnt()->cmdTimeoutStartTick = DWCi_Np_GetTick();
            DWCi_GetMatchCnt()->resvWaitCount = 0;
            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY)
            {
                DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_TempNewNodeInfo_Clear()\n");
                memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
                DWCi_GetMatchCnt()->sbUpdateFlag = 1;
                DWCi_GetMatchCnt()->sbUpdateTick = DWCi_Np_GetTick();
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
            {
                DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_TempNewNodeInfo_Clear()\n");
                memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
            {
                DWCi_GetMatchCnt()->_8E6 = 1;
                DWCi_GetMatchCnt()->scResvRetryCount = 0;
                DWCi_GetMatchCnt()->cmdTimeoutTime = 12000;
                DWCi_GetMatchCnt()->cmdTimeoutStartTick = DWCi_Np_GetTick();
            }
        }
        break;
    case DWC_MATCH_COMMAND_RESV_CANCEL:
        if (!DWCi_GetMatchCnt()->qr2IsReserved || srcPid != DWCi_GetMatchCnt()->qr2Reservation)
            break;
        if (fn_8046F650() == TRUE)
        {
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
            break;
        }
        if (DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_SV
            && DWCi_GetMatchCnt()->serverNode.profileID
            && DWCi_GetGT2ConnectionByProfileID(DWCi_GetMatchCnt()->serverNode.profileID, FALSE)
            && srcPid == DWCi_GetMatchCnt()->serverNode.profileID)
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        if (!DWCi_CancelPendingServer(srcPid))
            return FALSE;
        break;
    case DWC_MATCH_COMMAND_TELL_ADDR:
    {
        SOInAddr ip;
        struct sockaddr_in remoteaddr;
        ip.addr = data[0];
        port = DWCi_LEtoHl(data[1]);
        if (!DWCi_GetMatchCnt()->serverNode.profileID)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "new node info is invaild. ignore.\n");
            break;
        }
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
            "NN parent is behind same NAT as me. Received IP %x & port %d\n", ip.addr, port);
        if (DWCi_GetMatchCnt()->topology != 2
            && DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_CL
            && !DWCi_GetMatchCnt()->meshConnecting)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "I'm client. not in mesh making..ignore.\n");
            break;
        }
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING)
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
        else if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN
                && DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)
            || srcPid != DWCi_GetMatchCnt()->qr2Reservation)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "But already canceled reservation.\n");
            break;
        }
        DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;
        remoteaddr.sin_addr = ip;
        remoteaddr.sin_port = SOHtoNs(port);
        DWCi_GetMatchCnt()->nnInfo.isQR2 = TRUE;
        DWCi_NNCompletedCallback(nr_success, gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
            &remoteaddr, &DWCi_GetMatchCnt()->nnInfo);
        DWCi_GetMatchCnt()->nnFinishTime = 0;
        break;
    }
    case DWC_MATCH_COMMAND_NEW_PID_AID:
    {
        u8 aid;
        int profileID;
        if (fn_8046F650() == TRUE)
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING
            || srcPid != DWCi_GetNodeByIndex(0)->profileID)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed NEW_PID_AID command.\n");
            break;
        }
        profileID = DWCi_LEtoHl(data[0]);
        aid = DWCi_LEtoHl(data[1]);
        if (DWCi_GetMatchCnt()->newClientNode.profileID)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore dup NEW_PID_AID command. I'm waiting pid(%u)..\n",
                DWCi_GetMatchCnt()->newClientNode.profileID);
            break;
        }
        DWCi_GetMatchCnt()->serverDownAckBitmap = 0;
        if (DWCi_GetMatchCnt()->topology == 2)
        {
            DWCi_GetMatchCnt()->serverNode.profileID = profileID;
            DWCi_GetMatchCnt()->serverNode.aid = aid;
        }
        else
            DWCi_GetMatchCnt()->newClientNode.profileID = profileID;
        if (DWCi_GetMatchCnt()->newClientCallback)
            DWCi_GetMatchCnt()->newClientCallback(DWCi_GetFriendListIndex(profileID),
                DWCi_GetMatchCnt()->newClientParam);
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Received new client's profileID = %u & aid = %d.\n", profileID, aid);
        break;
    }
    case DWC_MATCH_COMMAND_LINK_CLS_SUC:
    {
        DWCNodeInfo* node = DWCi_GetNodeByIndex(DWCi_GetNodeCount() - 1 - DWCi_GetMatchCnt()->clLinkProgress);
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_WAIT_CL_LINK
            && node && node->profileID == DWCi_LEtoHl(data[0]))
        {
            DWCi_GetMatchCnt()->clLinkProgress++;
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SV_CONNECT);
        }
        else
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Ignore delayed LINK_CLS_SUC command %d.\n", DWC_MATCH_COMMAND_LINK_CLS_SUC);
        break;
    }
    case DWC_MATCH_COMMAND_LINK_CLS_REQ:
    {
        struct
        {
            DWCNodeInfo* node;
            int profileID;
            u8 aid;
        } linkRequest;
        SOInAddr ip;
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING
            || srcPid != (linkRequest.node = &DWCi_GetMatchCnt()->serverNode)->profileID)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed LINK_CLS_REQ command.\n");
            break;
        }
        linkRequest.profileID = DWCi_LEtoHl(data[0]);
        linkRequest.aid = DWCi_LEtoHl(data[2]);
        if (DWCi_GetGT2Connection(linkRequest.aid))
        {
            u32 reply = DWCi_HtoLEl(linkRequest.profileID);
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Resend command %d for delayed command %d.\n",
                DWC_MATCH_COMMAND_LINK_CLS_SUC, DWC_MATCH_COMMAND_LINK_CLS_REQ);
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_SUC, srcPid,
                linkRequest.node->publicIP, linkRequest.node->publicPort, &reply, 1);
            if (DWCi_HandleMatchCommandError(result))
                return FALSE;
            break;
        }
        DWCi_GetMatchCnt()->newClientNode = DWCi_GetMatchCnt()->serverNode;
        linkRequest.node->profileID = linkRequest.profileID;
        linkRequest.node->aid = linkRequest.aid;
        linkRequest.node->publicIP = data[3];
        linkRequest.node->publicPort = DWCi_LEtoHl(data[4]);
        linkRequest.node->privateIP = data[5];
        linkRequest.node->privatePort = DWCi_LEtoHl(data[6]);
        linkRequest.node->hasPrivateAddress = DWCi_LEtoHl(data[7]);
        memcpy(linkRequest.node->reservationData, &data[8], sizeof(linkRequest.node->reservationData));
        ip.addr = linkRequest.node->publicIP;
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Client IP:%s, port:%d\n", SOInetNtoA(ip), linkRequest.node->publicPort);
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
        DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
        DWCi_GetMatchCnt()->cmdResendFlag = FALSE;
        fn_8045F330(FALSE, 0, linkRequest.node);
        break;
    }
    case DWC_MATCH_COMMAND_LINK_CLS_REP:
    {
        int profileID;
        u8 aid;
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING
            || srcPid != DWCi_GetFirstNode()->profileID)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed LINK_CLS_REP command.\n");
            break;
        }
        profileID = DWCi_LEtoHl(data[0]);
        aid = DWCi_LEtoHl(data[1]);
        if (profileID != DWCi_GetMatchCnt()->serverNode.profileID
            || aid != DWCi_GetMatchCnt()->serverNode.aid)
        {
            DWC_Printf(DWC_REPORTFLAG_INFO, "LINK_CLS_REP: recovery aid(%d->%d) and profileid(%d->%d).\n",
                DWCi_GetMatchCnt()->serverNode.aid, aid, DWCi_GetMatchCnt()->serverNode.profileID, profileID);
            DWCi_GetMatchCnt()->serverNode.aid = aid;
            DWCi_GetMatchCnt()->serverNode.profileID = profileID;
        }
        else
            DWC_Printf(DWC_REPORTFLAG_INFO, "LINK_CLS_REP: client info is valid.\n");
        break;
    }
    case DWC_MATCH_COMMAND_CANCEL:
        if (srcPid != DWCi_GetMatchCnt()->serverNode.profileID)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed CANCEL command.\n");
            break;
        }
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Received cancel command from %u data[0] = %d.\n",
            srcPid, DWCi_LEtoHl(data[0]));
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY
            || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
        {
            if (!DWCi_CancelPreConnectedClientProcess(srcPid))
                return FALSE;
        }
        else if (DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_CL)
        {
            DWCi_GetMatchCnt()->cbEventPid = srcPid;
            DWCi_CloseAllConnectionsByTimeout();
            DWCi_RestartFromCancel(DWC_MATCH_RESET_ALL);
        }
        break;
    case DWC_MATCH_COMMAND_CANCEL_SYN:
    case DWC_MATCH_COMMAND_CANCEL_SYN_ACK:
    case DWC_MATCH_COMMAND_CANCEL_ACK:
        if (!fn_80467AC0(srcPid, command, DWCi_LEtoHl(data[0])))
            return FALSE;
        break;
    case DWC_MATCH_COMMAND_SC_CLOSE_CL:
    {
        DWCNodeInfo* server = DWCi_GetFirstNode();
        u8 closeState;
        int i;
        if (!server)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "server node is NULL!!\n");
            return TRUE;
        }
        if (srcPid != server->profileID)
            return TRUE;
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Close shutdown client.\n");
        closeState = DWCi_GetMatchCnt()->closeState;
        DWCi_GetMatchCnt()->closeState = 1;
        for (i = 0; i < len; i++)
        {
            DWCNodeInfo* node = DWCi_GetNodeByProfileID(DWCi_LEtoHl(data[i]));
            if (!node && DWCi_GetMatchCnt()->newClientNode.profileID == (int)DWCi_LEtoHl(data[i]))
            {
                DWCi_CloseConnectionHard(DWCi_GetMatchCnt()->newClientNode.aid);
                DWCi_GetMatchCnt()->newClientNode.profileID = 0;
            }
            else if (node)
                DWCi_CloseConnectionHard(node->aid);
        }
        DWCi_GetMatchCnt()->closeState = closeState;
        break;
    }
    case DWC_MATCH_COMMAND_CL_WAIT_POLL:
        if (DWCi_GetMatchCnt()->serverNode.profileID
            && srcPid == DWCi_GetMatchCnt()->serverNode.profileID)
        {
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SV_KA_TO_CL, srcPid, srcIP, srcPort, NULL, 0);
            if (DWCi_HandleMatchCommandError(result))
                return FALSE;
        }
        break;
    case DWC_MATCH_COMMAND_SV_KA_TO_CL:
        break;
    case DWC_MATCH_COMMAND_SVDOWN_QUERY:
    {
        u8 aid;
        BOOL down;
        if (!DWCi_GetMatchCnt()->matchingComplete)
            break;
        if (DWCi_IsBackupMine())
        {
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "WARN!! SVDOWNQUERY to Next server!!\n");
            return FALSE;
        }
        aid = DWCi_GetNodeAIDFromProfileID(DWCi_GetMatchCnt()->profileID);
        if (aid == 0xFF)
            return FALSE;
        if (DWCi_GetNodeAIDFromProfileID(srcPid) == 0xFF)
        {
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "already next server downed[%u].\n", srcPid);
            return FALSE;
        }
        if (srcPid != DWCi_GetNodeByIndex(0)->profileID)
            down = FALSE;
        else
            down = (DWCi_GetMatchCnt()->serverDownAckBitmap & (1 << aid)) ? TRUE : FALSE;
        result = DWCi_SendMatchCommand(down ? DWC_MATCH_COMMAND_SVDOWN_ACK : DWC_MATCH_COMMAND_SVDOWN_NAK, srcPid, srcIP, srcPort, NULL, 0);
        if (DWCi_HandleMatchCommandError(result))
            return FALSE;
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Send ServerDown is %s:%u.\n", down ? "down" : "alive", srcPid);
        DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick();
        if (down && DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SVDOWN_2)
        {
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
            DWCi_ResetNodePacketReceiveTicks();
        }
        break;
    }
    case DWC_MATCH_COMMAND_SVDOWN_ACK:
        if (!DWCi_GetMatchCnt()->matchingComplete)
            break;
        if (DWCi_IsBackupMine())
        {
            u8 aid = DWCi_GetNodeAIDFromProfileID(srcPid);
            if (aid == 0xFF)
            {
                DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "WARN!! idx of PID %u is 0xff.\n", srcPid);
                return FALSE;
            }
            DWCi_GetMatchCnt()->serverDownAckBitmap |= 1 << aid;
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Recv ServerDown State: ACK:%u.\n", srcPid);
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SVDOWN_ACK, srcPid, srcIP, srcPort, NULL, 0);
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Send ACK to [%u]: IP:%d Port:%d\n", srcPid, srcIP, srcPort);
            if (DWCi_HandleMatchCommandError(result))
            {
                DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Send Failed!\n");
                return FALSE;
            }
        }
        else
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Recv ACK From Backup.\n");
        break;
    case DWC_MATCH_COMMAND_SVDOWN_NAK:
        if (!DWCi_GetMatchCnt()->matchingComplete)
            break;
        if (++DWCi_GetMatchCnt()->_8CD > DWCi_GetNodeCount() * 5)
        {
            DWC_CloseAllConnectionsHard();
            break;
        }
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Recv ServerDown State: NAK:%u.\n", srcPid);
        DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick();
        break;
    case DWC_MATCH_COMMAND_SVDOWN_KEEP:
        if (DWCi_GetMatchCnt()->matchingComplete)
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "recv SVDOWN_KEEP.\n");
        break;
    case DWC_MATCH_COMMAND_RETRY_SB_SEARCH:
        DWCi_SBUpdateAsync(0);
        DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
        DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick();
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST);
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Retry SB Search.\n");
        break;
    case DWC_MATCH_COMMAND_SUSPEND_MATCH:
    {
        int profileID = DWCi_LEtoHl(data[0]);
        u8 fromServer = DWCi_LEtoHl(data[1]);
        u8 suspend = DWCi_LEtoHl(data[2]);
        if (!DWCi_GetNodeByProfileID(profileID))
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore suspend cmd(from [%u] who isn't in list).\n", profileID);
            break;
        }
        if (DWCi_GetHostState() == 2)
        {
            u8 aid = DWCi_LEtoHl(data[3]);
            if (fromServer == 1)
            {
                DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore suspend cmd(from SV[%u],I'm SV).\n", profileID);
                break;
            }
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Server: AID(%u) suspend(%u).\n", aid, suspend);
            fn_8046E250(aid, suspend, 1);
        }
        else if (DWCi_GetHostState() == 1)
        {
            if (!fromServer)
            {
                DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore suspend cmd(from CL[%u],I'm CL).\n", profileID);
                break;
            }
            if (len == 2)
            {
                if (DWCi_GetMatchCnt()->_030 != 1)
                {
                    DWCi_GetMatchCnt()->_030 = 1;
                    DWCi_GetMatchCnt()->suspendRetryInterval = 8000;
                    DWCi_GetMatchCnt()->_020 = DWCi_Np_GetTick();
                    DWC_Printf(DWC_REPORTFLAG_DEBUG, "*** DWC_MATCH_COMMAND_SUSPEND_MATCH: get server responce.\n");
                }
                else
                    DWC_Printf(DWC_REPORTFLAG_DEBUG, "*** DWC_MATCH_COMMAND_SUSPEND_MATCH: It has not completed it yet.\n");
            }
            else
            {
                u32 bitmap = DWCi_LEtoHl(data[3]);
                DWCi_GetMatchCnt()->_030 = 1;
                DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Client: suspend(%u). Bitmap(%u)\n", suspend, bitmap);
                DWCi_GetMatchCnt()->_01C = bitmap;
            }
        }
        break;
    }
    case DWC_MATCH_COMMAND_SERVER_POLL:
        fn_8045DBB0(data);
        break;
    default:
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Received unexpected matching command 0x%02x.\n", command);
        break;
    }
    return TRUE;
}

u32 fn_80462850(int profileID, u32 qr2IP, u16 qr2Port, u32 matchType,
    BOOL priorFlag, u32 topology, const void* reservationData, BOOL mesh)
{
    u32 command = profileID;
    DWCi_GetMatchCnt()->_8E4 = 1;
    switch (DWCi_GetMatchCnt()->hostState)
    {
    case 1:
        if (DWCi_IsMeshConnectionPending(profileID))
        {
            if (!DWCi_GetMatchCnt()->meshConnecting)
            {
                if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING)
                {
                    if (!DWCi_GetMatchCnt()->newClientNode.profileID)
                    {
                        DWCi_GetMatchCnt()->meshConnecting = TRUE;
                        DWCi_GetMatchCnt()->meshState = DWC_MATCH_STATE_INIT;
                        memset(&DWCi_GetMatchCnt()->meshNode, 0, sizeof(DWCNodeInfo));
                        command = DWC_MATCH_COMMAND_RESV_OK;
                        DWCi_GetMatchCnt()->cmdCnt.sendTime = DWCi_Np_GetTick();
                        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** 1-A\n");
                    }
                    else
                    {
                        command = DWC_MATCH_COMMAND_RESV_WAIT;
                        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** 1-B\n");
                    }
                }
                else
                {
                    command = DWC_MATCH_COMMAND_RESV_WAIT;
                    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** 2\n");
                }
            }
            else if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV)
            {
                DWCNodeInfo* local = DWCi_GetLocalNode();
                if (profileID == DWCi_GetMatchCnt()->serverNode.profileID)
                {
                    if (local->profileID < profileID)
                    {
                        command = DWC_MATCH_COMMAND_RESV_OK;
                        DWCi_GetMatchCnt()->cmdCnt.sendTime = DWCi_Np_GetTick();
                        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** 3\n");
                    }
                    else
                    {
                        command = DWC_MATCH_COMMAND_DUMMY;
                        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** 4\n");
                    }
                }
                else
                {
                    command = DWC_MATCH_COMMAND_RESV_WAIT;
                    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** 5\n");
                }
            }
            else
            {
                command = DWC_MATCH_COMMAND_RESV_WAIT;
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** 6\n");
            }
        }
        else
        {
            DWCi_GetMatchCnt()->_8E4 = 20;
            command = DWC_MATCH_COMMAND_RESV_DENY;
        }
        break;
    case 0:
    case 2:
        command = fn_80462B30(profileID, qr2IP, qr2Port, matchType, priorFlag, topology, reservationData, mesh);
        break;
    }
    return command;
}

u32 fn_80462B30(int profileID, u32 qr2IP, u16 qr2Port, u32 matchType,
    BOOL priorFlag, u32 topology, const void* reservationData, BOOL mesh)
{
    u32 command;
    int numConnection;
    if (DWCi_IsMatchLocked())
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "*** this server is locked!!!\n");
        DWCi_GetMatchCnt()->_8E4 = 19;
        return DWC_MATCH_COMMAND_RESV_DENY;
    }
    if (mesh)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "*** illegal mesh reserve command(%u).\n", profileID);
        DWCi_GetMatchCnt()->_8E4 = 24;
        return DWC_MATCH_COMMAND_RESV_DENY;
    }
    if (!priorFlag)
    {
        if ((matchType == DWC_MATCH_TYPE_ANYBODY && DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_ANYBODY)
            || (matchType == DWC_MATCH_TYPE_FRIEND && DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_FRIEND)
            || (matchType == DWC_MATCH_TYPE_SC_CL && DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV))
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** my match type(%d) resv match type(%d).\n",
                DWCi_GetMatchCnt()->qr2MatchType, matchType);
            DWCi_GetMatchCnt()->_8E4 = 18;
            return DWC_MATCH_COMMAND_RESV_DENY;
        }
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
        {
            if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID)
                || DWCi_GetFriendListIndex(profileID) == -1)
                return DWC_MATCH_COMMAND_DUMMY;
            if (!DWCi_IsFriendByIdxList(profileID))
            {
                DWCi_GetMatchCnt()->_8E4 = 21;
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "This friend doesn't exist in friendIdxList.\n");
                return DWC_MATCH_COMMAND_RESV_DENY;
            }
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV)
        {
            if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID)
                || DWCi_GetFriendListIndex(profileID) == -1)
                return DWC_MATCH_COMMAND_DUMMY;
        }
    }
    numConnection = DWC_GetNumConnectionHost();
    if (DWCi_IsMatchCapacityFull(numConnection, DWCi_GetMatchCnt()->qr2NumEntry))
    {
        DWCi_GetMatchCnt()->_8E4 = 16;
        command = DWC_MATCH_COMMAND_RESV_DENY;
    }
    else if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SEARCH_OWN)
        command = DWC_MATCH_COMMAND_RESV_WAIT;
    else if (fn_80456F10() || DWCi_GetMatchCnt()->_04C)
    {
        DWCi_GetMatchCnt()->_8E4 = 17;
        command = DWC_MATCH_COMMAND_RESV_DENY;
    }
    else if (topology != DWCi_GetMatchCnt()->topology)
    {
        command = DWC_MATCH_COMMAND_RESV_DENY;
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** my topology(%d) resv topology(%d).\n",
            DWCi_GetMatchCnt()->topology, topology);
        DWCi_GetMatchCnt()->_8E4 = 18;
    }
    else if (DWCi_GetNodeByProfileID(profileID))
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "*** pid(%u) has already been in list.\n", profileID);
        command = DWC_MATCH_COMMAND_DUMMY;
    }
    else if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_WAITING
                 && DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_SEARCH_HOST
                 && DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV)
        || (!DWCi_GetMatchCnt()->qr2IP && !DWCi_GetMatchCnt()->qr2Port)
        || (!qr2IP && !qr2Port))
    {
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN
            && profileID == DWCi_GetMatchCnt()->qr2Reservation)
            command = DWC_MATCH_COMMAND_DUMMY;
        else
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                "please wait: qr2IP:%u qr2Port:%u qr2IPM:%u qr2PortM: %u\n",
                qr2IP, qr2Port, DWCi_GetMatchCnt()->qr2IP, DWCi_GetMatchCnt()->qr2Port);
            command = DWC_MATCH_COMMAND_RESV_WAIT;
        }
    }
    else if (DWCi_GetMatchCnt()->matchType != DWC_MATCH_TYPE_SC_SV && DWCi_GetMatchCnt()->reqProfileID)
    {
        if (profileID == DWCi_GetMatchCnt()->reqProfileID)
        {
            if (DWCi_GetMatchCnt()->profileID < profileID)
                command = DWC_MATCH_COMMAND_RESV_OK;
            else
                command = DWC_MATCH_COMMAND_DUMMY;
        }
        else
        {
            DWCi_GetMatchCnt()->_8E4 = 23;
            command = DWC_MATCH_COMMAND_RESV_DENY;
        }
    }
    else
        command = DWC_MATCH_COMMAND_RESV_OK;

    if (command == DWC_MATCH_COMMAND_RESV_OK)
    {
        if (DWCi_GetMatchCnt()->reservationCallback
            && !DWCi_GetMatchCnt()->reservationCallback(reservationData, DWCi_GetMatchCnt()->reservationParam))
        {
            DWCi_GetMatchCnt()->_8E4 = 22;
            return DWC_MATCH_COMMAND_RESV_DENY;
        }
        if (DWCi_GetMatchCnt()->hostState == 0)
            fn_8046DCC0(1);
    }
    return command;
}

int fn_80463010(int profileID, u32 ip, u16 port)
{
    u32 senddata[16];
    DWCNodeInfo* local;
    int i;
    int result;
    if (DWCi_GetMatchCnt()->qr2IsReserved && DWCi_GetMatchCnt()->qr2Reservation == profileID)
        return 0;
    DWCi_GetMatchCnt()->qr2IsReserved = TRUE;
    DWCi_GetMatchCnt()->qr2Reservation = profileID;
    DWCi_GetMatchCnt()->cmdResendFlag = FALSE;
    DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    DWCi_GetMatchCnt()->reqProfileID = 0;
    DWCi_GetMatchCnt()->serverNode.aid = DWCi_GetUnusedAID();
    if (!DWCi_GetMatchCnt()->meshConnecting)
    {
        senddata[0] = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverNode.profileID);
        senddata[1] = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverNode.aid);
        local = DWCi_GetLocalNode();
        for (i = 0; i < DWCi_GetNodeCount(); i++)
        {
            DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
            if (local->profileID != node->profileID)
            {
                result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_NEW_PID_AID,
                    node->profileID, node->publicIP, node->publicPort, senddata, 2);
                if (result)
                    return result;
            }
        }
    }
    return 0;
}

int DWCi_SendResvCommand(int profileID, BOOL delay)
{
    u32 senddata[16];
    u32 ip = 0;
    u16 port = 0;
    if (delay || (!DWCi_GetMatchCnt()->qr2IP && !DWCi_GetMatchCnt()->qr2Port))
    {
        DWCi_GetMatchCnt()->cmdResendFlag = TRUE;
        DWCi_GetMatchCnt()->cmdResendTick = DWCi_Np_GetTick();
        DWCi_GetMatchCnt()->reqProfileID = 0;
        if (!delay)
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Delay ResvCommand - qr2IP & qr2Port = 0.\n");
        return 0;
    }
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY)
    {
        ip = DWCi_GetMatchCnt()->serverNode.publicIP;
        port = DWCi_GetMatchCnt()->serverNode.publicPort;
        DWCi_GetMatchCnt()->reqProfileID = profileID;
    }
    else
    {
        DWCi_GetMatchCnt()->reqProfileID = profileID;
        if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID))
        {
            ip = DWCi_GetMatchCnt()->serverNode.publicIP;
            port = DWCi_GetMatchCnt()->serverNode.publicPort;
        }
    }
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL
        || (DWCi_GetMatchCnt()->_8C8 && gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID)))
    {
        DWCi_GetMatchCnt()->_8E6 = 0;
        DWCi_GetMatchCnt()->cmdTimeoutTime = 30000;
    }
    else
    {
        DWCi_GetMatchCnt()->_8E6 = 5;
        DWCi_GetMatchCnt()->cmdTimeoutTime = 12000;
    }
    DWCi_GetMatchCnt()->cmdTimeoutStartTick = DWCi_Np_GetTick();
    DWCi_GetMatchCnt()->cmdResendFlag = FALSE;
    {
        int length = DWCi_MakeReservationCommand(senddata);
        return DWCi_SendMatchCommand(DWC_MATCH_COMMAND_RESERVATION,
            profileID, ip, port, senddata, length);
    }
}

int DWCi_SendResvCommandToFriend(BOOL delay, BOOL init, int resendPid)
{
    int profileID;
    int buddyIdx;
    int i;
    BOOL found = FALSE;
    GPBuddyStatus status;
    GPResult gpResult;
    if (!init)
    {
        DWCi_GetMatchCnt()->friendCount++;
        DWCi_GetMatchCnt()->friendCount %= DWCi_GetMatchCnt()->friendIdxListLen;
    }
    DWCi_GetMatchCnt()->matchType = DWC_MATCH_TYPE_SC_CL;
    for (i = 0; i < DWCi_GetMatchCnt()->friendIdxListLen;
         ++i, DWCi_GetMatchCnt()->friendCount =
             (DWCi_GetMatchCnt()->friendCount + i) % DWCi_GetMatchCnt()->friendIdxListLen)
    {
        profileID = DWC_GetGsProfileId(DWCi_GetUserData(),
            &DWCi_GetMatchCnt()->friendList[DWCi_GetMatchCnt()->friendIdxList[DWCi_GetMatchCnt()->friendCount]]);
        if (!profileID || profileID == -1)
            continue;
        if (!DWCi_Acc_IsValidFriendData(
                &DWCi_GetMatchCnt()->friendList[DWCi_GetMatchCnt()->friendIdxList[DWCi_GetMatchCnt()->friendCount]]))
            continue;
        if (DWCi_GetNodeByProfileID(profileID))
            continue;
        gpResult = gpGetBuddyIndex(DWCi_GetMatchCnt()->pGpObj, profileID, &buddyIdx);
        gpResult |= gpGetBuddyStatus(DWCi_GetMatchCnt()->pGpObj, buddyIdx, &status);
        if (DWCi_IsBuddyMatchLocked(&status))
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "this buddy is locked. search next buddy...\n");
            continue;
        }
        if (!gpResult && status.status == DWC_STATUS_MATCH_FRIEND)
        {
            found = TRUE;
            break;
        }
    }
    if (found)
    {
        if (profileID == resendPid)
            delay = TRUE;
        memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
        DWCi_GetMatchCnt()->serverNode.aid = 0xFF;
        DWCi_GetMatchCnt()->serverNode.profileID = profileID;
        return DWCi_SendResvCommand(profileID, delay);
    }
    DWCi_GetMatchCnt()->cmdTimeoutTime = 3000;
    DWCi_GetMatchCnt()->cmdTimeoutStartTick = DWCi_Np_GetTick();
    DWCi_GetMatchCnt()->cmdResendFlag = FALSE;
    return 0;
}

BOOL DWCi_RetryReserving(int resendPid)
{
    int result;
    DWCi_GetMatchCnt()->reqProfileID = 0;
    DWCi_GetMatchCnt()->resvWaitCount = 0;
    DWCi_GetMatchCnt()->cmdTimeoutStartTick = DWCi_Np_GetTick();
    memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCNodeInfo));
    if (DWCi_GetMatchCnt()->_8C8)
    {
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST);
        if (DWCi_HandleSBError(DWCi_SBUpdateAsync(0)))
            return FALSE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY)
    {
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_SEARCH_HOST);
        if (resendPid)
        {
            DWCi_GetMatchCnt()->sbUpdateFlag = 1;
            DWCi_GetMatchCnt()->sbUpdateTick = DWCi_Np_GetTick();
        }
        else if (DWCi_HandleSBError(DWCi_SBUpdateAsync(0)))
            return FALSE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
    {
        result = DWCi_SendResvCommandToFriend(TRUE, FALSE, resendPid);
        if (DWCi_HandleMatchCommandError(result))
            return FALSE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
    {
        DWCi_StopMatching(DWC_ERROR_NETWORK, -80410);
        return FALSE;
    }
    return TRUE;
}

BOOL DWCi_CancelPreConnectedServerProcess(int clientPid, int level)
{
    int i;
    if (DWCi_GetMatchCnt()->serverNode.profileID)
        memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCNodeInfo));
    DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;
    if (DWCi_GetMatchCnt()->nnInfo.cookie)
    {
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }
    DWCi_GetMatchCnt()->reqProfileID = 0;
    switch (DWCi_GetMatchCnt()->hostState)
    {
    case 2:
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_CancelPreConnectedServerProcess : server\n");
        DWCi_GetMatchCnt()->qr2IsReserved = FALSE;
        DWCi_GetMatchCnt()->qr2Reservation = 0;
        DWCi_EnterMatchState(DWC_MATCH_STATE_SV_CANCEL_SYN);
        DWCi_GetMatchCnt()->cancelSynAckBit = 0;
        DWCi_GetMatchCnt()->cancelBaseLatency = 0;
        DWCi_CloseCancelHostAsync(clientPid);
        for (i = 1; i < DWCi_GetNodeCount(); i++)
        {
            if (!DWCi_SendCancelMatchSynCommand(DWCi_GetNodeByIndex(i)->profileID, DWC_MATCH_COMMAND_CANCEL_SYN))
                return FALSE;
        }
        if (DWCi_GetNodeCount() == 1)
            DWCi_RestartFromCancel(level);
        break;
    default:
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_CancelPreConnectedServerProcess : client\n");
        fn_8046F650();
    }
    return TRUE;
}

BOOL DWCi_CancelPreConnectedClientProcess(int serverPid)
{
    BOOL result = TRUE;
    (void)serverPid;
    if (DWCi_GetMatchCnt()->_8C8 || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
    {
        if (DWCi_GetNodeCount() > 1)
            DWCi_CloseAllConnectionsByTimeout();
        if (DWCi_GetMatchCnt()->_8C8)
            DWCi_StopMatching((DWCError)24, -80432);
        else
            DWCi_StopMatching((DWCError)24, -80430);
        return FALSE;
    }
    if (fn_8046F650() == TRUE)
        return TRUE;
    if (DWCi_GetMatchCnt()->newClientNode.profileID)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Clear temp new client info.\n");
        DWCi_GetMatchCnt()->newClientNode.profileID = 0;
    }
    if (DWCi_GetMatchCnt()->nnInfo.cookie)
    {
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }
    if ((DWCi_GetMatchCnt()->topology == 2 && DWCi_GetMatchCnt()->_757)
        || (DWCi_GetMatchCnt()->serverNode.profileID
            && DWCi_GetGT2ConnectionByProfileID(DWCi_GetMatchCnt()->serverNode.profileID, TRUE))
        || DWCi_GetNodeCount() > 1)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Close all connection and restart matching.\n");
        DWCi_RestartFromTimeout();
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Cancel and restart client process.\n");
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAIT_RESV);
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Cancel and retry to reserve.\n");
        result = DWCi_RetryReserving(0);
    }
    return result;
}

void DWCi_PostProcessConnection(DWCPostProcessConnection type)
{
    int result;
    switch (type)
    {
    case DWC_PP_CONNECTION_SV_CONNECT:
        if (DWCi_GetMatchCnt()->topology == 2)
            fn_80465D80();
        else
            fn_80465AB0();
        break;
    case DWC_PP_CONNECTION_CL_GT2_CONNECT:
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAITING);
        if (DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_CL)
            DWCi_GetMatchCnt()->cbEventPid = DWCi_GetMatchCnt()->serverNode.profileID;
        break;
    case DWC_PP_CONNECTION_CL_GT2_ACCEPT:
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAITING);
        if (fn_8046F650() == TRUE)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Mesh create completed.\n");
            DWCi_GetMatchCnt()->meshState = DWC_MATCH_STATE_INIT;
            memset(&DWCi_GetMatchCnt()->meshNode, 0, sizeof(DWCi_GetMatchCnt()->meshNode));
            break;
        }
        DWCi_SetHostState(1);
        DWCi_GetMatchCnt()->matchType = DWC_MATCH_TYPE_SC_CL;
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY
            || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
        {
            DWCi_GetMatchCnt()->qr2IsReserved = TRUE;
            DWCi_GetMatchCnt()->qr2Reservation = DWCi_GetMatchCnt()->profileID;
        }
        DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
        DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick();
        if (DWCi_GetMatchCnt()->topology == 2)
        {
            if (DWCi_GetMatchCnt()->newClientNode.profileID)
            {
                u32 senddata = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverNode.profileID);
                DWCi_PrependNode(&DWCi_GetMatchCnt()->serverNode);
                DWCi_GetMatchCnt()->serverNode = DWCi_GetMatchCnt()->newClientNode;
                result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_SUC,
                    DWCi_GetMatchCnt()->serverNode.profileID,
                    DWCi_GetMatchCnt()->serverNode.publicIP,
                    DWCi_GetMatchCnt()->serverNode.publicPort, &senddata, 1);
                memset(&DWCi_GetMatchCnt()->newClientNode, 0, sizeof(DWCi_GetMatchCnt()->newClientNode));
                if (DWCi_HandleMatchCommandError(result))
                    return;
            }
            DWCi_GetMatchCnt()->_757 = 1;
        }
        break;
    case DWC_PP_CONNECTION_SYN_FINISH:
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Accepted connectionCompleted matching!\n");
        DWCi_GetMatchCnt()->matchingComplete = TRUE;
        DWCi_GetMatchCnt()->nnFailureCount = 0;
        if (DWCi_GetMatchCnt()->topology == 2)
        {
            fn_804653E0();
            DWCi_GetMatchCnt()->_757 = 0;
        }
        else
            fn_804653E0();
        DWCi_GetMatchCnt()->_8C8 = 0;
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
        DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE, FALSE,
            DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE, FALSE,
            DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
            DWCi_GetMatchCnt()->matchedParam);
        DWCi_FreeNegotiateList();
        if (DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_SV)
        {
            result = DWCi_GPSetServerStatus(-1);
            if (DWCi_HandleGPError(result))
                return;
            DWCi_EnterMatchState(DWC_MATCH_STATE_SV_WAITING);
        }
        else
        {
            DWCi_GPSetServerStatus(DWC_STATUS_PLAYING);
            DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAITING);
        }
        DWCi_GetMatchCnt()->cbEventPid = 0;
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_INIT;
        DWCi_ResetNodePacketReceiveTicks();
        break;
    }
    if (DWCi_GetMatchCnt()->sbObj)
    {
        ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
        DWCi_GetMatchCnt()->sbUpdateRequestTick = 0;
    }
}

void fn_804653E0(void)
{
    DWCMatchControl* control = DWCi_GetMatchCnt();
    if (control->matchType == DWC_MATCH_TYPE_SC_SV)
    {
        if (control->serverNode.profileID)
        {
            DWCi_AppendNode(&control->serverNode);
            memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
        }
    }
    else if (DWCi_HasOnlyLocalNode())
    {
        DWCi_PrependNode(&control->serverNode);
        memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
    }
    else if (control->newClientNode.profileID)
    {
        DWCNodeInfo* node = DWCi_GetNodeByProfileID(control->newClientNode.profileID);
        if (node)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "*** WARNING *** pid(%u) aid(%d) is in list..close\n",
                node->profileID, node->aid);
            DWCi_CloseConnectionHard(node->aid);
        }
        node = DWCi_GetNodeByAID(control->newClientNode.aid);
        if (node)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "*** WARNING *** pid(%u) aid(%d) is in list..close\n",
                node->profileID, node->aid);
            DWCi_CloseConnectionHard(node->aid);
        }
        DWCi_AppendNode(&control->newClientNode);
        control->newClientNode.profileID = 0;
    }
}

BOOL fn_80465AB0(void)
{
    BOOL result = FALSE;
    int i;
    if (DWCi_GetMatchCnt()->meshConnecting || DWCi_GetMatchCnt()->meshState == DWC_MATCH_STATE_SV_OWN_GT2)
    {
        DWCi_GetMatchCnt()->qr2IsReserved = FALSE;
        DWCi_GetMatchCnt()->qr2Reservation = 0;
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAITING);
        fn_8046F650();
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Mesh create completed.\n");
        return FALSE;
    }
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Tell new client completion of matching.\n");
    DWCi_GetMatchCnt()->qr2IsReserved = FALSE;
    DWCi_GetMatchCnt()->qr2Reservation = 0;
    if (!DWCi_GetMatchCnt()->matchingComplete && DWCi_GetNodeCount() == 1)
        DWCi_GetMatchCnt()->_8D0 = DWCi_Np_GetTick();
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    if (DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_SV)
        DWCi_GetMatchCnt()->cbEventPid = DWCi_GetMatchCnt()->serverNode.profileID;
    DWCi_EnterMatchState(DWC_MATCH_STATE_SV_SYN);
    DWCi_GetMatchCnt()->synAckBit = 0;
    for (i = 1; i < DWCi_GetNodeCount(); i++)
        fn_804671A0(DWCi_GetNodeByIndex(i)->aid, DWC_SEND_TYPE_MATCH_SYN);
    fn_804671A0(DWCi_GetMatchCnt()->serverNode.aid, DWC_SEND_TYPE_MATCH_SYN);
    if (DWCi_GetMatchCnt()->matchType != DWC_MATCH_TYPE_SC_SV)
        result = TRUE;
    return result;
}

BOOL fn_80465D80(void)
{
    u32 senddata[16];
    int sendlen = 0;
    DWCMatchControl* control = DWCi_GetMatchCnt();
    DWCNodeInfo pending = control->serverNode;
    u8 index;
    int result;
    if (DWCi_GetClientLinkProgress() < DWCi_GetNodeCount() - 1)
    {
        DWCNodeInfo* node;
        index = DWCi_GetClientLinkIndex(DWCi_GetNodeCount(), DWCi_GetClientLinkProgress());
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Send client-client link request(%d).\n", index);
        DWCi_EnterMatchState(DWC_MATCH_STATE_SV_WAIT_CL_LINK);
        node = DWCi_GetNodeByIndex(index);
        senddata[sendlen++] = DWCi_HtoLEl(node->profileID);
        senddata[sendlen++] = DWCi_HtoLEl(index);
        senddata[sendlen++] = DWCi_HtoLEl(node->aid);
        senddata[sendlen++] = node->publicIP;
        senddata[sendlen++] = DWCi_HtoLEl(node->publicPort);
        senddata[sendlen++] = node->privateIP;
        senddata[sendlen++] = DWCi_HtoLEl(node->privatePort);
        senddata[sendlen++] = DWCi_HtoLEl(node->hasPrivateAddress);
        senddata[sendlen++] = *(const u32*)node->reservationData;
    }
    else
    {
        control->clLinkProgress = 0;
        return fn_80465AB0();
    }
    if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_SYN)
    {
        DWCNodeInfo* node;
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_REQ,
            pending.profileID, pending.publicIP, pending.publicPort, senddata, sendlen);
        if (DWCi_HandleMatchCommandError(result))
            return FALSE;
        node = DWCi_GetNodeByIndex(index);
        sendlen = 0;
        senddata[sendlen++] = DWCi_HtoLEl(pending.profileID);
        senddata[sendlen++] = DWCi_HtoLEl(pending.aid);
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_REP,
            node->profileID, node->publicIP, node->publicPort, senddata, sendlen);
        if (DWCi_HandleMatchCommandError(result))
            return FALSE;
        DWCi_GetMatchCnt()->cmdCnt.count = 0;
    }
    return FALSE;
}

void fn_80466160(void)
{
    BOOL hasConnections;
    int result;
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "CANCEL! state %d, numHost=%d.\n",
        DWCi_GetMatchCnt()->state, DWCi_GetNodeCount());
    DWCi_GetMatchCnt()->cbEventPid = 0;
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SEARCH_OWN)
    {
        DWCi_FinishCancelMatching();
        return;
    }
    hasConnections = DWCi_GT2GetConnectionListIdx() ? TRUE : FALSE;
    DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;
    if (DWCi_GetMatchCnt()->matchType == DWC_MATCH_TYPE_SC_CL)
    {
        if (DWCi_GetMatchCnt()->nnInfo.cookie)
        {
            NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
            DWCi_GetMatchCnt()->nnInfo.cookie = 0;
        }
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;
        if (hasConnections)
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        else
        {
            result = DWCi_CancelReservation();
            if (DWCi_HandleMatchCommandError(result))
                return;
        }
        DWCi_FinishCancelMatching();
        return;
    }
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_EVAL_HOST
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_NN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_GT2
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2)
    {
        if (DWCi_GetMatchCnt()->reqProfileID)
        {
            result = DWCi_CancelReservation();
            if (DWCi_HandleMatchCommandError(result))
                return;
        }
        result = DWCi_InvalidateReservation();
        if (DWCi_HandleMatchCommandError(result))
            return;
    }
    if (DWCi_GetMatchCnt()->nnInfo.cookie)
    {
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }
    if (hasConnections)
    {
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;
        gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
    }
    DWCi_FinishCancelMatching();
}

void DWCi_RestartFromCancel(DWCMatchResetLevel level)
{
    SBError result;
    if (level == DWC_MATCH_RESET_ALL)
        DWCi_FinishCancelMatching();
    else
    {
        DWCi_ResetMatchParam(level);
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV
            || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
        {
            DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE, TRUE,
                DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE, FALSE,
                DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                DWCi_GetMatchCnt()->matchedParam);
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY)
        {
            if (DWCi_GetMatchCnt()->matchingComplete)
                DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE, TRUE,
                    DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE, FALSE,
                    DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                    DWCi_GetMatchCnt()->matchedParam);
            if (level == DWC_MATCH_RESET_RESTART)
            {
                DWCi_SetHostState(0);
                DWCi_EnterMatchState(DWC_MATCH_STATE_SEARCH_OWN);
                result = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->profileID);
                if (DWCi_HandleSBError(result))
                    return;
            }
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
        {
            if (DWCi_GetMatchCnt()->matchingComplete)
                DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE, TRUE,
                    DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE, FALSE,
                    DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                    DWCi_GetMatchCnt()->matchedParam);
            if (level == DWC_MATCH_RESET_RESTART)
            {
                DWCi_SetHostState(0);
                DWCi_GetMatchCnt()->_8C8 = 0;
                DWCi_EnterMatchState(DWC_MATCH_STATE_SEARCH_OWN);
                result = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->profileID);
                if (DWCi_HandleSBError(result))
                    return;
            }
        }
        else
            DWC_Printf(DWC_REPORTFLAG_ERROR,
                "ERROR - DWCi_RestartFromCancel : matchType %d, level %d\n",
                DWCi_GetMatchCnt()->matchType, level);
    }
}

BOOL fn_80466EA0(u32 bitmap)
{
    u32 senddata[DWC_MAX_CONNECTIONS];
    int validPid[DWC_MAX_CONNECTIONS];
    int senddataLen = 0;
    int validPidLen = 0;
    BOOL closePending = FALSE;
    BOOL result = TRUE;
    int i;
    for (i = 1; i < DWCi_GetNodeCount(); i++)
    {
        DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
        if (node)
        {
            if (bitmap & (1 << node->aid))
            {
                validPid[validPidLen] = node->profileID;
                validPidLen++;
            }
            else
            {
                senddata[senddataLen] = DWCi_HtoLEl(node->profileID);
                senddataLen++;
            }
        }
    }
    if (DWCi_GetMatchCnt()->serverNode.profileID)
    {
        if (bitmap & (1 << DWCi_GetMatchCnt()->serverNode.aid))
            validPid[validPidLen++] = DWCi_GetMatchCnt()->serverNode.profileID;
        else
        {
            senddata[senddataLen++] = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverNode.profileID);
            closePending = TRUE;
        }
    }
    for (i = 0; i < validPidLen; i++)
    {
        int profileID = validPid[i];
        DWCNodeInfo* pending;
        DWCNodeInfo* node = DWCi_GetNodeByProfileID(profileID);
        pending = &DWCi_GetMatchCnt()->serverNode;
        if (pending->profileID && !node && profileID == pending->profileID)
            node = pending;
        if (node)
        {
            int error = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SC_CLOSE_CL,
                profileID, node->publicIP, node->publicPort, senddata, senddataLen);
            if (DWCi_HandleMatchCommandError(error))
                result = FALSE;
        }
    }
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;
    if (closePending)
    {
        DWCi_CloseConnectionHard(DWCi_GetMatchCnt()->serverNode.aid);
        memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
    }
    for (i = 0; i < senddataLen; i++)
    {
        u8 aid = DWCi_GetNodeAIDFromProfileID(DWCi_LEtoHl(senddata[i]));
        if (aid != 0xFF)
            DWCi_CloseConnectionHard(aid);
    }
    DWCi_GetMatchCnt()->closeState = 0;
    return result;
}

void fn_804671A0(u8 aid, u16 type)
{
    u8 buf[8];
    int size = 4;
    u8* packet = buf;
    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Sent SYN %d packet to aid %d.\n",
        type - DWC_SEND_TYPE_MATCH_SYN, aid);
    switch (type)
    {
    case DWC_SEND_TYPE_MATCH_SYN:
        if (DWCi_GetMatchCnt()->serverNode.profileID && aid == DWCi_GetMatchCnt()->serverNode.aid)
        {
            *(u32*)packet = DWCi_HtoLEl(1);
        }
        else
        {
            u32* data;
            size = 28;
            data = DWC_Alloc(DWC_ALLOCTYPE_BASE, size);
            if (!data)
                return;
            data[0] = DWCi_HtoLEl(0);
            packet = (u8*)data;
            DWCi_WriteNodePacket(data, 1, &DWCi_GetMatchCnt()->serverNode);
        }
        break;
    case DWC_SEND_TYPE_MATCH_SYN_ACK:
        packet[0] = DWCi_GetMatchCnt()->baseLatency;
        packet[1] = (DWCi_GetMatchCnt()->baseLatency >> 8) & 0xFF;
        break;
    case DWC_SEND_TYPE_MATCH_ACK:
        if (DWCi_GetMatchCnt()->serverNode.profileID && aid == DWCi_GetMatchCnt()->serverNode.aid)
        {
            int count;
            if (DWCi_GetMatchCnt()->topology == 2)
                count = 0;
            else
                count = DWCi_GetNodeCount() - 1;
            if (count == 0)
            {
                packet[0] = 0;
                packet[1] = 0;
                packet[2] = 0;
                packet[3] = 0;
                size = 8;
                ((u32*)packet)[1] = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverPollingUID);
            }
            else
            {
                u32* data;
                u8 i;
                int index;
                size = count * 24 + 8;
                data = DWC_Alloc(DWC_ALLOCTYPE_BASE, size);
                if (!data)
                    return;
                data[0] = DWCi_HtoLEl(count);
                index = 1;
                for (i = 0; i < count; i++)
                {
                    DWCNodeInfo* node = DWCi_GetNodeByIndex(i + 1);
                    data[index++] = DWCi_HtoLEl((node->hasPrivateAddress << 16) | node->aid);
                    data[index++] = DWCi_HtoLEl(node->profileID);
                    data[index++] = node->publicIP;
                    data[index++] = node->privateIP;
                    data[index++] = DWCi_HtoLEl((node->publicPort << 16) | node->privatePort);
                    data[index++] = *(const u32*)node->reservationData;
                }
                packet = (u8*)data;
                data[index] = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverPollingUID);
            }
        }
        else
        {
            packet[0] = 0;
            packet[1] = 0;
            packet[2] = 0;
            packet[3] = 0;
            size = 8;
            ((u32*)packet)[1] = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverPollingUID);
        }
        break;
    }
    fn_8046FEB0(type, aid, packet, size);
    if (packet != buf)
        DWC_Free(DWC_ALLOCTYPE_BASE, packet, 0);
    DWCi_GetMatchCnt()->lastSynSent = DWCi_Np_GetTick();
}

BOOL fn_80467570(void)
{
    u64 elapsed;
    int i;
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SYN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN_CLOSE_WAIT
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN_WAIT)
    {
        elapsed = OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->lastSynSent);
    }
    else
    {
        return TRUE;
    }
    switch (DWCi_GetMatchCnt()->state)
    {
    case DWC_MATCH_STATE_CL_SYN:
        if (elapsed > DWCi_GetMatchCnt()->clWaitTime)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Timeout: [SYN] Connection to server was shut down.\n");
            DWCi_GetMatchCnt()->newClientNode.profileID = 0;
            if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetFirstNode()->profileID))
                return FALSE;
        }
        break;
    case DWC_MATCH_STATE_SV_SYN:
        if (elapsed > 6000)
        {
            DWCi_GetMatchCnt()->synResendCount++;
            if (DWCi_GetMatchCnt()->synResendCount > 5)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                    "Timeout: wait SYN-ACK (aidbitmap 0x%x). Restart matching.\n", DWCi_GetMatchCnt()->synAckBit);
                DWCi_GetMatchCnt()->synResendCount = 0;
                DWCi_GetMatchCnt()->lastSynSent = DWCi_Np_GetTick();
                if (fn_80466EA0(DWCi_GetMatchCnt()->synAckBit))
                {
                    if (DWCi_GetNodeCount() > 1 || DWCi_GetMatchCnt()->serverNode.profileID)
                    {
                        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_ServerSynTimeoutExec: Continue Syn exec.\n");
                        DWCi_GetMatchCnt()->lastSynSent = DWCi_Np_GetTick();
                        DWCi_EnterMatchState(DWC_MATCH_STATE_SV_SYN_CLOSE_WAIT);
                    }
                    else
                    {
                        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_ServerSynTimeoutExec: Restart match.\n");
                        DWCi_CancelPendingServer(DWCi_GetMatchCnt()->cbEventPid);
                    }
                }
            }
            else
            {
                DWCMatchControl* control;
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "** Timeout: resent SYN-ACK...(%d/%d)\n",
                    DWCi_GetMatchCnt()->synResendCount, 5);
                for (i = 1; i < DWCi_GetNodeCount(); i++)
                {
                    DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
                    if (!(DWCi_GetMatchCnt()->synAckBit & (1 << node->aid)))
                    {
                        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "resent SYN-ACK to aid(%d).\n", node->aid);
                        fn_804671A0(node->aid, DWC_SEND_TYPE_MATCH_SYN);
                    }
                }
                control = DWCi_GetMatchCnt();
                if (!((1 << control->serverNode.aid) & control->synAckBit))
                {
                    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "resent SYN-ACK to aid(%d)(new client).\n", control->serverNode.aid);
                    fn_804671A0(control->serverNode.aid, DWC_SEND_TYPE_MATCH_SYN);
                }
            }
        }
        break;
    case DWC_MATCH_STATE_SV_SYN_CLOSE_WAIT:
        if (elapsed > 3000)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "DWC_MATCH_STATE_SV_SYN_CLOSE_WAIT: send ack command.\n");
            DWCi_SendTypeMatchAck();
        }
        break;
    case DWC_MATCH_STATE_SV_SYN_WAIT:
        if (elapsed > DWCi_GetMatchCnt()->baseLatency)
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SYN_FINISH);
        break;
    }
    return TRUE;
}

BOOL fn_80467AC0(int profileID, u8 command, u32 data)
{
    u8 aid;
    u32 bitmap;
    u64 baseTime;
    u64 latency;
    int i;
    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Received CANCEL SYN %d command from %u.\n",
        command - DWC_MATCH_COMMAND_CANCEL_SYN, profileID);
    if (DWC_GetState() != DWC_STATE_CONNECTED)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Ignore delayed CANCEL SYN.\n");
        return TRUE;
    }
    switch (command)
    {
    case DWC_MATCH_COMMAND_CANCEL_SYN:
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_CANCEL_SYN)
        {
            DWCi_EnterMatchState(DWC_MATCH_STATE_CL_CANCEL_SYN);
            DWCi_GetMatchCnt()->cbEventPid = data;
            if (DWCi_GetMatchCnt()->newClientNode.profileID)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Clear temp new client info.\n");
                DWCi_GetMatchCnt()->newClientNode.profileID = 0;
            }
            memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
        }
        if (!DWCi_SendCancelMatchSynCommand(profileID, DWC_MATCH_COMMAND_CANCEL_SYN_ACK))
            return FALSE;
        break;
    case DWC_MATCH_COMMAND_CANCEL_SYN_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN)
        {
            baseTime = DWCi_Np_GetTick();
            if ((OSTicksToMilliseconds(baseTime - DWCi_GetMatchCnt()->lastCancelSynSent) >> 1) > 300)
            {
                latency = (OSTicksToMilliseconds(baseTime - DWCi_GetMatchCnt()->lastCancelSynSent) >> 1) - 300;
                if (latency > DWCi_GetMatchCnt()->cancelBaseLatency)
                    DWCi_GetMatchCnt()->cancelBaseLatency = (u16)latency;
            }
            aid = DWCi_GetNodeAIDFromProfileID(profileID);
            if (aid != 0xFF)
                DWCi_GetMatchCnt()->cancelSynAckBit |= 1 << aid;
            bitmap = DWCi_GetClientAIDBitmap(FALSE);
            if (DWCi_GetMatchCnt()->cancelSynAckBit == bitmap)
            {
                for (i = 1; i < DWCi_GetNodeCount(); i++)
                {
                    if (!DWCi_SendCancelMatchSynCommand(DWCi_GetNodeByIndex(i)->profileID, DWC_MATCH_COMMAND_CANCEL_ACK))
                        return FALSE;
                }
                DWCi_EnterMatchState(DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT);
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Wait max latency %d msec.\n", DWCi_GetMatchCnt()->cancelBaseLatency);
            }
        }
        else if (!DWCi_SendCancelMatchSynCommand(profileID, DWC_MATCH_COMMAND_CANCEL_ACK))
            return FALSE;
        break;
    case DWC_MATCH_COMMAND_CANCEL_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_CANCEL_SYN)
            DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
        break;
    }
    return TRUE;
}

BOOL fn_804681D0(void)
{
    u64 elapsed;
    int i;
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_CANCEL_SYN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT)
        elapsed = OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->lastCancelSynSent);
    else
        return TRUE;
    switch (DWCi_GetMatchCnt()->state)
    {
    case DWC_MATCH_STATE_CL_CANCEL_SYN:
        if (elapsed > 6000)
        {
            if (!DWCi_SendCancelMatchSynCommand(DWCi_GetNodeByIndex(0)->profileID,
                    DWC_MATCH_COMMAND_CANCEL_SYN_ACK))
                return FALSE;
        }
        break;
    case DWC_MATCH_STATE_SV_CANCEL_SYN:
        if (elapsed > 6000)
        {
            DWCi_GetMatchCnt()->cancelSynResendCount++;
            if (DWCi_GetMatchCnt()->cancelSynResendCount > 5)
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                    "Timeout: wait cancel SYN-ACK (aidbitmap 0x%x).\n", DWCi_GetMatchCnt()->cancelSynAckBit);
                if (!fn_80466EA0(DWCi_GetMatchCnt()->cancelSynAckBit))
                    return FALSE;
                if (DWCi_GetNodeCount() > 1)
                {
                    DWCi_GetMatchCnt()->cancelSynResendCount = 0;
                    DWCi_GetMatchCnt()->lastCancelSynSent = DWCi_Np_GetTick();
                }
                else
                    DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
            }
            else
            {
                for (i = 1; i < DWCi_GetNodeCount(); i++)
                {
                    DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
                    if (!(DWCi_GetMatchCnt()->cancelSynAckBit & (1 << node->aid)))
                    {
                        if (!DWCi_SendCancelMatchSynCommand(node->profileID, DWC_MATCH_COMMAND_CANCEL_SYN))
                            return FALSE;
                    }
                }
            }
        }
        break;
    case DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT:
        if (elapsed > DWCi_GetMatchCnt()->cancelBaseLatency)
            DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
        break;
    }
    return TRUE;
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

GT2Result fn_80468E40(GT2Result error)
{
    int type;
    int code;
    if (error == GT2Success)
        return GT2Success;
    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, GT2 error %d\n", error);
    switch (error)
    {
    case GT2OutOfMemory:
        type = DWC_ERROR_FATAL;
        code = DWC_ECODE_TYPE_ALLOC;
        break;
    case GT2Rejected:
    case GT2DuplicateAddress:
        type = DWC_ERROR_NONE;
        code = 0;
        error = GT2Success;
        break;
    case GT2NetworkError:
        type = DWC_ERROR_NETWORK;
        code = DWC_ECODE_TYPE_NETWORK;
        break;
    case GT2AddressError:
        type = DWC_ERROR_NETWORK;
        code = DWC_ECODE_TYPE_DNS;
        break;
    case GT2TimedOut:
        type = DWC_ERROR_NETWORK;
        code = DWC_ECODE_TYPE_TIMEOUT;
        break;
    case GT2NegotiationError:
        type = DWC_ERROR_NETWORK;
        code = DWC_ECODE_TYPE_PEER;
        break;
    }
    if (type != DWC_ERROR_NONE)
        DWCi_StopMatching(type, code + DWC_ECODE_SEQ_MATCH + DWC_ECODE_GS_GT2);
    return error;
}

void DWCi_SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void* instance)
{
    int profileID;
    int result;
    int i;
    (void)instance;
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "SBCallback : reason %d (state = %d)\n", reason, DWCi_GetMatchCnt()->state);
    s_sbCallbackLevel++;
    if (DWCi_GetMatchCnt()->sbUpdateRequestTick)
    {
        if (reason == sbc_updatecomplete || reason == sbc_queryerror)
        {
            DWCi_GetMatchCnt()->sbUpdateRequestTick = 0;
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "ServerBrowserLimitUpdate timeout reset.(%s)\n", __FUNCTION__);
        }
        else
        {
            DWCi_GetMatchCnt()->sbUpdateRequestTick = DWCi_Np_GetTick()
                + DWCi_Np_MilliSecondsToTicks(30000);
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "ServerBrowserLimitUpdate timeout update.(%s)\n", __FUNCTION__);
        }
    }
    switch (reason)
    {
    case sbc_serveradded:
        DWCi_SBPrintServerData(server);
        break;
    case sbc_updatecomplete:
        DWCi_GetMatchCnt()->sbUpdateRequestTick = 0;
        for (i = 0; i < ServerBrowserCount(sb); i++)
        {
            server = ServerBrowserGetServer(sb, i);
            if (!DWCi_CheckDWCServer(server))
            {
                ServerBrowserRemoveServer(sb, server);
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "Deleted server [%d].\n", i);
                i--;
            }
        }
        switch (DWCi_GetMatchCnt()->state)
        {
        case DWC_MATCH_STATE_SEARCH_OWN:
            for (i = 0; i < ServerBrowserCount(sb); i++)
            {
                server = ServerBrowserGetServer(sb, i);
                if (DWCi_GetMatchCnt()->qr2IP && DWCi_GetMatchCnt()->qr2IP == SBServerGetPublicInetAddress(server)
                    && DWCi_GetMatchCnt()->qr2Port && DWCi_GetMatchCnt()->qr2Port == SBServerGetPublicQueryPort(server))
                    break;
            }
            if (i < ServerBrowserCount(sb))
            {
                DWCNodeInfo node;
                memset(&node, 0, sizeof(node));
                node.profileID = DWCi_GetMatchCnt()->profileID;
                node.aid = 0xFF;
                node.publicIP = DWCi_GetMatchCnt()->qr2IP;
                node.publicPort = DWCi_GetMatchCnt()->qr2Port;
                node.privateIP = SBServerGetPrivateInetAddress(server);
                node.privatePort = SBServerGetPrivateQueryPort(server);
                node.hasPrivateAddress = SBServerHasPrivateAddress(server);
                DWCi_Np_CPUCopyFast(node.reservationData, DWCi_GetMatchCnt()->reservationData, sizeof(node.reservationData));
                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY)
                {
                    DWCi_PrependNode(&node);
                    fn_8046E730();
                }
                else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
                {
                    DWCi_PrependNode(&node);
                    fn_8046E730();
                }
                else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV)
                {
                    node.aid = DWCi_GetUnusedAID();
                    DWCi_PrependNode(&node);
                    fn_8046E730();
                }
                else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
                {
                    DWCi_PrependNode(&node);
                    fn_8046E730();
                }
            }
            else
            {
                DWCi_GetMatchCnt()->sbUpdateFlag = 2;
                DWCi_GetMatchCnt()->sbUpdateTick = DWCi_Np_GetTick();
            }
            break;
        case DWC_MATCH_STATE_CL_SEARCH_HOST:
            DWCi_EvaluateServers(TRUE);
            DWCi_RandomizeServers();
            if (ServerBrowserCount(sb))
            {
                SBServer host = ServerBrowserGetServer(sb, 0);
                DWCi_SetServerNode(host);
                DWCi_GetMatchCnt()->matchType = DWC_MATCH_TYPE_SC_CL;
                if (!DWCi_GetMatchCnt()->sbObj)
                    DWCi_GetMatchCnt()->sbObj = ServerBrowserNewA(DWCi_GetMatchCnt()->gameName,
                        DWCi_GetMatchCnt()->gameName, DWCi_GetMatchCnt()->secretKey,
                        0, 20, QVERSION_QR2, SBFalse, DWCi_SBCallback, NULL);
                if (!DWCi_GetMatchCnt()->sbObj)
                    if (DWCi_HandleSBError(sbe_allocerror))
                        break;
                DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAIT_RESV);
                result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->serverNode.profileID, FALSE);
                DWCi_HandleMatchCommandError(result);
            }
            else
            {
                DWCi_GetMatchCnt()->sbUpdateFlag = 2;
                DWCi_GetMatchCnt()->sbUpdateTick = DWCi_Np_GetTick();
                memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
            }
            break;
        case DWC_MATCH_STATE_CL_SEARCH_EVAL_HOST:
        {
            SOInAddr ip;
            ip.addr = DWCi_GetMatchCnt()->serverNode.publicIP;
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "searchIP: %s, searchPort: %d\n",
                SOInetNtoA(ip), DWCi_GetMatchCnt()->serverNode.publicPort);
            while (ServerBrowserCount(sb))
            {
                server = ServerBrowserGetServer(sb, 0);
                if (SBServerGetPublicInetAddress(server) == DWCi_GetMatchCnt()->serverNode.publicIP
                    && SBServerGetPublicQueryPort(server) == DWCi_GetMatchCnt()->serverNode.publicPort)
                    break;
                ServerBrowserRemoveServer(sb, server);
            }
            if (ServerBrowserCount(sb))
            {
                profileID = SBServerGetIntValueA(ServerBrowserGetServer(sb, 0), DWC_QR2_PID_KEY_STR, 0);
                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
                {
                    if (profileID != DWCi_GetMatchCnt()->reqProfileID)
                        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                            "@@@@ @@@ BAD **** profileID != DWCi_GetMatchCnt()->reqProfileID ****.\n");
                    if (profileID != DWCi_GetMatchCnt()->reqProfileID || !DWCi_EvaluateServers(FALSE))
                    {
                        result = DWCi_CancelReservation();
                        if (DWCi_HandleMatchCommandError(result))
                            break;
                        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAIT_RESV);
                        memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                        result = DWCi_SendResvCommandToFriend(FALSE, FALSE, DWCi_GetMatchCnt()->reqProfileID);
                        DWCi_HandleMatchCommandError(result);
                        break;
                    }
                }
                DWCi_EnterMatchState(DWC_MATCH_STATE_CL_NN);
                ServerBrowserGetServer(sb, 0);
                if (DWCi_HandleNNError(fn_8045F330(FALSE, 0, &DWCi_GetMatchCnt()->serverNode)))
                    break;
            }
            else
            {
                DWCi_GetMatchCnt()->sbUpdateFlag = 2;
                DWCi_GetMatchCnt()->sbUpdateTick = DWCi_Np_GetTick();
            }
            break;
        }
        case DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST:
        {
            int count = ServerBrowserCount(sb);
            if (count)
            {
                SBServer selected;
                if (count > 1)
                {
                    int numPlayers;
                    int maxPlayers = -1;
                    SBServer candidate;
                    for (i = 0; i < count; i++)
                    {
                        candidate = ServerBrowserGetServer(sb, i);
                        numPlayers = SBServerGetIntValueA(candidate, "numplayers", -1);
                        DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                            "*** groupID search *** groupID: find server numplayers: %d.\n", numPlayers);
                        if (maxPlayers < numPlayers)
                        {
                            selected = candidate;
                            maxPlayers = numPlayers;
                            DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                                "*** groupID search *** select this server(%d).\n", numPlayers);
                        }
                    }
                }
                else
                    selected = ServerBrowserGetServer(sb, 0);
                DWCi_SetServerNode(selected);
                DWCi_GetMatchCnt()->qr2MatchType = SBServerGetIntValueA(selected, DWC_QR2_MATCH_TYPE_KEY_STR, -1);
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "Find server num by groupID: %d.\n", count);
                DWCi_ConnectToGroupServer(selected);
            }
            else
            {
                DWCi_GetMatchCnt()->sbUpdateFlag = 2;
                DWCi_GetMatchCnt()->sbUpdateTick = DWCi_Np_GetTick();
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "Find server: host not found.\n");
            }
            break;
        }
        }
        break;
    }
    s_sbCallbackLevel--;
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

void DWCi_QR2NatnegCallback(int cookie, void* userdata)
{
    NegotiateError nnError;
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Got NN request, cookie = %x.\n", cookie);
    if (!fn_80454BB0() && DWCi_GetMatchCnt()->newClientNode.profileID)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "But I'm waiting new client...\n", cookie);
        return;
    }
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING)
    {
        if (DWCi_GetMatchCnt()->topology == 2)
            DWCi_EnterMatchState(DWC_MATCH_STATE_CL_NN);
        else
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "I'm in CL_WAITING stat..ignore.\n");
            return;
        }
    }
    else if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN
        && DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)
    {
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "But already canceled reservation.\n");
        return;
    }
    if (cookie == DWCi_GetMatchCnt()->nnLastCookie)
        DWCi_GetMatchCnt()->nnRecvCount++;
    else
    {
        DWCi_GetMatchCnt()->nnRecvCount = 0;
        DWCi_GetMatchCnt()->nnLastCookie = cookie;
    }
    DWCi_GetMatchCnt()->nnFailedTime = 0;
    nnError = fn_8045F330(1, cookie, &DWCi_GetMatchCnt()->serverNode);
    if (!DWCi_HandleNNError(nnError))
        DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;
}

void fn_8046C590(const void* data, u32 len)
{
    DWCSBMessage sbMsg;
    if (DWC_GetState() != DWC_STATE_MATCHING)
    {
        if (DWC_GetState() != DWC_STATE_CONNECTED
            || (DWCi_GetMatchCnt()->matchType != DWC_MATCH_TYPE_SC_SV
                && DWCi_GetMatchCnt()->matchType != DWC_MATCH_TYPE_SC_CL))
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed GT2 matching command.\n");
            return;
        }
    }
    DWCi_Np_CpuCopy8(data, &sbMsg, sizeof(DWCSBMessageHeader));
    sbMsg.header.version = DWCi_LEtoHl(sbMsg.header.version);
    sbMsg.header.qr2Port = DWCi_LEtoHs(sbMsg.header.qr2Port);
    sbMsg.header.profileID = DWCi_LEtoHl(sbMsg.header.profileID);
    if (sbMsg.header.version != DWC_MATCHING_VERSION)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "Got different version GT2 command.\n");
        return;
    }
    if (len != sizeof(DWCSBMessageHeader) + sbMsg.header.size)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "Got wrong data size GT2 command.\n");
        return;
    }
    DWCi_Np_CpuCopy8((const u8*)data + sizeof(DWCSBMessageHeader), sbMsg.data, sbMsg.header.size);
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "<GT2> RECV-0x%02x <- [--------:-----] [pid=%u]\n",
        sbMsg.header.command, sbMsg.header.profileID);
    fn_8045FB60(sbMsg.header.command, sbMsg.header.profileID,
        sbMsg.header.qr2IP, sbMsg.header.qr2Port, sbMsg.data, sbMsg.header.size >> 2);
}

void DWCi_QR2ClientMsgCallback(
    gsi_char* data, int len, void* userdata)
{
    int offset = 0;
    BOOL boolResult;
    DWCSBMessage sbMsg;

    if (DWC_GetState() != DWC_STATE_MATCHING)
    {
        if (DWC_GetState() != DWC_STATE_CONNECTED
            || (DWCi_GetMatchCnt()->matchType != 2
                && DWCi_GetMatchCnt()->matchType != 3))
        {
            DWC_Printf(4, "Ignore delayed SB matching command.\n");
            return;
        }
    }

    while (offset + (int)sizeof(DWCSBMessageHeader) <= len)
    {
        DWCi_Np_CpuCopy8(data, &sbMsg, sizeof(DWCSBMessageHeader));
        sbMsg.header.version = DWCi_LEtoHl(sbMsg.header.version);
        sbMsg.header.qr2Port = DWCi_LEtoHs(sbMsg.header.qr2Port);
        sbMsg.header.profileID = DWCi_LEtoHl(sbMsg.header.profileID);
        if (strncmp(sbMsg.header.identifier, DWC_SB_COMMAND_STRING, 4) != 0)
        {
            DWC_Printf(8, "Got undefined SBcommand.\n");
            return;
        }
        if (sbMsg.header.version != DWC_MATCHING_VERSION)
        {
            DWC_Printf(8, "Got different version SBcommand.\n");
            return;
        }
        DWCi_Np_CpuCopy8(data + sizeof(DWCSBMessageHeader), sbMsg.data,
            sbMsg.header.size);
        DWC_Printf(0x40, "<SB> RECV-0x%02x <- [%08x:%d] [pid=%u]\n",
            sbMsg.header.command, sbMsg.header.qr2IP, sbMsg.header.qr2Port,
            sbMsg.header.profileID);
        boolResult = fn_8045FB60(sbMsg.header.command,
            sbMsg.header.profileID, sbMsg.header.qr2IP, sbMsg.header.qr2Port,
            sbMsg.data, sbMsg.header.size >> 2);
        if (!boolResult)
        {
            return;
        }
        offset += sizeof(DWCSBMessageHeader) + sbMsg.header.size;
    }
}

static void DWCi_NNProgressCallback(NegotiateState state, void* userdata)
{
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN, Got state update: %d\n", state);
}

void DWCi_NNCompletedCallback(NegotiateResult result, SOCKET gamesocket,
    struct sockaddr_in* remoteaddr, void* userdata)
{
    char pidStr[24];
    NegotiateError nnError;
    NegotiateResult nnResult;
    GT2Result gt2Result;
    DWCNNInfo* nnInfo = (DWCNNInfo*)userdata;
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
        "NN, Complete NAT Negotiation. result : %d\n", result);
    if (nnInfo)
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN cookie = %x.\n", nnInfo->cookie);
    if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN
            && DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)
        || !nnInfo)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed NN after cancel.\n");
        return;
    }
    if (result == nr_success)
    {
        if (remoteaddr)
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN, remote address : %s\n",
                gt2AddressToString(remoteaddr->sin_addr.addr,
                    SO_NtoHs(remoteaddr->sin_port), NULL));
        nnInfo->cookie = 0;
        if (nnInfo->isQR2)
        {
            DWCi_GetMatchCnt()->serverNode.connectionIP = remoteaddr->sin_addr.addr;
            DWCi_GetMatchCnt()->serverNode.connectionPort = SO_NtoHs(remoteaddr->sin_port);
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN child finished Nat Negotiation.\n");
            DWCi_GetMatchCnt()->nnRecvCount = 0;
            DWCi_GetMatchCnt()->nnLastCookie = 0;
            DWCi_GetMatchCnt()->nnFailedTime = 0;
            if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN)
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_OWN_GT2);
            else
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_GT2);
            DWCi_GetMatchCnt()->gt2ConnectCount = 0;
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Connect() to (%s)\n",
                gt2AddressToString(DWCi_GetMatchCnt()->serverNode.connectionIP,
                    DWCi_GetMatchCnt()->serverNode.connectionPort, NULL));
            snprintf(pidStr, sizeof(pidStr), "%u", DWCi_GetMatchCnt()->profileID);
            gt2Result = gt2Connect(*DWCi_GetMatchCnt()->pGt2Socket, NULL,
                gt2AddressToString(DWCi_GetMatchCnt()->serverNode.connectionIP,
                    DWCi_GetMatchCnt()->serverNode.connectionPort, NULL),
                (GT2Byte*)pidStr, -1, DWC_GT2_CONNECT_TIMEOUT,
                DWCi_GetMatchCnt()->gt2Callbacks, GT2False);
            if (DWCi_GetMatchCnt()->meshConnecting && gt2Result != GT2Success)
            {
                DWCi_AbortMeshNegotiation();
                return;
            }
            if (gt2Result == GT2OutOfMemory)
            {
                fn_80468E40(gt2Result);
                return;
            }
            else if (gt2Result != GT2Success)
            {
                if (!DWCi_CancelPendingServer(DWCi_GetMatchCnt()->serverNode.profileID))
                    return;
            }
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN parent finished Nat Negotiation.\n");
            if (remoteaddr)
            {
                DWCi_GetMatchCnt()->serverNode.connectionIP = remoteaddr->sin_addr.addr;
                DWCi_GetMatchCnt()->serverNode.connectionPort = SO_NtoHs(remoteaddr->sin_port);
            }
            DWCi_GetMatchCnt()->nnFinishTime = DWCi_Np_GetTick();
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_GT2);
        }
    }
    else
    {
        if (!nnInfo->cookie)
        {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed NN error after cancel.\n");
            return;
        }
        if (DWCi_GetMatchCnt()->meshConnecting)
        {
            DWCi_AbortMeshNegotiation();
            return;
        }
        nnResult = DWCi_HandleNNResult(result);
        if (nnResult != nr_inittimeout && nnResult != nr_deadbeatpartner
            && nnResult != nr_pingtimeout)
            return;
        if (!nnInfo->isQR2)
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Failed %d/%d NN send.\n",
                nnInfo->retryCount, DWC_MAX_MATCH_NN_RETRY);
            if (nnResult == nr_deadbeatpartner || nnResult == nr_pingtimeout
                || (nnResult == nr_inittimeout && nnInfo->retryCount >= DWC_MAX_MATCH_NN_RETRY))
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Abort NN.\n");
                nnInfo->cookie = 0;
                if (!DWCi_ProcessNNFailure(FALSE))
                    return;
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->serverNode.profileID))
                    return;
            }
            else
            {
                nnInfo->retryCount++;
                nnError = DWCi_DoNatNegotiationAsync(nnInfo);
                if (DWCi_HandleNNError(nnError))
                    return;
            }
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Failed %d/%d NN recv.\n",
                DWCi_GetMatchCnt()->nnRecvCount, DWC_MAX_MATCH_NN_RETRY);
            DWCi_GetMatchCnt()->nnFailedTime = DWCi_Np_GetTick();
            if (nnResult == nr_deadbeatpartner || nnResult == nr_pingtimeout
                || (nnResult == nr_inittimeout
                    && DWCi_GetMatchCnt()->nnRecvCount >= DWC_MAX_MATCH_NN_RETRY))
            {
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Abort NN.\n");
                nnInfo->cookie = 0;
                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL
                    || DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV)
                {
                    if (!DWCi_ProcessNNFailure(TRUE))
                        return;
                }
                else if (!DWCi_ProcessNNFailure(FALSE))
                {
                    return;
                }
                DWCi_GetMatchCnt()->nnRecvCount = 0;
                DWCi_GetMatchCnt()->nnLastCookie = 0;
                DWCi_GetMatchCnt()->nnFailedTime = 0;
                if (!DWCi_CancelPendingServer(DWCi_GetMatchCnt()->serverNode.profileID))
                    return;
            }
        }
    }
}

BOOL fn_8046DB10(void)
{
    int i;
    int count = DWCi_GetNodeCount();
    for (i = 1; i < count; i++)
    {
        DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
        u32 bitmap = DWCi_HtoLEl(DWC_GetAIDBitmap());
        u8 command = DWC_MATCH_COMMAND_SVDOWN_QUERY;
        int result;
        if (DWCi_GetMatchCnt()->serverDownAckBitmap & (1 << node->aid))
            command = DWC_MATCH_COMMAND_SVDOWN_KEEP;
        result = DWCi_SendMatchCommand(command,
            node->profileID, node->publicIP, node->publicPort, &bitmap, 1);
        result = DWCi_HandleMatchCommandError(result);
        if (result)
            return FALSE;
    }
    DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
    DWCi_GetMatchCnt()->clWaitTime = DWCi_Np_GetTick();
    return TRUE;
}

BOOL DWCi_IsBackupMine(void)
{
    DWCNodeInfo* node = DWCi_GetNodeByIndex(0);
    if (node == NULL)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO,
            "*** DWCi_IsBackupMine. ignored(state=%d)\n",
            DWCi_GetMatchStatus());
        return FALSE;
    }
    return node->profileID == DWCi_GetMatchCnt()->profileID;
}

void fn_8046DCC0(int newGroup)
{
    int status;
    if (newGroup)
    {
        DWCi_SetGroupID(DWCi_GenerateGroupID());
        DWCi_GetLocalNode()->aid = 0;
        DWCi_GetMatchCnt()->_8F4 = 1;
    }
    DWCi_GetMatchCnt()->matchType = DWC_MATCH_TYPE_SC_SV;
    DWCi_EnterMatchState(DWC_MATCH_STATE_SV_WAITING);
    DWCi_SetHostState(2);
    DWCi_GetMatchCnt()->qr2Reservation = 0;
    DWCi_GetMatchCnt()->qr2IsReserved = 0;
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY)
        status = DWC_STATUS_MATCH_ANYBODY;
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
        status = DWC_STATUS_MATCH_FRIEND;
    else
        status = DWC_STATUS_MATCH_SC_SV;
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
        DWCi_GetMatchCnt()->qr2MatchType = DWC_MATCH_TYPE_SC_SV;
    DWCi_HandleGPError(DWCi_GPSetServerStatus(status));
    DWCi_GetMatchCnt()->reqProfileID = 0;
}

int fn_8046E040(BOOL force)
{
    u32 senddata[3];
    int i;
    int result;
    if (!fn_80454BB0())
        return 1;
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN_WAIT)
        return 1;
    if (force || OSTicksToMilliseconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->_8D0) > 15000)
    {
        senddata[0] = DWCi_HtoLEl(DWCi_GetMatchCnt()->profileID);
        senddata[1] = DWCi_HtoLEl(DWC_GetAIDBitmap());
        senddata[2] = DWCi_HtoLEl(DWCi_GetMatchCnt()->serverPollingUID);
        for (i = 0; i < DWCi_GetNodeCount(); i++)
        {
            DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
            if (node != DWCi_GetLocalNode())
            {
                result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SERVER_POLL, node->profileID,
                    node->publicIP, node->publicPort, senddata, 3);
                if (DWCi_HandleMatchCommandError(result))
                    return -1;
            }
        }
        DWCi_GetMatchCnt()->_8D0 = DWCi_Np_GetTick();
    }
    return 1;
}

BOOL fn_8046E250(u8 aid, BOOL suspend, int reply)
{
    DWCNodeInfo* local = DWCi_GetNodeByIndex(0);
    u8 localAid = local->aid;
    u32 oldBitmap = DWCi_GetMatchCnt()->_01C;
    u32 bitmap;
    u32 senddata[4];
    BOOL ready;
    BOOL suspended;
    int result;
    int i;
    if (suspend)
        DWCi_GetMatchCnt()->_01C |= 1 << aid;
    else
        DWCi_GetMatchCnt()->_01C &= ~(1 << aid);
    if (oldBitmap != DWCi_GetMatchCnt()->_01C)
    {
        if (aid == localAid)
            DWC_Printf(0x10000, "**Server** AID(%u) suspend(%u) bitmap(%u->%u).\n",
                aid, suspend, oldBitmap, DWCi_GetMatchCnt()->_01C);
        DWCi_GPSetServerStatus(-1);
    }
    bitmap = DWCi_GetMatchCnt()->_01C;
    if (!DWCi_IsReadyToSuspend())
        bitmap &= ~(1 << DWC_GetMyAID());
    suspended = fn_80456F10();
    ready = (suspend && suspended) || (!suspend && !suspended);
    if (reply == 1)
    {
        DWCNodeInfo* node = DWCi_GetNodeByAID(aid);
        int len = 2;
        senddata[0] = DWCi_HtoLEl(DWCi_GetMatchCnt()->profileID);
        senddata[1] = DWCi_HtoLEl(1);
        if (ready && DWCi_GetMatchCnt()->_030)
        {
            senddata[2] = DWCi_HtoLEl(suspend);
            senddata[3] = DWCi_HtoLEl(bitmap);
            len = 4;
        }
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SUSPEND_MATCH, node->profileID,
            node->publicIP, node->publicPort, senddata, len);
        if (DWCi_HandleMatchCommandError(result))
            return FALSE;
    }
    if (ready && DWCi_GetMatchCnt()->_030 != 1)
    {
        DWCi_GetMatchCnt()->_030 = 1;
        senddata[0] = DWCi_HtoLEl(DWCi_GetMatchCnt()->profileID);
        senddata[1] = DWCi_HtoLEl(1);
        senddata[2] = DWCi_HtoLEl(suspend);
        senddata[3] = DWCi_HtoLEl(bitmap);
        for (i = 0; i < DWCi_GetNodeCount(); i++)
        {
            DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
            if (node != local)
            {
                result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SUSPEND_MATCH, node->profileID,
                    node->publicIP, node->publicPort, senddata, 4);
                if (DWCi_HandleMatchCommandError(result))
                    return FALSE;
            }
        }
    }
    return TRUE;
}

BOOL fn_8046E730(void)
{
    int result;
    if (DWCi_GetMatchCnt()->sbObj)
        ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
    if (DWCi_GetMatchCnt()->_8C8)
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST);
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY)
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_SEARCH_HOST);
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)
    {
        DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAIT_RESV);
        result = DWCi_SendResvCommandToFriend(FALSE, TRUE, 0);
        if (DWCi_HandleMatchCommandError(result))
            return FALSE;
        return TRUE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV)
    {
        DWCi_EnterMatchState(DWC_MATCH_STATE_SV_WAITING);
        DWCi_GetMatchCnt()->matchingComplete = TRUE;
        return TRUE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)
    {
        if (gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, DWCi_GetMatchCnt()->serverNode.profileID))
        {
            DWCi_EnterMatchState(DWC_MATCH_STATE_CL_WAIT_RESV);
            result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->serverNode.profileID, FALSE);
            if (DWCi_HandleMatchCommandError(result))
                return FALSE;
        }
        return TRUE;
    }
    DWCi_GetMatchCnt()->reqProfileID = 0;
    return !DWCi_HandleSBError(DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->reqProfileID));
}

static BOOL DWCi_RequestSuspendMatch(BOOL suspend,
    DWCSuspendCallback callback, void* param)
{
    if (!callback || DWCi_IsError())
        return FALSE;
    if (!DWCi_GetMatchCnt() || !DWCi_GetMatchCnt()->hostState)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "but ignored.\n");
        return FALSE;
    }
    if (DWC_GetState() != DWC_STATE_MATCHING && DWC_GetState() != DWC_STATE_CONNECTED)
        return FALSE;
    if (DWCi_GetMatchCnt()->_04C)
    {
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
            "***<SUSPEND> suspend process(%s) has not ended yet!\n",
            DWCi_GetMatchCnt()->_02C ? "TRUE" : "FALSE");
        return FALSE;
    }
    DWCi_GetMatchCnt()->_030 = 0;
    DWCi_GetMatchCnt()->_02C = suspend;
    DWCi_GetMatchCnt()->suspendCallback = callback;
    DWCi_GetMatchCnt()->suspendParam = param;
    DWCi_GetMatchCnt()->_020 = 0;
    DWCi_GetMatchCnt()->suspendStartTick = DWCi_Np_GetTick();
    DWCi_GetMatchCnt()->_04C = TRUE;
    DWCi_GetMatchCnt()->_048 = FALSE;
    return TRUE;
}

BOOL fn_8046EC30(void)
{
    BOOL suspend;
    BOOL suspended;
    if (!DWCi_GetMatchCnt()->_04C)
        return FALSE;
    if (DWCi_GetMatchCnt()->_048)
        DWCi_StartSuspendRequest(DWCi_GetMatchCnt()->_02C,
            DWCi_GetMatchCnt()->suspendCallback, DWCi_GetMatchCnt()->suspendParam);
    suspend = DWCi_GetMatchCnt()->_02C;
    if (suspend == 0xFF)
        return FALSE;
    suspended = fn_80456F10();
    if ((suspend && suspended) || (!suspend && !suspended))
    {
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
            "suspend Match(%s) completed!\n", suspend ? "TRUE" : "FALSE");
        if (DWCi_GetMatchCnt()->suspendCallback)
            DWCi_GetMatchCnt()->suspendCallback(DWC_SUSPEND_SUCCESS,
                DWCi_GetMatchCnt()->_02C, DWCi_GetMatchCnt()->suspendParam);
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
        DWCi_GetMatchCnt()->_02C = 0xFF;
        DWCi_GetMatchCnt()->_04C = FALSE;
        return TRUE;
    }
    fn_8046ED90(suspend);
    return TRUE;
}

BOOL fn_8046ED90(BOOL suspend)
{
    switch (DWCi_GetMatchCnt()->hostState)
    {
    case 2:
        return fn_8046E250(DWCi_GetLocalNode()->aid, suspend, 0);
    }
    if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING)
        return TRUE;
    if (DWCi_GetMatchCnt()->_030
        && DWCi_Np_TicksToMilliSeconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->_020)
            < DWCi_GetMatchCnt()->suspendRetryInterval)
        return TRUE;
    if (DWCi_Np_TicksToMilliSeconds(DWCi_Np_GetTick() - DWCi_GetMatchCnt()->_020) > 3000)
    {
        DWCNodeInfo* local = DWCi_GetLocalNode();
        DWCNodeInfo* server = DWCi_GetNodeByIndex(0);
        u32 data[4];
        data[0] = DWCi_HtoLEl(DWCi_GetMatchCnt()->profileID);
        data[1] = 0;
        data[2] = DWCi_HtoLEl(suspend);
        data[3] = DWCi_HtoLEl(local->aid);
        if (DWCi_HandleMatchCommandError(DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SUSPEND_MATCH,
                server->profileID, server->publicIP, server->publicPort, data, 4)))
            return FALSE;
        DWCi_GetMatchCnt()->_020 = DWCi_Np_GetTick();
        DWCi_GetMatchCnt()->suspendRetryInterval = 3000;
    }
    return TRUE;
}

BOOL fn_8046F060(void)
{
    switch (DWCi_GetMatchCnt()->state)
    {
    case DWC_MATCH_STATE_CL_SVDOWN_1:
    case DWC_MATCH_STATE_CL_SVDOWN_2:
    case DWC_MATCH_STATE_CL_SVDOWN_3:
        return TRUE;
    default:
        return FALSE;
    }
}

void DWCi_SetMatchStatus(DWCMatchState state)
{

    if (stpMatchCnt->state != state)
    {
        DWCMatchState oldState = stpMatchCnt->state;
        OSTime lastTick = stStateChangedTick;
        int milliseconds;
        int seconds;
        stStateChangedTick = OSGetTime();
        milliseconds = OSTicksToMilliseconds(stStateChangedTick - lastTick);
        seconds = milliseconds / 1000;
        // Preserve the retail diagnostic's seconds subtraction and rounding.
        milliseconds = (milliseconds - seconds + 50) / 100;
        DWC_Printf(1, "status changed %s(%d.%d sec) -> %s\n", DWCi_GetMatchStateName(oldState),
            seconds, milliseconds, DWCi_GetMatchStateName(state));
    }
    stpMatchCnt->state = state;
}

BOOL fn_8046F180(void)
{
    return DWCi_GetMatchCnt()->topology != 1;
}

void DWCi_ClearNodeList(void)
{
    memset(&DWCi_GetMatchCnt()->nodeList, 0, sizeof(DWCi_GetMatchCnt()->nodeList));
}

int DWCi_GetNodeCount(void)
{
    return DWCi_GetMatchCnt()->nodeList.count;
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

BOOL fn_8046F2C0(int profileID)
{
    int i;
    BOOL removed;
    if (DWCi_GetMatchCnt()->nodeList.count == 0)
        return FALSE;
    for (i = 0; i < DWCi_GetMatchCnt()->nodeList.count; i++)
        if (DWCi_GetMatchCnt()->nodeList.nodes[i].profileID == profileID)
            break;
    if (i == DWCi_GetMatchCnt()->nodeList.count)
        removed = FALSE;
    else
    {
        for (; i < DWCi_GetMatchCnt()->nodeList.count - 1; i++)
            DWCi_GetMatchCnt()->nodeList.nodes[i] = DWCi_GetMatchCnt()->nodeList.nodes[i + 1];
        DWCi_GetMatchCnt()->nodeList.count--;
        removed = TRUE;
    }
    return removed;
}

DWCNodeInfo* DWCi_GetFirstNode(void)
{
    if (DWCi_GetMatchCnt()->nodeList.count >= 1)
        return &DWCi_GetMatchCnt()->nodeList.nodes[0];
    return NULL;
}

DWCNodeInfo* DWCi_GetNodeByIndex(int index)
{
    if (index < DWCi_GetMatchCnt()->nodeList.count)
        return &DWCi_GetMatchCnt()->nodeList.nodes[index];
    return NULL;
}

DWCNodeInfo* DWCi_GetNodeByProfileID(int profileID)
{
    int i;
    for (i = 0; i < DWCi_GetMatchCnt()->nodeList.count; i++)
        if (DWCi_GetMatchCnt()->nodeList.nodes[i].profileID == profileID)
            return &DWCi_GetMatchCnt()->nodeList.nodes[i];
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

OSTime fn_8046F4A0(void)
{
    DWCNodeInfo* local = DWCi_GetLocalNode();
    OSTime deadline = DWCi_Np_GetTick();
    int connectedCount = 0;
    int localIndex = 0;
    int i;
    deadline += DWCi_Np_MilliSecondsToTicks(3000);
    if (DWCi_GetNodeCount() > 2)
    {
        for (i = 1; i < DWCi_GetNodeCount(); i++)
        {
            DWCNodeInfo* node = DWCi_GetNodeByIndex(i);
            if (local == node)
                localIndex = i;
            else
            {
                DWCi_GetGT2Connection(node->aid);
                if (DWCi_GetGT2Connection(node->aid))
                    connectedCount++;
            }
        }
    }
    deadline += DWCi_Np_MilliSecondsToTicks(connectedCount) * 3000;
    deadline += DWCi_Np_MilliSecondsToTicks((DWCi_GetNodeCount() - 1 - localIndex) * 6000);
    return deadline;
}

int fn_8046F650(void)
{
    DWC_Printf(DWC_REPORTFLAG_INFO, "mesh making stat(%d)\n", DWCi_GetMatchCnt()->meshConnecting);
    if (!DWCi_GetMatchCnt()->meshConnecting)
        return FALSE;
    DWCi_GetMatchCnt()->meshConnecting = FALSE;
    DWCi_GetMatchCnt()->meshState = DWCi_GetMatchCnt()->state;
    DWC_Printf(DWC_REPORTFLAG_INFO, "temp new node info validity(%d)\n",
        DWCi_GetMatchCnt()->serverNode.profileID ? TRUE : FALSE);
    if (DWCi_GetMatchCnt()->serverNode.profileID)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "stop mesh create.(aid:%2d pid:%u)\n",
            DWCi_GetMatchCnt()->serverNode.aid, DWCi_GetMatchCnt()->serverNode.profileID);
        DWCi_GetMatchCnt()->meshNode = DWCi_GetMatchCnt()->serverNode;
    }
    else
        memset(&DWCi_GetMatchCnt()->meshNode, 0, sizeof(DWCi_GetMatchCnt()->meshNode));
    DWCi_CancelMatchNegotiation();
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_NN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_GT2
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN
        || DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2)
    {
        if (DWCi_GetMatchCnt()->reqProfileID)
        {
            if (DWCi_HandleMatchCommandError(DWCi_CancelReservation()))
            {
                memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
                return TRUE;
            }
        }
        DWCi_GetMatchCnt()->qr2IsReserved = FALSE;
        DWCi_GetMatchCnt()->qr2Reservation = 0;
    }
    memset(&DWCi_GetMatchCnt()->serverNode, 0, sizeof(DWCi_GetMatchCnt()->serverNode));
    return TRUE;
}

BOOL fn_8046F8A0(void)
{
    return DWCi_GetMatchCnt()->topology == 2;
}

BOOL DWCi_IsBuddyMatchLocked(const GPBuddyStatus* status)
{
    char value[4] = { 0 };
    if (DWC_GetCommonValueString("LCK", value, status->statusString, '/') > 0)
    {
        return (u8)strtoul(value, NULL, 10);
    }
    return FALSE;
}

void fn_8046F920(int profileID)
{
    DWCi_GetMatchCnt()->profileID = profileID;
}

void fn_8046F930(void)
{
    qr2_register_key(DWC_QR2_PID_KEY, DWC_QR2_PID_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_TYPE_KEY, DWC_QR2_MATCH_TYPE_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_VER_KEY, DWC_QR2_MATCH_VER_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_EVAL_KEY, DWC_QR2_MATCH_EVAL_KEY_STR);
    qr2_register_key(DWC_QR2_GROUP_ID_KEY, DWC_QR2_GROUP_ID_KEY_STR);
    qr2_register_key(DWC_QR2_HOST_STATE_KEY, DWC_QR2_HOST_STATE_KEY_STR);
    qr2_register_key(DWC_QR2_SUSPEND_KEY, DWC_QR2_SUSPEND_KEY_STR);
}

static const char* DWCi_GetMatchStateName(DWCMatchState state)
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
    return names[state];
}
