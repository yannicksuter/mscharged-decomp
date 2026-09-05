#include <dwc/dwc_match.h>

#include <dwc/dwc_common.h>
#include <dwc/dwc_error.h>
#include <dwc/dwc_friend.h>
#include <dwc/dwc_main.h>
#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_report.h>
#include <gamespy/GP/gp.h>
#include <gamespy/gt2/gt2.h>
#include <gamespy/natneg/natneg.h>
#include <gamespy/qr2/qr2.h>
#include <gamespy/serverbrowsing/sb_serverbrowsing.h>
#include <revolution/os/OSTime.h>
#include <stdio.h>
#include <string.h>

#define DWC_ECODE_SEQ_MATCHING   (-80000)
#define DWC_ECODE_SEQ_UPDATE_SVR (-70000)
#define DWC_ECODE_GS_QR2         (-4000)
#define DWC_ECODE_GS_SB          (-5000)
#define DWC_ECODE_GS_NN          (-6000)

typedef enum
{
    DWC_MATCH_RESET_ALL = 0,
    DWC_MATCH_RESET_RESTART,
    DWC_MATCH_RESET_CONTINUE,
    DWC_MATCH_RESET_NUM
} DWCMatchResetLevel;

typedef void (*DWCMatchedSCCallback)(int error, BOOL cancel, BOOL self,
    BOOL isServer, int index, void* param);
typedef int (*DWCEvalPlayerCallback)(int index, void* param);

typedef struct DWCstConnectionInfo
{
    u8 index;
    u8 aid;
    u16 reserve;
    void* param;
} DWCConnectionInfo;

typedef struct DWCMatchControlView
{
    GPConnection* _00;
    GT2Socket* pGT2Socket;
    GT2ConnectionCallbacks* _08;
    u8 _0C;
    u8 _0D;
    u8 _0E;
    u8 _0F;
    qr2_t qr2;
    vu8 _14;
    vu8 matchType;
    u8 _16;
    u8 _17;
    u8 _18;
    u8 _19;
    u16 _1A;
    int _1C;
    volatile int _20;
    u32 _24[32];
    u16 _A4[32];
    ServerBrowser sb;
    int _E8;
    u8 _EC[4];
    s64 _F0;
    int pidList[32];
    s64 _178;
    u8 _180;
    u8 _181;
    u16 _182;
    int _184;
    s64 _188;
    s64 _190;
    u8 _198;
    u8 _199;
    u16 _19A;
    u32 _19C;
    int _1A0;
    int state;
    u8 _1A8;
    u8 _1A9;
    u8 _1AA;
    u8 _1AB;
    u8 closeState;
    u8 _1AD;
    u8 _1AE;
    u8 _1AF;
    u8 _1B0;
    u8 _1B1;
    u8 _1B2;
    u8 _1B3;
    u16 _1B4;
    u16 _1B6;
    u16 _1B8;
    u16 _1BA;
    int _1BC;
    int _1C0;
    u8 _1C4[4];
    s64 _1C8;
    int _1D0;
    u8 _1D4[4];
    s64 _1D8;
    u32 _1E0;
    int _1E4;
    int _1E8;
    u8 _1EC[4];
    s64 _1F0;
    s64 _1F8;
    s64 _200;
    s64 _208;
    int _210;
    int _214;
    int _218;
    int _21C;
    u32 ipList[32];
    u16 portList[32];
    u8 aidList[32];
    u32 aidBitmap;
    const char* gamename;
    const char* secretKey;
    int _30C;
    int _310;
    u8 _314[0x40];
    int _354;
    u8 _358[0x84];
    u32 _3DC;
    u8 _3E0;
    u8 _3E1;
    u16 _3E2;
    u32 _3E4;
    u8 _3E8[0x80];
    int _468;
    int _46C;
    s64 _470;
    DWCMatchedSCCallback matchedCallback;
    void* matchedParam;
    void* _480;
    void* _484;
    DWCEvalPlayerCallback _488;
    void* _48C;
    void (*_490)(void* param);
    void* _494;
} DWCMatchControlView;

typedef struct DWCMatchKeyDataView
{
    u8 keyID;
    u8 _01;
    u16 _02;
    char* keyString;
    const void* value;
} DWCMatchKeyDataView;

typedef struct DWCMatchNNInfoView
{
    u8 isClient;
    u8 retryCount;
    u16 port;
    u32 ip;
    int cookie;
} DWCMatchNNInfoView;

typedef struct DWCMatchSBMessageView
{
    char magic[4];
    u32 version;
    u8 command;
    u8 length;
    u16 port;
    u32 ip;
    u32 pid;
} DWCMatchSBMessageView;

typedef struct DWCMatchOptMinCompleteView
{
    u8 valid;
    u8 minEntry;
    u8 _02;
    u8 _03;
    u32 timeout;
    u32 _08;
    u32 _0C;
    s64 startTime;
    s64 _18;
} DWCMatchOptMinCompleteView;

typedef struct DWCMatchOptSCBlockView
{
    u8 valid;
    u8 _01;
    u16 _02;
} DWCMatchOptSCBlockView;

GPResult fn_8048AFCC(GPEnum status, const char* statusString,
    const char* locationString);
int fn_8048AEC4(int profileId);
void fn_8048C54C(int error, int errorCode);
void fn_8049AE0C(int type, int aid, const void* data, int size);
void fn_80499A30(SBServer server);
int fn_80499CA8(int sort);
void fn_80499E90(void);

extern const int lbl_804F31F8[];

void SBServerAddIntKeyValue(SBServer server, const char* keyname, int value);

void fn_804970B0(void);
void fn_80491EDC(int error, int errorCode);
int fn_80493128(int type);
void fn_80492D4C(int index);
int fn_80498B24(int error);
int fn_80498C78(int error);
int fn_80498DF0(int error);
int fn_80495B90(int profileId, int arg1);
int fn_80496000(int arg0);
int fn_80496188(int profileId);
int fn_80496520(int profileId);
int fn_80496740(int arg0);
int fn_80497654(void);
int fn_804979F4(void);
int fn_8049811C(void);
void fn_80498440(void);
GPResult fn_8049382C(int command, u32 profileId, u32 ip, u16 port,
    const u32* data, int count);
SBError fn_80493A54(u8 command, u32 ip, u16 port, const u32* data,
    int count);
void fn_8049A048(int keyid, qr2_buffer_t outbuf, void* userdata);
void fn_8049A27C(int keyid, int index, qr2_buffer_t outbuf, void* userdata);
void fn_8049A280(int keyid, int index, qr2_buffer_t outbuf, void* userdata);
void fn_8049A284(qr2_key_type keytype, qr2_keybuffer_t keybuffer,
    void* userdata);
int fn_8049A374(qr2_key_type keytype, void* userdata);
void fn_8049A37C(qr2_error_t error, gsi_char* errmsg, void* userdata);
void fn_8049A3E4(unsigned int ip, unsigned short port, void* userdata);
void fn_8049A444(int cookie, void* userdata);
void fn_8049A530(gsi_char* data, int len, void* userdata);

void fn_804993C8(ServerBrowser sb, SBCallbackReason reason,
    SBServer server, void* instance);
void fn_8049A6E8(NegotiateState state, void* userdata);
void fn_8049A700(NegotiateResult result, SOCKET gamesocket,
    struct sockaddr_in* remoteaddr, void* userdata);

int fn_8048E27C(void);
GT2Connection* fn_8048E320(int index);
DWCConnectionInfo* fn_8048E430(int index);
void* fn_8048C530(void);
GT2Connection* fn_8048E334(int index);
int fn_8048AE58(u8 friendIndex);
void DWC_CloseConnectionHard(u8 aid);
u8 fn_80495530(u32 profileId, u32 ip, u16 port, u32 version, BOOL resend);
GPResult fn_80495884(u32 profileId, u32 ip, u16 port);
int fn_80495D7C(int arg0, BOOL keepIndex, u32 profileId);
int fn_80497CB4(u32 profileId, int command, u32 data0);
void fn_804978F0(u8 aid, int type);
int fn_80498FB8(int error);
int fn_8049925C(int error);
void fn_804974BC(DWCMatchResetLevel level);

void fn_804929E0(void);

GPResult fn_80492BA0(void);
int fn_80493B94(char* dstMsg, const char* srcMsg, int index);
BOOL fn_80493C58(u8 command, u32 profileId, u32 ip, u16 port, u32* data,
    int count);
int fn_80493434(int isRetry, int cookie, SBServer server);
BOOL fn_80492D3C(void);
static DWCMatchControlView* DWCi_GetMatchCnt(void);
static void DWCi_FinishCancelMatching(void);
static void DWCi_CloseAllConnectionsByTimeout(void);
static void DWCi_ClearGameMatchKeys(void);
static int DWCi_ChangeToClient(void);
static int DWCi_GetDefaultMatchFilter(
    char* filter, int profileID, u8 numEntry, u8 matchType);

static char* stpAddFilter;
DWCMatchOptMinCompleteView* lbl_806E2EEC;
u32 lbl_806E2EF0;
u32 lbl_806E2EF4;
static DWCMatchControlView* lbl_806E2EF8;
DWCMatchOptSCBlockView lbl_806E2EFC;

u8 lbl_806C9920[0x100];
DWCMatchKeyDataView lbl_806C9A20[154];
u8 lbl_806CA158[0x20];

static u32 fn_ByteSwap32(u32 value)
{
    value = ((value >> 8) & 0x00FF00FF) | ((value << 8) & 0xFF00FF00);
    return (value >> 16) | (value << 16);
}

static s64 fn_ElapsedMSec(s64 time)
{
    return (OSGetTime() - time) / (OS_BUS_CLOCK_SPEED / 4 / 1000);
}

BOOL DWC_RegisterMatchingStatus(void)
{
    DWC_Printf(4, "!!DWC_RegisterMatchingStatus() was called!!\n");
    DWC_Printf(4, "But ignored.\n");
    return TRUE;
}

void ClearMOSCConnectBlock(void)
{
    DWC_Printf(4, "[OPT_SC_BLOCK] ClearMOSCConnectBlock\n");
    lbl_806E2EFC.valid = 0;
    lbl_806E2EFC._01 = 0;
    lbl_806E2EFC._02 = 0;
}

BOOL DWC_CancelMatch(void)
{
    if (DWC_IsValidMatchCancel())
    {
        fn_804970B0();
        return TRUE;
    }
    DWC_Printf(4, "Now unable to cancel.\n");
    return FALSE;
}

BOOL DWC_IsValidMatchCancel(void)
{
    if (DWCi_IsError())
    {
        return FALSE;
    }
    if (lbl_806E2EF8 == NULL || DWC_GetState() != DWC_STATE_MATCHING
        || lbl_806E2EF8->_1AD != 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL fn_8048F648(void (*callback)(void* param), void* param)
{
    if (DWCi_IsError() || lbl_806E2EF8 == NULL
        || lbl_806E2EF8->matchType != 2
        || (DWC_GetState() != DWC_STATE_MATCHING
            && DWC_GetState() != DWC_STATE_CONNECTED))
    {
        return FALSE;
    }

    if (lbl_806E2EF8->state == 10)
    {
        lbl_806E2EF8->_16 = lbl_806E2EF8->_0D;
        fn_80492BA0();
        if (callback != NULL)
        {
            callback(param);
        }
    }
    else
    {
        lbl_806E2EF8->_490 = callback;
        lbl_806E2EF8->_494 = param;
        lbl_806E2EF8->_1B2 = 1;
    }
    return TRUE;
}

u8 DWC_AddMatchKeyInt(u8 keyID, const char* keyString, const int* value)
{
    int key = keyID;
    int i;

    if (keyString == NULL || value == NULL)
    {
        return 0;
    }

    if (keyID >= 0x64 && lbl_806C9A20[keyID - 0x64].keyID != 0)
    {
        if (lbl_806C9A20[keyID - 0x64].keyString != NULL
            && strcmp(lbl_806C9A20[keyID - 0x64].keyString, keyString) != 0)
        {
            return 0;
        }
    }
    else
    {
        for (i = 0; i < 154; i++)
        {
            if (lbl_806C9A20[(u8)i].keyID == 0)
            {
                key = (u8)(i + 0x64);
                break;
            }
        }
        if (i == 154)
        {
            key = 0;
        }
        if (key == 0)
        {
            return 0;
        }
    }

    lbl_806C9A20[(u8)key - 0x64].keyID = key;
    lbl_806C9A20[(u8)key - 0x64]._01 = 0;
    lbl_806C9A20[(u8)key - 0x64]._02 = 0;
    if (lbl_806C9A20[(u8)key - 0x64].keyString != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, lbl_806C9A20[(u8)key - 0x64].keyString,
            0);
    }
    lbl_806C9A20[(u8)key - 0x64].keyString
        = DWC_Alloc(DWC_ALLOCTYPE_BASE, strlen(keyString) + 1);
    if (lbl_806C9A20[(u8)key - 0x64].keyString == NULL)
    {
        fn_80491EDC(DWC_ERROR_FATAL,
            DWC_ECODE_SEQ_MATCHING + DWC_ECODE_TYPE_ALLOC);
        return 0;
    }
    strcpy(lbl_806C9A20[(u8)key - 0x64].keyString, keyString);
    DWC_Printf(4, "DWC_AddMatchKeyInt: key='%s', value=%d\n", keyString,
        *value);
    lbl_806C9A20[(u8)key - 0x64].value = value;
    qr2_register_keyA((u8)key, lbl_806C9A20[(u8)key - 0x64].keyString);
    return key;
}

u8 DWC_AddMatchKeyString(u8 keyID, const char* keyString,
    const char* const* value)
{
    int key = keyID;
    int i;

    if (keyString == NULL || value == NULL)
    {
        return 0;
    }

    if (keyID >= 0x64 && lbl_806C9A20[keyID - 0x64].keyID != 0)
    {
        if (lbl_806C9A20[keyID - 0x64].keyString != NULL
            && strcmp(lbl_806C9A20[keyID - 0x64].keyString, keyString) != 0)
        {
            return 0;
        }
    }
    else
    {
        for (i = 0; i < 154; i++)
        {
            if (lbl_806C9A20[(u8)i].keyID == 0)
            {
                key = (u8)(i + 0x64);
                break;
            }
        }
        if (i == 154)
        {
            key = 0;
        }
        if (key == 0)
        {
            return 0;
        }
    }

    lbl_806C9A20[(u8)key - 0x64].keyID = key;
    lbl_806C9A20[(u8)key - 0x64]._01 = 1;
    lbl_806C9A20[(u8)key - 0x64]._02 = 0;
    if (lbl_806C9A20[(u8)key - 0x64].keyString != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, lbl_806C9A20[(u8)key - 0x64].keyString,
            0);
    }
    lbl_806C9A20[(u8)key - 0x64].keyString
        = DWC_Alloc(DWC_ALLOCTYPE_BASE, strlen(keyString) + 1);
    if (lbl_806C9A20[(u8)key - 0x64].keyString == NULL)
    {
        fn_80491EDC(DWC_ERROR_FATAL,
            DWC_ECODE_SEQ_MATCHING + DWC_ECODE_TYPE_ALLOC);
        return 0;
    }
    strcpy(lbl_806C9A20[(u8)key - 0x64].keyString, keyString);
    DWC_Printf(4, "DWC_AddMatchKeyString: key='%s' value='%s'\n", keyString,
        *value);
    lbl_806C9A20[(u8)key - 0x64].value = value;
    qr2_register_keyA((u8)key, lbl_806C9A20[(u8)key - 0x64].keyString);
    return key;
}

int fn_8048FACC(int index, const char* key, int defaultValue)
{
    SBServer server;

    if (lbl_806E2EF8 == NULL || DWCi_IsError())
    {
        return defaultValue;
    }
    server = ServerBrowserGetServer(lbl_806E2EF8->sb, index);
    if (server != NULL)
    {
        return SBServerGetIntValueA(server, key, defaultValue);
    }
    return defaultValue;
}

int DWC_SetMatchingOption(int option, const void* optval)
{
    DWC_Printf(4, "!!DWC_SetMatchingOption() was called!! type %d\n", option);

    if (lbl_806E2EF8 == NULL)
    {
        return 1;
    }
    if (optval == NULL)
    {
        return 3;
    }

    switch (option)
    {
    case DWC_MATCH_OPTION_MIN_COMPLETE:
        if (lbl_806E2EF8->state == 19)
        {
            return 1;
        }
        if (((const DWCMatchOptMinComplete*)optval)->valid != 0)
        {
            if (((const DWCMatchOptMinComplete*)optval)->minEntry <= 1)
            {
                return 3;
            }
        }
        if (lbl_806E2EEC == NULL)
        {
            lbl_806E2EEC
                = DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(*lbl_806E2EEC));
            if (lbl_806E2EEC == NULL)
            {
                return 4;
            }
        }
        lbl_806E2EEC->valid
            = ((const DWCMatchOptMinComplete*)optval)->valid;
        lbl_806E2EEC->minEntry
            = ((const DWCMatchOptMinComplete*)optval)->minEntry;
        lbl_806E2EEC->_02 = 0;
        lbl_806E2EEC->_03 = 0;
        lbl_806E2EEC->timeout
            = ((const DWCMatchOptMinComplete*)optval)->timeout;
        lbl_806E2EEC->_08 = 0;
        lbl_806E2EEC->_0C = 0;
        lbl_806E2EEC->startTime = OSGetTime();
        lbl_806E2EEC->_18 = OSGetTime();
        return 0;

    case DWC_MATCH_OPTION_SC_CONNECT_BLOCK:
        lbl_806E2EFC.valid
            = ((const DWCMatchOptSCConnectBlock*)optval)->valid != 0;
        lbl_806E2EFC._01 = 0;
        return 0;

    default:
        return 2;
    }
}

int fn_8048FCD8(s64* elapsedOut)
{
    s64 elapsed;

    if (lbl_806E2EEC == NULL || lbl_806E2EEC->valid == 0)
    {
        return -1;
    }

    elapsed = (OSGetTime() - lbl_806E2EEC->startTime)
        / (OS_BUS_CLOCK_SPEED / 4 / 1000);
    if (elapsedOut != NULL)
    {
        *elapsedOut = elapsed;
    }
    if ((u64)elapsed >= lbl_806E2EEC->timeout)
    {
        return 1;
    }
    return 0;
}

void fn_8048FD9C(DWCMatchControlView* control, void* p0, GT2Socket* socket,
    void* p2, const char* gamename, const char* secretKey, int p5, int p6)
{
    lbl_806E2EF8 = control;
    lbl_806E2EF8->_00 = p0;
    lbl_806E2EF8->pGT2Socket = socket;
    lbl_806E2EF8->_08 = p2;
    lbl_806E2EF8->qr2 = NULL;
    lbl_806E2EF8->_1C = 0;
    lbl_806E2EF8->_1A = 0;
    lbl_806E2EF8->sb = NULL;
    lbl_806E2EF8->state = 0;
    lbl_806E2EF8->_0F = 0;
    lbl_806E2EF8->_19 = 0;
    lbl_806E2EF8->_1B3 = 0;
    lbl_806E2EF8->_1BA = 0;
    lbl_806E2EF8->_1E8 = 0;
    lbl_806E2EF8->_210 = 0;
    lbl_806E2EF8->gamename = gamename;
    lbl_806E2EF8->secretKey = secretKey;
    lbl_806E2EF8->_30C = p5;
    lbl_806E2EF8->_310 = p6;
    memset(lbl_806E2EF8->_314, 0, sizeof(lbl_806E2EF8->_314));
    lbl_806E2EF8->_354 = 0;
    lbl_806E2EF8->matchedCallback = NULL;
    lbl_806E2EF8->matchedParam = NULL;
    lbl_806E2EF8->_488 = NULL;
    lbl_806E2EF8->_48C = NULL;
    lbl_806E2EF8->_490 = NULL;
    lbl_806E2EF8->_494 = NULL;
    lbl_806E2EF8->_178 = 0;

    DWCi_ClearGameMatchKeys();

    lbl_806E2EFC.valid = 0;
    lbl_806E2EFC._01 = 0;
    lbl_806E2EFC._02 = 0;

    fn_80492D4C(0);
}

int fn_8048FF20(int type)
{
    qr2_error_t error;
    int i;
    u16 port;
    SOCKET sock;

    if (lbl_806E2EF8->qr2 != NULL)
    {
        DWC_Printf(0x40, "QR2 is already set up.\n");
        return 0;
    }
    lbl_806E2EF8->_210 = type;

    for (i = 0; i < 5; i++)
    {
        port = gt2GetLocalPort(*lbl_806E2EF8->pGT2Socket);
        sock = gt2GetSocketSOCKET(*lbl_806E2EF8->pGT2Socket);
        error = qr2_init_socketA(&lbl_806E2EF8->qr2, sock, port,
            lbl_806E2EF8->gamename, lbl_806E2EF8->secretKey, 1, 1,
            fn_8049A048, fn_8049A27C, fn_8049A280, fn_8049A284, fn_8049A374,
            fn_8049A37C, NULL);
        if (error == e_qrnoerror)
        {
            break;
        }
        qr2_shutdown(lbl_806E2EF8->qr2);
        lbl_806E2EF8->qr2 = NULL;
        if (error != e_qrbinderror || i == 4)
        {
            fn_80498DF0(error);
            return error;
        }
    }

    lbl_806E2EF8->_1C = 0;
    lbl_806E2EF8->_1A = 0;
    qr2_register_publicaddress_callback(lbl_806E2EF8->qr2, fn_8049A3E4);
    qr2_register_natneg_callback(lbl_806E2EF8->qr2, fn_8049A444);
    qr2_register_clientmessage_callback(lbl_806E2EF8->qr2, fn_8049A530);
    qr2_send_statechanged(lbl_806E2EF8->qr2);
    return error;
}

