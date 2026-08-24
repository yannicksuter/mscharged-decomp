#ifndef REVOLUTION_EXI_H
#define REVOLUTION_EXI_H

#include <revolution/os/OSContext.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum EXIChannel {
    EXI_CHAN_0,
    EXI_CHAN_1,
    EXI_CHAN_2,
    EXI_MAX_CHAN,
} EXIChannel;

typedef void (*EXICallback)(EXIChannel channel, OSContext* context);

typedef enum EXIDevice {
    EXI_DEV_EXT,
    EXI_DEV_INT,
    EXI_DEV_NET,
    EXI_MAX_DEV,
} EXIDevice;

typedef enum EXIState {
    EXI_STATE_DMA_ACCESS = (1 << 0),
    EXI_STATE_IMM_ACCESS = (1 << 1),
    EXI_STATE_SELECTED = (1 << 2),
    EXI_STATE_ATTACHED = (1 << 3),
    EXI_STATE_LOCKED = (1 << 4),

    EXI_STATE_BUSY = EXI_STATE_DMA_ACCESS | EXI_STATE_IMM_ACCESS
} EXIState;

typedef enum EXIFreq {
    EXI_FREQ_1MHZ,
    EXI_FREQ_2MHZ,
    EXI_FREQ_4MHZ,
    EXI_FREQ_8MHZ,
    EXI_FREQ_16MHZ,
    EXI_FREQ_32HZ,
    EXI_MAX_FREQ
} EXIFreq;

typedef enum EXIType {
    EXI_READ,
    EXI_WRITE,
    EXI_TYPE_2,
    EXI_MAX_TYPE
} EXIType;

typedef enum EXIDeviceID {
    EXI_ID_MEMCARD_59 = 0x00000004,
    EXI_ID_MEMCARD_123 = 0x00000008,
    EXI_ID_MEMCARD_251 = 0x00000010,
    EXI_ID_MEMCARD_507 = 0x00000020,
    EXI_ID_MEMCARD_1019 = 0x00000040,
    EXI_ID_MEMCARD_2043 = 0x00000080,
    EXI_ID_USB_ADAPTER = 0x01010000,
    EXI_ID_IS_DOL_VIEWER = 0x05070000,
    EXI_ID_BROADBAND_ADAPTER = 0x04020200,
    EXI_ID_INVALID = 0xFFFFFFFF
} EXIDeviceID;

typedef struct EXIItem {
    u32 dev;
    EXICallback callback;
} EXIItem;

typedef struct EXIData {
    EXICallback exiCallback;
    EXICallback tcCallback;
    EXICallback extCallback;
    volatile s32 state;
    s32 bytesRead;
    void* buffer;
    u32 dev;
    u32 id;
    s32 lastInsert;
    s32 numItems;
    EXIItem items[3];
} EXIData;

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
#define EXI_CPR_CLK (1 << 4 | 1 << 5 | 1 << 6)
#define EXI_CPR_CS0B (1 << 7)
#define EXI_CPR_CS1B (1 << 8)
#define EXI_CPR_CS2B (1 << 9)
#define EXI_CPR_EXTINTMASK (1 << 10)
#define EXI_CPR_EXTINT (1 << 11)
#define EXI_CPR_EXT (1 << 12)
#define EXI_CPR_ROMDIS (1 << 13)

#define EXI_CR_TSTART (1 << 0)
#define EXI_CR_DMA (1 << 1)
#define EXI_CR_RW (1 << 2 | 1 << 3)
#define EXI_CR_TLEN (1 << 4 | 1 << 5)

void EXIInit(void);
BOOL EXISync(EXIChannel channel);
BOOL EXIImm(EXIChannel channel, void* buffer, s32 length, u32 type, EXICallback callback);
BOOL EXIDma(EXIChannel channel, void* buffer, s32 length, u32 type, EXICallback callback);
BOOL EXISelect(EXIChannel channel, u32 device, u32 frequency);
BOOL EXIDeselect(EXIChannel channel);
BOOL EXILock(EXIChannel channel, u32 device, EXICallback callback);
BOOL EXIUnlock(EXIChannel channel);
BOOL EXIImmEx(EXIChannel channel, void* buffer, s32 length, u32 type);
EXICallback EXISetExiCallback(EXIChannel channel, EXICallback callback);
void EXIClearInterrupts(EXIChannel channel, BOOL exi, BOOL tc, BOOL ext);
void EXIProbeReset(void);
BOOL EXIProbe(EXIChannel channel);
BOOL EXIAttach(EXIChannel channel, EXICallback callback);
BOOL EXIDetach(EXIChannel channel);
s32 EXIGetID(EXIChannel channel, u32 device, u32* out);
BOOL EXIWriteReg(EXIChannel channel, u32 device, u32 cmd, const void* buffer, s32 length);

extern const u32 __EXIFreq;

static u32 __EXISwap32(u32 value)
{
    return value >> 24 & 0x000000FF | value >> 8 & 0x0000FF00 |
           value << 8 & 0x00FF0000 | value << 24 & 0xFF000000;
}
extern const char* __EXIVersion;

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_EXI_H
