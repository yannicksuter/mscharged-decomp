#ifndef REVOLUTION_OS_ERROR_H
#define REVOLUTION_OS_ERROR_H

#include <revolution/os/OSContext.h>
#include <revolution/types.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define OS_ERROR(...) OSPanic(__FILE__, __LINE__, __VA_ARGS__)

#define OSAssert(file, line, expression, ...) \
    if (!(expression))                        \
    {                                         \
        OSPanic(file, line, __VA_ARGS__);     \
    }

typedef enum OSErrorType {
    OS_ERR_SYSTEM_RESET,
    OS_ERR_MACHINE_CHECK,
    OS_ERR_DSI,
    OS_ERR_ISI,
    OS_ERR_EXT_INTERRUPT,
    OS_ERR_ALIGNMENT,
    OS_ERR_PROGRAM,
    OS_ERR_FP_UNAVAIL,
    OS_ERR_DECREMENTER,
    OS_ERR_SYSTEM_CALL,
    OS_ERR_TRACE,
    OS_ERR_PERF_MONITOR,
    OS_ERR_IABR,
    OS_ERR_SMI,
    OS_ERR_THERMAL_INT,
    OS_ERR_PROTECTION,
    OS_ERR_FP_EXCEPTION,
    OS_ERR_MAX
} OSErrorType;

typedef void (*OSErrorHandler)(u8 error, OSContext* context, u32 dsisr, u32 dar, ...);

extern OSErrorHandler __OSErrorTable[OS_ERR_MAX];
extern u32 __OSFpscrEnableBits;

void OSReport(const char* message, ...);
void OSVReport(const char* message, va_list list);
void OSPanic(const char* file, int line, const char* message, ...);
OSErrorHandler OSSetErrorHandler(u16 error, OSErrorHandler handler);
void __OSUnhandledException(u8 error, OSContext* context, u32 dsisr, u32 dar);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_ERROR_H
