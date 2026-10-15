#pragma once

#include <dwc/dwc_match.h>
#include <gamespy/gt2/gt2.h>
#include <gamespy/serverbrowsing/sb_serverbrowsing.h>

typedef struct DWCNodeInfo
{
    int profileID;
    u32 publicIP;
    u8 _008[4];
    u16 publicPort;
    u8 _00E[8];
    u8 aid;
    u8 _017[9];
    OSTime connectionDeadline;
    u8 reservationData[4];
    u8 _02C[4];
} DWCNodeInfo;

typedef struct DWCNodeList
{
    int count;
    DWCNodeInfo nodes[32];
} DWCNodeList;

typedef struct DWCstNNInfo
{
    u8 isQR2;
    u8 retryCount;
    u16 port;
    u32 ip;
    int cookie;
} DWCNNInfo;

typedef struct DWCstMatchControl
{
    GPConnection* pGpObj;
    GT2Socket* pGt2Socket;
    GT2ConnectionCallbacks* gt2Callbacks;
    u8 gt2ConnectCount;
    u8 matchingComplete;
    u8 _00E[2];
    qr2_t qr2Obj;
    volatile u8 matchType;
    volatile u8 qr2MatchType;
    volatile u8 hostState;
    volatile u8 qr2NumEntry;
    volatile u8 qr2IsReserved;
    u8 _019;
    u8 _01A[2];
    int _01C;
    OSTime _020;
    u32 _028;
    int _02C;
    u8 _030;
    u8 _031[0x1B];
    u32 _04C;
    u8 qr2ShutdownFlag;
    u8 _051[7];
    DWCNodeList nodeList;
    DWCNodeInfo serverNode;
    u32 _690;
    u8 _694[0x2C];
    u32 _6C0;
    u32 _6C4;
    u8 _6C8[0x30];
    u16 qr2Port;
    u8 _6FA[2];
    u32 qr2IP;
    volatile int qr2Reservation;
    ServerBrowser sbObj;
    int sbUpdateFlag;
    u8 _70C[0xC];
    OSTime sbUpdateRequestTick;
    u8 nnRecvCount;
    u8 _721;
    u16 nnCookieRand;
    int nnLastCookie;
    OSTime nnFailedTime;
    OSTime nnFinishTime;
    DWCNNInfo nnInfo;
    DWCMatchState state;
    u8 clLinkProgress;
    u8 friendCount;
    u8 _74A[6];
    u8 resvWaitCount;
    u8 closeState;
    u8 cancelState;
    u8 scResvRetryCount;
    u8 synResendCount;
    u8 cancelSynResendCount;
    u8 clWaitTimeoutCount;
    u8 _757;
    u16 baseLatency;
    u16 cancelBaseLatency;
    u32 _75C;
    int cmdResendFlag;
    u8 _764[4];
    OSTime cmdResendTick;
    u32 cmdTimeoutTime;
    u8 _774[4];
    OSTime cmdTimeoutStartTick;
    u32 synAckBit;
    u8 _784[4];
    OSTime lastSynSent;
    u8 _790[8];
    OSTime closedTime;
    OSTime clWaitTime;
    volatile int profileID;
    int reqProfileID;
    int cbEventPid;
    const char* gameName;
    const char* secretKey;
    const DWCFriendData* friendList;
    int friendListLen;
    u8 friendIdxList[DWC_MAX_MATCH_IDX_LIST];
    int friendIdxListLen;
    DWCMatchCommandControl cmdCnt;
    DWCMatchedSCCallback matchedCallback;
    void* matchedParam;
    DWCNewClientCallback newClientCallback;
    void* newClientParam;
    DWCEvalPlayerCallback evalCallback;
    void* evalParam;
    DWCStopSCCallback stopSCCallback;
    u32 serverDownBaseBitmap;
    u32 groupID;
    u8 _8C4;
    u8 _8C5[3];
    u32 _8C8;
    u8 _8CC[0xC];
    u8 searchServerNext;
    u8 _8D9[3];
    u32 _8DC;
    int topology;
    u8 _8E4;
    u8 closeAllRequested;
    u8 _8E6;
    u8 _8E7;
    DWCReservationCallback reservationCallback;
    u8 reservationData[4];
    void* reservationParam;
    int _8F4;
    u32 _8F8;
    u32 _8FC;
    u64 _900;
    int _908;
    u8 _90C[4];
} DWCMatchControl;

BOOL DWCi_IsBuddyMatchLocked(const GPBuddyStatus* status);

int DWCi_GetNodeCount(void);
DWCNodeInfo* DWCi_GetNodeByIndex(int index);
DWCNodeInfo* DWCi_GetNodeByProfileID(int profileID);
DWCNodeInfo* DWCi_GetNodeByAID(u8 aid);

void DWCi_ClearNodeList(void);
void DWCi_SetMatchStatus(DWCMatchState state);
void DWCi_CloseMatching(void);

void DWCi_FreeNegotiateList(void);
DWCNodeInfo* DWCi_GetFirstNode(void);
BOOL DWCi_RemoveNodeByAID(u8 aid);
BOOL DWCi_IsBackupMine(void);
BOOL fn_8046F180(void);
BOOL fn_8046F8A0(void);
