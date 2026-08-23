#ifndef REVOLUTION_OS_STATE_FLAGS_H
#define REVOLUTION_OS_STATE_FLAGS_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSStateFlags {
    u32 checksum;
    u8 BYTE_0x4;
    u8 BYTE_0x5;
    u8 discState;
    u8 BYTE_0x7;
    u32 WORD_0x8;
    u32 WORD_0xC;
    u32 WORD_0x10;
    u32 WORD_0x14;
    u32 WORD_0x18;
    u32 WORD_0x1C;
} OSStateFlags;

BOOL __OSWriteStateFlags(const OSStateFlags* state);
BOOL __OSReadStateFlags(OSStateFlags* state);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_STATE_FLAGS_H
