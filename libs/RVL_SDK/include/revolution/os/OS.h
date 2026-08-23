#ifndef REVOLUTION_OS_BASE_H
#define REVOLUTION_OS_BASE_H

#include <revolution/os/OSContext.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*OSExceptionHandler)(u8 type, OSContext* context);

extern BOOL __OSInIPL;
extern u8 _stack_addr[];
extern u8 _stack_end[];

OSExceptionHandler __OSSetExceptionHandler(u8 type, OSExceptionHandler handler);
OSExceptionHandler __OSGetExceptionHandler(u8 type);
void OSDefaultExceptionHandler(u8 type, OSContext* context);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_BASE_H
