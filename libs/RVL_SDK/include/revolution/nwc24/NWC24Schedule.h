#ifndef RVL_SDK_NWC24_SCHEDULE_H
#define RVL_SDK_NWC24_SCHEDULE_H
#include <revolution/types.h>

#include <revolution/nwc24/NWC24Types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 NWC24SuspendScheduler(void);
s32 NWC24ResumeScheduler(void);
NWC24Err NWC24ExecDownloadTask(u32 flags, u16 taskId, u32 subTaskMask);

#ifdef __cplusplus
}
#endif
#endif