void DWCi_ConnectToAnybodyAsync(u8 numEntry, const char* addFilter,
    DWCMatchedSCCallback matchedCallback, void* matchedParam,
    DWCEvalPlayerCallback evalCallback, void* evalParam)
{
    char filter[0x100];
    unsigned long addFilterLenMax;

    if (stpAddFilter != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    if (addFilter != NULL)
    {
        addFilterLenMax = 0x100
            - DWCi_GetDefaultMatchFilter(filter, -1, 32, 3)
            - strlen(" and ()");
        stpAddFilter = DWC_Alloc(DWC_ALLOCTYPE_BASE, addFilterLenMax);
        if (stpAddFilter == NULL)
        {
            fn_80491EDC(DWC_ERROR_FATAL,
                DWC_ECODE_SEQ_MATCHING + DWC_ECODE_TYPE_ALLOC);
            return;
        }
        memcpy(stpAddFilter, addFilter, addFilterLenMax);
        stpAddFilter[addFilterLenMax - 1] = '\0';
    }

    fn_80492D4C(0);
    DWCi_GetMatchCnt()->matchType = 0;
    DWCi_GetMatchCnt()->_16 = numEntry;
    DWCi_GetMatchCnt()->matchedCallback = matchedCallback;
    DWCi_GetMatchCnt()->matchedParam = matchedParam;
    DWCi_GetMatchCnt()->_181 = 0;
    DWCi_GetMatchCnt()->aidList[0] = 0;
    qr2_register_keyA(0x32, "dwc_pid");
    qr2_register_keyA(0x33, "dwc_mtype");
    qr2_register_keyA(0x34, "dwc_mresv");
    qr2_register_keyA(0x35, "dwc_mver");
    qr2_register_keyA(0x36, "dwc_eval");
    DWCi_GetMatchCnt()->_488 = evalCallback;
    DWCi_GetMatchCnt()->_48C = evalParam;
    DWCi_GetMatchCnt()->state = 2;

    if (DWCi_GetMatchCnt()->sb == NULL)
    {
        DWCi_GetMatchCnt()->sb = ServerBrowserNewA(
            DWCi_GetMatchCnt()->gamename, DWCi_GetMatchCnt()->gamename,
            DWCi_GetMatchCnt()->secretKey, 0, 20, 1, SBFalse, fn_804993C8,
            NULL);
    }
    if (DWCi_GetMatchCnt()->sb == NULL)
    {
        if (fn_80498C78(5) != 0)
        {
            return;
        }
    }

    fn_8048AFCC(3, "", NULL);
    if (fn_80498C78(fn_80493128(DWCi_GetMatchCnt()->_210)) != 0)
    {
        return;
    }
    if (DWCi_GetMatchCnt()->qr2 == NULL)
    {
        fn_8048FF20(DWCi_GetMatchCnt()->_210);
    }

    if (lbl_806E2EEC != NULL && lbl_806E2EEC->valid != 0)
    {
        lbl_806E2EEC->_08 = 0;
        lbl_806E2EEC->_0C = 0;
        lbl_806E2EEC->_02 = 0;
        lbl_806E2EEC->_18 = OSGetTime();
        lbl_806E2EEC->startTime = OSGetTime();
    }
}

void fn_804903EC(u8 a0, DWCMatchedSCCallback callback, void* param,
    void* callback2, void* param2)
{
    fn_80492D4C(0);
    lbl_806E2EF8->matchType = 2;
    lbl_806E2EF8->_16 = a0;
    lbl_806E2EF8->matchedCallback = callback;
    lbl_806E2EF8->matchedParam = param;
    lbl_806E2EF8->_181 = 0;
    lbl_806E2EF8->aidList[0] = 0;
    qr2_register_keyA(0x32, "dwc_pid");
    qr2_register_keyA(0x33, "dwc_mtype");
    qr2_register_keyA(0x34, "dwc_mresv");
    qr2_register_keyA(0x35, "dwc_mver");
    qr2_register_keyA(0x36, "dwc_eval");
    lbl_806E2EF8->_480 = callback2;
    lbl_806E2EF8->_484 = param2;
    lbl_806E2EF8->pidList[0] = lbl_806E2EF8->_210;
    lbl_806E2EF8->aidBitmap = 1;
    lbl_806E2EF8->_0E = 0;
    lbl_806E2EFC._01 = 0;
    lbl_806E2EF8->state = 10;

    if (fn_80498B24(fn_80492BA0()) == 0)
    {
        if (lbl_806E2EF8->qr2 == NULL)
        {
            fn_8048FF20(lbl_806E2EF8->_210);
        }
    }
}

void fn_804905D0(int serverPid, DWCMatchedSCCallback matchedCallback,
    void* matchedParam, void* newClientCallback, void* newClientParam)
{
    int result;
    GPResult gpResult;

    fn_80492D4C(0);
    DWCi_GetMatchCnt()->matchType = 3;
    DWCi_GetMatchCnt()->_16 = 0;
    DWCi_GetMatchCnt()->matchedCallback = matchedCallback;
    DWCi_GetMatchCnt()->matchedParam = matchedParam;
    DWCi_GetMatchCnt()->_181 = 0;
    DWCi_GetMatchCnt()->aidList[0] = 0;
    qr2_register_keyA(0x32, "dwc_pid");
    qr2_register_keyA(0x33, "dwc_mtype");
    qr2_register_keyA(0x34, "dwc_mresv");
    qr2_register_keyA(0x35, "dwc_mver");
    qr2_register_keyA(0x36, "dwc_eval");
    DWCi_GetMatchCnt()->_480 = newClientCallback;
    DWCi_GetMatchCnt()->_484 = newClientParam;
    DWCi_GetMatchCnt()->_17 = 1;
    DWCi_GetMatchCnt()->_20 = DWCi_GetMatchCnt()->_210;
    DWCi_GetMatchCnt()->pidList[0] = serverPid;
    DWCi_GetMatchCnt()->state = 4;

    if (DWCi_GetMatchCnt()->sb == NULL)
    {
        DWCi_GetMatchCnt()->sb = ServerBrowserNewA(
            DWCi_GetMatchCnt()->gamename, DWCi_GetMatchCnt()->gamename,
            DWCi_GetMatchCnt()->secretKey, 0, 20, 1, SBFalse, fn_804993C8,
            NULL);
    }
    if (DWCi_GetMatchCnt()->sb == NULL)
    {
        if (fn_80498C78(5) != 0)
        {
            return;
        }
    }

    gpResult = fn_8048AFCC(5, "", NULL);
    if (fn_80498B24(gpResult) != 0)
    {
        return;
    }
    if (DWCi_GetMatchCnt()->qr2 == NULL)
    {
        if (fn_8048FF20(DWCi_GetMatchCnt()->_210) != 0)
        {
            return;
        }
    }

    result = fn_80495B90(DWCi_GetMatchCnt()->pidList[0], 0);
    if (DWCi_GetMatchCnt()->matchType == 0)
    {
        result = fn_80498C78(result);
    }
    else
    {
        result = fn_80498B24(result);
    }
    if (result != 0)
    {
        return;
    }
}

void fn_8049079C(BOOL enableProcess)
{
    int state;

    if (lbl_806E2EF8 == NULL)
    {
        return;
    }
    if (DWCi_IsError())
    {
        return;
    }

    if (!enableProcess)
    {
        if (lbl_806E2EF8->qr2 != NULL)
        {
            qr2_think(lbl_806E2EF8->qr2);
        }
        if (lbl_806E2EF8->pGT2Socket == NULL)
        {
            return;
        }
        gt2Think(*lbl_806E2EF8->pGT2Socket);
        return;
    }

    state = lbl_806E2EF8->state;
    if (state == 0)
    {
        return;
    }

    switch (state)
    {
    case 4:
        if (lbl_806E2EF8->_1D0 != 0
            && fn_ElapsedMSec(lbl_806E2EF8->_1D8) >= lbl_806E2EF8->_1D0)
        {
            lbl_806E2EF8->_1D0 = 0;
            if (lbl_806E2EF8->matchType == 3)
            {
                DWC_Printf(0x40, "Timeout: wait server response %d/%d.\n",
                    lbl_806E2EF8->_1AE, 5);
                lbl_806E2EF8->_1AE++;
                if (lbl_806E2EF8->_1AE > 5)
                {
                    fn_80491EDC(DWC_ERROR_TYPE_6,
                        DWC_ECODE_SEQ_MATCHING - 430);
                    return;
                }
                if (lbl_806E2EF8->matchType == 0)
                {
                    if (fn_80498C78(fn_80495B90(lbl_806E2EF8->pidList[0], 0))
                        != 0)
                    {
                        return;
                    }
                }
                else if (fn_80498B24(fn_80495B90(lbl_806E2EF8->pidList[0], 0))
                    != 0)
                {
                    return;
                }
            }
            else
            {
                DWC_Printf(0x40,
                    "NN resv(with %u) timed out. Try next server.\n",
                    lbl_806E2EF8->pidList[0]);
                if (fn_80496000(0) == 0)
                {
                    return;
                }
            }
        }
        if (lbl_806E2EF8->_1C0 != 0
            && fn_ElapsedMSec(lbl_806E2EF8->_1C8)
                >= lbl_806E2EF8->_0D * 3000 + 3000)
        {
            if (lbl_806E2EF8->matchType == 0)
            {
                if (fn_80498C78(fn_80495B90(lbl_806E2EF8->pidList[0], 0))
                    != 0)
                {
                    return;
                }
            }
            else if (fn_80498B24(fn_80495B90(lbl_806E2EF8->pidList[0], 0))
                != 0)
            {
                return;
            }
        }
        break;

    case 2:
    case 3:
    case 5:
        if (lbl_806E2EF8->_E8 > 0)
        {
            int limit;

            if (state == 3)
            {
                limit = lbl_806E2EF8->_0D * 3000 + 3000;
            }
            else if (lbl_806E2EF8->_E8 == 1)
            {
                limit = 1000;
            }
            else
            {
                limit = 3000;
            }
            if (fn_ElapsedMSec(lbl_806E2EF8->_F0) >= limit)
            {
                if (fn_80498C78(fn_80493128(lbl_806E2EF8->_214)) != 0)
                {
                    return;
                }
                lbl_806E2EF8->_E8 = 0;
            }
        }
        break;

    case 7:
        if (lbl_806E2EF8->_190 != 0)
        {
            if (fn_ElapsedMSec(lbl_806E2EF8->_190) >= 25000)
            {
                DWC_Printf(0x40, "Timeout: wait gt2Connect().\n");
                lbl_806E2EF8->_190 = 0;
                if (fn_80496520(lbl_806E2EF8->pidList[0]) == 0)
                {
                    return;
                }
            }
        }
        else if (lbl_806E2EF8->_3E0 == 6
            && fn_ElapsedMSec(lbl_806E2EF8->_470) >= 6000)
        {
            DWC_Printf(4, "RTT Timeout with DWC_MATCH_STATE_CL_GT2.\n");
            lbl_806E2EF8->_3E1++;
            if (lbl_806E2EF8->_3E1 > 5)
            {
                lbl_806E2EF8->_3E0 = 0xFF;
                lbl_806E2EF8->_3E1 = 0;
                DWC_Printf(0x40, "Stop resending command %d.\n", 6);
                if (fn_80496520(lbl_806E2EF8->pidList[0]) == 0)
                {
                    return;
                }
            }
            else
            {
                GPResult result = fn_8049382C(6, lbl_806E2EF8->_468,
                    lbl_806E2EF8->_3E4, lbl_806E2EF8->_3E2,
                    (u32*)lbl_806E2EF8->_3E8, lbl_806E2EF8->_46C);
                if (lbl_806E2EF8->matchType == 0)
                {
                    if (fn_80498C78(result) != 0)
                    {
                        return;
                    }
                }
                else if (fn_80498B24(result) != 0)
                {
                    return;
                }
            }
        }
        break;

    case 11:
        if (lbl_806E2EF8->_3E0 == 2)
        {
            if (lbl_806E2EF8->matchType == 0
                && fn_ElapsedMSec(lbl_806E2EF8->_470) >= 6000)
            {
            }
            else if (lbl_806E2EF8->matchType == 0
                || fn_ElapsedMSec(lbl_806E2EF8->_470) < 19000)
            {
                break;
            }
            DWC_Printf(0x40, "Reservation timeout. Cancel reservation.\n");
            lbl_806E2EF8->_3E0 = 0xFF;
            lbl_806E2EF8->_3E1 = 0;
            if (fn_80496188(lbl_806E2EF8->pidList[lbl_806E2EF8->_0D + 1])
                == 0)
            {
                return;
            }
        }
        break;

    case 13:
        if (lbl_806E2EF8->_3E0 == 8
            && fn_ElapsedMSec(lbl_806E2EF8->_470) >= 30000)
        {
            lbl_806E2EF8->_3E1++;
            if (lbl_806E2EF8->_3E1 != 0)
            {
                lbl_806E2EF8->_3E0 = 0xFF;
                DWC_Printf(0x40, "Wait clients connecting timeout.\n");
                lbl_806E2EF8->_3E1 = 0;
                if (lbl_806E2EF8->matchType == 2)
                {
                    if (fn_80496188(lbl_806E2EF8->pidList[lbl_806E2EF8->_0D])
                        == 0)
                    {
                        return;
                    }
                }
                else if (lbl_806E2EF8->matchType == 2
                    || lbl_806E2EF8->matchType == 3)
                {
                    DWC_Printf(8,
                        "DWCi_RestartFromTimeout() shouldn't be called.\n");
                }
                else
                {
                    lbl_806E2EF8->closeState = 2;
                    gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
                    lbl_806E2EF8->closeState = 0;
                    DWC_Printf(0x40,
                        "Closed all connections and restart matching.\n");
                    fn_804974BC(1);
                }
            }
            else
            {
                GPResult result = fn_8049382C(8, lbl_806E2EF8->_468,
                    lbl_806E2EF8->_3E4, lbl_806E2EF8->_3E2,
                    (u32*)lbl_806E2EF8->_3E8, lbl_806E2EF8->_46C);
                if (lbl_806E2EF8->matchType == 0)
                {
                    if (fn_80498C78(result) != 0)
                    {
                        return;
                    }
                }
                else if (fn_80498B24(result) != 0)
                {
                    return;
                }
            }
        }
        break;

    case 1:
        if (DWC_GetState() != DWC_STATE_MATCHING)
        {
            break;
        }
        if (fn_ElapsedMSec(lbl_806E2EF8->_208) >= 30000)
        {
            DWC_Printf(0x40, "No data from server %d/%d.\n",
                lbl_806E2EF8->_1B1, 5);
            if (lbl_806E2EF8->_1B1 >= 5)
            {
                DWC_Printf(0x40,
                    "Timeout: Connection to server was shut down.\n");
                if (fn_80496520(lbl_806E2EF8->pidList[0]) == 0)
                {
                    return;
                }
            }
            else
            {
                GPResult result = fn_8049382C(0x40,
                    lbl_806E2EF8->pidList[0], lbl_806E2EF8->_24[0],
                    lbl_806E2EF8->_A4[0], NULL, 0);
                if (lbl_806E2EF8->matchType == 0)
                {
                    if (fn_80498C78(result) != 0)
                    {
                        return;
                    }
                }
                else if (fn_80498B24(result) != 0)
                {
                    return;
                }
                lbl_806E2EF8->_1B1++;
                lbl_806E2EF8->_208 = OSGetTime()
                    - (s64)(OS_BUS_CLOCK_SPEED / 4 / 1000) * 24000;
            }
        }
        break;

    default:
        break;
    }

    if (lbl_806E2EF8->state == 11 || lbl_806E2EF8->state == 6)
    {
        if (lbl_806E2EF8->_188 != 0
            && fn_ElapsedMSec(lbl_806E2EF8->_188) >= 10000)
        {
            DWC_Printf(0x40, "Timeout : wait NN retry.\n");
            fn_8049A700(nr_deadbeatpartner, 0, NULL, &lbl_806E2EF8->_198);
        }
    }

    if (lbl_806E2EF8->sb != NULL)
    {
        lbl_806E2EF0 = 0;
        lbl_806E2EF4 = 0;
        ServerBrowserThink(lbl_806E2EF8->sb);
        if (lbl_806E2EF4 != 0)
        {
            ServerBrowserFree(lbl_806E2EF8->sb);
            lbl_806E2EF8->sb = NULL;
        }
        if (lbl_806E2EF8->sb != NULL
            && ServerBrowserState(lbl_806E2EF8->sb) != 0)
        {
            if (lbl_806E2EF8->_178 != 0 && OSGetTime() >= lbl_806E2EF8->_178)
            {
                fn_80491EDC(DWC_ERROR_TYPE_6,
                    DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_SB
                        + DWC_ECODE_TYPE_NETWORK);
                DWC_Printf(0x400, "ServerBrowserLimitUpdate timeout.\n");
            }
        }
    }

    if (lbl_806E2EF8->qr2 != NULL)
    {
        qr2_think(lbl_806E2EF8->qr2);
        if (lbl_806E2EF8->qr2->userstatechangerequested == 0)
        {
            switch (lbl_806E2EF8->matchType)
            {
            case 0:
            case 1:
                switch (lbl_806E2EF8->state)
                {
                case 1:
                case 2:
                case 3:
                case 4:
                case 6:
                case 11:
                    qr2_send_statechanged(lbl_806E2EF8->qr2);
                    break;
                default:
                    break;
                }
                break;
            case 2:
                if (lbl_806E2EF8->state == 11)
                {
                    qr2_send_statechanged(lbl_806E2EF8->qr2);
                }
                break;
            default:
                break;
            }
        }
    }

    NNThink();

    if (lbl_806E2EF8->pGT2Socket != NULL)
    {
        gt2Think(*lbl_806E2EF8->pGT2Socket);
    }

    if (lbl_806E2EF8->state == 18
        && fn_ElapsedMSec(lbl_806E2EF8->_200) >= 3000)
    {
        DWC_Printf(4, "RTT Timeout with DWCi_MatchProcess.\n");
        DWC_Printf(0x40, "Timeout : Wait prior profileID.\n");
        if (fn_80497654() != 0)
        {
            return;
        }
    }

    if (fn_804979F4() == 0)
    {
        return;
    }
    if (fn_8049811C() == 0)
    {
        return;
    }
    fn_80498440();

    if (lbl_806E2EF8->_1B2 != 0 && lbl_806E2EF8->state == 10)
    {
        lbl_806E2EF8->_16 = lbl_806E2EF8->_0D;
        fn_80492BA0();
        lbl_806E2EF8->_1B2 = 0;
        if (lbl_806E2EF8->_490 != NULL)
        {
            lbl_806E2EF8->_490(lbl_806E2EF8->_494);
        }
    }
}

GT2Bool fn_80491578(GT2Socket socket, unsigned int ip, unsigned short port,
    GT2Byte* message, int len)
{
    struct sockaddr_in addr;

    if (len == 0 || message == NULL)
    {
        return GT2False;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = 2;
    addr.sin_port = SOHtoNs(port);
    addr.sin_addr.addr = ip;

    if ((message[0] == 0xFE && message[1] == 0xFD) || message[0] == 0x5C)
    {
        DWC_Printf(0x40, "GT2 Unrecognized : Received QR2 data.\n");
        if (lbl_806E2EF8->qr2 != NULL)
        {
            qr2_parse_queryA(lbl_806E2EF8->qr2, (char*)message, len,
                (struct sockaddr*)&addr);
        }
        else
        {
            DWC_Printf(0x40, "  ignore qr2 message.\n");
        }
    }
    else if (memcmp(message, NNMagicData, 6) == 0)
    {
        DWC_Printf(0x40, "GT2 Unrecognized : Received NN data.\n");
        NNProcessData((char*)message, len, &addr);
    }
    else if (message[0] == 0xFE && message[1] == 0xFE)
    {
        DWC_Printf(0x80, "GT2 Unrecognized : Not Connected gt2 data.\n");
        return GT2False;
    }
    else
    {
        DWC_Printf(8, "GT2 Unrecognized : Received unrecognized data.\n");
        return GT2False;
    }
    return GT2True;
}

void fn_804916EC(GT2Socket socket, GT2Connection connection, unsigned int ip,
    unsigned short port, int latency, GT2Byte* message, int len)
{
    int index;
    GT2Connection* pGt2Con;
    DWCConnectionInfo* pConInfo;

    if (lbl_806E2EF8 != NULL && lbl_806E2EF8->state == 6)
    {
        return;
    }
    if (lbl_806E2EF8 == NULL || lbl_806E2EF8->state != 7
        || lbl_806E2EF8->_1AD != 0)
    {
        gt2Reject(connection, (GT2Byte*)"Init state", -1);
        DWC_Printf(0x80, "gt2Reject was called : Init state\n");
        return;
    }

    index = fn_8048E27C();
    if (index == -1)
    {
        gt2Reject(connection, (GT2Byte*)"Server full", -1);
        DWC_Printf(0x80, "gt2Reject was called : Server full\n");
        fn_80491EDC(DWC_ERROR_TYPE_6,
            DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_GT2 - 100);
        return;
    }

    if (lbl_806E2EF8->ipList[lbl_806E2EF8->_0D] != ip
        || lbl_806E2EF8->portList[lbl_806E2EF8->_0D] != SONtoHs(port))
    {
        if (message[0] != 0
            && strtoul((char*)message, NULL, 10)
                == lbl_806E2EF8->pidList[lbl_806E2EF8->_0D])
        {
            DWC_Printf(8, "gt2Connect() came before NN complete.\n");
            lbl_806E2EF8->ipList[lbl_806E2EF8->_0D] = ip;
            lbl_806E2EF8->portList[lbl_806E2EF8->_0D] = SONtoHs(port);
        }
        else
        {
            gt2Reject(connection, (GT2Byte*)"Unknown connect attempt", -1);
            DWC_Printf(8,
                "gt2Reject was called : Unknown connect attempt from %s\n",
                gt2AddressToString(ip, port, NULL));
            return;
        }
    }

    lbl_806E2EF8->_190 = 0;
    if (!gt2Accept(connection, lbl_806E2EF8->_08))
    {
        DWC_Printf(8, "Unexpected failure to gt2Accept.\n");
        fn_80491EDC(DWC_ERROR_TYPE_6, DWC_ECODE_SEQ_MATCHING - 410);
        return;
    }

    DWC_Printf(0x80, "Accepted connection from %s (latency %d)\n",
        gt2AddressToString(ip, port, NULL), latency);
    lbl_806E2EF8->_3E0 = 0xFF;
    lbl_806E2EF8->_3E1 = 0;
    if (lbl_806E2EF8->_0D == 0)
    {
        int half = latency >> 1;
        if (half < 0xFFFF)
        {
            lbl_806E2EF8->_1B4 = half;
        }
        else
        {
            lbl_806E2EF8->_1B4 = 0xFFFF;
        }
    }

    pGt2Con = fn_8048E320(index);
    pConInfo = fn_8048E430(index);
    *pGt2Con = connection;
    lbl_806E2EF8->_0D++;
    pConInfo->index = index;
    pConInfo->aid = lbl_806E2EF8->aidList[lbl_806E2EF8->_0D - 1];
    pConInfo->reserve = 0;
    pConInfo->param = NULL;
    gt2SetConnectionData(connection, pConInfo);
    fn_80496740(2);
}

void fn_80491AE0(GT2Connection connection, GT2Result result, GT2Byte* message,
    int len)
{
    char pidStr[12];
    int index;
    GT2Connection* pGt2Con;
    GT2Result gt2Result;
    DWCConnectionInfo* pConInfo;

    if (DWCi_GetMatchCnt() == NULL
        || (DWCi_GetMatchCnt()->state != 7
            && DWCi_GetMatchCnt()->state != 12))
    {
        DWC_Printf(0x80, "gt2ConnectedCallback: Already cancel\n");
        return;
    }

    if (result != GT2Success)
    {
        if (message == NULL)
        {
            message = (GT2Byte*)"";
        }
        DWC_Printf(0x80, "GT2 connect failed %d: %s\n", result, message);
        if (result == GT2DuplicateAddress)
        {
            return;
        }
        else if (result == GT2TimedOut)
        {
            DWCi_GetMatchCnt()->_0C++;
            if (DWCi_GetMatchCnt()->_0C > 5)
            {
                DWC_Printf(0x80, "gt2Connect() retry over.\n");
                DWCi_GetMatchCnt()->_0C = 0;
                fn_80496188(DWCi_GetMatchCnt()->pidList[DWCi_GetMatchCnt()->_14]);
                return;
            }
            DWC_Printf(0x80, "Retry to gt2Connect.\n");
            (void)snprintf(pidStr, sizeof(pidStr), "%u",
                DWCi_GetMatchCnt()->_210);
            gt2Result = gt2Connect(*DWCi_GetMatchCnt()->pGT2Socket, NULL,
                gt2AddressToString(
                    DWCi_GetMatchCnt()->ipList[DWCi_GetMatchCnt()->_14],
                    DWCi_GetMatchCnt()->portList[DWCi_GetMatchCnt()->_14], NULL),
                (GT2Byte*)pidStr, -1, 5000, DWCi_GetMatchCnt()->_08, GT2False);
            if (gt2Result == GT2OutOfMemory)
            {
                fn_8049925C(gt2Result);
                return;
            }
            else if (gt2Result == GT2Success)
            {
            }
            else if (fn_80496188(
                         DWCi_GetMatchCnt()->pidList[DWCi_GetMatchCnt()->_14])
                == 0)
            {
                return;
            }
        }
        else if (fn_80496188(DWCi_GetMatchCnt()->pidList[
                     DWCi_GetMatchCnt()->_0D + 1])
            == 0)
        {
            return;
        }
    }
    else
    {
        DWC_Printf(0x80, "GT2 connected.\n");
        index = fn_8048E27C();
        if (index == -1)
        {
            DWC_Printf(8,
                "Don't continue matching without closing connections!!\n");
            fn_80491EDC(DWC_ERROR_TYPE_6,
                DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_GT2 - 100);
            return;
        }

        pGt2Con = fn_8048E320(index);
        pConInfo = fn_8048E430(index);
        *pGt2Con = connection;
        DWCi_GetMatchCnt()->_0D++;
        pConInfo->index = index;
        pConInfo->reserve = 0;
        pConInfo->param = NULL;
        pConInfo->aid = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->_0D];
        gt2SetConnectionData(connection, pConInfo);

        if (DWCi_GetMatchCnt()->state == 12)
        {
            fn_80496740(0);
        }
        else
        {
            fn_80496740(1);
        }
    }
}

void fn_80491E18(GPConnection* connection, u32 profileId, char* message)
{
    char buf[16];
    u32 list[0x80];
    int i;

    DWC_Printf(0x40, "<GP> RECV-0x%02x <- [--------:-----] [pid=%u]\n",
        (s8)message[0], profileId);

    for (i = 0; i < 0x80; i++)
    {
        if (fn_80493B94(buf, message + 1, i) == -1)
        {
            break;
        }
        list[i] = strtoul(buf, NULL, 10);
    }
    fn_80493C58(message[0], profileId, 0, 0, list, i);
}

void fn_80491EDC(int error, int errorCode)
{
    if (DWCi_GetMatchCnt() == NULL || error == 0)
    {
        return;
    }

    DWCi_CloseAllConnectionsByTimeout();
    DWCi_SetError(error, errorCode);
    fn_8048AFCC(1, "", NULL);
    DWCi_GetMatchCnt()->matchedCallback(error, FALSE,
        DWCi_GetMatchCnt()->_21C ? FALSE : TRUE,
        DWCi_GetMatchCnt()->matchType == 2 ? TRUE : FALSE,
        fn_8048AEC4(DWCi_GetMatchCnt()->_21C),
        DWCi_GetMatchCnt()->matchedParam);
    DWCi_CloseMatching();
}

void fn_80491FC4(void)
{
    if (lbl_806E2EF8->matchType == 2)
    {
        return;
    }
    lbl_806E2EF8->_14 = 0;
    lbl_806E2EF8->_16 = 0;
    qr2_send_statechanged(lbl_806E2EF8->qr2);
}

void fn_80491FF4(int aid, int type, const u8* data)
{
    int i;

    DWC_Printf(0x80, "Received SYN %d packet from aid %d.\n", type - 2, aid);

    switch (type)
    {
    case 2:
        if (lbl_806E2EF8->state == 1)
        {
            if (data[0] == 1)
            {
                lbl_806E2EF8->_21C = 0;
            }
            lbl_806E2EF8->aidList[data[1]] = data[2];
            lbl_806E2EF8->pidList[data[1]] = lbl_806E2EF8->_210;
            if (lbl_806E2EF8->matchType == 0
                || lbl_806E2EF8->matchType == 1)
            {
                lbl_806E2EF8->_16 = lbl_806E2EF8->_0D;
            }
            lbl_806E2EF8->_1B1 = 0;
            lbl_806E2EF8->state = 9;
        }
        fn_804978F0(aid, 3);
        break;

    case 3:
        if (lbl_806E2EF8->state != 16)
        {
            fn_804978F0(aid, 4);
            break;
        }
        lbl_806E2EF8->_1E0 |= 1 << aid;
        {
            u16 latency = data[0] | (data[1] << 8);
            if (latency > lbl_806E2EF8->_1B4)
            {
                lbl_806E2EF8->_1B4 = latency;
            }
        }
        {
            u32 bitmap = 0;
            for (i = 1; i <= lbl_806E2EF8->_0D; i++)
            {
                bitmap |= 1 << lbl_806E2EF8->aidList[i];
            }
            if (bitmap == lbl_806E2EF8->_1E0)
            {
                for (i = 1; i <= lbl_806E2EF8->_0D; i++)
                {
                    fn_804978F0(lbl_806E2EF8->aidList[i], 4);
                }
                lbl_806E2EF8->state = 17;
                DWC_Printf(0x80, "Wait max latency %d msec.\n",
                    lbl_806E2EF8->_1B4);
            }
        }
        break;

    case 4:
        if (lbl_806E2EF8->state == 9)
        {
            fn_80496740(4);
        }
        break;

    default:
        break;
    }
}

BOOL fn_8049236C(int error, int errorCode)
{
    if (DWC_GetState() != DWC_STATE_MATCHING)
    {
        return FALSE;
    }
    if (lbl_806E2EF8->matchType == 2)
    {
        DWC_Printf(4, "DWCi_ProcessMatchClosing: SC_SV.\n");
        return TRUE;
    }

    if (error != 0)
    {
        if (lbl_806E2EF8 != NULL && error != 0)
        {
            lbl_806E2EF8->closeState = 2;
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
            lbl_806E2EF8->closeState = 0;
            DWCi_SetError(error, errorCode + DWC_ECODE_SEQ_MATCHING);
            fn_8048AFCC(1, "", NULL);
            lbl_806E2EF8->matchedCallback(error, FALSE,
                lbl_806E2EF8->_21C == 0, lbl_806E2EF8->matchType == 2,
                fn_8048AEC4(lbl_806E2EF8->_21C), lbl_806E2EF8->matchedParam);
            DWCi_CloseMatching();
        }
        return TRUE;
    }

    lbl_806E2EF8->aidList[0] = 0;
    if (lbl_806E2EF8->_1AD == 1
        || (u8)(lbl_806E2EF8->closeState - 1) <= 1)
    {
        return TRUE;
    }

    if (lbl_806E2EF8->_1A0 != 0)
    {
        NNCancel(lbl_806E2EF8->_1A0);
        lbl_806E2EF8->_1A0 = 0;
    }

    if (lbl_806E2EF8->_0D != 0)
    {
        if (lbl_806E2EF8->closeState == 0)
        {
            lbl_806E2EF8->closeState = 3;
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
        }
        return TRUE;
    }

    if (lbl_806E2EF8->matchType == 3)
    {
        fn_80491EDC(DWC_ERROR_TYPE_6, DWC_ECODE_SEQ_MATCHING - 430);
        return TRUE;
    }

    if (lbl_806E2EF8->_218 != 0)
    {
        DWC_Printf(0x40, "Reserve NN to %u.\n", lbl_806E2EF8->_218);
        fn_80497654();
        return TRUE;
    }

    if (lbl_806E2EF8->state == 1)
    {
        DWC_Printf(0x40, "Wait prior profileID.\n");
        lbl_806E2EF8->state = 18;
        lbl_806E2EF8->_200 = OSGetTime();
        return TRUE;
    }

    DWC_Printf(0x40, "Restart matching immediately.\n");
    fn_804974BC(1);
    return TRUE;
}

int fn_80492648(int profileId)
{
    if (lbl_806E2EF8->closeState != 2)
    {
        return fn_80496188(profileId);
    }
    return profileId;
}

int fn_80492660(int index, int count)
{
    int removedPid;
    int i;

    if (lbl_806E2EF8 == NULL)
    {
        return 0;
    }

    removedPid = lbl_806E2EF8->pidList[index];
    lbl_806E2EF8->aidBitmap
        &= ~(1 << lbl_806E2EF8->aidList[index]);
    fn_804929E0();

    for (i = index; i < count - 1; i++)
    {
        lbl_806E2EF8->_24[i] = lbl_806E2EF8->_24[i + 1];
        lbl_806E2EF8->_A4[i] = lbl_806E2EF8->_A4[i + 1];
        lbl_806E2EF8->pidList[i] = lbl_806E2EF8->pidList[i + 1];
        lbl_806E2EF8->ipList[i] = lbl_806E2EF8->ipList[i + 1];
        lbl_806E2EF8->portList[i] = lbl_806E2EF8->portList[i + 1];
        lbl_806E2EF8->aidList[i] = lbl_806E2EF8->aidList[i + 1];
    }

    if (count > 0)
    {
        lbl_806E2EF8->_24[count - 1] = 0;
        lbl_806E2EF8->_A4[count - 1] = 0;
        lbl_806E2EF8->pidList[count - 1] = 0;
        lbl_806E2EF8->ipList[count - 1] = 0;
        lbl_806E2EF8->portList[count - 1] = 0;
        lbl_806E2EF8->aidList[count - 1] = 0;
    }
    return removedPid;
}

int fn_804929A8(void)
{
    if (lbl_806E2EF8 != NULL)
    {
        return lbl_806E2EF8->_0D;
    }
    return 0;
}

int fn_804929C4(void)
{
    if (lbl_806E2EF8 != NULL)
    {
        return lbl_806E2EF8->_0E;
    }
    return 0;
}

void fn_804929E0(void)
{
    int count = -1;
    int i;

    for (i = 0; i < 32; i++)
    {
        if ((1 << i) & lbl_806E2EF8->aidBitmap)
        {
            count++;
        }
    }
    if (count == -1)
    {
        lbl_806E2EF8->_0E = 0;
    }
    else
    {
        lbl_806E2EF8->_0E = count;
    }
}

int fn_80492ABC(u8** aidList)
{
    if (lbl_806E2EF8 == NULL)
    {
        return 0;
    }
    *aidList = lbl_806E2EF8->aidList;
    return lbl_806E2EF8->_0D + 1;
}

int fn_80492AE8(u8** aidList)
{
    int i;

    if (lbl_806E2EF8 == NULL)
    {
        return 0;
    }

    memset(lbl_806CA158, 0, sizeof(lbl_806CA158));
    for (i = 0; i <= lbl_806E2EF8->_0E; i++)
    {
        if (!(lbl_806E2EF8->aidBitmap
                & (1 << lbl_806E2EF8->aidList[i])))
        {
            break;
        }
        lbl_806CA158[i] = lbl_806E2EF8->aidList[i];
    }
    *aidList = lbl_806CA158;
    return lbl_806E2EF8->_0E + 1;
}

GPResult fn_80492BA0(void)
{
    char value[12];
    char status[32];

    if (lbl_806E2EF8->matchType != 2)
    {
        return GP_NO_ERROR;
    }
    snprintf(value, sizeof(value), "%u", lbl_806E2EF8->_16 + 1);
    DWC_SetCommonKeyValueString("SCM", value, status, '/');
    snprintf(value, sizeof(value), "%u", lbl_806E2EF8->_0D + 1);
    DWC_AddCommonKeyValueString("SCN", value, status, '/');
    snprintf(value, sizeof(value), "%u", 3);
    DWC_AddCommonKeyValueString("VER", value, status, '/');
    return fn_8048AFCC(6, status, NULL);
}

void fn_80492C74(void)
{
    lbl_806E2EF8 = NULL;
    if (stpAddFilter != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    DWCi_ClearGameMatchKeys();

    if (lbl_806E2EEC != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, lbl_806E2EEC, 0);
        lbl_806E2EEC = NULL;
    }

    lbl_806E2EFC.valid = 0;
    lbl_806E2EFC._01 = 0;
}

BOOL fn_80492D3C(void)
{
    return lbl_806E2EF8 == NULL;
}

void fn_80492D4C(int mode)
{
    lbl_806E2EF8->_0C = 0;
    lbl_806E2EF8->_180 = 0;
    lbl_806E2EF8->_182 = DWCi_GetMathRand32(0x10000);
    lbl_806E2EF8->_184 = 0;
    lbl_806E2EF8->_188 = 0;
    lbl_806E2EF8->_190 = 0;
    lbl_806E2EF8->_1A8 = 0;
    lbl_806E2EF8->_1AD = 0;
    lbl_806E2EF8->_1AE = 0;
    lbl_806E2EF8->_1AF = 0;
    lbl_806E2EF8->_1B0 = 0;
    lbl_806E2EF8->_1AB = 0;
    lbl_806E2EF8->closeState = 0;
    lbl_806E2EF8->_1B6 = 0;
    lbl_806E2EF8->_1B8 = 0;
    lbl_806E2EF8->_1BC = 0;
    lbl_806E2EF8->_1F0 = 0;
    lbl_806E2EF8->_200 = 0;
    memset(&lbl_806E2EF8->_3E0, 0, 0x98);

    if (mode == 2)
    {
        lbl_806E2EF8->_14 = lbl_806E2EF8->_0D;
        if (lbl_806E2EF8->matchType == 3)
        {
            lbl_806E2EF8->state = 1;
        }
        else if (lbl_806E2EF8->matchType == 2)
        {
            lbl_806E2EF8->state = 10;
        }
        return;
    }

    lbl_806E2EF8->_0D = 0;
    lbl_806E2EF8->_0E = 0;
    lbl_806E2EF8->_14 = 0;
    lbl_806E2EF8->_17 = 0;
    lbl_806E2EF8->_20 = 0;
    lbl_806E2EF8->_E8 = 0;
    lbl_806E2EF8->_1A9 = 0;
    lbl_806E2EF8->_1B4 = 0;
    lbl_806E2EF8->_1C0 = 0;
    lbl_806E2EF8->_1C8 = 0;
    lbl_806E2EF8->_1D0 = 0;
    lbl_806E2EF8->_1D8 = 0;
    lbl_806E2EF8->_1E0 = 0;
    lbl_806E2EF8->_214 = 0;
    lbl_806E2EF8->_218 = 0;
    lbl_806E2EF8->aidBitmap = 0;
    memset(lbl_806E2EF8->_24, 0, sizeof(lbl_806E2EF8->_24));
    memset(lbl_806E2EF8->_A4, 0, sizeof(lbl_806E2EF8->_A4));
    memset(lbl_806E2EF8->pidList, 0, sizeof(lbl_806E2EF8->pidList));
    memset(&lbl_806E2EF8->_198, 0, 0xC);
    memset(lbl_806E2EF8->ipList, 0, sizeof(lbl_806E2EF8->ipList));
    memset(lbl_806E2EF8->portList, 0, sizeof(lbl_806E2EF8->portList));
    memset(lbl_806E2EF8->aidList, 0, sizeof(lbl_806E2EF8->aidList));
    memset(lbl_806E2EF8->_358, 0, 0x84);

    if (mode == 1)
    {
        if (lbl_806E2EF8->matchType == 0)
        {
            lbl_806E2EF8->state = 3;
        }
        else if (lbl_806E2EF8->matchType == 1)
        {
            lbl_806E2EF8->state = 4;
        }
        return;
    }

    lbl_806E2EF8->matchType = 0;
    lbl_806E2EF8->_16 = 0;
    lbl_806E2EF8->_18 = 0;
    lbl_806E2EF8->_21C = 0;
    lbl_806E2EF8->_1AA = 0;
    lbl_806E2EF8->_1B2 = 0;
    lbl_806E2EF8->_1B1 = 0;
    lbl_806E2EF8->_208 = 0;
    lbl_806E2EF8->_480 = NULL;
    lbl_806E2EF8->_484 = NULL;
}

void DWCi_CloseMatching(void)
{
    DWC_Printf(0x40, " Close Matching....\n");
    if (lbl_806E2EF8 == NULL)
    {
        return;
    }

    if (lbl_806E2EF8->sb != NULL)
    {
        if (lbl_806E2EF0 == 0)
        {
            ServerBrowserFree(lbl_806E2EF8->sb);
            lbl_806E2EF8->sb = NULL;
        }
        else
        {
            lbl_806E2EF4 = 1;
        }
    }

    NNFreeNegotiateList();
    lbl_806E2EF8->state = 0;

    if (stpAddFilter != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    DWCi_ClearGameMatchKeys();

    lbl_806E2EF8->_18 = 1;
}

int fn_80493128(int profileId)
{
    int i;
    int retry;
    int error;
    int numKeys = 7;
    u8 keys[0xA8];
    char filter[0x100];

    keys[0] = 8;
    keys[1] = 10;
    keys[2] = 0x32;
    keys[3] = 0x33;
    keys[4] = 0x34;
    keys[5] = 0x35;
    keys[6] = 0x36;
    if (lbl_806E2EF8->matchType == 0 || lbl_806E2EF8->matchType == 1)
    {
        for (i = 0; i < 154; i++)
        {
            if (lbl_806C9A20[i].keyID != 0)
            {
                keys[numKeys++] = lbl_806C9A20[i].keyID;
            }
        }
    }

    switch (lbl_806E2EF8->state)
    {
    case 3:
        profileId = lbl_806E2EF8->_218;
        if (profileId == 0)
        {
            snprintf(filter, sizeof(filter),
                "%s = %d and %s != %u and maxplayers = %d and numplayers < "
                "%d and %s = %d and %s != %s",
                "dwc_mver", 3, "dwc_pid", lbl_806E2EF8->_210,
                lbl_806E2EF8->_16, lbl_806E2EF8->_16, "dwc_mtype",
                lbl_806E2EF8->matchType, "dwc_mresv", "dwc_pid");
            if (stpAddFilter != NULL)
            {
                snprintf(filter, sizeof(filter), "%s and (%s)", filter,
                    stpAddFilter);
            }
            break;
        }
    case 2:
    case 4:
    case 5:
        snprintf(filter, sizeof(filter), "%s = %u", "dwc_pid", profileId);
        lbl_806E2EF8->_214 = profileId;
        break;

    default:
        DWC_Printf(2, "---DWCi_SBUpdateAsync() illegal state %d.\n",
            lbl_806E2EF8->state);
        break;
    }

    DWC_Printf(0x40, "ServerBrowserFilter : %s\n", filter);
    ServerBrowserClear(lbl_806E2EF8->sb);

    for (retry = 0; retry < 5; retry++)
    {
        error = ServerBrowserLimitUpdateA(lbl_806E2EF8->sb, SBTrue, SBFalse,
            keys, numKeys, filter, 6);
        if (error == sbe_noerror || error != 2)
        {
            break;
        }
        DWC_Printf(0x400, "Retry\n");
    }

    if (error == sbe_noerror)
    {
        lbl_806E2EF8->_178
            = OSGetTime() + (s64)(OS_BUS_CLOCK_SPEED / 4 / 1000) * 30000;
    }
    return error;
}

static int DWCi_GetDefaultMatchFilter(
    char* filter, int profileID, u8 numEntry, u8 matchType)
{
    return snprintf(filter, 0x100,
        "%s = %d and %s != %u and maxplayers = %d and numplayers < %d and %s "
        "= %d and %s != %s",
        "dwc_mver", 3, "dwc_pid", profileID, numEntry, numEntry, "dwc_mtype",
        matchType, "dwc_mresv", "dwc_pid");
}

int fn_80493434(int isRetry, int cookie, SBServer server)
{
    int index = lbl_806E2EF8->_14;
    int result = 0;
    BOOL needNN;
    int retry;

    if (isRetry == 0)
    {
        cookie = (lbl_806E2EF8->_182 << 16) | (u16)lbl_806E2EF8->_210;
        if (SBServerHasPrivateAddress(server))
        {
            if (ServerBrowserGetMyPublicIPAddr(lbl_806E2EF8->sb)
                == SBServerGetPublicInetAddress(server))
            {
                DWC_Printf(0x40, "Server[%d] is behind same NAT as me.\n",
                    index);
                lbl_806E2EF8->ipList[index]
                    = SBServerGetPrivateInetAddress(server);
                lbl_806E2EF8->portList[index]
                    = SBServerGetPrivateQueryPort(server);
                needNN = FALSE;
            }
            else
            {
                DWC_Printf(0x40, "Server[%d] is behind NAT.\n", index);
                needNN = TRUE;
            }
        }
        else
        {
            u32 host = SONtoHl(SOGetHostID());
            u32 fieldA = host >> 24;
            u32 fieldB = (host >> 16) & 0xFF;
            BOOL privateAddress;

            if (fieldA == 10)
            {
                privateAddress = TRUE;
            }
            else if (fieldA == 0xAC && fieldB >= 0x10 && fieldB <= 0x1F)
            {
                privateAddress = TRUE;
            }
            else if (fieldA == 0xC0 && fieldB == 0xA8)
            {
                privateAddress = TRUE;
            }
            else
            {
                privateAddress = FALSE;
            }

            if (privateAddress)
            {
                DWC_Printf(0x40,
                    "Server[%d] is not behind NAT. But I'm behind NAT.\n",
                    index);
                needNN = TRUE;
            }
            else
            {
                DWC_Printf(0x40,
                    "Both I and Server[%d] are not behind NAT.\n", index);
                lbl_806E2EF8->ipList[index]
                    = SBServerGetPublicInetAddress(server);
                lbl_806E2EF8->portList[index]
                    = SBServerGetPublicQueryPort(server);
                needNN = FALSE;
            }
        }

        if (needNN)
        {
            lbl_806E2EF8->_182 = DWCi_GetMathRand32(0x10000);
            lbl_806E2EF8->_1A0 = cookie;
        }
        else
        {
            u32 payload[2];
            u32 port;

            payload[0] = SOGetHostID();
            port = gt2GetLocalPort(*lbl_806E2EF8->pGT2Socket);
            payload[1] = (((port >> 8) & 0xFF) | ((port & 0xFF) << 8))
                    << 16
                | (((port >> 8) & 0xFF) | ((port & 0xFF) << 8));
            payload[1] = (payload[1] >> 16) | (payload[1] << 16);
            result = fn_8049382C(6, lbl_806E2EF8->pidList[index],
                SBServerGetPublicInetAddress(server),
                SBServerGetPublicQueryPort(server), payload, 2);
            lbl_806E2EF8->_3E1 = 0;
            if (result != 0)
            {
                return 2;
            }
            lbl_806E2EF8->_1A0 = 0;
        }

        lbl_806E2EF8->_198 = 0;
        lbl_806E2EF8->_199 = 0;
        lbl_806E2EF8->_19A = SBServerGetPublicQueryPort(server);
        lbl_806E2EF8->_19C = SBServerGetPublicInetAddress(server);
    }
    else
    {
        lbl_806E2EF8->_198 = 1;
        needNN = TRUE;
        lbl_806E2EF8->_199 = 0;
        lbl_806E2EF8->_19A = 0;
        lbl_806E2EF8->_19C = 0;
        lbl_806E2EF8->_1A0 = cookie;
    }

    if (needNN)
    {
        if (lbl_806E2EF8->_198 == 0)
        {
            if (fn_80498C78(ServerBrowserSendNatNegotiateCookieToServerA(
                    lbl_806E2EF8->sb,
                    gt2AddressToString(lbl_806E2EF8->_19C, 0, NULL),
                    lbl_806E2EF8->_19A, lbl_806E2EF8->_1A0))
                != 0)
            {
                result = 2;
                return result;
            }
            DWC_Printf(0x40, "Send NN cookie = %x.\n", lbl_806E2EF8->_1A0);
        }

        for (retry = 0; retry < 5; retry++)
        {
            result = NNBeginNegotiationWithSocket(
                gt2GetSocketSOCKET(*lbl_806E2EF8->pGT2Socket),
                lbl_806E2EF8->_1A0, lbl_806E2EF8->_198, fn_8049A6E8,
                fn_8049A700, &lbl_806E2EF8->_198);
            if (result == 0 || result != 3)
            {
                break;
            }
            DWC_Printf(4,
                " dns error occurs when NatNegotiation begin... retry\n");
        }
    }
    else
    {
        fn_8049A700(nr_success,
            gt2GetSocketSOCKET(*lbl_806E2EF8->pGT2Socket), NULL,
            &lbl_806E2EF8->_198);
        lbl_806E2EF8->_190 = 0;
    }
    return result;
}

GPResult fn_8049382C(int command, u32 profileId, u32 ip, u16 port,
    const u32* data, int count)
{
    GPResult result = GP_NO_ERROR;
    int len;
    int i;
    int tokenLen;
    char token[0x10];
    char message[0x200];
    char buf[0x200];

    if (lbl_806E2EF8->matchType == 0
        || ((lbl_806E2EF8->matchType == 3 || lbl_806E2EF8->_1AA != 0)
            && command == 6))
    {
        result = fn_80493A54(command, ip, port, data, count);
    }
    else
    {
        len = 0;
        if (data != NULL && count != 0)
        {
            len = snprintf(message, sizeof(message), "%u", data[0]);
            for (i = 1; i < count; i++)
            {
                tokenLen = snprintf(token, sizeof(token), "/%u", data[i]);
                memcpy(message + len, token, tokenLen);
                len += tokenLen;
            }
        }
        message[len] = '\0';

        len = snprintf(buf, sizeof(buf), "%s%dv%s", "GPCM", 3, "MAT");
        buf[len] = command;
        buf[len + 1] = '\0';
        tokenLen = strlen(message);
        memcpy(buf + len + 1, message, tokenLen);
        buf[len + 1 + tokenLen] = '\0';
        result = gpSendBuddyMessageA(lbl_806E2EF8->_00, profileId, buf);
        DWC_Printf(0x40, "<GP> SEND-0x%02x -> [--------:-----] [pid=%u]\n",
            command, profileId);
    }

    if (command == 2 || command == 6 || (u8)(command - 8) <= 1)
    {
        lbl_806E2EF8->_3E0 = command;
        lbl_806E2EF8->_3E2 = port;
        lbl_806E2EF8->_3E4 = ip;
        lbl_806E2EF8->_468 = profileId;
        lbl_806E2EF8->_46C = count;
        lbl_806E2EF8->_470 = OSGetTime();
        if (data != NULL && count != 0)
        {
            memcpy(lbl_806E2EF8->_3E8, data, count * 4);
        }
    }
    return result;
}

SBError fn_80493A54(u8 command, u32 ip, u16 port, const u32* data, int count)
{
    SBError error;
    int retry;
    struct
    {
        char magic[4];
        u32 version;
        u8 command;
        u8 length;
        u16 sequence;
        u32 _0C;
        u32 profileId;
        u8 payload[0x80];
    } message;

    if (data != NULL && count != 0)
    {
        memcpy(message.payload, data, count * 4);
    }
    else
    {
        count = 0;
    }

    strcpy(message.magic, "SBCM");
    message.version = 0x03000000;
    message.command = command;
    message.length = count * 4;
    message.sequence
        = (lbl_806E2EF8->_1A >> 8) | (lbl_806E2EF8->_1A << 8);
    message._0C = lbl_806E2EF8->_1C;
    {
        u32 pid = lbl_806E2EF8->_210;
        u32 swapped = ((pid >> 24) & 0xFF) | ((pid >> 8) & 0xFF00)
            | ((pid << 8) & 0xFF0000) | (pid << 24);
        message.profileId = swapped;
    }
    DWC_Printf(0x40, "<SB> SEND-0x%02x -> [%08x:%d] [pid=--------]\n",
        command, ip, port);

    for (retry = 0; retry < 5; retry++)
    {
        error = ServerBrowserSendMessageToServerA(lbl_806E2EF8->sb,
            gt2AddressToString(ip, 0, NULL), port, (char*)&message,
            message.length + 0x14);
        if (error == sbe_noerror || error != 2)
        {
            break;
        }
    }
    return error;
}

int fn_80493B94(char* dstMsg, const char* srcMsg, int index)
{
    const char* pSrcBegin = srcMsg;
    char* pSrcNext = NULL;
    char* pSrcEnd;
    int len;
    int i;

    pSrcEnd = strchr(pSrcBegin, '\0');

    for (i = 0; i < index; i++)
    {
        pSrcNext = strchr(pSrcBegin, '/');
        if (pSrcNext == NULL)
        {
            return -1;
        }
        pSrcBegin = pSrcNext + 1;
    }

    pSrcNext = strchr(pSrcBegin, '/');
    if (pSrcNext == NULL)
    {
        pSrcNext = pSrcEnd;
    }
    if (pSrcBegin == pSrcNext)
    {
        return -1;
    }
    len = pSrcNext - pSrcBegin;
    memcpy(dstMsg, pSrcBegin, len);
    dstMsg[len] = '\0';
    return len;
}

BOOL fn_80493C58(u8 command, u32 profileId, u32 ip, u16 port, u32* data,
    int count)
{
    int i;
    u32 buf[0x40];
    int n = 0;

    if (lbl_806E2EF8 == NULL || lbl_806E2EF8->state == 0)
    {
        return TRUE;
    }

    if (DWC_GetState() == DWC_STATE_MATCHING && lbl_806E2EF8->state == 1
        && (int)profileId == lbl_806E2EF8->pidList[0])
    {
        lbl_806E2EF8->_1B1 = 0;
        lbl_806E2EF8->_208 = OSGetTime();
    }

    switch (command)
    {
    case 1:
    case 11:
    {
        u8 response;

        if (lbl_806E2EF8->matchType != 0)
        {
            ip = data[1];
            port = fn_ByteSwap32(data[2]);
            DWC_Printf(4, "friend IP:%x, port:%d\n", ip, port);
        }
        response = fn_80495530(profileId, ip, port,
            fn_ByteSwap32(data[0]), command == 11);
        if (response == 2)
        {
            GPResult result;

            lbl_806E2EF8->_1E8 = 0;
            result = fn_80495884(profileId, ip, port);
            if (lbl_806E2EF8->matchType == 0)
            {
                result = fn_80498C78(result);
            }
            else
            {
                result = fn_80498B24(result);
            }
            if (result != 0)
            {
                return FALSE;
            }
            {
                DWCMatchControlView* control = lbl_806E2EF8;

                if (control->matchType == 2 && control->_480 != NULL)
                {
                    ((void (*)(int, void*))control->_480)(
                        fn_8048AEC4(profileId), control->_484);
                }
            }
            buf[0] = fn_ByteSwap32(lbl_806E2EF8->_14);
            for (n = 1; n <= lbl_806E2EF8->_14; n++)
            {
                buf[n] = fn_ByteSwap32(lbl_806E2EF8->pidList[n]);
            }
            buf[n++] = lbl_806E2EF8->_1C;
            buf[n++] = fn_ByteSwap32(lbl_806E2EF8->_1A);
            lbl_806E2EF8->state = 11;
        }
        else if (response == 3)
        {
            if (lbl_806E2EF8->_16 != 0
                && lbl_806E2EF8->_14 == lbl_806E2EF8->_16
                && lbl_806E2EF8->matchType == 2)
            {
                buf[0] = 0x10000000;
                n = 1;
            }
        }

        if (response != 0xFF)
        {
            GPResult result
                = fn_8049382C(response, profileId, ip, port, buf, n);
            if (lbl_806E2EF8->matchType == 0)
            {
                result = fn_80498C78(result);
            }
            else
            {
                result = fn_80498B24(result);
            }
            if (result != 0)
            {
                return FALSE;
            }
        }
        break;
    }

    case 2:
    {
        u32 numEntries = fn_ByteSwap32(data[0]);
        GPResult result;
        int total;

        if (lbl_806E2EF8->state != 4)
        {
            break;
        }
        DWC_Printf(0x40, "Succeeded NN reservation.\n");
        if ((int)profileId != lbl_806E2EF8->_214)
        {
            break;
        }
        lbl_806E2EF8->_218 = 0;
        lbl_806E2EF8->_1AB = 0;
        lbl_806E2EF8->_1D0 = 0;
        lbl_806E2EF8->_1C0 = 0;
        lbl_806E2EF8->_24[0] = data[numEntries + 1];
        lbl_806E2EF8->_A4[0] = fn_ByteSwap32(data[numEntries + 2]);
        lbl_806E2EF8->_1BC = data[numEntries + 1];
        lbl_806E2EF8->_1B8 = fn_ByteSwap32(data[numEntries + 2]);
        DWC_Printf(4, "Server IP:%x, port:%d\n", lbl_806E2EF8->_1BC,
            lbl_806E2EF8->_1B8);

        if (lbl_806E2EF8->matchType == 1)
        {
            BOOL allFriends;

            if (lbl_806E2EF8->_1AA != 0 && lbl_806E2EF8->state == 4)
            {
                allFriends = TRUE;
            }
            else
            {
                u32* entry = data;
                u32 idx = 0;

                allFriends = TRUE;
                for (; idx < numEntries; idx++, entry++)
                {
                    u32 pid = fn_ByteSwap32(entry[1]);
                    BOOL found;

                    if (lbl_806E2EF8->_30C == 0)
                    {
                        found = FALSE;
                    }
                    else
                    {
                        int j;

                        found = FALSE;
                        for (j = 0; j < lbl_806E2EF8->_354; j++)
                        {
                            int friendPid
                                = fn_8048AE58(lbl_806E2EF8->_314[j]);
                            if (friendPid > 0 && (u32)friendPid == pid)
                            {
                                found = TRUE;
                                break;
                            }
                        }
                    }
                    if (!found)
                    {
                        allFriends = FALSE;
                        break;
                    }
                    if (lbl_806E2EF8->_1AA != 0
                        && lbl_806E2EF8->state == 1)
                    {
                        allFriends = TRUE;
                        break;
                    }
                }
            }

            if (allFriends)
            {
                if (lbl_806E2EF8->_0D != 0)
                {
                    total = data[0] + 2;
                    if (total > 2)
                    {
                        memcpy(lbl_806E2EF8->_358 + 8, data + 1,
                            (total - 2) * 4);
                    }
                    *(int*)lbl_806E2EF8->_358 = total - 1;
                    *((u32*)lbl_806E2EF8->_358 + 1) = profileId;
                }
            }
            else
            {
                DWC_Printf(0x40, "But some clients are not friends.\n");
                result = fn_8049382C(5, profileId, lbl_806E2EF8->_24[0],
                    lbl_806E2EF8->_A4[0], NULL, 0);
                lbl_806E2EF8->_214 = 0;
                if (lbl_806E2EF8->matchType == 0)
                {
                    result = fn_80498C78(result);
                }
                else
                {
                    result = fn_80498B24(result);
                }
                if (result != 0)
                {
                    return FALSE;
                }
                result = fn_80495D7C(0, 0, profileId);
                if (lbl_806E2EF8->matchType == 0)
                {
                    result = fn_80498C78(result);
                }
                else
                {
                    result = fn_80498B24(result);
                }
                if (result != 0)
                {
                    return FALSE;
                }
                break;
            }
        }

        if (lbl_806E2EF8->matchType == 0)
        {
            if (lbl_806E2EF8->_0D != 0)
            {
                total = data[0] + 2;
                if (total > 2)
                {
                    memcpy(lbl_806E2EF8->_358 + 8, data + 1,
                        (total - 2) * 4);
                }
                *(int*)lbl_806E2EF8->_358 = total - 1;
                *((u32*)lbl_806E2EF8->_358 + 1) = profileId;

                result = GP_NO_ERROR;
                for (i = 1; i <= lbl_806E2EF8->_0D; i++)
                {
                    result = fn_8049382C(10, lbl_806E2EF8->pidList[i],
                        lbl_806E2EF8->_24[i], lbl_806E2EF8->_A4[i],
                        (const u32*)lbl_806E2EF8->_358,
                        *(const int*)lbl_806E2EF8->_358 + 1);
                    if (result != 0)
                    {
                        break;
                    }
                }
                if (result == 0)
                {
                    lbl_806E2EF8->_17 = 0;
                    lbl_806E2EF8->_20 = 0;
                    lbl_806E2EF8->closeState = 1;
                    gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
                    lbl_806E2EF8->closeState = 0;
                    DWC_Printf(0x40,
                        "Closed all connections. Begin NN to %u\n",
                        lbl_806E2EF8->_214);
                    result = GP_NO_ERROR;
                }
                if (lbl_806E2EF8->matchType == 0)
                {
                    result = fn_80498C78(result);
                }
                else
                {
                    result = fn_80498B24(result);
                }
                if (result != 0)
                {
                    return FALSE;
                }
            }
            lbl_806E2EF8->state = 6;
            if (fn_80498FB8(fn_80493434(0, 0,
                    ServerBrowserGetServer(lbl_806E2EF8->sb, 0)))
                != 0)
            {
                return FALSE;
            }
        }
        else
        {
            lbl_806E2EF8->state = 5;
            if (fn_80498C78(fn_80493128(profileId)) != 0)
            {
                return FALSE;
            }
        }
        break;
    }

    case 3:
        if (lbl_806E2EF8->state != 4)
        {
            break;
        }
        if ((int)profileId != lbl_806E2EF8->_214)
        {
            break;
        }
        DWC_Printf(0x40, "Reservation was denied by %u.\n", profileId);
        if (count > 0 && fn_ByteSwap32(data[0]) == 0x10)
        {
            DWC_Printf(0x40, "Game server is fully occupied.\n");
            if (lbl_806E2EF8 != NULL)
            {
                BOOL isServer;
                BOOL self;

                lbl_806E2EF8->closeState = 2;
                gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
                lbl_806E2EF8->closeState = 0;
                DWCi_SetError(13, 0);
                fn_8048AFCC(1, "", NULL);
                {
                    DWCMatchControlView* control = lbl_806E2EF8;

                    isServer = control->matchType == 2;
                    self = control->_21C == 0;
                    control->matchedCallback(13, FALSE, self, isServer,
                        fn_8048AEC4(control->_21C),
                        control->matchedParam);
                }
                DWCi_CloseMatching();
            }
            return FALSE;
        }
        return fn_80496000(lbl_806E2EF8->_214);

    case 4:
        if (lbl_806E2EF8->state != 4)
        {
            break;
        }
        if ((int)profileId != lbl_806E2EF8->_214)
        {
            break;
        }
        lbl_806E2EF8->_1D8 = OSGetTime();
        if (lbl_806E2EF8->_218 != 0
            && (lbl_806E2EF8->_1AB < 0x10
                || lbl_806E2EF8->matchType == 3))
        {
            lbl_806E2EF8->_1C0 = 1;
            lbl_806E2EF8->_1C8 = OSGetTime();
            if (lbl_806E2EF8->matchType != 3)
            {
                lbl_806E2EF8->_1AB++;
            }
            break;
        }
        lbl_806E2EF8->_218 = 0;
        lbl_806E2EF8->_1AB = 0;
        if (lbl_806E2EF8->matchType == 0)
        {
            lbl_806E2EF8->state = 3;
            lbl_806E2EF8->_E8 = 1;
            lbl_806E2EF8->_F0 = OSGetTime();
        }
        else if (lbl_806E2EF8->matchType == 1)
        {
            fn_80495D7C(1, 0, 0);
        }
        break;

    case 5:
        if (lbl_806E2EF8->_17 == 0)
        {
            break;
        }
        if ((int)profileId != lbl_806E2EF8->_20)
        {
            break;
        }
        if (lbl_806E2EF8->matchType == 2 && lbl_806E2EF8->_0D == 1
            && (int)profileId == lbl_806E2EF8->pidList[1])
        {
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
        }
        if (fn_80496188(profileId) == 0)
        {
            return FALSE;
        }
        break;

    case 6:
    {
        struct sockaddr_in addr;
        u32 serverIp;
        u16 serverPort;

        serverIp = data[0];
        serverPort = fn_ByteSwap32(data[1]);
        DWC_Printf(0x40,
            "NN parent is behind same NAT as me. Received IP %x & "
            "port %d\n",
            serverIp, serverPort);
        if (lbl_806E2EF8->state == 1)
        {
            lbl_806E2EF8->state = 6;
        }
        else if ((lbl_806E2EF8->state != 6 && lbl_806E2EF8->state != 11)
            || (int)profileId != lbl_806E2EF8->_20)
        {
            DWC_Printf(0x40, "But already canceled reservation.\n");
            break;
        }
        lbl_806E2EF8->_3E0 = 0xFF;
        if ((int)profileId
            != lbl_806E2EF8->pidList[lbl_806E2EF8->_0D + 1])
        {
            lbl_806E2EF8->pidList[lbl_806E2EF8->_0D + 1] = profileId;
        }
        addr.sin_addr.addr = serverIp;
        addr.sin_port = SOHtoNs(serverPort);
        lbl_806E2EF8->_198 = 1;
        fn_8049A700(nr_success,
            gt2GetSocketSOCKET(*lbl_806E2EF8->pGT2Socket), &addr,
            &lbl_806E2EF8->_198);
        lbl_806E2EF8->_190 = 0;
        break;
    }

    case 7:
    {
        u32 newPid;
        u8 aid;

        if (lbl_806E2EF8->state != 1
            || (int)profileId != lbl_806E2EF8->pidList[0])
        {
            DWC_Printf(4, "Ignore delayed NEW_PID_AID command.\n");
            break;
        }
        newPid = fn_ByteSwap32(data[0]);
        aid = fn_ByteSwap32(data[1]);
        if (lbl_806E2EF8->matchType == 1 && lbl_806E2EF8->_1AA == 0)
        {
            BOOL acceptable;

            if (lbl_806E2EF8->_30C == 0)
            {
                acceptable = FALSE;
            }
            else
            {
                acceptable = FALSE;
                for (i = 0; i < lbl_806E2EF8->_354; i++)
                {
                    int friendPid
                        = fn_8048AE58(lbl_806E2EF8->_314[i]);
                    if (friendPid > 0 && friendPid == (int)newPid)
                    {
                        acceptable = TRUE;
                        break;
                    }
                }
            }
            {
                GPResult result;

                buf[0] = fn_ByteSwap32(acceptable);
                DWC_Printf(0x40, "profileID %d is acceptable? - %d.\n",
                    newPid, buf[0]);
                result = fn_8049382C(0x20, profileId, ip, port, buf, 1);
                if (lbl_806E2EF8->matchType == 0)
                {
                    result = fn_80498C78(result);
                }
                else
                {
                    result = fn_80498B24(result);
                }
                if (result != 0)
                {
                    return FALSE;
                }
            }
        }
        lbl_806E2EF8->pidList[lbl_806E2EF8->_14 + 1] = newPid;
        lbl_806E2EF8->aidList[lbl_806E2EF8->_14 + 1] = aid;
        qr2_send_statechanged(lbl_806E2EF8->qr2);
        {
            DWCMatchControlView* control = lbl_806E2EF8;

            if (control->_480 != NULL)
            {
                ((void (*)(int, void*))control->_480)(fn_8048AEC4(newPid),
                    control->_484);
            }
        }
        DWC_Printf(0x40,
            "Received new client's profileID = %u & aid = %d.\n", newPid,
            aid);
        break;
    }

    case 8:
    {
        u32 clientCount;

        if (lbl_806E2EF8->state != 1
            || (int)profileId != lbl_806E2EF8->pidList[0])
        {
            DWC_Printf(4, "Ignore delayed LINK_CLS_REQ command.\n");
            break;
        }
        clientCount = fn_ByteSwap32(data[0]);
        if (clientCount == 0)
        {
            u32 index = fn_ByteSwap32(data[1]);
            lbl_806E2EF8->aidList[index] = fn_ByteSwap32(data[2]);
            lbl_806E2EF8->pidList[index] = lbl_806E2EF8->_210;
            fn_80496740(3);
            break;
        }
        {
            u32 index = fn_ByteSwap32(data[1]);
            u32 clientPid = fn_ByteSwap32(data[2]);
            u8 aid = fn_ByteSwap32(data[2]) >> 16;

            if ((int)clientPid == lbl_806E2EF8->pidList[index]
                && index == (u32)(lbl_806E2EF8->_0D - 1))
            {
                u32 resendBuf[1];
                GPResult result;

                resendBuf[0] = fn_ByteSwap32(clientPid);
                DWC_Printf(0x40,
                    "Resend command %d for delayed command %d.\n", 9, 8);
                result = fn_8049382C(9, profileId, lbl_806E2EF8->_24[0],
                    lbl_806E2EF8->_A4[0], resendBuf, 1);
                if (lbl_806E2EF8->matchType == 0)
                {
                    result = fn_80498C78(result);
                }
                else
                {
                    result = fn_80498B24(result);
                }
                if (result != 0)
                {
                    return FALSE;
                }
                break;
            }
            lbl_806E2EF8->pidList[index] = clientPid;
            lbl_806E2EF8->aidList[index] = aid;
            {
                u32 addr = data[3];

                lbl_806E2EF8->_24[index] = addr;
                lbl_806E2EF8->_A4[index] = fn_ByteSwap32(addr);
                lbl_806E2EF8->_1BC = addr;
                lbl_806E2EF8->_1B8 = fn_ByteSwap32(addr);
            }
            DWC_Printf(4, "Client IP:%x, port:%d\n", lbl_806E2EF8->_1BC,
                lbl_806E2EF8->_1B8);
            lbl_806E2EF8->state = 5;
            DWC_Printf(0x40, "Next, try to connect to %u.\n", clientPid);
            if (fn_80498C78(fn_80493128(clientPid)) != 0)
            {
                return FALSE;
            }
            lbl_806E2EF8->_1D0 = 0;
            lbl_806E2EF8->_1C0 = 0;
        }
        break;
    }

    case 9:
        if (lbl_806E2EF8->state == 13
            && fn_ByteSwap32(data[0])
                == (u32)lbl_806E2EF8->pidList[lbl_806E2EF8->_1A8 + 1])
        {
            lbl_806E2EF8->_1A8++;
            fn_80496740(0);
        }
        else
        {
            DWC_Printf(0x40, "Ignore delayed command %d.\n", 9);
        }
        break;

    case 10:
    {
        BOOL accept = TRUE;

        if (lbl_806E2EF8->state != 1 && lbl_806E2EF8->state != 18)
        {
            DWC_Printf(4, "Ignore delayed CLOSE_LINK command.\n");
            break;
        }
        if (lbl_806E2EF8->matchType != 0)
        {
            u32 numPids = fn_ByteSwap32(data[0]);

            if (lbl_806E2EF8->_1AA != 0 && lbl_806E2EF8->state == 4)
            {
                accept = TRUE;
            }
            else
            {
                u32* entry = data;
                u32 idx = 0;

                accept = TRUE;
                for (; idx < numPids; idx++, entry++)
                {
                    u32 pid = fn_ByteSwap32(entry[1]);
                    BOOL found;

                    if (lbl_806E2EF8->_30C == 0)
                    {
                        found = FALSE;
                    }
                    else
                    {
                        int j;

                        found = FALSE;
                        for (j = 0; j < lbl_806E2EF8->_354; j++)
                        {
                            int friendPid
                                = fn_8048AE58(lbl_806E2EF8->_314[j]);
                            if (friendPid > 0 && (u32)friendPid == pid)
                            {
                                found = TRUE;
                                break;
                            }
                        }
                    }
                    if (!found)
                    {
                        accept = FALSE;
                        break;
                    }
                    if (lbl_806E2EF8->_1AA != 0
                        && lbl_806E2EF8->state == 1)
                    {
                        accept = TRUE;
                        break;
                    }
                }
            }
        }
        if (accept)
        {
            lbl_806E2EF8->_218 = fn_ByteSwap32(data[1]);
            lbl_806E2EF8->_1AB = 0;
            DWC_Printf(0x40, "Received close command. Next try to %u.\n",
                fn_ByteSwap32(data[1]));
        }
        else
        {
            lbl_806E2EF8->_218 = 0;
            DWC_Printf(0x40,
                "Received close command. Server %u or its clients are "
                "not friends.\n",
                fn_ByteSwap32(data[1]));
        }
        if (lbl_806E2EF8->_0D != 0)
        {
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
            break;
        }
        if (fn_80497654() != 0)
        {
            return FALSE;
        }
        break;
    }

    case 12:
        if ((int)profileId != lbl_806E2EF8->pidList[0])
        {
            DWC_Printf(4, "Ignore delayed CANCEL command.\n");
            break;
        }
        DWC_Printf(0x40, "Received cancel command from %u data[0] = %d.\n",
            profileId, fn_ByteSwap32(data[0]));
        DWC_Printf(4, "numHost nn=%d gt2=%d, state %d\n",
            lbl_806E2EF8->_14, lbl_806E2EF8->_0D, lbl_806E2EF8->state);
        if (lbl_806E2EF8->matchType == 0 || lbl_806E2EF8->matchType == 1)
        {
            if (fn_80496520(profileId) == 0)
            {
                return FALSE;
            }
        }
        else if (lbl_806E2EF8->matchType == 3)
        {
            lbl_806E2EF8->_21C = profileId;
            lbl_806E2EF8->closeState = 2;
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
            lbl_806E2EF8->closeState = 0;
            fn_804974BC(0);
        }
        break;

    case 13:
    case 14:
    case 15:
        if (fn_80497CB4(profileId, command, fn_ByteSwap32(data[0])) == 0)
        {
            return FALSE;
        }
        break;

    case 16:
        if ((int)profileId != lbl_806E2EF8->pidList[0])
        {
            return TRUE;
        }
        DWC_Printf(4, "Close shutdown client.\n");
        for (i = 0; i < count; i++)
        {
            u32 cpid = fn_ByteSwap32(data[i]);
            u8 aid = 0xFF;
            int j;

            for (j = 1; j <= lbl_806E2EF8->_0D; j++)
            {
                if ((int)cpid == lbl_806E2EF8->pidList[j])
                {
                    aid = lbl_806E2EF8->aidList[j];
                    break;
                }
            }
            if (aid != 0xFF)
            {
                DWC_CloseConnectionHard(aid);
            }
        }
        break;

    case 17:
    {
        DWCMatchOptMinCompleteView* opt = lbl_806E2EEC;
        GPResult result;

        if (opt != NULL && opt->valid != 0
            && fn_ElapsedMSec(opt->startTime) >= opt->timeout)
        {
            buf[0] = 0x01000000;
            DWC_Printf(0x80, "[OPT_MIN_COMP] time is %lu.\n",
                fn_ElapsedMSec(opt->startTime));
        }
        else
        {
            buf[0] = 0;
        }
        result = fn_8049382C(18, profileId, ip, port, buf, 1);
        if (lbl_806E2EF8->matchType == 0)
        {
            result = fn_80498C78(result);
        }
        else
        {
            result = fn_80498B24(result);
        }
        if (result != 0)
        {
            return FALSE;
        }
        break;
    }

    case 18:
    {
        u8 aid = 0xFF;

        if (lbl_806E2EF8->state != 19)
        {
            break;
        }
        for (i = 1; i <= lbl_806E2EF8->_0D; i++)
        {
            if ((int)profileId == lbl_806E2EF8->pidList[i])
            {
                aid = lbl_806E2EF8->aidList[i];
                break;
            }
        }
        if (aid == 0xFF)
        {
            break;
        }
        lbl_806E2EEC->_08 |= 1 << aid;
        if (fn_ByteSwap32(data[0]) != 0)
        {
            lbl_806E2EEC->_0C |= 1 << aid;
        }
        break;
    }

    case 19:
        if (lbl_806E2EF8 != NULL)
        {
            lbl_806E2EF8->closeState = 2;
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
            lbl_806E2EF8->closeState = 0;
            DWCi_SetError(12, 0);
            fn_8048AFCC(1, "", NULL);
            {
                DWCMatchControlView* control = lbl_806E2EF8;
                BOOL isServer = control->matchType == 2;
                BOOL self = control->_21C == 0;

                control->matchedCallback(12, FALSE, self, isServer,
                    fn_8048AEC4(control->_21C), control->matchedParam);
            }
            DWCi_CloseMatching();
        }
        return FALSE;

    case 0x20:
        if (lbl_806E2EF8->matchType != 1 || lbl_806E2EF8->_1AA != 0)
        {
            break;
        }
        for (i = 1; i <= lbl_806E2EF8->_0D; i++)
        {
            if ((int)profileId != lbl_806E2EF8->pidList[i])
            {
                continue;
            }
            DWC_Printf(0x40, "New client was accepted? - %d.\n",
                fn_ByteSwap32(data[0]));
            if (fn_ByteSwap32(data[0]) == 0)
            {
                if ((u32)(lbl_806E2EF8->state - 11) <= 1)
                {
                    u32 clsBuf[1];
                    GPResult result = GP_NO_ERROR;

                    clsBuf[0] = 0;
                    if (lbl_806E2EF8->_17 != 0
                        && lbl_806E2EF8->_20 != 0
                        && lbl_806E2EF8->_20 != lbl_806E2EF8->_210)
                    {
                        result = fn_8049382C(12, lbl_806E2EF8->_20,
                            lbl_806E2EF8->_24[lbl_806E2EF8->_0D + 1],
                            lbl_806E2EF8->_A4[lbl_806E2EF8->_0D + 1],
                            clsBuf, 1);
                        lbl_806E2EF8->_17 = 0;
                        lbl_806E2EF8->_20 = 0;
                    }
                    if (lbl_806E2EF8->matchType == 0)
                    {
                        result = fn_80498C78(result);
                    }
                    else
                    {
                        result = fn_80498B24(result);
                    }
                    if (result != 0)
                    {
                        return FALSE;
                    }
                }
                if (lbl_806E2EF8->_1A0 != 0)
                {
                    NNCancel(lbl_806E2EF8->_1A0);
                    lbl_806E2EF8->_1A0 = 0;
                }
                if (lbl_806E2EF8->matchType == 2
                    || lbl_806E2EF8->matchType == 3)
                {
                    DWC_Printf(8,
                        "DWCi_RestartFromTimeout() shouldn't be "
                        "called.\n");
                }
                else
                {
                    lbl_806E2EF8->closeState = 2;
                    gt2CloseAllConnectionsHard(
                        *lbl_806E2EF8->pGT2Socket);
                    lbl_806E2EF8->closeState = 0;
                    DWC_Printf(0x40,
                        "Closed all connections and restart "
                        "matching.\n");
                    fn_804974BC(1);
                }
            }
            else
            {
                u8 aid = 0xFF;
                int j;

                for (j = 1; j <= lbl_806E2EF8->_0D; j++)
                {
                    if ((int)profileId == lbl_806E2EF8->pidList[j])
                    {
                        aid = lbl_806E2EF8->aidList[j];
                        break;
                    }
                }
                lbl_806E2EF8->_1E8 |= 1 << aid;
            }
            break;
        }
        break;

    case 0x40:
        for (i = 1; i <= lbl_806E2EF8->_0D; i++)
        {
            if ((int)profileId != lbl_806E2EF8->pidList[i])
            {
                continue;
            }
            {
                GPResult result
                    = fn_8049382C(0x41, profileId, ip, port, NULL, 0);
                if (lbl_806E2EF8->matchType == 0)
                {
                    result = fn_80498C78(result);
                }
                else
                {
                    result = fn_80498B24(result);
                }
                if (result != 0)
                {
                    return FALSE;
                }
            }
            break;
        }
        break;

    case 0x41:
        break;

    default:
        DWC_Printf(2, "Received unexpected matching command 0x%02x.\n",
            command);
        break;
    }
    return TRUE;
}

u8 fn_80495530(u32 profileId, u32 ip, u16 port, u32 matchType, BOOL resend)
{
    u8 response;
    int i;

    switch (lbl_806E2EF8->matchType)
    {
    case 1:
        if (!gpIsBuddy(lbl_806E2EF8->_00, profileId))
        {
            response = 0xFF;
            break;
        }
        {
            BOOL found = FALSE;

            if (lbl_806E2EF8->_30C != 0)
            {
                for (i = 0; i < lbl_806E2EF8->_354; i++)
                {
                    int friendPid = fn_8048AE58(lbl_806E2EF8->_314[i]);
                    if (friendPid > 0 && friendPid == (int)profileId)
                    {
                        found = TRUE;
                        break;
                    }
                }
            }
            if (!found)
            {
                DWC_Printf(0x40,
                    "This friend doesn't exist in friendIdxList.\n");
                response = 3;
                break;
            }
        }
    case 0:
        if ((matchType == lbl_806E2EF8->matchType
                && lbl_806E2EF8->_1AD == 0
                && lbl_806E2EF8->_14 == lbl_806E2EF8->_16)
            || (lbl_806E2EF8->_17 != 0
                && lbl_806E2EF8->_20 != lbl_806E2EF8->_210))
        {
            response = 3;
            if (lbl_806E2EF8->matchType != 0)
            {
                break;
            }
            if (lbl_806E2EF8->qr2->userstatechangerequested != 0)
            {
                break;
            }
            if (lbl_806E2EF8->_17 == 0)
            {
                break;
            }
            if (lbl_806E2EF8->_20 != lbl_806E2EF8->_210)
            {
                break;
            }
            qr2_send_statechanged(lbl_806E2EF8->qr2);
            break;
        }
        if ((lbl_806E2EF8->state != 3 && lbl_806E2EF8->state != 4)
            || (lbl_806E2EF8->_1C == 0 && lbl_806E2EF8->_1A == 0)
            || (ip == 0 && port == 0))
        {
            response = 4;
            break;
        }
        if (lbl_806E2EF8->_214 != 0)
        {
            if ((int)profileId == lbl_806E2EF8->_214)
            {
                if (resend
                    || (lbl_806E2EF8->_210 < (int)profileId
                        && (int)profileId != lbl_806E2EF8->_218))
                {
                    response = 2;
                }
                else
                {
                    response = 0xFF;
                }
                break;
            }
            if (resend
                || (lbl_806E2EF8->_210 < (int)profileId
                    && lbl_806E2EF8->_218 == 0))
            {
                if (fn_8049382C(5, lbl_806E2EF8->_214,
                        lbl_806E2EF8->_24[0], lbl_806E2EF8->_A4[0], NULL, 0)
                    != 0)
                {
                }
                lbl_806E2EF8->_214 = 0;
                if (lbl_806E2EF8->matchType == 0)
                {
                    if (fn_80498C78(GP_NO_ERROR) != 0)
                    {
                        return 0xFF;
                    }
                }
                else if (fn_80498B24(GP_NO_ERROR) != 0)
                {
                    return 0xFF;
                }
                response = 2;
                break;
            }
            response = 3;
            break;
        }
        response = 2;
        break;

    case 2:
        if (!gpIsBuddy(lbl_806E2EF8->_00, profileId))
        {
            response = 0xFF;
            break;
        }
        if (matchType == 3
            && lbl_806E2EF8->_14 == lbl_806E2EF8->_16)
        {
            response = 3;
            break;
        }
        if (lbl_806E2EFC.valid == 1 && lbl_806E2EFC._01 == 1)
        {
            response = 0x13;
            break;
        }
        if (lbl_806E2EF8->state == 10
            && (lbl_806E2EF8->_1C != 0 || lbl_806E2EF8->_1A != 0)
            && (ip != 0 || port != 0))
        {
            response = 2;
        }
        else
        {
            response = 4;
        }
        break;

    default:
        response = 0xFF;
        break;
    }
    return response;
}

GPResult fn_80495884(u32 profileId, u32 ip, u16 port)
{
    int aid;
    int i;
    u32 buf[2];
    GPResult result;

    if (lbl_806E2EF8->_17 != 0 && (int)profileId == lbl_806E2EF8->_20)
    {
        return GP_NO_ERROR;
    }

    lbl_806E2EF8->_17 = 1;
    lbl_806E2EF8->_20 = profileId;
    lbl_806E2EF8->_1C0 = 0;
    lbl_806E2EF8->_1D0 = 0;
    qr2_send_statechanged(lbl_806E2EF8->qr2);
    lbl_806E2EF8->_214 = 0;
    lbl_806E2EF8->pidList[lbl_806E2EF8->_14 + 1] = profileId;
    lbl_806E2EF8->_24[lbl_806E2EF8->_14 + 1] = ip;
    lbl_806E2EF8->_A4[lbl_806E2EF8->_14 + 1] = port;
    lbl_806E2EF8->_1BC = ip;
    lbl_806E2EF8->_1B8 = port;

    for (aid = 0; aid < 8; aid++)
    {
        for (i = 0; i <= lbl_806E2EF8->_14; i++)
        {
            if ((u8)aid == lbl_806E2EF8->aidList[i])
            {
                break;
            }
        }
        if (i > lbl_806E2EF8->_14)
        {
            break;
        }
    }
    lbl_806E2EF8->aidList[lbl_806E2EF8->_14 + 1] = aid;

    buf[0] = fn_ByteSwap32(profileId);
    buf[1] = fn_ByteSwap32(
        lbl_806E2EF8->aidList[lbl_806E2EF8->_14 + 1]);
    for (i = 1; i <= lbl_806E2EF8->_14; i++)
    {
        result = fn_8049382C(7, lbl_806E2EF8->pidList[i],
            lbl_806E2EF8->_24[i], lbl_806E2EF8->_A4[i], buf, 2);
        if (result != 0)
        {
            return result;
        }
    }

    if (lbl_806E2EEC != NULL && lbl_806E2EEC->valid != 0)
    {
        lbl_806E2EEC->_08 = 0;
        lbl_806E2EEC->_0C = 0;
        lbl_806E2EEC->_02 = 0;
        lbl_806E2EEC->_18 = OSGetTime();
    }
    return GP_NO_ERROR;
}

int fn_80495B90(int profileId, int arg1)
{
    int count;
    u32 buf[3];

    if (arg1 != 0
        || (lbl_806E2EF8->_1C == 0 && lbl_806E2EF8->_1A == 0))
    {
        lbl_806E2EF8->_1C0 = 1;
        lbl_806E2EF8->_1C8 = OSGetTime();
        lbl_806E2EF8->pidList[0] = profileId;
        if (arg1 == 0)
        {
            DWC_Printf(4, "Delay ResvCommand - qr2IP & qr2Port = 0.\n");
        }
        return 0;
    }

    if (lbl_806E2EF8->matchType == 0)
    {
        SBServer server
            = ServerBrowserGetServer(lbl_806E2EF8->sb, 0);
        lbl_806E2EF8->pidList[0]
            = SBServerGetIntValueA(server, "dwc_pid", 0);
        lbl_806E2EF8->_24[0] = SBServerGetPublicInetAddress(server);
        lbl_806E2EF8->_A4[0] = SBServerGetPublicQueryPort(server);
        count = 1;
        lbl_806E2EF8->_214 = lbl_806E2EF8->pidList[0];
    }
    else
    {
        if (lbl_806E2EF8->matchType == 1)
        {
            lbl_806E2EF8->pidList[0] = profileId;
        }
        lbl_806E2EF8->_214 = profileId;
        count = 3;
        buf[1] = lbl_806E2EF8->_1C;
        buf[2] = fn_ByteSwap32(lbl_806E2EF8->_1A);
    }

    lbl_806E2EF8->_1D0 = 6000;
    lbl_806E2EF8->_1D8 = OSGetTime();
    lbl_806E2EF8->_1C0 = 0;
    buf[0] = fn_ByteSwap32(lbl_806E2EF8->matchType);
    return fn_8049382C(lbl_806E2EF8->_218 == 0 ? 1 : 11, profileId,
        lbl_806E2EF8->_24[0], lbl_806E2EF8->_A4[0], buf, count);
}

int fn_80495D7C(int arg0, BOOL keepIndex, u32 targetPid)
{
    u8 endIndex;
    BOOL wrapped = FALSE;
    int profileId;
    int index;
    GPBuddyStatus status;
    char ver[4];
    char fme[4];
    char mdf[8];

    if (keepIndex)
    {
        endIndex = lbl_806E2EF8->_1A9;
    }
    else
    {
        endIndex = (lbl_806E2EF8->_1A9 < lbl_806E2EF8->_354 - 1)
            ? lbl_806E2EF8->_1A9 + 1
            : 0;
    }

    for (;;)
    {
        if (!keepIndex || wrapped)
        {
            lbl_806E2EF8->_1A9++;
            if (lbl_806E2EF8->_1A9 >= lbl_806E2EF8->_354)
            {
                lbl_806E2EF8->_1A9 = 0;
            }
        }
        if (wrapped && endIndex == lbl_806E2EF8->_1A9)
        {
            lbl_806E2EF8->_1D0 = 3000;
            lbl_806E2EF8->_1D8 = OSGetTime();
            lbl_806E2EF8->_1C0 = 0;
            return 0;
        }
        wrapped = TRUE;

        profileId = DWC_GetGsProfileId(fn_8048C530(),
            (DWCAccFriendData*)((u8*)lbl_806E2EF8->_30C
                + lbl_806E2EF8->_314[lbl_806E2EF8->_1A9] * 0xC));
        if (profileId == 0 || profileId == -1)
        {
            continue;
        }
        if (!DWCi_Acc_IsValidFriendData(
                (DWCAccFriendData*)((u8*)lbl_806E2EF8->_30C
                    + lbl_806E2EF8->_314[lbl_806E2EF8->_1A9] * 0xC)))
        {
            continue;
        }
        {
            int i;
            for (i = 1; i <= lbl_806E2EF8->_0D; i++)
            {
                if (profileId == lbl_806E2EF8->pidList[i])
                {
                    break;
                }
            }
            if (i <= lbl_806E2EF8->_0D)
            {
                continue;
            }
        }
        if ((gpGetBuddyIndex(lbl_806E2EF8->_00, profileId, &index)
                | gpGetBuddyStatus(lbl_806E2EF8->_00, index, &status))
            != 0)
        {
            continue;
        }
        if (status.status != 4)
        {
            continue;
        }
        if (DWC_GetCommonValueString("VER", ver, status.statusString, '/')
                <= 0
            || DWC_GetCommonValueString("FME", fme, status.statusString,
                   '/')
                <= 0
            || DWC_GetCommonValueString("MDF", mdf, status.statusString,
                   '/')
                <= 0)
        {
            continue;
        }
        if (strtoul(ver, NULL, 10) != 3)
        {
            continue;
        }
        if (strtoul(fme, NULL, 10) != lbl_806E2EF8->_16)
        {
            continue;
        }
        if (profileId == (int)targetPid)
        {
            arg0 = 1;
        }
        return fn_80495B90(profileId, arg0);
    }
}

int fn_80496000(int profileId)
{
    lbl_806E2EF8->_218 = 0;
    lbl_806E2EF8->_214 = 0;
    lbl_806E2EF8->_1AB = 0;
    lbl_806E2EF8->_1D8 = OSGetTime();

    if (lbl_806E2EF8->matchType == 0)
    {
        lbl_806E2EF8->state = 3;
        if (fn_80498C78(fn_80493128(0)) != 0)
        {
            return 0;
        }
    }
    else if (lbl_806E2EF8->matchType == 1)
    {
        fn_80495D7C(0, 0, profileId);
        if (lbl_806E2EF8->matchType == 0)
        {
            if (fn_80498C78(GP_NO_ERROR) != 0)
            {
                return 0;
            }
        }
        else if (fn_80498B24(GP_NO_ERROR) != 0)
        {
            return 0;
        }
    }
    else if (lbl_806E2EF8->matchType == 3)
    {
        fn_80491EDC(DWC_ERROR_TYPE_6, DWC_ECODE_SEQ_MATCHING - 410);
        return 0;
    }
    return 1;
}

int fn_80496188(int profileId)
{
    BOOL wasClient;
    int i;

    if (lbl_806E2EF8->_17 != 0
        && lbl_806E2EF8->_20 == lbl_806E2EF8->_210)
    {
        DWC_Printf(4, "DWCi_CancelPreConnectedServerProcess : client\n");
        wasClient = FALSE;
    }
    else
    {
        DWC_Printf(4, "DWCi_CancelPreConnectedServerProcess : server\n");
        wasClient = TRUE;
    }

    if (wasClient)
    {
        lbl_806E2EF8->_17 = 0;
        lbl_806E2EF8->_20 = 0;
        qr2_send_statechanged(lbl_806E2EF8->qr2);
    }
    if (lbl_806E2EF8->_0D < 31)
    {
        lbl_806E2EF8->pidList[lbl_806E2EF8->_0D + 1] = 0;
    }
    lbl_806E2EF8->_3E0 = 0xFF;
    if (lbl_806E2EF8->_1A0 != 0)
    {
        NNCancel(lbl_806E2EF8->_1A0);
        lbl_806E2EF8->_1A0 = 0;
    }
    lbl_806E2EF8->_14 = lbl_806E2EF8->_0D;
    lbl_806E2EF8->_214 = 0;

    if (wasClient == 0)
    {
        if (lbl_806E2EF8->matchType == 3)
        {
            return 1;
        }
        if (lbl_806E2EF8->matchType == 2
            || lbl_806E2EF8->matchType == 3)
        {
            DWC_Printf(8,
                "DWCi_RestartFromTimeout() shouldn't be called.\n");
            return 1;
        }
        lbl_806E2EF8->closeState = 2;
        gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
        lbl_806E2EF8->closeState = 0;
        DWC_Printf(0x40,
            "Closed all connections and restart matching.\n");
        fn_804974BC(1);
        return 1;
    }

    if (lbl_806E2EF8->matchType == 0)
    {
        lbl_806E2EF8->state = 3;
        lbl_806E2EF8->_E8 = 2;
        lbl_806E2EF8->_F0 = OSGetTime();
        return 1;
    }
    if (lbl_806E2EF8->matchType == 1)
    {
        lbl_806E2EF8->state = 4;
        fn_80495D7C(1, 0, 0);
        return 1;
    }
    if (lbl_806E2EF8->matchType == 2)
    {
        GT2Connection* connection;
        u32 buf[1];

        lbl_806E2EF8->state = 14;
        lbl_806E2EF8->_1E4 = 0;
        lbl_806E2EF8->_1B6 = 0;
        lbl_806E2EF8->_21C = profileId;
        connection = fn_8048E334(lbl_806E2EF8->_0D + 1);
        if (connection != NULL)
        {
            lbl_806E2EF8->closeState = 2;
            gt2CloseConnectionHard(*connection);
            lbl_806E2EF8->closeState = 0;
        }
        else
        {
            for (i = 0; i <= lbl_806E2EF8->_0D; i++)
            {
                if (profileId == lbl_806E2EF8->pidList[i])
                {
                    fn_80492660(i, lbl_806E2EF8->_0D + 1);
                    break;
                }
            }
        }

        for (i = 1; i <= lbl_806E2EF8->_0D; i++)
        {
            int clientPid = lbl_806E2EF8->pidList[i];

            DWC_Printf(0x80, "Sent CANCEL SYN %d command to %u.\n", 0,
                clientPid);
            buf[0] = fn_ByteSwap32(lbl_806E2EF8->_21C);
            if (fn_8049382C(13, clientPid, 0, 0, buf, 1) != 0)
            {
            }
            if (lbl_806E2EF8->matchType == 0)
            {
                if (fn_80498C78(GP_NO_ERROR) != 0)
                {
                    return 0;
                }
            }
            else if (fn_80498B24(GP_NO_ERROR) != 0)
            {
                return 0;
            }
            lbl_806E2EF8->_1F8 = OSGetTime();
        }
        if (lbl_806E2EF8->_0D == 0)
        {
            fn_804974BC(2);
        }
    }
    return 1;
}

int fn_80496520(int profileId)
{
    if (lbl_806E2EF8->matchType == 3)
    {
        if (lbl_806E2EF8->_0D != 0)
        {
            lbl_806E2EF8->closeState = 2;
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
            lbl_806E2EF8->closeState = 0;
        }
        fn_80491EDC(DWC_ERROR_TYPE_6, DWC_ECODE_SEQ_MATCHING - 430);
        return 0;
    }

    lbl_806E2EF8->_14 = lbl_806E2EF8->_0D;
    lbl_806E2EF8->_218 = 0;
    if (lbl_806E2EF8->_1A0 != 0)
    {
        NNCancel(lbl_806E2EF8->_1A0);
        lbl_806E2EF8->_1A0 = 0;
    }

    if (lbl_806E2EF8->_0D != 0)
    {
        DWC_Printf(0x40, "Close all connection and restart matching.\n");
        if (lbl_806E2EF8->matchType == 2
            || lbl_806E2EF8->matchType == 3)
        {
            DWC_Printf(8,
                "DWCi_RestartFromTimeout() shouldn't be called.\n");
            return 1;
        }
        lbl_806E2EF8->closeState = 2;
        gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
        lbl_806E2EF8->closeState = 0;
        DWC_Printf(0x40,
            "Closed all connections and restart matching.\n");
        fn_804974BC(1);
        return 1;
    }

    DWC_Printf(0x40, "Cancel and restart client process.\n");
    lbl_806E2EF8->state = 4;
    DWC_Printf(0x40, "Cancel and retry to reserve.\n");
    return fn_80496000(0);
}

int fn_80496740(int mode)
{
    int count = 3;
    BOOL clear = FALSE;
    u32 buf[5];
    int i;

    switch (mode)
    {
    case 0:
        if (lbl_806E2EF8->_1A8 < lbl_806E2EF8->_0D - 1)
        {
            DWC_Printf(0x40, "Send client-client link request.\n");
            lbl_806E2EF8->state = 13;
            count = 5;
            buf[0] = fn_ByteSwap32(
                lbl_806E2EF8->pidList[lbl_806E2EF8->_1A8 + 1]);
            buf[1] = fn_ByteSwap32(lbl_806E2EF8->_1A8 + 1);
            buf[2] = fn_ByteSwap32(
                lbl_806E2EF8->aidList[lbl_806E2EF8->_1A8 + 1]);
            buf[3] = lbl_806E2EF8->_24[lbl_806E2EF8->_1A8 + 1];
            buf[4] = fn_ByteSwap32(
                lbl_806E2EF8->_A4[lbl_806E2EF8->_1A8 + 1]);
        }
        else
        {
            DWC_Printf(0x40, "Tell new client completion of matching.\n");
            lbl_806E2EF8->_17 = 0;
            lbl_806E2EF8->_20 = 0;
            qr2_send_statechanged(lbl_806E2EF8->qr2);
            if (lbl_806E2EF8->matchType == 0)
            {
                lbl_806E2EF8->state = 3;
            }
            else if (lbl_806E2EF8->matchType == 1)
            {
                lbl_806E2EF8->state = 4;
            }
            else
            {
                lbl_806E2EF8->state = 10;
            }
            lbl_806E2EF8->_1A8 = 0;

            if (lbl_806E2EF8->matchType == 2
                || lbl_806E2EF8->_0D == lbl_806E2EF8->_16)
            {
                if (lbl_806E2EF8->matchType == 2)
                {
                    lbl_806E2EF8->_21C
                        = lbl_806E2EF8->pidList[lbl_806E2EF8->_0D];
                }
                else
                {
                    lbl_806E2EF8->_21C = 0;
                    lbl_806E2EF8->pidList[0] = lbl_806E2EF8->_210;
                }
                lbl_806E2EF8->state = 16;
                lbl_806E2EF8->_1E0 = 0;
                for (i = 1; i <= lbl_806E2EF8->_0D; i++)
                {
                    fn_804978F0(lbl_806E2EF8->aidList[i], 2);
                }
            }
            else
            {
                buf[0] = 0;
                buf[1] = fn_ByteSwap32(lbl_806E2EF8->_0D);
                buf[2] = fn_ByteSwap32(
                    lbl_806E2EF8->aidList[lbl_806E2EF8->_0D]);
                if (lbl_806E2EF8->matchType == 0)
                {
                    lbl_806E2EF8->_E8 = 2;
                    lbl_806E2EF8->_F0 = OSGetTime();
                }
                else if (lbl_806E2EF8->matchType == 1)
                {
                    fn_80495D7C(1, 0, 0);
                    if (lbl_806E2EF8->_1AA == 0
                        && lbl_806E2EF8->_0D >= 2)
                    {
                        u32 bitmap = 0;

                        for (i = 1; i < lbl_806E2EF8->_0D; i++)
                        {
                            bitmap
                                |= 1 << lbl_806E2EF8->aidList[i];
                        }
                        bitmap &= ~(
                            1 << lbl_806E2EF8
                                     ->aidList[lbl_806E2EF8->_0D]);
                        if (lbl_806E2EF8->_1E8 != bitmap)
                        {
                            DWC_Printf(0x40,
                                "FRIEND_ACCEPT command droped.\n");
                            if (lbl_806E2EF8->matchType == 2
                                || lbl_806E2EF8->matchType == 3)
                            {
                                DWC_Printf(8,
                                    "DWCi_RestartFromTimeout() "
                                    "shouldn't be called.\n");
                                return 1;
                            }
                            lbl_806E2EF8->closeState = 2;
                            gt2CloseAllConnectionsHard(
                                *lbl_806E2EF8->pGT2Socket);
                            lbl_806E2EF8->closeState = 0;
                            DWC_Printf(0x40,
                                "Closed all connections and restart "
                                "matching.\n");
                            fn_804974BC(1);
                            return 1;
                        }
                    }
                }
                if (lbl_806E2EF8->matchType != 2)
                {
                    clear = TRUE;
                }
            }
        }
        if (lbl_806E2EF8->state != 16)
        {
            if (lbl_806E2EF8->matchType == 0)
            {
                if (fn_80498C78(fn_8049382C(8,
                        lbl_806E2EF8->pidList[lbl_806E2EF8->_0D],
                        lbl_806E2EF8->_24[lbl_806E2EF8->_0D],
                        lbl_806E2EF8->_A4[lbl_806E2EF8->_0D], buf,
                        count))
                    != 0)
                {
                    return 0;
                }
            }
            else if (fn_80498B24(fn_8049382C(8,
                         lbl_806E2EF8->pidList[lbl_806E2EF8->_0D],
                         lbl_806E2EF8->_24[lbl_806E2EF8->_0D],
                         lbl_806E2EF8->_A4[lbl_806E2EF8->_0D], buf,
                         count))
                != 0)
            {
                return 0;
            }
            lbl_806E2EF8->_3E1 = 0;
        }
        break;

    case 1:
        lbl_806E2EF8->state = 1;
        if (lbl_806E2EF8->matchType == 3)
        {
            lbl_806E2EF8->_21C
                = lbl_806E2EF8->pidList[lbl_806E2EF8->_0D];
        }
        clear = TRUE;
        break;

    case 2:
        lbl_806E2EF8->state = 1;
        if (lbl_806E2EF8->matchType == 0
            || lbl_806E2EF8->matchType == 1)
        {
            lbl_806E2EF8->_17 = 1;
            lbl_806E2EF8->_20 = lbl_806E2EF8->_210;
        }
        lbl_806E2EF8->_1B1 = 0;
        lbl_806E2EF8->_208 = OSGetTime();
        if (lbl_806E2EF8->_0D > 1)
        {
            buf[0] = fn_ByteSwap32(
                lbl_806E2EF8->pidList[lbl_806E2EF8->_0D - 1]);
            if (lbl_806E2EF8->matchType == 0)
            {
                if (fn_80498C78(fn_8049382C(9, lbl_806E2EF8->pidList[0],
                        lbl_806E2EF8->_24[0], lbl_806E2EF8->_A4[0], buf,
                        1))
                    != 0)
                {
                    return 0;
                }
            }
            else if (fn_80498B24(fn_8049382C(9,
                         lbl_806E2EF8->pidList[0], lbl_806E2EF8->_24[0],
                         lbl_806E2EF8->_A4[0], buf, 1))
                != 0)
            {
                return 0;
            }
        }
        return 1;

    case 3:
        lbl_806E2EF8->state = 1;
        lbl_806E2EF8->_1B1 = 0;
        lbl_806E2EF8->_208 = OSGetTime();
        lbl_806E2EF8->_21C = 0;
        clear = TRUE;
        count = 0;
        return 1;

    case 4:
        DWC_Printf(0x40, "Completed matching!\n");
        if (lbl_806E2EF8->matchType != 2)
        {
            fn_8048AFCC(2, "", NULL);
        }
        lbl_806E2EF8->matchedCallback(0, FALSE, lbl_806E2EF8->_21C == 0,
            FALSE, fn_8048AEC4(lbl_806E2EF8->_21C),
            lbl_806E2EF8->matchedParam);
        if (lbl_806E2EF8->matchType == 0
            || lbl_806E2EF8->matchType == 1)
        {
            DWCi_CloseMatching();
        }
        else
        {
            if (lbl_806E2EF8->sb != NULL)
            {
                ServerBrowserFree(lbl_806E2EF8->sb);
                lbl_806E2EF8->sb = NULL;
            }
            NNFreeNegotiateList();
            if (lbl_806E2EF8->matchType == 2)
            {
                if (fn_80498B24(fn_80492BA0()) != 0)
                {
                    return 0;
                }
                if (lbl_806E2EFC.valid == 1)
                {
                    lbl_806E2EFC._01 = 1;
                    DWC_Printf(4,
                        "[OPT_SC_BLOCK] Connect block start!\n");
                }
                lbl_806E2EF8->state = 10;
            }
            else
            {
                lbl_806E2EF8->state = 1;
            }
            lbl_806E2EF8->_21C = 0;
        }
        lbl_806E2EF8->_1AD = 0;
        break;

    default:
        break;
    }

    if (clear && lbl_806E2EF8->matchType != 3)
    {
        ServerBrowserClear(lbl_806E2EF8->sb);
    }
    return 1;
}

void fn_804970B0(void)
{
    DWC_Printf(4, "CANCEL! state %d, numHost nn=%d gt2=%d.\n",
        lbl_806E2EF8->state, lbl_806E2EF8->_14, lbl_806E2EF8->_0D);
    lbl_806E2EF8->_21C = 0;

    if (lbl_806E2EF8->state == 2)
    {
        DWCi_FinishCancelMatching();
        return;
    }

    lbl_806E2EF8->_1AD = 1;
    if (lbl_806E2EF8->matchType == 3)
    {
        lbl_806E2EF8->_1AD = 1;
        if (lbl_806E2EF8->_0D != 0)
        {
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
        }
        else
        {
            GPResult result = fn_8049382C(5, lbl_806E2EF8->pidList[0],
                lbl_806E2EF8->_24[0], lbl_806E2EF8->_A4[0], NULL, 0);
            lbl_806E2EF8->_214 = 0;
            if (lbl_806E2EF8->matchType == 0)
            {
                result = fn_80498C78(result);
            }
            else
            {
                result = fn_80498B24(result);
            }
            if (result != 0)
            {
                return;
            }
        }
        DWCi_FinishCancelMatching();
        return;
    }

    if (lbl_806E2EF8->state == 4 || lbl_806E2EF8->state == 5
        || lbl_806E2EF8->state == 6 || lbl_806E2EF8->state == 7
        || lbl_806E2EF8->state == 11 || lbl_806E2EF8->state == 12)
    {
        GPResult result;

        if (lbl_806E2EF8->_214 != 0)
        {
            result = fn_8049382C(5, lbl_806E2EF8->pidList[0],
                lbl_806E2EF8->_24[0], lbl_806E2EF8->_A4[0], NULL, 0);
            lbl_806E2EF8->_214 = 0;
            if (lbl_806E2EF8->matchType == 0)
            {
                result = fn_80498C78(result);
            }
            else
            {
                result = fn_80498B24(result);
            }
            if (result != 0)
            {
                return;
            }
        }

        {
            u32 buf = 0;

            result = GP_NO_ERROR;
            if (lbl_806E2EF8->_17 != 0 && lbl_806E2EF8->_20 != 0
                && lbl_806E2EF8->_20 != lbl_806E2EF8->_210)
            {
                result = fn_8049382C(12, lbl_806E2EF8->_20,
                    lbl_806E2EF8->_24[lbl_806E2EF8->_0D + 1],
                    lbl_806E2EF8->_A4[lbl_806E2EF8->_0D + 1], &buf, 1);
                lbl_806E2EF8->_17 = 0;
                lbl_806E2EF8->_20 = 0;
            }
            if (lbl_806E2EF8->matchType == 0)
            {
                result = fn_80498C78(result);
            }
            else
            {
                result = fn_80498B24(result);
            }
            if (result != 0)
            {
                return;
            }
        }
    }

    if (lbl_806E2EF8->_1A0 != 0)
    {
        NNCancel(lbl_806E2EF8->_1A0);
        lbl_806E2EF8->_1A0 = 0;
    }
    if (lbl_806E2EF8->_0D != 0)
    {
        lbl_806E2EF8->_1AD = 1;
        gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
    }
    DWCi_FinishCancelMatching();
}

static void DWCi_FinishCancelMatching(void)
{
    GPResult gpResult;

    gpResult = fn_8048AFCC(1, "", NULL);
    if (fn_80498B24(gpResult))
    {
        return;
    }

    DWCi_CloseMatching();
    DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE, TRUE,
        DWCi_GetMatchCnt()->_21C ? FALSE : TRUE,
        DWCi_GetMatchCnt()->_21C
            ? TRUE
            : ((DWCi_GetMatchCnt()->matchType == 2) ? TRUE : FALSE),
        fn_8048AEC4(DWCi_GetMatchCnt()->_21C),
        DWCi_GetMatchCnt()->matchedParam);

    DWCi_GetMatchCnt()->_1AD = 0;
}

void fn_804974BC(DWCMatchResetLevel level)
{
    SBError sbError;

    if (level == DWC_MATCH_RESET_ALL)
    {
        DWCi_FinishCancelMatching();
    }
    else
    {
        fn_80492D4C(level);

        if (DWCi_GetMatchCnt()->matchType == 2
            || DWCi_GetMatchCnt()->matchType == 3)
        {
            DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE, TRUE,
                DWCi_GetMatchCnt()->_21C ? FALSE : TRUE, FALSE,
                fn_8048AEC4(DWCi_GetMatchCnt()->_21C),
                DWCi_GetMatchCnt()->matchedParam);
        }
        else if (DWCi_GetMatchCnt()->matchType == 0)
        {
            if (level == DWC_MATCH_RESET_RESTART)
            {
                sbError = fn_80493128(0);
                if (fn_80498C78(sbError))
                {
                    return;
                }
            }
        }
        else if (DWCi_GetMatchCnt()->matchType == 1)
        {
            if (level == DWC_MATCH_RESET_RESTART)
            {
                (void)fn_80495D7C(FALSE, FALSE, 0);
            }
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR,
                "ERROR - DWCi_RestartFromCancel : matchType %d, level %d\n",
                DWCi_GetMatchCnt()->matchType, level);
        }
    }
}

