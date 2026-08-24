#ifndef REVOLUTION_AI_H
#define REVOLUTION_AI_H

#include <revolution/types.h>

volatile u32 AI_HW_REGS[] AT_ADDRESS(0xCD006C00);

typedef enum AIHwReg {
    AI_AICR,
    AI_AIVR,
    AI_AISCNT,
    AI_AIIT,
} AIHwReg;

#define AI_AICR_SCRESET (1 << 5)
#define AI_AICR_AIINT (1 << 3)
#define AI_AICR_AIINTMSK (1 << 2)

#endif  // REVOLUTION_AI_H
