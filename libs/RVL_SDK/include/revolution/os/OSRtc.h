#ifndef REVOLUTION_OS_RTC_H
#define REVOLUTION_OS_RTC_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSSram {
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} OSSram;

typedef struct OSSramEx {
    u8 flashID[2][12];
    u32 wirelessKeyboardID;
    u16 wirelessPadID[4];
    u8 dvdErrorCode;
    u8 padding;
    u8 flashIDCheckSum[2];
    u16 gbs;
    u8 padding2[2];
} OSSramEx;

void __OSInitSram(void);
BOOL __OSSyncSram(void);
BOOL __OSReadROM(void* buffer, s32 length, s32 offset);
u16 OSGetWirelessID(s32 channel);
void OSSetWirelessID(s32 channel, u16 id);
BOOL __OSGetRTCFlags(u32* flags);
BOOL __OSClearRTCFlags(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_RTC_H
