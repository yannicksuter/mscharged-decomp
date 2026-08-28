#ifndef REVOLUTION_AI_H
#define REVOLUTION_AI_H

#include <revolution/types.h>

volatile u32 AI_HW_REGS[4] AT_ADDRESS(0xCD006C00);

typedef enum AIHwReg {
    AI_AICR,
    AI_AIVR,
    AI_AISCNT,
    AI_AIIT,
} AIHwReg;

#define AI_AICR_SAMPLERATE (1 << 6)
#define AI_AICR_SCRESET (1 << 5)
#define AI_AICR_AIINTVLD (1 << 4)
#define AI_AICR_AIINT (1 << 3)
#define AI_AICR_AIINTMSK (1 << 2)
#define AI_AICR_AFR (1 << 1)
#define AI_AICR_PSTAT (1 << 0)

typedef struct OSContext OSContext;

typedef void (*AIDMACallback)(void);

typedef enum {
    AI_DSP_32KHZ,
    AI_DSP_48KHZ,
} AIDSPSampleRate;

#ifdef __cplusplus
extern "C"
{
#endif

AIDMACallback AIRegisterDMACallback(AIDMACallback callback);
void AIInitDMA(void* buffer, u32 length);
void AIStartDMA(void);
u32 AIGetDMABytesLeft(void);
u32 AIGetDMAStartAddr(void);
u32 AIGetDMALength(void);
BOOL AICheckInit(void);
void AISetDSPSampleRate(u32 rate);
u32 AIGetDSPSampleRate(void);
void AIInit(void* stack);
void __AIDHandler(s16 intr, OSContext* ctx);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_AI_H
