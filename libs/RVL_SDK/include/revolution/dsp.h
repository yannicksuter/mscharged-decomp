#ifndef REVOLUTION_DSP_H
#define REVOLUTION_DSP_H

#include <revolution/types.h>

volatile u16 DSP_HW_REGS[] AT_ADDRESS(0xCC005000);

typedef enum DSPHwReg {
    DSP_DSPMBOX_H,
    DSP_DSPMBOX_L,
    DSP_CPUMBOX_H,
    DSP_CPUMBOX_L,
    DSP_REG_0x8,
    DSP_CSR,
    DSP_REG_0xC,
    DSP_REG_0xE,
    DSP_REG_0x10,
    DSP_AR_SIZE,
    DSP_REG_0x14,
    DSP_AR_MODE,
    DSP_REG_0x18,
    DSP_AR_REFRESH,
    DSP_REG_0x1C,
    DSP_REG_0x1E,
    DSP_AR_DMA_MMADDR_H,
    DSP_AR_DMA_MMADDR_L,
    DSP_AR_DMA_ARADDR_H,
    DSP_AR_DMA_ARADDR_L,
    DSP_AR_DMA_CNT_H,
    DSP_AR_DMA_CNT_L,
    DSP_REG_0x2C,
    DSP_REG_0x2E,
    DSP_AI_DMA_START_H,
    DSP_AI_DMA_START_L,
    DSP_REG_0x34,
    DSP_AI_DMA_CSR,
    DSP_REG_0x38,
    DSP_AI_DMA_BYTES_LEFT,
} DSPHwReg;

#define DSP_CPUMBOX_H_STATUS (1 << 15)
#define DSP_CSR_RES (1 << 11)
#define DSP_CSR_DMAINT (1 << 9)
#define DSP_CSR_DSPINTMSK (1 << 8)
#define DSP_CSR_DSPINT (1 << 7)
#define DSP_CSR_ARINTMSK (1 << 6)
#define DSP_CSR_ARINT (1 << 5)
#define DSP_CSR_AIDINTMSK (1 << 4)
#define DSP_CSR_AIDINT (1 << 3)
#define DSP_CSR_HALT (1 << 2)

#define DSP_AI_DMA_CSR_PLAY (1 << 15)

#endif  // REVOLUTION_DSP_H