int fn_80497654(void)
{
    int result;

    lbl_806E2EF8->_17 = 0;
    lbl_806E2EF8->_20 = 0;
    lbl_806E2EF8->closeState = 0;

    if (lbl_806E2EF8->_218 != 0)
    {
        if (lbl_806E2EF8->matchType == 0)
        {
            lbl_806E2EF8->state = 3;
            result = fn_80493128(0);
            if (fn_80498C78(result) != 0)
            {
                return result;
            }
        }
        else if (lbl_806E2EF8->matchType == 1)
        {
            int error;

            lbl_806E2EF8->state = 4;
            result = fn_80495B90(lbl_806E2EF8->_218, 0);
            if (lbl_806E2EF8->matchType == 0)
            {
                error = fn_80498C78(result);
            }
            else
            {
                error = fn_80498B24(result);
            }
            if (error != 0)
            {
                return result;
            }
        }
    }
    else
    {
        fn_804974BC(1);
    }
    return 0;
}

int fn_80497738(u32 aidBitmap)
{
    u32 ackedPids[32];
    u32 pendingPids[32];
    int ackedCount = 0;
    int pendingCount = 0;
    int i;
    GPResult result;

    for (i = 1; i <= lbl_806E2EF8->_0D; i++)
    {
        if (aidBitmap & (1 << lbl_806E2EF8->aidList[i]))
        {
            ackedPids[ackedCount++] = lbl_806E2EF8->pidList[i];
        }
        else
        {
            pendingPids[pendingCount++]
                = fn_ByteSwap32(lbl_806E2EF8->pidList[i]);
        }
    }

    for (i = 0; i < ackedCount; i++)
    {
        result = fn_8049382C(16, ackedPids[i], 0, 0, pendingPids,
            pendingCount);
        if (lbl_806E2EF8->matchType == 0)
        {
            result = fn_80498C78(result);
        }
        else
        {
            result = fn_80498B24(result);
        }
        if (result != 0)
        {
            return 0;
        }
    }

    lbl_806E2EF8->closeState = 2;
    for (i = 0; i < pendingCount; i++)
    {
        u8 aid = 0xFF;
        int j;

        for (j = 1; j <= lbl_806E2EF8->_0D; j++)
        {
            if (lbl_806E2EF8->pidList[j] == pendingPids[i])
            {
                aid = lbl_806E2EF8->aidList[j];
                break;
            }
        }
        if (aid != 0xFF)
        {
            DWC_CloseConnectionHard(aid);
        }
    }
    lbl_806E2EF8->closeState = 0;
    return 1;
}

