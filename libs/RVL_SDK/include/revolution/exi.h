#ifndef REVOLUTION_EXI_H
#define REVOLUTION_EXI_H

#include <revolution/os/OSContext.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*EXICallback)(s32 channel, OSContext* context);

typedef enum EXIChannel {
    EXI_CHAN_0,
    EXI_CHAN_1,
    EXI_CHAN_2,
    EXI_MAX_CHAN,
} EXIChannel;

typedef enum EXIDevice {
    EXI_DEV_EXT,
    EXI_DEV_INT,
    EXI_DEV_NET,
    EXI_MAX_DEV,
} EXIDevice;

typedef struct EXIChannelParam {
    u32 cpr;
    void* mar;
    u32 length;
    u32 cr;
    u32 data;
} EXIChannelParam;

volatile EXIChannelParam EXI_CHAN_PARAMS[EXI_MAX_CHAN] AT_ADDRESS(0xCD006800);

#define EXI_CPR_EXIINTMASK (1 << 0)
#define EXI_CPR_EXIINT (1 << 1)
#define EXI_CPR_TCINTMASK (1 << 2)
#define EXI_CPR_TCINT (1 << 3)
#define EXI_CPR_EXTINTMASK (1 << 10)
#define EXI_CPR_EXTINT (1 << 11)
#define EXI_CPR_ROMDIS (1 << 13)
#define EXI_CR_TSTART (1 << 0)

BOOL EXISync(s32 channel);
BOOL EXIImm(s32 channel, void* buffer, s32 length, u32 type, EXICallback callback);
BOOL EXIDma(s32 channel, void* buffer, s32 length, u32 type, EXICallback callback);
BOOL EXISelect(s32 channel, u32 device, u32 frequency);
BOOL EXIDeselect(s32 channel);
BOOL EXILock(s32 channel, u32 device, EXICallback callback);
BOOL EXIUnlock(s32 channel);
BOOL EXIImmEx(s32 channel, void* buffer, s32 length, u32 type);
EXICallback EXISetExiCallback(s32 channel, EXICallback callback);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_EXI_H
