#ifndef REVOLUTION_OS_HARDWARE_H
#define REVOLUTION_OS_HARDWARE_H

#include <revolution/os/OSContext.h>
#include <revolution/os/OSThread.h>
#include <revolution/types.h>
#include <revolution/dvd/dvd.h>

#define OS_PHYS_WIFI_AFH_CHANNEL 0x000031A2
#define OS_PHYS_BOOT_INFO 0x00000000
#define OS_PHYS_TV_FORMAT 0x000000CC
#define OS_PHYS_INTR_HANDLER_TABLE 0x3040
#define OS_PHYS_PREV_INTR_MASK 0xC4
#define OS_PHYS_CURRENT_INTR_MASK 0xC8
#define OS_PHYS_ACCESSIBLE_MEM2_END 0x3120
#define OS_PHYS_DB_INTEGRATOR_HOOK 0x00000060
#define OS_PHYS_DVD_BI2 0x000000F4
#define OS_PHYS_EXCEPTION_TABLE 0x3000
#define OS_PHYS_BI2_DEBUG_FLAG 0x30E8
#define OS_PHYS_PAD_SPEC 0x30E9
#define OS_PHYS_USABLE_MEM1_START 0x310C
#define OS_PHYS_USABLE_MEM1_END 0x3110
#define OS_PHYS_USABLE_MEM2_START 0x3124
#define OS_PHYS_USABLE_MEM2_END 0x3128
#define OS_PHYS_HOLLYWOOD_REV 0x3138
#define OS_PHYS_IOS_VERSION 0x3140
#define OS_PHYS_IOS_BUILD_DATE 0x3144
#define OS_PHYS_BOOT_PROGRAM_TARGET 0x315C
#define OS_PHYS_APPLOADER_TARGET 0x315D
#define OS_PHYS_CURRENT_APP_NAME 0x3180
#define OS_PHYS_CURRENT_APP_TYPE 0x3184

OSContext* OS_CURRENT_CONTEXT_PHYS AT_ADDRESS(0x800000C0);
OSContext* OS_CURRENT_CONTEXT AT_ADDRESS(0x800000D4);
OSContext* OS_CURRENT_FPU_CONTEXT AT_ADDRESS(0x800000D8);
OSThreadQueue OS_THREAD_QUEUE AT_ADDRESS(0x800000DC);
OSThread* OS_CURRENT_THREAD AT_ADDRESS(0x800000E4);
u32 OS_TV_FORMAT AT_ADDRESS(0x800000CC);
u32 OS_BUS_CLOCK_SPEED AT_ADDRESS(0x800000F8);
s64 OS_SYSTEM_TIME AT_ADDRESS(0x800030D8);
extern volatile s32 OS_EXI_LAST_INSERT[2] AT_ADDRESS(0x800030C0);
u16 OS_GC_PAD_3_BTN AT_ADDRESS(0x800030E4);
u8 OS_PAD_FLAGS AT_ADDRESS(0x800030E3);
void* OS_DEBUG_MONITOR AT_ADDRESS(0x800000EC);
u32 OS_DEBUG_MONITOR_SIZE AT_ADDRESS(0x800000E8);
u32 OS_PHYSICAL_MEM1_SIZE AT_ADDRESS(0x80003100);
u32 OS_SIMULATED_MEM1_SIZE AT_ADDRESS(0x80003104);
u32 OS_PHYSICAL_MEM2_SIZE AT_ADDRESS(0x80003118);
u32 OS_SIMULATED_MEM2_SIZE AT_ADDRESS(0x8000311C);
u32 OS_HOLLYWOOD_REV AT_ADDRESS(0x80003138);

volatile u16 OS_DVD_DEVICE_CODE AT_ADDRESS(0x800030E6);
u8 OS_BI2_DEBUG_FLAG AT_ADDRESS(0x800030E8);
void* OS_USABLE_MEM1_END AT_ADDRESS(0x80003110);

typedef struct OSBI2 {
    u32 dbgMonitorSize;
    u32 simulatedMemSize;
    u32 argumentOfs;
    u32 debugFlag;
    u32 trackLocation;
    u32 trackSize;
    u32 countryCode;
    u32 WORD_0x1C;
    u32 lastInsert;
    u32 padSpec;
    u32 totalTextDataLimit;
    u32 simulatedMem2Size;
} OSBI2;

typedef enum {
    OS_BOOT_MAGIC_BOOTROM = 0xD15EA5E,
    OS_BOOT_MAGIC_JTAG = 0xE5207C22,
} OSBootMagic;