void fn_804978F0(u8 aid, int type)
{
    u8 buf[4];

    DWC_Printf(0x80, "Sent SYN %d packet to aid %d.\n", type - 2, aid);
    switch (type)
    {
    case 2:
    {
        u8 i;

        buf[0] = lbl_806E2EF8->aidList[lbl_806E2EF8->_0D] == aid;
        for (i = 1; i <= lbl_806E2EF8->_0D; i++)
        {
            if (lbl_806E2EF8->aidList[i] == aid)
            {
                buf[1] = i;
                buf[2] = aid;
                break;
            }
        }
        break;
    }

    case 3:
        buf[0] = lbl_806E2EF8->_1B4;
        buf[1] = (lbl_806E2EF8->_1B4 >> 8) & 0xFF;
        break;
    }
    fn_8049AE0C(type, aid, buf, 4);
    lbl_806E2EF8->_1F0 = OSGetTime();
}

int fn_804979F4(void)
{
    s64 elapsed;

    if (lbl_806E2EF8->state == 9 || lbl_806E2EF8->state == 16
        || lbl_806E2EF8->state == 17)
    {
        elapsed = fn_ElapsedMSec(lbl_806E2EF8->_1F0);
    }
    else
    {
        return 1;
    }

    switch (lbl_806E2EF8->state)
    {
    case 9:
        if (elapsed > 6000)
        {
            DWC_Printf(0x80, "[SYN] No ACK from server %d/%d.\n",
                lbl_806E2EF8->_1B1, 5);
            if (DWC_GetState() == DWC_STATE_MATCHING
                && lbl_806E2EF8->_1B1 >= 5)
            {
                DWC_Printf(0x80,
                    "Timeout: [SYN] Connection to server was shut down.\n");
                if (fn_80496520(lbl_806E2EF8->pidList[0]) == 0)
                {
                    return 0;
                }
            }
            else
            {
                lbl_806E2EF8->_1B1++;
                fn_804978F0(lbl_806E2EF8->aidList[0], 3);
            }
        }
        break;

    case 16:
        if (elapsed > 6000)
        {
            lbl_806E2EF8->_1AF++;
            if (lbl_806E2EF8->_1AF > 5)
            {
                DWC_Printf(0x40,
                    "Timeout: wait SYN-ACK (aidbitmap 0x%x). "
                    "Restart matching.\n",
                    lbl_806E2EF8->_1E0);
                if (lbl_806E2EF8->matchType == 0
                    || lbl_806E2EF8->matchType == 1)
                {
                    lbl_806E2EF8->closeState = 2;
                    gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
                    lbl_806E2EF8->closeState = 0;
                    fn_804974BC(1);
                }
                else
                {
                    if (fn_80497738(lbl_806E2EF8->_1E0) == 0)
                    {
                        return 0;
                    }
                    if (lbl_806E2EF8->_0D != 0)
                    {
                        lbl_806E2EF8->_1AF = 0;
                        lbl_806E2EF8->_1F0 = OSGetTime();
                    }
                    else if (fn_80496188(lbl_806E2EF8->_21C) == 0)
                    {
                        return 0;
                    }
                }
            }
            else
            {
                int i;

                for (i = 1; i <= lbl_806E2EF8->_0D; i++)
                {
                    if ((lbl_806E2EF8->_1E0
                            & (1 << lbl_806E2EF8->aidList[i]))
                        == 0)
                    {
                        fn_804978F0(lbl_806E2EF8->aidList[i], 2);
                    }
                }
            }
        }
        break;

    case 17:
        if (elapsed > lbl_806E2EF8->_1B4)
        {
            fn_80496740(4);
        }
        break;
    }
    return 1;
}

