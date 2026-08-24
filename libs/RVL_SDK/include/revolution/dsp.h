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

#define DSP_DSPMBOX_H_STATUS (1 << 15)
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

#define DSP_SEND_MAIL_SYNC(x)          \
    do {                               \
        DSPSendMailToDSP((DSPMail)(u32)(x)); \
        while (DSPCheckMailToDSP()) {  \
            ;                          \
        }                              \
    } while (0)

typedef struct OSContext OSContext;

typedef void* DSPMail;

typedef enum {
    DSP_TASK_ACTIVE = (1 << 0),
    DSP_TASK_CANCELED = (1 << 1),
} DSPTaskFlags;

typedef enum {
    DSP_TASK_STATE_0,
    DSP_TASK_STATE_1,
    DSP_TASK_STATE_2,
    DSP_TASK_STATE_3,
} DSPTaskState;

typedef struct DSPTask DSPTask;

typedef void (*DSPTaskCallback)(DSPTask* task);

typedef struct DSPTask {
    /* 0x00 */ u32 state;
    /* 0x04 */ volatile u32 prio;
    /* 0x08 */ u32 flags;
    /* 0x0C */ void* iramMmemAddr;
    /* 0x10 */ u32 iramMmemLen;
    /* 0x14 */ u32 iramDspAddr;
    /* 0x18 */ void* dramMmemAddr;
    /* 0x1C */ u32 dramMmemLen;
    /* 0x20 */ u32 dramDspAddr;
    /* 0x24 */ u16 startVector;
    /* 0x26 */ u16 resumeVector;
    /* 0x28 */ DSPTaskCallback initCallback;
    /* 0x2C */ DSPTaskCallback resumeCallback;
    /* 0x30 */ DSPTaskCallback doneCallback;
    /* 0x34 */ DSPTaskCallback requestCallback;
    /* 0x38 */ DSPTask* next;
    /* 0x3C */ DSPTask* prev;
    /* 0x40 */ char UNK_0x40[0x50 - 0x40];
} DSPTask; // size = 0x50

extern BOOL __DSP_rude_task_pending;
extern DSPTask* __DSP_rude_task;
extern DSPTask* __DSP_tmp_task;
extern DSPTask* __DSP_last_task;
extern DSPTask* __DSP_first_task;
extern DSPTask* __DSP_curr_task;

BOOL DSPCheckMailToDSP(void);
BOOL DSPCheckMailFromDSP(void);
DSPMail DSPReadMailFromDSP(void);
void DSPSendMailToDSP(DSPMail mail);
void DSPAssertInt(void);
void DSPInit(void);
BOOL DSPCheckInit(void);
DSPTask* DSPAddTask(DSPTask* task);
DSPTask* DSPAssertTask(DSPTask* task);

void __DSP_debug_printf(const char* fmt, ...);

void __DSPHandler(s16 intr, OSContext* ctx);
void __DSP_exec_task(DSPTask* task1, DSPTask* task2);
void __DSP_boot_task(DSPTask* task);
void __DSP_insert_task(DSPTask* task);
void __DSP_add_task(DSPTask* task);
void __DSP_remove_task(DSPTask* task);

#define DSP_CSR_PIINT (1 << 1)

#endif  // REVOLUTION_DSP_H
