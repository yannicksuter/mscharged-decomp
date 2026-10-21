#ifndef REVOLUTION_OS_PLAY_TIME_H
#define REVOLUTION_OS_PLAY_TIME_H

#if defined(VERSION_R4QK01)
#include <revolution/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void __OSInitPlayTime(void);

#if defined(VERSION_R4QK01)
struct ESPTicketView;

typedef enum OSPlayTimeType {
    OS_PLAYTIME_PERMANENT = 0,
    OS_PLAYTIME_TIME_LIMIT = 1,
    OS_PLAYTIME_LAUNCH_LIMIT = 4,
    OS_PLAYTIME_UNKNOWN = 9
} OSPlayTimeType;

BOOL OSPlayTimeIsLimited(void);
s32 __OSGetPlayTime(struct ESPTicketView*, OSPlayTimeType*, u32*);
BOOL __OSWriteExpiredFlag(void);
BOOL __OSWriteExpiredFlagIfSet(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_PLAY_TIME_H