static int fn_SendCancelSyn(int command, u32 profileId, const u32* data,
    int count)
{
    GPResult result;

    DWC_Printf(0x80, "Sent CANCEL SYN %d command to %u.\n", command - 13,
        profileId);
    result = fn_8049382C(command, profileId, 0, 0, data, count);
    if (lbl_806E2EF8->matchType == 0)
    {
        result = fn_80498C78(result);
    }
    else
    {
        result = fn_80498B24(result);
    }
    if (result != 0)
    {
        return 0;
    }
    lbl_806E2EF8->_1F8 = OSGetTime();
    return 1;
}

int fn_80497CB4(u32 profileId, int command, u32 data0)
{
    DWC_Printf(0x80, "Received CANCEL SYN %d command from %u.\n",
        command - 13, profileId);
    if (DWC_GetState() != DWC_STATE_CONNECTED)
    {
        DWC_Printf(0x80, "Ignore delayed CANCEL SYN.\n");
        return 1;
    }

    switch (command)
    {
    case 13:
    {
        u32 buf;

        if (lbl_806E2EF8->state != 8)
        {
            GT2Connection* connection;

            lbl_806E2EF8->state = 8;
            lbl_806E2EF8->_21C = data0;
            connection = fn_8048E334(lbl_806E2EF8->_0D + 1);
            if (connection != NULL)
            {
                lbl_806E2EF8->closeState = 2;
                gt2CloseConnectionHard(*connection);
                lbl_806E2EF8->closeState = 0;
            }
            else if (lbl_806E2EF8 != NULL)
            {
                int count = lbl_806E2EF8->_0D + 1;
                int i;

                for (i = 0; i < count; i++)
                {
                    if (lbl_806E2EF8->pidList[i] == profileId)
                    {
                        fn_80492660(i, count);
                        break;
                    }
                }
            }
        }
        if (fn_SendCancelSyn(14, profileId, &buf, 0) == 0)
        {
            return 0;
        }
        break;
    }

    case 14:
        if (lbl_806E2EF8->state == 14)
        {
            u64 elapsed;
            u8 aid = 0xFF;
            int i;

            elapsed = (u64)(OSGetTime() - lbl_806E2EF8->_1F8)
                / (OS_BUS_CLOCK_SPEED / 4 / 1000) / 2;
            if (elapsed > 300 && elapsed - 300 > lbl_806E2EF8->_1B6)
            {
                lbl_806E2EF8->_1B6 = elapsed - 300;
            }

            for (i = 1; i <= lbl_806E2EF8->_0D; i++)
            {
                if (lbl_806E2EF8->pidList[i] == profileId)
                {
                    aid = lbl_806E2EF8->aidList[i];
                    break;
                }
            }
            if (aid != 0xFF)
            {
                lbl_806E2EF8->_1E4 |= 1 << aid;
            }

            if ((lbl_806E2EF8->aidBitmap & ~1) == lbl_806E2EF8->_1E4)
            {
                u32 buf;

                for (i = 1; i <= lbl_806E2EF8->_0D; i++)
                {
                    if (fn_SendCancelSyn(15, lbl_806E2EF8->pidList[i], &buf,
                            0)
                        == 0)
                    {
                        return 0;
                    }
                }
                lbl_806E2EF8->state = 15;
                DWC_Printf(0x80, "Wait max latency %d msec.\n",
                    lbl_806E2EF8->_1B6);
            }
        }
        else
        {
            u32 buf;

            if (fn_SendCancelSyn(15, profileId, &buf, 0) == 0)
            {
                return 0;
            }
        }
        break;

    case 15:
        if (lbl_806E2EF8->state == 8)
        {
            fn_804974BC(2);
        }
        break;
    }
    return 1;
}

