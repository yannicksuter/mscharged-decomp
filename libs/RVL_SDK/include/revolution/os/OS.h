#ifndef REVOLUTION_OS_BASE_H
#define REVOLUTION_OS_BASE_H

#include <revolution/os/OSContext.h>
#include <revolution/os/OSExec.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum OSConsoleType {
    OS_CONSOLE_MASK = 0xF0000000,
    OS_CONSOLE_MASK_RVL = 0x00000000,
    OS_CONSOLE_MASK_EMU = 0x10000000,
    OS_CONSOLE_MASK_TDEV = 0x20000000,

    OS_CONSOLE_RVL_PP_1 = 0x00000011,
    OS_CONSOLE_RVL_PP_2_1 = 0x00000012,
    OS_CONSOLE_RVL_PP_2_2 = 0x00000020,

    OS_CONSOLE_RVL_EMU = 0x10000008,
    OS_CONSOLE_NDEV_1_0 = 0x10000010,
    OS_CONSOLE_NDEV_1_1 = 0x10000011,
    OS_CONSOLE_NDEV_1_2 = 0x10000012,
    OS_CONSOLE_NDEV_2_0 = 0x10000020,
    OS_CONSOLE_NDEV_2_1 = 0x10000021
} OSConsoleType;

typedef enum OSAppType {
    OS_APP_TYPE_IPL = 0x40,
    OS_APP_TYPE_DVD = 0x80,
    OS_APP_TYPE_CHANNEL = 0x81
} OSAppType;

typedef enum OSExceptionType {
    OS_EXC_SYSTEM_RESET,
    OS_EXC_MACHINE_CHECK,
    OS_EXC_DSI,
    OS_EXC_ISI,
    OS_EXC_EXT_INTERRUPT,
    OS_EXC_ALIGNMENT,
    OS_EXC_PROGRAM,
    OS_EXC_FP_UNAVAIL,
    OS_EXC_DECREMENTER,
    OS_EXC_SYSTEM_CALL,
    OS_EXC_TRACE,
    OS_EXC_PERF_MONITOR,
    OS_EXC_IABR,
    OS_EXC_SMI,
    OS_EXC_THERMAL_INT,

    OS_EXC_MAX
} OSExceptionType;

typedef struct OSIOSRev {
    u8 idHi;
    u8 idLo;
    u8 verMajor;
    u8 verMinor;
    u8 buildMon;
    u8 buildDay;
    u16 buildYear;
} OSIOSRev;

typedef void (*OSExceptionHandler)(u8 type, OSContext* context);

extern BOOL __OSInIPL;
extern BOOL __OSInNandBoot;
extern BOOL __OSIsGcam;
extern s64 __OSStartTime;
extern const char* __OSVersion;

void __OSFPRInit(void);
u32 __OSGetHollywoodRev(void);
void __OSGetIOSRev(OSIOSRev* rev);
u32 OSGetConsoleType(void);
void OSInit(void);
OSExceptionHandler __OSSetExceptionHandler(u8 type, OSExceptionHandler handler);
OSExceptionHandler __OSGetExceptionHandler(u8 type);
void OSDefaultExceptionHandler(u8 type, OSContext* context);
void __OSPSInit(void);
void __OSInitNet(void);
void __OSInitSystemCall(void);
void __OSModuleInit(void);
void __OSInitIPCBuffer(void);
u32 __OSGetDIConfig(void);
void OSRegisterVersion(const char* version);
const char* OSGetAppGamename(void);
u8 OSGetAppType(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_BASE_H
