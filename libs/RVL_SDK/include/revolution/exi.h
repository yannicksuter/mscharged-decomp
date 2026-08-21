#ifndef REVOLUTION_EXI_H
#define REVOLUTION_EXI_H

#include <revolution/os/OSContext.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*EXICallback)(s32 channel, OSContext* context);

BOOL EXISync(s32 channel);
BOOL EXIImm(s32 channel, void* buffer, s32 length, u32 type, EXICallback callback);
BOOL EXIDma(s32 channel, void* buffer, s32 length, u32 type, EXICallback callback);
BOOL EXISelect(s32 channel, u32 device, u32 frequency);
BOOL EXIDeselect(s32 channel);
BOOL EXILock(s32 channel, u32 device, EXICallback callback);
BOOL EXIUnlock(s32 channel);
BOOL EXIImmEx(s32 channel, void* buffer, s32 length, u32 type);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_EXI_H