int fn_8049811C(void)
{
    s64 elapsed;

    if (lbl_806E2EF8->state == 8 || lbl_806E2EF8->state == 14
        || lbl_806E2EF8->state == 15)
    {
        elapsed = fn_ElapsedMSec(lbl_806E2EF8->_1F8);
    }
    else
    {
        return 1;
    }

    switch (lbl_806E2EF8->state)
    {
    case 8:
        if (elapsed > 6000)
        {
            u32 buf;

            if (fn_SendCancelSyn(14, lbl_806E2EF8->pidList[0], &buf, 0)
                == 0)
            {
                return 0;
            }
        }
        break;

    case 14:
        if (elapsed > 6000)
        {
            lbl_806E2EF8->_1B0++;
            if (lbl_806E2EF8->_1B0 > 5)
            {
                DWC_Printf(0x40,
                    "Timeout: wait cancel SYN-ACK (aidbitmap 0x%x).\n",
                    lbl_806E2EF8->_1E4);
                if (fn_80497738(lbl_806E2EF8->_1E4) == 0)
                {
                    return 0;
                }
                if (lbl_806E2EF8->_0D != 0)
                {
                    lbl_806E2EF8->_1B0 = 0;
                    lbl_806E2EF8->_1F8 = OSGetTime();
                }
                else
                {
                    fn_804974BC(2);
                }
            }
            else
            {
                int i;

                for (i = 1; i <= lbl_806E2EF8->_0D; i++)
                {
                    if ((lbl_806E2EF8->_1E4
                            & (1 << lbl_806E2EF8->aidList[i]))
                        == 0)
                    {
                        u32 buf = fn_ByteSwap32(lbl_806E2EF8->_21C);

                        if (fn_SendCancelSyn(13, lbl_806E2EF8->pidList[i],
                                &buf, 1)
                            == 0)
                        {
                            return 0;
                        }
                    }
                }
            }
        }
        break;

    case 15:
        if (elapsed > lbl_806E2EF8->_1B6)
        {
            fn_804974BC(2);
        }
        break;
    }
    return 1;
}

