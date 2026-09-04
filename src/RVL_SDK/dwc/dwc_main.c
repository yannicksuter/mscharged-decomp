#include <dwc/dwc_main.h>

#include <dwc/dwc_error.h>
#include <dwc/dwc_friend.h>
#include <dwc/dwc_report.h>
#include <gamespy/common/gsAvailable.h>
#include <gamespy/common/gsPlatformSocket.h>
#include <gamespy/gt2/gt2.h>
#include <gamespy/natneg/natneg.h>
#include <string.h>

typedef void (*DWCMatchCallback)(int error, BOOL cancelled, void* param);
typedef int (*DWCMatchEvaluationCallback)(void* server);
typedef void (*DWCFriendMatchCallback)(int error, BOOL cancelled, BOOL self,
    BOOL isServer, int index, void* param);
typedef void (*DWCServerBrowserCallback)(int result);
typedef void (*DWCMatchedSCCallbackView)(int error, BOOL cancel, BOOL self,
    BOOL isServer, int index, void* param);

typedef struct DWCMainControlView
{
    GT2Socket _00;
    u8 _04[0x10];
    int _14;
    int _18;
    u8 _1C[8];
    DWCState state;
    DWCState savedState;
    u8 myAid;
    u8 isClosing;
    u16 ingamesn[26];
    u8 _62[0x2];
    int profileID;
    const char* gameName;
    u8 _6C[0x4];
    DWCLoginCallback loginCallback;
    void* loginParam;
    DWCUpdateServersCallback updateCallback;
    void* updateParam;
    DWCMatchCallback matchCallback;
    void* matchCallbackParam;
    DWCFriendMatchCallback friendMatchCallback;
    void* friendMatchCallbackParam;
    DWCConnectionClosedCallback connectionClosedCallback;
    void* connectionClosedParam;
    u8 _98[0x2D5];
    u8 numConnections;
    u8 _36E[0x7];
    volatile u8 matchType;
    u8 _376[0xE2];
    int pidList[32];
    u8 _4D8[0x168];
    u8 aidList[0x20];
    u32 aidBitmap;
    u8 _664[0x8AC];
} DWCMainControlView;

typedef struct DWCConnectionDataView
{
    u8 _00;
    u8 aid;
} DWCConnectionDataView;

typedef struct DWCConnectionInfo
{
    u8 index;
    u8 aid;
    u16 reserve;
    void* param;
} DWCConnectionInfo;

extern DWCConnectionInfo lbl_806C9920[32];
extern u8 lbl_806E03A8[4];

static GT2Connection lbl_806C98A0[32];
static DWCMainControlView* sMainControl;
static s32 lbl_806E2EE4;

int fn_804929A8(void);
int fn_804929C4(void);
int fn_80492ABC(u8** aidList);
int fn_80492AE8(u8** aidList);
BOOL fn_8048E638(GT2Result result);
void fn_8048C54C(int arg0, int arg1);
BOOL DWC_UpdateServersAsync(const char* reserved,
    DWCUpdateServersCallback updateCallback, void* updateParam,
    DWCFriendStatusCallback statusCallback, void* statusParam,
    DWCDeleteFriendListCallback deleteCallback, void* deleteParam);
void fn_8048E7DC(void);
void fn_804900B8(u8 numEntry, const char* addFilter,
    DWCMatchedSCCallbackView matchedCallback, void* matchedParam,
    void* evalCallback, void* evalParam);
GT2Bool fn_80491578(GT2Socket socket, unsigned int ip, unsigned short port,
    GT2Byte* message, int len);
void fn_804916EC(GT2Socket socket, GT2Connection connection, unsigned int ip,
    unsigned short port, int latency, GT2Byte* message, int len);
BOOL DWC_isValidAid(u8 aid);
int fn_8048E27C(void);
int DWC_LoginAsync(const u16* ingamesn, void* unused,
    DWCLoginCallback loginCallback, void* loginParam);
int fn_8048A504(const u16* string);
int DWC_CloseConnectionHard(u8 aid);
int DWC_SetupGameServer(int maxEntry, DWCFriendMatchCallback callback,
    void* callbackParam, DWCServerBrowserCallback browserCallback,
    void* browserParam);
void fn_804903EC(u8 numEntry, DWCMatchedSCCallbackView matchedCallback,
    void* matchedParam, void* callback2, void* param2);
int DWC_ConnectToAnybodyAsync(u8 maxEntry, const char* filter,
    DWCMatchCallback callback, void* callbackParam,
    DWCMatchEvaluationCallback evaluationCallback, void* evaluationParam);