typedef struct OSBootInfo {
    DVDDiskID diskID;
    u32 bootMagic;
    u32 apploaderVersion;
    u32 physicalMemorySize;
    u32 consoleType;
    void* arenaLo;
    void* arenaHi;
    void* fstStart;
    u32 fstSize;
} OSBootInfo;

u8 OS_SC_PRDINFO[0x100] AT_ADDRESS(0x80003800);

extern volatile u32 PI_HW_REGS[] AT_ADDRESS(0xCC003000);

typedef enum PIHwReg {
    PI_INTSR,
    PI_INTMR,
} PIHwReg;

#define PI_INTSR_ERROR (1 << 0)
#define PI_INTSR_RSW (1 << 1)
#define PI_INTSR_DI (1 << 2)
#define PI_INTSR_SI (1 << 3)
#define PI_INTSR_EXI (1 << 4)
#define PI_INTSR_AI (1 << 5)
#define PI_INTSR_DSP (1 << 6)
#define PI_INTSR_MEM (1 << 7)
#define PI_INTSR_VI (1 << 8)
#define PI_INTSR_PE_TOKEN (1 << 9)
#define PI_INTSR_PE_FINISH (1 << 10)
#define PI_INTSR_CP (1 << 11)
#define PI_INTSR_DEBUG (1 << 12)
#define PI_INTSR_HSP (1 << 13)
#define PI_INTSR_ACR (1 << 14)
#define PI_INTSR_RSWST (1 << 16)

#define PI_INTMR_ERROR (1 << 0)
#define PI_INTMR_RSW (1 << 1)
#define PI_INTMR_DI (1 << 2)
#define PI_INTMR_SI (1 << 3)
#define PI_INTMR_EXI (1 << 4)
#define PI_INTMR_AI (1 << 5)
#define PI_INTMR_DSP (1 << 6)
#define PI_INTMR_MEM (1 << 7)
#define PI_INTMR_VI (1 << 8)
#define PI_INTMR_PE_TOKEN (1 << 9)
#define PI_INTMR_PE_FINISH (1 << 10)
#define PI_INTMR_CP (1 << 11)
#define PI_INTMR_DEBUG (1 << 12)
#define PI_INTMR_HSP (1 << 13)
#define PI_INTMR_ACR (1 << 14)

extern volatile u16 MI_HW_REGS[] AT_ADDRESS(0xCC004000);

typedef enum MIHwReg {
    MI_PAGE_MEM0_H,
    MI_PAGE_MEM0_L,
    MI_PAGE_MEM1_H,
    MI_PAGE_MEM1_L,
    MI_PAGE_MEM2_H,
    MI_PAGE_MEM2_L,
    MI_PAGE_MEM3_H,
    MI_PAGE_MEM3_L,
    MI_PROT_MEM0,
    MI_PROT_MEM1,
    MI_PROT_MEM2,
    MI_PROT_MEM3,
    MI_REG_0x18,
    MI_REG_0x1A,
    MI_INTMR,
    MI_INTSR,
    MI_REG_0x20,
    MI_ADDRLO,
    MI_ADDRHI,
    MI_REG_0x26,
    MI_REG_0x28,
} MIHwReg;

#define MI_INTMR_MEM0 (1 << 0)
#define MI_INTMR_MEM1 (1 << 1)
#define MI_INTMR_MEM2 (1 << 2)
#define MI_INTMR_MEM3 (1 << 3)
#define MI_INTMR_ADDR (1 << 4)

#define MI_INTSR_MEM0 (1 << 0)
#define MI_INTSR_MEM1 (1 << 1)
#define MI_INTSR_MEM2 (1 << 2)
#define MI_INTSR_MEM3 (1 << 3)
#define MI_INTSR_ADDR (1 << 4)

extern volatile u32 DI_HW_REGS[] AT_ADDRESS(0xCD006000);

typedef enum DIHwReg {
    DI_DMA_ADDR = 5,
    DI_CONFIG = 9,
} DIHwReg;

typedef struct OSDebugInterface {
    BOOL usingDebugger;
    u32 exceptionMask;
    void* exceptionHook;
    void* exceptionHookLR;
} OSDebugInterface;

#define OS_PHYS_DEBUG_INTERFACE 0x40
#define OS_PHYS_CURRENT_CONTEXT_PHYS 0xC0

OSBootInfo OS_BOOT_INFO AT_ADDRESS(0x80000000);
OSDebugInterface OS_DEBUG_INTERFACE AT_ADDRESS(0x80000040);
OSBI2* OS_DVD_BI2 AT_ADDRESS(0x800000F4);

#endif // REVOLUTION_OS_HARDWARE_H