static void DWCi_CloseAllConnectionsByTimeout(void)
{
    DWCi_GetMatchCnt()->closeState = 2;
    gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGT2Socket);
    DWCi_GetMatchCnt()->closeState = 0;
}

static void DWCi_ClearGameMatchKeys(void)
{
    int i;

    for (i = 0; i < 154; i++)
    {
        if (lbl_806C9A20[i].keyString != NULL)
        {
            DWC_Free(DWC_ALLOCTYPE_BASE, lbl_806C9A20[i].keyString, 0);
        }
    }
    memset(lbl_806C9A20, 0, sizeof(lbl_806C9A20));
}

void fn_80498440(void)
{
    if (lbl_806E2EEC == NULL)
    {
        return;
    }
    if (lbl_806E2EEC->valid == 0)
    {
        return;
    }
    if (lbl_806E2EF8->matchType == 2)
    {
        return;
    }
    if (lbl_806E2EF8->matchType == 3)
    {
        return;
    }

    if (lbl_806E2EF8->state == 19)
    {
        u32 bitmap = 0;
        int i;
        GPResult result;

        for (i = 1; i <= lbl_806E2EF8->_0D; i++)
        {
            bitmap |= 1 << lbl_806E2EF8->aidList[i];
        }

        if (lbl_806E2EEC->_08 == bitmap)
        {
            if (lbl_806E2EEC->_0C == bitmap)
            {
                DWC_Printf(0x80,
                    "[OPT_MIN_COMP] Timeout occured in all hosts.\n");
                lbl_806E2EF8->_16 = lbl_806E2EF8->_0D;
                lbl_806E2EF8->_1A8 = lbl_806E2EF8->_0D - 1;
                fn_80496740(0);
            }
            else
            {
                DWC_Printf(0x80,
                    "[OPT_MIN_COMP] Some clients is in time.\n");
                lbl_806E2EEC->_18 = OSGetTime();
                lbl_806E2EEC->_08 = 0;
                if (lbl_806E2EF8->matchType == 0)
                {
                    lbl_806E2EF8->state = 3;
                    lbl_806E2EF8->_E8 = 2;
                    lbl_806E2EF8->_F0 = OSGetTime();
                }
                else
                {
                    lbl_806E2EF8->state = 4;
                    fn_80495D7C(1, 0, 0);
                }
            }
        }
        else if (fn_ElapsedMSec(lbl_806E2EEC->_18)
            >= lbl_806E2EEC->_02 * 6000)
        {
            DWC_Printf(4, "[OPT_MIN_COMP] Timeout: wait poll-ACK %d/%d.\n",
                lbl_806E2EEC->_02 - 1, 5);
            if (lbl_806E2EEC->_02 > 5)
            {
                DWC_Printf(4,
                    "[OPT_MIN_COMP] Timeout: aidbitmap 0x%x. "
                    "Restart matching.\n",
                    lbl_806E2EEC->_08);
                if (lbl_806E2EEC != NULL && lbl_806E2EEC->valid != 0)
                {
                    lbl_806E2EEC->_08 = 0;
                    lbl_806E2EEC->_0C = 0;
                    lbl_806E2EEC->_02 = 0;
                    lbl_806E2EEC->_18 = OSGetTime();
                }
                lbl_806E2EF8->closeState = 2;
                gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
                lbl_806E2EF8->closeState = 0;
                fn_804974BC(1);
            }
            else
            {
                for (i = 1; i <= lbl_806E2EF8->_0D; i++)
                {
                    if ((lbl_806E2EEC->_08
                            & (1 << lbl_806E2EF8->aidList[i]))
                        == 0)
                    {
                        result = fn_8049382C(17, lbl_806E2EF8->pidList[i],
                            lbl_806E2EF8->_24[i], lbl_806E2EF8->_A4[i],
                            NULL, 0);
                        if (lbl_806E2EF8->matchType == 0)
                        {
                            result = fn_80498C78(result);
                        }
                        else
                        {
                            result = fn_80498B24(result);
                        }
                        if (result != 0)
                        {
                            return;
                        }
                    }
                }
                lbl_806E2EEC->_02++;
            }
        }
    }
    else if (lbl_806E2EF8->state == 3 || lbl_806E2EF8->state == 4)
    {
        int i;
        GPResult result;

        if (lbl_806E2EF8->_0D < lbl_806E2EEC->minEntry - 1)
        {
            return;
        }
        if (lbl_806E2EEC->_02 != 0
            || fn_ElapsedMSec(lbl_806E2EEC->startTime)
                < lbl_806E2EEC->timeout)
        {
            if (lbl_806E2EEC->_02 == 0)
            {
                return;
            }
            if (fn_ElapsedMSec(lbl_806E2EEC->_18)
                < lbl_806E2EEC->timeout / 4)
            {
                return;
            }
        }

        if (lbl_806E2EF8->_214 != 0)
        {
            result = fn_8049382C(5, lbl_806E2EF8->_214,
                lbl_806E2EF8->_24[0], lbl_806E2EF8->_A4[0], NULL, 0);
            lbl_806E2EF8->_214 = 0;
            if (lbl_806E2EF8->matchType == 0)
            {
                result = fn_80498C78(result);
            }
            else
            {
                result = fn_80498B24(result);
            }
            if (result != 0)
            {
                return;
            }
        }

        lbl_806E2EF8->state = 19;
        DWC_Printf(0x80, "[OPT_MIN_COMP] Poll timeout (my time is %lu).\n",
            fn_ElapsedMSec(lbl_806E2EEC->startTime));
        for (i = 1; i <= lbl_806E2EF8->_0D; i++)
        {
            result = fn_8049382C(17, lbl_806E2EF8->pidList[i],
                lbl_806E2EF8->_24[i], lbl_806E2EF8->_A4[i], NULL, 0);
            if (lbl_806E2EF8->matchType == 0)
            {
                result = fn_80498C78(result);
            }
            else
            {
                result = fn_80498B24(result);
            }
            if (result != 0)
            {
                return;
            }
        }
        lbl_806E2EEC->_18 = OSGetTime();
        lbl_806E2EEC->_02 = 1;
    }
}

int fn_80498B24(int error)
{
    int type;
    int code;

    if (error == 0)
    {
        return 0;
    }
    DWC_Printf(2, "Match, GP error %d\n", error);
    switch (error)
    {
    case GP_MEMORY_ERROR:
        type = DWC_ERROR_FATAL;
        code = DWC_ECODE_TYPE_ALLOC;
        break;
    case GP_PARAMETER_ERROR:
        type = DWC_ERROR_FATAL;
        code = DWC_ECODE_TYPE_PARAM;
        break;
    case GP_NETWORK_ERROR:
        type = DWC_ERROR_TYPE_6;
        code = DWC_ECODE_TYPE_NETWORK;
        break;
    case GP_SERVER_ERROR:
        type = DWC_ERROR_TYPE_6;
        code = DWC_ECODE_TYPE_SERVER;
        break;
    }
    if (lbl_806E2EF8 != NULL && type != 0)
    {
        BOOL isServer;
        BOOL self;

        lbl_806E2EF8->closeState = 2;
        gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
        lbl_806E2EF8->closeState = 0;
        DWCi_SetError(type, code + DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_GP);
        fn_8048AFCC(1, "", NULL);
        {
            DWCMatchControlView* control = lbl_806E2EF8;

            isServer = control->matchType == 2;
            self = control->_21C == 0;
            control->matchedCallback(type, FALSE, self, isServer,
                fn_8048AEC4(control->_21C), control->matchedParam);
        }
        DWCi_CloseMatching();
    }
    return error;
}

int fn_80498C78(int error)
{
    int type;
    int code;

    if (error == 0)
    {
        return 0;
    }
    DWC_Printf(2, "Match, SB error %d\n", error);
    switch (error)
    {
    case sbe_socketerror:
        type = DWC_ERROR_TYPE_6;
        code = -50;
        break;
    case sbe_dnserror:
        type = DWC_ERROR_TYPE_6;
        code = -30;
        break;
    case sbe_connecterror:
        type = DWC_ERROR_TYPE_6;
        code = -20;
        break;
    case sbe_dataerror:
        type = DWC_ERROR_TYPE_6;
        code = -40;
        break;
    case sbe_allocerror:
        type = DWC_ERROR_FATAL;
        code = DWC_ECODE_TYPE_ALLOC;
        break;
    case sbe_paramerror:
        type = DWC_ERROR_FATAL;
        code = DWC_ECODE_TYPE_PARAM;
        break;
    }
    if (lbl_806E2EF8 != NULL && type != 0)
    {
        BOOL isServer;
        BOOL self;

        lbl_806E2EF8->closeState = 2;
        gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
        lbl_806E2EF8->closeState = 0;
        DWCi_SetError(type, code + DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_SB);
        fn_8048AFCC(1, "", NULL);
        {
            DWCMatchControlView* control = lbl_806E2EF8;

            isServer = control->matchType == 2;
            self = control->_21C == 0;
            control->matchedCallback(type, FALSE, self, isServer,
                fn_8048AEC4(control->_21C), control->matchedParam);
        }
        DWCi_CloseMatching();
    }
    return error;
}

int fn_80498DF0(int error)
{
    int type;
    int code;

    if (error == 0)
    {
        return 0;
    }
    DWC_Printf(2, "Match, QR2 error %d\n", error);
    switch (error)
    {
    case e_qrwsockerror:
        type = DWC_ERROR_TYPE_6;
        code = -50;
        break;
    case e_qrbinderror:
        type = DWC_ERROR_TYPE_6;
        code = -60;
        break;
    case e_qrdnserror:
        type = DWC_ERROR_TYPE_6;
        code = -30;
        break;
    case e_qrconnerror:
        type = DWC_ERROR_TYPE_6;
        code = -80;
        break;
    case e_qrnochallengeerror:
        type = DWC_ERROR_TYPE_6;
        code = -20;
        break;
    }

    switch (DWC_GetState())
    {
    case DWC_STATE_LOGIN:
        fn_8048C54C(type,
            code + DWC_ECODE_SEQ_LOGIN + DWC_ECODE_GS_QR2);
        break;
    case DWC_STATE_UPDATE_SERVERS:
        DWCi_StopFriendProcess(type,
            code + DWC_ECODE_SEQ_UPDATE_SVR + DWC_ECODE_GS_QR2);
        break;
    case DWC_STATE_MATCHING:
        if (lbl_806E2EF8 != NULL && type != 0)
        {
            BOOL isServer;
            BOOL self;

            lbl_806E2EF8->closeState = 2;
            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
            lbl_806E2EF8->closeState = 0;
            DWCi_SetError(type, code + DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_QR2);
            fn_8048AFCC(1, "", NULL);
            {
                DWCMatchControlView* control = lbl_806E2EF8;

                isServer = control->matchType == 2;
                self = control->_21C == 0;
                control->matchedCallback(type, FALSE, self, isServer,
                    fn_8048AEC4(control->_21C), control->matchedParam);
            }
            DWCi_CloseMatching();
        }
        break;
    default:
        DWCi_SetError(type, code + DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_QR2);
        break;
    }
    return error;
}

int fn_80498FB8(int error)
{
    int type;
    int code;

    if (error == 0)
    {
        return 0;
    }
    DWC_Printf(2, "Match, NN error %d\n", error);
    switch (error)
    {
    case ne_allocerror:
        type = DWC_ERROR_FATAL;
        code = DWC_ECODE_TYPE_ALLOC;
        break;
    case ne_socketerror:
        type = DWC_ERROR_TYPE_6;
        code = -50;
        break;
    case ne_dnserror:
        type = DWC_ERROR_TYPE_6;
        code = -30;
        break;
    }
    if (lbl_806E2EF8 != NULL && type != 0)
    {
        BOOL isServer;
        BOOL self;

        lbl_806E2EF8->closeState = 2;
        gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
        lbl_806E2EF8->closeState = 0;
        DWCi_SetError(type, code + DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_NN);
        fn_8048AFCC(1, "", NULL);
        {
            DWCMatchControlView* control = lbl_806E2EF8;

            isServer = control->matchType == 2;
            self = control->_21C == 0;
            control->matchedCallback(type, FALSE, self, isServer,
                fn_8048AEC4(control->_21C), control->matchedParam);
        }
        DWCi_CloseMatching();
    }
    return error;
}

int fn_804990FC(int result)
{
    int type;
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
        DWC_Printf(8, "NN Ping Timeout\n");
        type = DWC_ERROR_TYPE_6;
        code = -70;
        break;
    default:
        type = DWC_ERROR_TYPE_6;
        code = -9;
        break;
    }
    if (type != 0)
    {
        fn_80491EDC(type,
            code + DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_NN);
    }
    return result;
}

int fn_8049925C(int error)
{
    int type;
    int code;

    if (error == 0)
    {
        return 0;
    }
    DWC_Printf(2, "Match, GT2 error %d\n", error);
    switch (error)
    {
    case GT2OutOfMemory:
        type = DWC_ERROR_FATAL;
        code = DWC_ECODE_TYPE_ALLOC;
        break;
    case GT2Rejected:
    case GT2DuplicateAddress:
        type = 0;
        code = 0;
        error = 0;
        break;
    case GT2NetworkError:
        type = DWC_ERROR_TYPE_6;
        code = DWC_ECODE_TYPE_NETWORK;
        break;
    case GT2AddressError:
        type = DWC_ERROR_TYPE_6;
        code = -30;
        break;
    case GT2TimedOut:
        type = DWC_ERROR_TYPE_6;
        code = -70;
        break;
    case GT2NegotiationError:
        type = DWC_ERROR_TYPE_6;
        code = -80;
        break;
    }
    if (type != 0)
    {
        fn_80491EDC(type,
            code + DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_GT2);
    }
    return error;
}

static int DWCi_ChangeToClient(void)
{
    int result;
    int i;

    for (i = 1; i <= lbl_806E2EF8->_0D; i++)
    {
        result = fn_8049382C(10, lbl_806E2EF8->pidList[i],
            lbl_806E2EF8->_24[i], lbl_806E2EF8->_A4[i],
            (const u32*)lbl_806E2EF8->_358,
            *(const int*)lbl_806E2EF8->_358 + 1);
        if (result != 0)
        {
            return result;
        }
    }

    lbl_806E2EF8->_17 = 0;
    lbl_806E2EF8->_20 = 0;
    lbl_806E2EF8->closeState = 1;
    gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
    lbl_806E2EF8->closeState = 0;
    DWC_Printf(0x40, "Closed all connections. Begin NN to %u\n",
        lbl_806E2EF8->_214);

    return 0;
}

static int DWCi_CheckDWCServer(SBServer server)
{
    if (SBServerGetIntValueA(server, "numplayers", -1) == -1)
    {
        return 0;
    }
    if (SBServerGetIntValueA(server, "maxplayers", -1) == -1)
    {
        return 0;
    }
    if (SBServerGetIntValueA(server, "dwc_mtype", -1) == -1)
    {
        return 0;
    }
    if (SBServerGetIntValueA(server, "dwc_mresv", -1) == -1)
    {
        if (SBServerGetIntValueA(server, "dwc_mresv", 0) == 0)
        {
            return 0;
        }
    }
    if (SBServerGetIntValueA(server, "dwc_mver", -1) == -1)
    {
        return 0;
    }
    return SBServerGetIntValueA(server, "dwc_pid", 0);
}

