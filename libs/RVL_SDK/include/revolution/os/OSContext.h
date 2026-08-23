#ifndef REVOLUTION_OS_CONTEXT_H
#define REVOLUTION_OS_CONTEXT_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_CONTEXT_STATE_FP_SAVED (1 << 0)

typedef struct OSContext {
    u32 gprs[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fprs[32];
    u32 fpscr_pad;
    u32 fpscr;
    u32 srr0;
    u32 srr1;
    u16 mode;
    u16 state;
    u32 gqrs[8];
    u32 psf_pad;
    f64 psfs[32];
} OSContext;

void OSSaveFPUContext(OSContext* context);
void OSSetCurrentContext(OSContext* context);
OSContext* OSGetCurrentContext(void);
BOOL OSSaveContext(OSContext* context);
void OSLoadContext(OSContext* context);
void* OSGetStackPointer(void);
void OSSwitchFiber(void* function, void* stack);
void OSSwitchFiberEx(u32 r3, u32 r4, u32 r5, u32 r6, void* function, void* stack);
void OSClearContext(OSContext* context);
void OSInitContext(OSContext* context, void* srr0, void* stack);
void OSDumpContext(const OSContext* context);
void __OSContextInit(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_CONTEXT_H