GT2Connection* fn_8048E334(int profileId, int count);
int DWC_CloseAllConnectionsHard(void);
void fn_8048AFCC(int arg0, void* arg1, int arg2);
void fn_8048E708(int error, int profileID);
void DWCi_InitGPProcessCount(void);
void fn_8048E7A8(int error, BOOL isChanged, void* param);
GT2Connection* fn_8048E320(int index);
DWCConnectionInfo* fn_8048E430(int index);
void fn_8048F048(GT2Connection connection, GT2Byte* message, int len,
    GT2Bool reliable);
void fn_8049B244(GT2Connection connection, GT2Byte* message, int len,
    GT2Bool reliable);
void fn_8048F4A0(GT2Connection connection, int latency);
void fn_8049B380(GT2Connection connection, int latency);
static void fn_8048F4F8(GT2Socket socket);

BOOL DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback,
    void* param)
{
    if (sMainControl == NULL)
    {
        return FALSE;
    }
    sMainControl->connectionClosedCallback = callback;
    sMainControl->connectionClosedParam = param;
    return TRUE;
}

int DWC_GetNumConnectionHost(void)
{
    if (sMainControl == NULL)
    {
        return 0;
    }
    if (sMainControl->matchType == 2 || sMainControl->matchType == 3)
    {
        return fn_804929C4() + 1;
    }
    return fn_804929A8() + 1;
}

u8 DWC_GetMyAID(void)
{
    if (sMainControl != NULL)
    {
        return sMainControl->myAid;
    }
    return 0;
}

int DWC_GetAIDList(u8** aidList)
{
    if (sMainControl == NULL)
    {
        return 0;
    }
    *aidList = sMainControl->aidList;
    if (sMainControl->matchType == 2 || sMainControl->matchType == 3)
    {
        return fn_80492AE8(aidList);
    }
    return fn_80492ABC(aidList);
}

BOOL DWC_IsValidAID(u8 aid)
{
    if (sMainControl == NULL)
    {
        return FALSE;
    }
    if ((sMainControl->aidBitmap & (1 << aid)) == 0)
    {
        return FALSE;
    }
    return DWC_isValidAid(aid);
}

DWCState DWC_GetState(void)
{
    if (sMainControl != NULL)
    {
        return sMainControl->state;
    }
    return DWC_STATE_INIT;
}

GT2Result fn_8048E0C4(void)
{
    u16 port;
    GT2Result result;

    if (sMainControl->_00 != NULL)
    {
        DWC_Printf(8, "gt2Socket is already made.\n");
        return GT2Success;
    }

    port = fn_8048A3BC(0x4000) + 0xC000;
    DWC_Printf(0x40, "--- Private port = %d ---\n", port);
    result = gt2CreateSocket(&sMainControl->_00,
        gt2AddressToString(0, port, NULL),
        sMainControl->_14,
        sMainControl->_18,
        fn_8048F4F8);
    if (fn_8048E638(result))
    {
        return result;
    }
    gt2Listen(sMainControl->_00, fn_804916EC);
    gt2SetUnrecognizedMessageCallback(sMainControl->_00, fn_80491578);
    return result;
}

GT2Connection DWCi_GetGT2Connection(u8 aid)
{
    int i;

    if (sMainControl == NULL)
    {
        return NULL;
    }
    for (i = 0; i < 32; i++)
    {
        if (lbl_806C98A0[i] != NULL && ((DWCConnectionDataView*)gt2GetConnectionData(lbl_806C98A0[i]))->aid == aid)
        {
            return lbl_806C98A0[i];
        }
    }
    return NULL;
}

u8 DWCi_GetConnectionAid(GT2Connection connection)
{
    return ((DWCConnectionDataView*)gt2GetConnectionData(connection))->aid;
}

BOOL DWC_isValidAid(u8 aid)
{
    int i;

    for (i = 0; i < 32; i++)
    {
        if (lbl_806C98A0[i] != NULL && ((DWCConnectionDataView*)gt2GetConnectionData(lbl_806C98A0[i]))->aid == aid)
        {
            return TRUE;
        }
    }
    return FALSE;
}

static void fn_8048F4F8(GT2Socket socket)
{
    lbl_806E2EE4 = GOAGetLastError(gt2GetSocketSOCKET(socket));
    DWC_Printf(2, "Socket fatal error! (%d)\n", lbl_806E2EE4);
    DWCi_SetError(DWC_ERROR_FATAL,
        DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_ECODE_TYPE_SO_SOCKET);
    sMainControl->_00 = 0;
}

GT2Connection* fn_8048E320(int index)
{
    return &lbl_806C98A0[index];
}

DWCConnectionInfo* fn_8048E430(int index)
{
    return &lbl_806C9920[index];
}