void fn_804993C8(ServerBrowser sb, SBCallbackReason reason, SBServer server,
    void* instance)
{
    int profileID;
    int result;
    int i;
    NegotiateError nnError;

    DWC_Printf(0x40, "SBCallback : reason %d (state = %d)\n", reason,
        lbl_806E2EF8->state);
    lbl_806E2EF0++;

    switch (reason)
    {
    case sbc_serveradded:
        fn_80499A30(server);
        lbl_806E2EF8->_178 = OSGetTime()
            + (u64)(OS_BUS_CLOCK_SPEED / 4 / 1000) * 30000;
        break;

    case sbc_updatecomplete:
        lbl_806E2EF8->_178 = 0;
        for (i = 0; i < ServerBrowserCount(sb); i++)
        {
            server = ServerBrowserGetServer(sb, i);
            if (!DWCi_CheckDWCServer(server))
            {
                ServerBrowserRemoveServer(sb, server);
                DWC_Printf(0x400, "Deleted server [%d].\n", i);
                i--;
            }
        }

        switch (lbl_806E2EF8->state)
        {
        case 2:
            for (i = 0; i < ServerBrowserCount(sb); i++)
            {
                server = ServerBrowserGetServer(sb, i);
                if (DWCi_GetMatchCnt()->_1C
                    && (DWCi_GetMatchCnt()->_1C
                        == SBServerGetPublicInetAddress(server))
                    && DWCi_GetMatchCnt()->_1A
                    && (DWCi_GetMatchCnt()->_1A
                        == SBServerGetPublicQueryPort(server)))
                {
                    break;
                }
            }
            if (i < ServerBrowserCount(sb))
            {
                lbl_806E2EF8->state = 3;
                lbl_806E2EF8->_214 = 0;
                fn_80498C78(fn_80493128(lbl_806E2EF8->_214));
            }
            else
            {
                lbl_806E2EF8->_E8 = 2;
                lbl_806E2EF8->_F0 = OSGetTime();
            }
            break;

        case 3:
            fn_80499CA8(1);
            fn_80499E90();
            if (ServerBrowserCount(sb) != 0)
            {
                result = fn_80495B90(0, 0);

                if (lbl_806E2EF8->matchType == 0)
                {
                    result = fn_80498C78(result);
                }
                else
                {
                    result = fn_80498B24(result);
                }
                if (result != 0)
                {
                    break;
                }
                lbl_806E2EF8->state = 4;
                lbl_806E2EF8->_E8 = 0;
            }
            else
            {
                lbl_806E2EF8->_E8 = 2;
                lbl_806E2EF8->_F0 = OSGetTime();
            }
            break;

        case 5:
        {
            DWC_Printf(4, "searchIP: %x, searchPort: %d\n",
                lbl_806E2EF8->_1BC, lbl_806E2EF8->_1B8);
            while (ServerBrowserCount(sb) != 0)
            {
                server = ServerBrowserGetServer(sb, 0);
                if ((SBServerGetPublicInetAddress(server)
                        == DWCi_GetMatchCnt()->_1BC)
                    && (SBServerGetPublicQueryPort(server)
                        == DWCi_GetMatchCnt()->_1B8))
                {
                    break;
                }
                else
                {
                    ServerBrowserRemoveServer(sb, server);
                }
            }
            if (ServerBrowserCount(sb) != 0)
            {
                profileID = SBServerGetIntValueA(
                    ServerBrowserGetServer(sb, 0), "dwc_pid", 0);
                if (lbl_806E2EF8->matchType == 1
                    && profileID == lbl_806E2EF8->pidList[0])
                {
                    if (fn_80499CA8(0) != 0)
                    {
                        if (lbl_806E2EF8->_0D != 0)
                        {
                            result = DWCi_ChangeToClient();
                            if (lbl_806E2EF8->matchType == 0)
                            {
                                result = fn_80498C78(result);
                            }
                            else
                            {
                                result = fn_80498B24(result);
                            }
                            if (result != 0)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        result = fn_8049382C(5, lbl_806E2EF8->pidList[0],
                            lbl_806E2EF8->_24[0], lbl_806E2EF8->_A4[0],
                            NULL, 0);
                        lbl_806E2EF8->_214 = 0;
                        if (lbl_806E2EF8->matchType == 0)
                        {
                            result = fn_80498C78(result);
                        }
                        else
                        {
                            result = fn_80498B24(result);
                        }
                        if (result != 0)
                        {
                            break;
                        }
                        lbl_806E2EF8->state = 4;
                        result
                            = fn_80495D7C(0, 0, lbl_806E2EF8->pidList[0]);
                        if (lbl_806E2EF8->matchType == 0)
                        {
                            fn_80498C78(result);
                        }
                        else
                        {
                            fn_80498B24(result);
                        }
                        break;
                    }
                }
                lbl_806E2EF8->state = 6;
                nnError = fn_80493434(0, 0, ServerBrowserGetServer(sb, 0));
                fn_80498FB8(nnError);
            }
            else
            {
                lbl_806E2EF8->_E8 = 2;
                lbl_806E2EF8->_F0 = OSGetTime();
            }
            break;
        }

        default:
            lbl_806E2EF8->_178 = OSGetTime()
                + (u64)(OS_BUS_CLOCK_SPEED / 4 / 1000) * 30000;
            break;
        }
        break;

    case sbc_queryerror:
        break;

    default:
        break;
    }

    lbl_806E2EF0--;
}

void fn_80499A30(SBServer server)
{
    int i;

    DWC_Printf(0x400, "SBServerGetPrivateAddress     = %s\n",
        SBServerGetPrivateAddress(server));
    DWC_Printf(0x400, "SBServerGetPrivateInetAddress = %x\n",
        SBServerGetPrivateInetAddress(server));
    DWC_Printf(0x400, "SBServerGetPrivateQueryPort   = %d\n",
        SBServerGetPrivateQueryPort(server));
    DWC_Printf(0x400, "SBServerGetPublicAddress      = %s\n",
        SBServerGetPublicAddress(server));
    DWC_Printf(0x400, "SBServerGetPublicInetAddres   = %x\n",
        SBServerGetPublicInetAddress(server));
    DWC_Printf(0x400, "SBServerGetPublicQueryPort    = %d\n",
        SBServerGetPublicQueryPort(server));
    DWC_Printf(0x400, "SBServerHasPrivateAddress     = %d\n",
        SBServerHasPrivateAddress(server));
    DWC_Printf(0x400, "numplayers  = %d\n",
        SBServerGetIntValueA(server, "numplayers", -1));
    DWC_Printf(0x400, "maxplayers  = %d\n",
        SBServerGetIntValueA(server, "maxplayers", -1));
    DWC_Printf(0x400, "%s     = %u\n", "dwc_pid",
        SBServerGetIntValueA(server, "dwc_pid", 0));
    DWC_Printf(0x400, "%s   = %u\n", "dwc_mresv",
        SBServerGetIntValueA(server, "dwc_mresv", -1));
    DWC_Printf(0x400, "%s   = %d\n", "dwc_mtype",
        SBServerGetIntValueA(server, "dwc_mtype", -1));
    DWC_Printf(0x400, "%s   = %d\n", "dwc_mver",
        SBServerGetIntValueA(server, "dwc_mver", -1));

    for (i = 0; i < 154; i++)
    {
        if (lbl_806C9A20[i].keyID != 0)
        {
            if (lbl_806C9A20[i]._01 != 0)
            {
                DWC_Printf(0x400, "%s  = %s\n", lbl_806C9A20[i].keyString,
                    SBServerGetStringValueA(server,
                        lbl_806C9A20[i].keyString, "NONE"));
            }
            else
            {
                DWC_Printf(0x400, "%s  = %d\n", lbl_806C9A20[i].keyString,
                    SBServerGetIntValueA(server, lbl_806C9A20[i].keyString,
                        -1));
            }
        }
    }
}

int fn_80499CA8(int sort)
{
    BOOL deleted = FALSE;
    int i = 0;

    for (i = 0; i < ServerBrowserCount(lbl_806E2EF8->sb); i++)
    {
        SBServer server
            = ServerBrowserGetServer(lbl_806E2EF8->sb, i);

        if (lbl_806E2EF8->matchType == 0)
        {
            int pid = SBServerGetIntValueA(server, "dwc_pid", 0);
            BOOL removed = FALSE;
            int j;

            for (j = 1; j <= lbl_806E2EF8->_0D; j++)
            {
                if (lbl_806E2EF8->pidList[j] == pid)
                {
                    ServerBrowserRemoveServer(lbl_806E2EF8->sb, server);
                    removed = TRUE;
                    i--;
                    break;
                }
            }
            if (removed)
            {
                continue;
            }
        }

        if (lbl_806E2EF8->_488 != 0)
        {
            int point = ((int (*)(int, void*))lbl_806E2EF8->_488)(i,
                lbl_806E2EF8->_48C);

            if (point > 0)
            {
                if (point > 0x800000 - 1)
                {
                    point = 0x800000 - 1;
                }
                SBServerAddIntKeyValue(server, "dwc_eval",
                    (point << 8) | DWCi_GetMathRand32(0x100));
            }
            else
            {
                ServerBrowserRemoveServer(lbl_806E2EF8->sb, server);
                DWC_Printf(0x400,
                    "Deleted server [%d] (eval point is %d).\n", i, point);
                deleted = TRUE;
                i--;
            }
        }
        else
        {
            SBServerAddIntKeyValue(server, "dwc_eval", DWCi_GetMathRand32(0x80));
        }
    }

    if (sort != 0 && ServerBrowserCount(lbl_806E2EF8->sb) != 0)
    {
        ServerBrowserSortA(lbl_806E2EF8->sb, SBFalse, "dwc_eval",
            sbcm_int);
    }

    if (deleted && ServerBrowserCount(lbl_806E2EF8->sb) == 0)
    {
        return 0;
    }
    return 1;
}

void fn_80499E90(void)
{
    u32 rand;
    int maxEval = 0;
    int total = 0;
    int i;
    int cumulative[6];

    if (ServerBrowserCount(lbl_806E2EF8->sb) <= 1)
    {
        return;
    }

    for (i = 0; i < ServerBrowserCount(lbl_806E2EF8->sb); i++)
    {
        int value = SBServerGetIntValueA(
            ServerBrowserGetServer(lbl_806E2EF8->sb, i), "dwc_eval", -1);

        if (value > maxEval)
        {
            maxEval = value;
        }
        total += lbl_804F31F8[i];
    }

    rand = DWCi_GetMathRand32(0x64);
    for (i = 0; i < ServerBrowserCount(lbl_806E2EF8->sb); i++)
    {
        if (i == ServerBrowserCount(lbl_806E2EF8->sb) - 1)
        {
            cumulative[i] = 100;
            break;
        }
        cumulative[i] = lbl_804F31F8[i] * 100 / total
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
    SBServerAddIntKeyValue(ServerBrowserGetServer(lbl_806E2EF8->sb, i),
        "dwc_eval", maxEval);
    ServerBrowserSortA(lbl_806E2EF8->sb, SBFalse, "dwc_eval", sbcm_int);
}

void fn_8049A048(int keyid, qr2_buffer_t outbuf, void* userdata)
{
    switch (keyid)
    {
    case NUMPLAYERS_KEY:
        qr2_buffer_add_int(outbuf, lbl_806E2EF8->_14);
        DWC_Printf(0x200, "QR2, Received ServerKeyReq : keyID %d - %d\n",
            keyid, lbl_806E2EF8->_14);
        break;
    case MAXPLAYERS_KEY:
        qr2_buffer_add_int(outbuf, lbl_806E2EF8->_16);
        DWC_Printf(0x200, "QR2, Received ServerKeyReq : keyID %d - %d\n",
            keyid, lbl_806E2EF8->_16);
        break;
    case 0x32:
        qr2_buffer_add_int(outbuf, lbl_806E2EF8->_210);
        DWC_Printf(0x200, "QR2, Received ServerKeyReq : keyID %d - %d\n",
            keyid, lbl_806E2EF8->_210);
        break;
    case 0x33:
        qr2_buffer_add_int(outbuf, lbl_806E2EF8->matchType);
        DWC_Printf(0x200, "QR2, Received ServerKeyReq : keyID %d - %d\n",
            keyid, lbl_806E2EF8->matchType);
        break;
    case 0x34:
        qr2_buffer_add_int(outbuf, lbl_806E2EF8->_20);
        DWC_Printf(0x200, "QR2, Received ServerKeyReq : keyID %d - %d\n",
            keyid, lbl_806E2EF8->_20);
        break;
    case 0x35:
        qr2_buffer_add_int(outbuf, 3);
        DWC_Printf(0x200, "QR2, Received ServerKeyReq : keyID %d - %d\n",
            keyid, 3);
        break;
    case 0x36:
        qr2_buffer_add_int(outbuf, 1);
        DWC_Printf(0x200, "QR2, Received ServerKeyReq : keyID %d - %d\n",
            keyid, 1);
        break;
    default:
        if (keyid - 0x64 >= 0 && keyid - 0x64 < 154)
        {
            DWCMatchKeyDataView* key = &lbl_806C9A20[keyid - 0x64];

            if (key->keyID == 0)
            {
                break;
            }
            if (key->_01 != 0)
            {
                qr2_buffer_addA(outbuf, (char*)key->value);
            }
            else
            {
                qr2_buffer_add_int(outbuf, *(const int*)key->value);
            }
        }
        break;
    }
}

void fn_8049A27C(int keyid, int index, qr2_buffer_t outbuf, void* userdata)
{
}

void fn_8049A280(int keyid, int index, qr2_buffer_t outbuf, void* userdata)
{
}

void fn_8049A284(qr2_key_type keytype, qr2_keybuffer_t keybuffer,
    void* userdata)
{
    switch (keytype)
    {
    case key_server:
    {
        int i;

        qr2_keybuffer_add(keybuffer, NUMPLAYERS_KEY);
        qr2_keybuffer_add(keybuffer, MAXPLAYERS_KEY);
        qr2_keybuffer_add(keybuffer, 0x32);
        qr2_keybuffer_add(keybuffer, 0x33);
        qr2_keybuffer_add(keybuffer, 0x34);
        qr2_keybuffer_add(keybuffer, 0x35);
        qr2_keybuffer_add(keybuffer, 0x36);
        for (i = 0; i < 154; i++)
        {
            if (lbl_806C9A20[i].keyID != 0)
            {
                qr2_keybuffer_add(keybuffer, lbl_806C9A20[i].keyID);
            }
        }
        break;
    }
    default:
        break;
    }
    DWC_Printf(0x200, "QR2, Received KeyListReq : keytype %d\n", keytype);
}

int fn_8049A374(qr2_key_type keytype, void* userdata)
{
    return 0;
}

void fn_8049A37C(qr2_error_t error, gsi_char* errmsg, void* userdata)
{
    DWC_Printf(2, "QR2 Failed query addition to master server %d\n", error);
    DWC_Printf(2, "%s\n", errmsg);
    fn_80498DF0(error);
}

void fn_8049A3E4(unsigned int ip, unsigned short port, void* userdata)
{
    DWC_Printf(0x40, "Got my query IP %08x & port %d.\n", ip, port);
    lbl_806E2EF8->_1C = ip;
    lbl_806E2EF8->_1A = port;
}

void fn_8049A444(int cookie, void* userdata)
{
    DWC_Printf(0x40, "Got NN request, cookie = %x.\n", cookie);
    if (lbl_806E2EF8->state == 1)
    {
        lbl_806E2EF8->state = 6;
    }
    else if (lbl_806E2EF8->state != 6 && lbl_806E2EF8->state != 11)
    {
        DWC_Printf(0x40, "But already canceled reservation.\n");
        return;
    }

    if (cookie == lbl_806E2EF8->_184)
    {
        lbl_806E2EF8->_180++;
    }
    else
    {
        lbl_806E2EF8->_180 = 0;
        lbl_806E2EF8->_184 = cookie;
    }
    lbl_806E2EF8->_188 = 0;
    if (fn_80498FB8(fn_80493434(1, cookie, NULL)) == 0)
    {
        lbl_806E2EF8->_3E0 = 0xFF;
    }
}

void fn_8049A530(gsi_char* data, int len, void* userdata)
{
    int offset = 0;

    if (DWC_GetState() != DWC_STATE_MATCHING)
    {
        if (DWC_GetState() != DWC_STATE_CONNECTED
            || (lbl_806E2EF8->matchType != 2
                && lbl_806E2EF8->matchType != 3))
        {
            DWC_Printf(4, "Ignore delayed SB matching command.\n");
            return;
        }
    }

    while (offset + 0x14 <= len)
    {
        DWCMatchSBMessageView msg;
        u32 payload[0x21];

        memcpy(&msg, data, sizeof(msg));
        msg.version = fn_ByteSwap32(msg.version);
        msg.pid = fn_ByteSwap32(msg.pid);
        msg.port = (msg.port >> 8) | (msg.port << 8);
        if (strncmp((char*)&msg, "SBCM", 4) != 0)
        {
            DWC_Printf(8, "Got undefined SBcommand.\n");
            return;
        }
        if (msg.version != 3)
        {
            DWC_Printf(8, "Got different version SBcommand.\n");
            return;
        }
        memcpy(payload, data + 0x14, msg.length);
        DWC_Printf(0x40, "<SB> RECV-0x%02x <- [%08x:%d] [pid=%u]\n",
            msg.command, msg.ip, msg.port, msg.pid);
        if (fn_80493C58(msg.command, msg.pid, msg.ip, msg.port, payload,
                msg.length >> 2)
            == 0)
        {
            return;
        }
        offset += msg.length + 0x14;
    }
}

void fn_8049A6E8(NegotiateState state, void* userdata)
{
    DWC_Printf(0x40, "NN, Got state update: %d\n", state);
}

void fn_8049A700(NegotiateResult result, SOCKET gamesocket,
    struct sockaddr_in* remoteaddr, void* userdata)
{
    DWCMatchNNInfoView* info = (DWCMatchNNInfoView*)userdata;

    DWC_Printf(0x40, "NN, Complete NAT Negotiation. result : %d\n", result);
    if (info != NULL)
    {
        DWC_Printf(0x40, "NN cookie = %x.\n", info->cookie);
    }
    if ((lbl_806E2EF8->state != 6 && lbl_806E2EF8->state != 11)
        || info == NULL)
    {
        DWC_Printf(4, "Ignore delayed NN after cancel.\n");
        return;
    }

    if (result == nr_success)
    {
        int count;

        if (remoteaddr != NULL)
        {
            DWC_Printf(0x40, "NN, remote address : %s\n",
                gt2AddressToString(remoteaddr->sin_addr.addr,
                    SONtoHs(remoteaddr->sin_port), NULL));
        }
        info->cookie = 0;
        lbl_806E2EF8->_14++;
        count = lbl_806E2EF8->_14;
        if (info->isClient != 0)
        {
            GT2Result res;
            char buf[0xC];

            lbl_806E2EF8->ipList[count] = remoteaddr->sin_addr.addr;
            lbl_806E2EF8->portList[count] = SONtoHs(remoteaddr->sin_port);
            DWC_Printf(0x40, "NN child finished Nat Negotiation.\n");
            lbl_806E2EF8->_180 = 0;
            lbl_806E2EF8->_184 = 0;
            lbl_806E2EF8->_188 = 0;
            if (lbl_806E2EF8->state == 11)
            {
                lbl_806E2EF8->state = 12;
            }
            else
            {
                lbl_806E2EF8->state = 7;
            }
            lbl_806E2EF8->_0C = 0;
            DWC_Printf(0x80, "gt2Connect() to pidList[%d] (%s)\n", count,
                gt2AddressToString(lbl_806E2EF8->ipList[count],
                    lbl_806E2EF8->portList[count], NULL));
            snprintf(buf, sizeof(buf), "%u", lbl_806E2EF8->_210);
            res = gt2Connect(*lbl_806E2EF8->pGT2Socket, NULL,
                gt2AddressToString(lbl_806E2EF8->ipList[count],
                    lbl_806E2EF8->portList[count], NULL),
                (const GT2Byte*)buf, -1, 5000, lbl_806E2EF8->_08,
                GT2False);
            if (res == GT2OutOfMemory)
            {
                fn_8049925C(res);
                return;
            }
            if (res != GT2Success)
            {
                if (fn_80496188(lbl_806E2EF8->pidList[count]) == 0)
                {
                    return;
                }
            }
        }
        else
        {
            DWC_Printf(0x40, "NN parent finished Nat Negotiation.\n");
            if (remoteaddr != NULL)
            {
                lbl_806E2EF8->ipList[count - 1] = remoteaddr->sin_addr.addr;
                lbl_806E2EF8->portList[count - 1]
                    = SONtoHs(remoteaddr->sin_port);
            }
            lbl_806E2EF8->_190 = OSGetTime();
            lbl_806E2EF8->state = 7;
        }
    }
    else
    {
        int level;

        if (info->cookie == 0)
        {
            DWC_Printf(4, "Ignore delayed NN error after cancel.\n");
            return;
        }
        level = fn_804990FC(result);
        if (level != 2 && level != 1)
        {
            return;
        }

        if (info->isClient == 0)
        {
            DWC_Printf(0x40, "Failed %d/%d NN send.\n", info->retryCount,
                1);
            if (level == 1 || (level == 2 && info->retryCount >= 1))
            {
                DWC_Printf(0x40, "Abort NN.\n");
                info->cookie = 0;
                if (lbl_806E2EF8->matchType != 3)
                {
                    lbl_806E2EF8->_181++;
                    DWC_Printf(0x40, "NN failure %d/%d.\n",
                        lbl_806E2EF8->_181, 5);
                }
                if (lbl_806E2EF8->matchType == 3
                    || lbl_806E2EF8->_181 >= 5)
                {
                    if (lbl_806E2EF8 != NULL)
                    {
                        BOOL isServer;
                        BOOL self;

                        lbl_806E2EF8->closeState = 2;
                        gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
                        lbl_806E2EF8->closeState = 0;
                        DWCi_SetError(DWC_ERROR_TYPE_6, DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_NN - 420);
                        fn_8048AFCC(1, "", NULL);
                        {
                            DWCMatchControlView* control = lbl_806E2EF8;

                            isServer = control->matchType == 2;
                            self = control->_21C == 0;
                            control->matchedCallback(DWC_ERROR_TYPE_6, FALSE, self, isServer,
                                fn_8048AEC4(control->_21C), control->matchedParam);
                        }
                        DWCi_CloseMatching();
                    }
                    return;
                }
                if (fn_80496520(
                        lbl_806E2EF8->pidList[lbl_806E2EF8->_0D])
                    == 0)
                {
                    return;
                }
            }
            else
            {
                int res = 0;
                int retry;

                info->retryCount++;
                if (info->isClient == 0
                    && fn_80498C78(
                           ServerBrowserSendNatNegotiateCookieToServerA(
                               lbl_806E2EF8->sb,
                               gt2AddressToString(info->ip, 0, NULL),
                               info->port, info->cookie))
                        != 0)
                {
                    res = 2;
                }
                else
                {
                    if (info->isClient == 0)
                    {
                        DWC_Printf(0x40, "Send NN cookie = %x.\n",
                            info->cookie);
                    }
                    for (retry = 0; retry < 5; retry++)
                    {
                        res = NNBeginNegotiationWithSocket(
                            gt2GetSocketSOCKET(*lbl_806E2EF8->pGT2Socket),
                            info->cookie, info->isClient, fn_8049A6E8,
                            fn_8049A700, info);
                        if (res == 0 || res != 3)
                        {
                            break;
                        }
                        DWC_Printf(4,
                            " dns error occurs when NatNegotiation "
                            "begin... retry\n");
                    }
                }
                if (fn_80498FB8(res) == 0)
                {
                    return;
                }
            }
        }
        else
        {
            DWC_Printf(0x40, "Failed %d/%d NN recv.\n", lbl_806E2EF8->_180,
                1);
            lbl_806E2EF8->_188 = OSGetTime();
            if (level == 1 || (level == 2 && lbl_806E2EF8->_180 >= 1))
            {
                DWC_Printf(0x40, "Abort NN.\n");
                info->cookie = 0;
                if (lbl_806E2EF8->matchType != 3
                    && lbl_806E2EF8->matchType != 2)
                {
                    if (lbl_806E2EF8->matchType != 3)
                    {
                        lbl_806E2EF8->_181++;
                        DWC_Printf(0x40, "NN failure %d/%d.\n",
                            lbl_806E2EF8->_181, 5);
                    }
                    if (lbl_806E2EF8->matchType == 3
                        || lbl_806E2EF8->_181 >= 5)
                    {
                        if (lbl_806E2EF8 != NULL)
                        {
                            BOOL isServer;
                            BOOL self;

                            lbl_806E2EF8->closeState = 2;
                            gt2CloseAllConnectionsHard(*lbl_806E2EF8->pGT2Socket);
                            lbl_806E2EF8->closeState = 0;
                            DWCi_SetError(DWC_ERROR_TYPE_6, DWC_ECODE_SEQ_MATCHING + DWC_ECODE_GS_NN - 420);
                            fn_8048AFCC(1, "", NULL);
                            {
                                DWCMatchControlView* control = lbl_806E2EF8;

                                isServer = control->matchType == 2;
                                self = control->_21C == 0;
                                control->matchedCallback(DWC_ERROR_TYPE_6, FALSE, self, isServer,
                                    fn_8048AEC4(control->_21C), control->matchedParam);
                            }
                            DWCi_CloseMatching();
                        }
                        return;
                    }
                }
                lbl_806E2EF8->_180 = 0;
                lbl_806E2EF8->_184 = 0;
                lbl_806E2EF8->_188 = 0;
                if (fn_80496188(
                        lbl_806E2EF8->pidList[lbl_806E2EF8->_0D + 1])
                    == 0)
                {
                    return;
                }
            }
        }
    }
}

static DWCMatchControlView* DWCi_GetMatchCnt(void)
{
    return lbl_806E2EF8;
}
