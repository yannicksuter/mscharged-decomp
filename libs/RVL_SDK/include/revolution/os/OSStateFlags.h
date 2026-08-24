#ifndef REVOLUTION_OS_STATE_FLAGS_H
#define REVOLUTION_OS_STATE_FLAGS_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSStateFlags {
    u32 checkSum;
    u8 lastAppType;
    u8 shutdownType;
    u8 discState;
    u8 menuMode;
    u8 padding[24];
} OSStateFlags;

BOOL __OSWriteStateFlags(const OSStateFlags* state);
BOOL __OSReadStateFlags(OSStateFlags* state);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_STATE_FLAGS_H