void fn_8048F048(GT2Connection connection, GT2Byte* message, int len,
    GT2Bool reliable)
{
    fn_8049B244(connection, message, len, reliable);
}

void fn_8048F4A0(GT2Connection connection, int latency)
{
    DWC_Printf(0x100, "Ping: %dms\n", latency);
    fn_8049B380(connection, latency);
}

int fn_8048E27C(void)
{
    int i;

    for (i = 0; i < 32; i++)
    {
        if (lbl_806C98A0[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

void fn_8048E7A8(int error, BOOL isChanged, void* param)
{
    DWCState savedState = sMainControl->savedState;

    if (savedState != DWC_STATE_UPDATE_SERVERS)
    {
        DWCState state = sMainControl->state;

        sMainControl->savedState = state;
        sMainControl->state = savedState;
    }
    sMainControl->updateCallback(error, isChanged, sMainControl->updateParam);
}

void fn_8048E708(int error, int profileID)
{
    if (error == 0)
    {
        sMainControl->profileID = profileID;
        sMainControl->savedState = sMainControl->state;
        sMainControl->state = DWC_STATE_ONLINE;
        DWCi_InitGPProcessCount();
    }
    else
    {
        sMainControl->savedState = sMainControl->state;
        sMainControl->state = DWC_STATE_INIT;
    }
    if (sMainControl->loginCallback != NULL)
    {
        sMainControl->loginCallback(error, profileID, sMainControl->loginParam);
    }
}

BOOL fn_8048E638(GT2Result result)
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
        error = DWC_ERROR_TYPE_6;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GT2AddressError:
        error = DWC_ERROR_TYPE_6;
        errorCode = -30;
        break;
    case GT2TimedOut:
        error = DWC_ERROR_TYPE_6;
        errorCode = -70;
        break;
    case GT2NegotiationError:
        error = DWC_ERROR_TYPE_6;
        errorCode = -80;
        break;
    }
    if (error != 0)
    {
        fn_8048C54C(error, DWC_ECODE_SEQ_LOGIN + DWC_ECODE_GS_GT2 + errorCode);
    }
    return result;
}

int DWC_CloseAllConnectionsHard(void)
{
    DWC_Printf(4, "!!DWC_CloseAllConnectionsHard() was called!!\n");
    if (sMainControl == NULL || DWCi_IsError() != 0
        || (sMainControl->state != DWC_STATE_MATCHING
            && sMainControl->state != DWC_STATE_CONNECTED))
    {
        DWC_Printf(4, "But ignored.\n");
        return -1;
    }
    if (sMainControl->numConnections == 0)
    {
        DWC_Printf(0x100, "Closed 0 connection.\n");
        fn_8048AFCC(1, lbl_806E03A8, 0);
        NNFreeNegotiateList();
        sMainControl->savedState = sMainControl->state;
        sMainControl->state = DWC_STATE_ONLINE;
        return 1;
    }
    sMainControl->isClosing = TRUE;
    gt2CloseAllConnectionsHard(sMainControl->_00);
    sMainControl->isClosing = FALSE;
    return 0;
}

BOOL DWC_UpdateServersAsync(const char* reserved,
    DWCUpdateServersCallback updateCallback, void* updateParam,
    DWCFriendStatusCallback statusCallback, void* statusParam,
    DWCDeleteFriendListCallback deleteCallback, void* deleteParam)
{
    DWC_Printf(4, "!!DWC_UpdateServersAsync() was called!!\n");
    if (DWCi_IsError() != 0 || sMainControl->state < DWC_STATE_ONLINE
        || sMainControl->state == DWC_STATE_UPDATE_SERVERS)
    {
        DWC_Printf(4, "But ignored.\n");
        return FALSE;
    }
    sMainControl->updateCallback = updateCallback;
    sMainControl->updateParam = updateParam;
    sMainControl->savedState = sMainControl->state;
    sMainControl->state = DWC_STATE_UPDATE_SERVERS;
    DWCi_UpdateServersAsync((const char*)&sMainControl->_98[0x4C],
        (const char*)&sMainControl->_98[0x14C],
        fn_8048E7A8, 0, statusCallback, statusParam, deleteCallback,
        deleteParam);
    return TRUE;
}

GT2Connection* fn_8048E334(int profileId, int count)
{
    u8 i;
    u8 aid;
    int j;
    GT2Connection connection;
    DWCConnectionDataView* data;

    for (i = 0; i < count; i++)
    {
        if (sMainControl->pidList[i] == profileId)
        {
            break;
        }
    }
    if (i >= count)
    {
        return NULL;
    }
    if (sMainControl == NULL)
    {
        return NULL;
    }
    aid = sMainControl->aidList[i];
    for (j = 0; j < 32; j++)
    {
        if (lbl_806C98A0[j] != NULL
            && ((DWCConnectionDataView*)gt2GetConnectionData(lbl_806C98A0[j]))->aid == aid)
        {
            connection = lbl_806C98A0[j];
            break;
        }
    }
    connection = NULL;
    data = (DWCConnectionDataView*)gt2GetConnectionData(connection);
    return &lbl_806C98A0[data->_00];
}

int DWC_ConnectToAnybodyAsync(u8 maxEntry, const char* filter,
    DWCMatchCallback callback, void* callbackParam,
    DWCMatchEvaluationCallback evaluationCallback, void* evaluationParam)
{
    DWC_Printf(4, "!!DWC_ConnectToAnybodyAsync() was called!!\n");
    if (DWCi_IsError() != 0 || sMainControl->state != DWC_STATE_ONLINE)
    {
        DWC_Printf(4, "But ignored.\n");
        return FALSE;
    }
    memset(lbl_806C98A0, 0, sizeof(lbl_806C98A0));
    memset(lbl_806C9920, 0, sizeof(lbl_806C9920));
    sMainControl->matchCallback = callback;
    sMainControl->matchCallbackParam = callbackParam;
    sMainControl->savedState = sMainControl->state;
    sMainControl->state = DWC_STATE_MATCHING;
    fn_804900B8((u8)(maxEntry - 1), filter, (DWCMatchedSCCallbackView)fn_8048E7DC,
        0, evaluationCallback, evaluationParam);
    return TRUE;
}

int DWC_SetupGameServer(int maxEntry, DWCFriendMatchCallback callback,
    void* callbackParam, DWCServerBrowserCallback browserCallback,
    void* browserParam)
{
    DWC_Printf(4, "!!DWC_SetupGameServer() was called!!\n");
    if (DWCi_IsError() != 0 || sMainControl->state != DWC_STATE_ONLINE)
    {
        DWC_Printf(4, "But ignored.\n");
        return FALSE;
    }
    memset(lbl_806C98A0, 0, sizeof(lbl_806C98A0));
    memset(lbl_806C9920, 0, sizeof(lbl_806C9920));
    sMainControl->friendMatchCallback = callback;
    sMainControl->friendMatchCallbackParam = callbackParam;
    sMainControl->myAid = 0;
    sMainControl->savedState = sMainControl->state;
    sMainControl->state = DWC_STATE_MATCHING;
    fn_804903EC((u8)(maxEntry - 1), (DWCMatchedSCCallbackView)fn_8048E7DC, 0,
        browserCallback, browserParam);
    return TRUE;
}

int DWC_CloseConnectionHard(u8 aid)
{
    GT2Connection connection;

    DWC_Printf(4, "!!DWC_CloseConnectionHard() was called!! aid = %d.\n", aid);
    if (sMainControl == NULL || DWCi_IsError() != 0
        || (sMainControl->state != DWC_STATE_MATCHING
            && sMainControl->state != DWC_STATE_CONNECTED))
    {
        DWC_Printf(4, "But ignored.\n");
        return -1;
    }
    connection = DWCi_GetGT2Connection(aid);
    if (connection == NULL)
    {
        DWC_Printf(4, "No connection!\n");
        return -2;
    }
    gt2CloseConnectionHard(connection);
    return 0;
}

int DWC_LoginAsync(const u16* ingamesn, void* unused,
    DWCLoginCallback loginCallback, void* loginParam)
{
    u32 len;

    DWC_Printf(4, "!!DWC_LoginAsync() was called!!\n");
    if (ingamesn == NULL)
    {
        DWC_Printf(8, "ingamesn is NULL!!\n");
        return FALSE;
    }
    if (DWCi_IsError() != 0 || sMainControl->state != DWC_STATE_INIT)
    {
        DWC_Printf(4, "But ignored.\n");
        return FALSE;
    }
    sMainControl->loginCallback = loginCallback;
    sMainControl->loginParam = loginParam;
    if (*ingamesn == 0)
    {
        len = 0;
    }
    else
    {
        memset(sMainControl->ingamesn, 0, sizeof(sMainControl->ingamesn));
        if ((u32)fn_8048A504(ingamesn) <= 0x19)
        {
            len = fn_8048A504(ingamesn);
        }
        else
        {
            len = 0x19;
        }
        memcpy(sMainControl->ingamesn, ingamesn, len * 2);
    }
    sMainControl->ingamesn[len] = 0;
    sMainControl->savedState = sMainControl->state;
    sMainControl->state = DWC_STATE_AVAILABLE_CHECK;
    GSIStartAvailableCheckA(sMainControl->gameName);
    return TRUE;
}
