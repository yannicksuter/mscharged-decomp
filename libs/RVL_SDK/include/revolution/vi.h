#ifndef REVOLUTION_VI_H
#define REVOLUTION_VI_H

#include <revolution/gx.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VI_TVMODE(format, scanMode) (((format) << 2) + (scanMode))

typedef enum VITVFormat {
    VI_TVFORMAT_NTSC,
    VI_TVFORMAT_PAL,
    VI_TVFORMAT_MPAL,
    VI_TVFORMAT_DEBUG,
    VI_TVFORMAT_DEBUG_PAL,
    VI_TVFORMAT_EURGB60,
} VITVFormat;

typedef enum VIScanMode {
    VI_SCANMODE_INT,
    VI_SCANMODE_DS,
    VI_SCANMODE_PROG,
} VIScanMode;

typedef enum VIXFBMode {
    VI_XFBMODE_SF,
    VI_XFBMODE_DF,
} VIXFBMode;

typedef void (*VIRetraceCallback)(u32 retraceCount);

volatile u16 VI_HW_REGS[0x3B] AT_ADDRESS(0xCC002000);

typedef enum VIHwReg
{
    VI_VTR,
    VI_DCR,
    VI_VICLK = 0x6C / 2,
    VI_VISEL = 0x6E / 2
} VIHwReg;

#define VI_VICLK_SPEED (1 << 0)

VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback callback);
VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback callback);
void VIInit(void);
void VIConfigure(const GXRenderModeObj* renderMode);
void VIConfigurePan(u16 x, u16 y, u16 width, u16 height);
void VIFlush(void);
void VISetNextFrameBuffer(void* frameBuffer);
void VISetBlack(BOOL black);
s32 VIGetRetraceCount(void);
VITVFormat VIGetTvFormat(void);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_VI_H
