#ifndef RVL_SDK_NWC24_INTERNAL_SCHEDULE_H
#define RVL_SDK_NWC24_INTERNAL_SCHEDULE_H
#include <revolution/types.h>

#include <revolution/nwc24/NWC24Types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum NWC24ScdTaskStage {
    NWC24_SCD_TASK_WAITING = 0,
    NWC24_SCD_TASK_ACCOUNT = 1,
    NWC24_SCD_TASK_CHECK = 2,
    NWC24_SCD_TASK_RECEIVE = 3,
    NWC24_SCD_TASK_SEND = 5,
    NWC24_SCD_TASK_SAVE = 6,
    NWC24_SCD_TASK_DOWNLOAD = 7,
    NWC24_SCD_TASK_PROCESS = 8,
} NWC24ScdTaskStage;

typedef struct NWC24ScdStat {
    NWC24Err result;  // 0x00

    u32 permission;               // 0x04
    s32 lastCriticalError;        // 0x08
    u32 newMsgFlag;               // 0x0C
    NWC24ScdTaskStage taskStage;  // 0x10

    u32 numErrors;  // 0x14

    u32 numMsgSent;      // 0x18
    u32 numMsgReceived;  // 0x1C
    u32 numMsgSaved;     // 0x20
    u32 numMsgRejected;  // 0x24
    u32 numMsgFiltered;  // 0x28

    u32 countMailChk;  // 0x2C
    u32 countMailRcv;  // 0x30
    u32 countMailSav;  // 0x34
    u32 countMailSnd;  // 0x38

    u32 countDL;           // 0x3C
    u32 countEstablished;  // 0x40

    u32 mailTaskTrace;  // 0x44
    u32 dlTaskTrace;    // 0x48

    u32 countMailPrc;     // 0x4C
    u32 countForceRecv;   // 0x50
    u32 countMailIdle;    // 0x54
    u32 countScriptExec;  // 0x58

    u32 reserved[9];  // 0x5C

    s32 errorLog[32];  // 0x80
} NWC24ScdStat;

NWC24Err NWC24iGetSchedulerStat(NWC24ScdStat* scdStat, u32 scdStatSize);
NWC24Err NWC24iDownloadNowEx(BOOL* saveMail, u32 flags, u16 taskId, u32 subTaskMask);
NWC24Err NWC24iSaveMailNow(void);

#define NWC24i_SCHEDULER_DEVICE "/dev/net/kd/request"

NWC24Err NWC24iRequestGenerateUserId(u64* pId, u32* arg1);
NWC24Err NWC24iTrySuspendForOpenLib(void);
NWC24Err NWC24iResumeForCloseLib(void);

#ifdef __cplusplus
}
#endif
#endif
