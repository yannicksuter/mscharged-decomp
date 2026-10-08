#ifndef RVL_SDK_VI_API_FWD_H
#define RVL_SDK_VI_API_FWD_H

#include <revolution/gx/GXRenderModeObj.h>
#include <revolution/vi/viPublicTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*VIRetraceCallback)(unsigned long retraceCount);
typedef void (*VIPositionCallback)(short displayX, short displayY);

VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback callback);
VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback callback);
void VIInit(void);
void VIWaitForRetrace(void);
void VIConfigure(const GXRenderModeObj* rmo);
void VIConfigurePan(unsigned short x, unsigned short y, unsigned short w, unsigned short h);
void VIFlush(void);
void VISetNextFrameBuffer(void* fb);
void* VIGetCurrentFrameBuffer(void);
void VISetBlack(int black);
unsigned long VIGetRetraceCount(void);
unsigned long VIGetCurrentLine(void);
VITvFormat VIGetTvFormat(void);
unsigned long VIGetDTVStatus(void);
VITimeToDIM VISetTimeToDimming(VITimeToDIM time);

void __VIInit(VITVMode mode);
void __VIDisplayPositionToXY(unsigned long hct, unsigned long vct, short* x, short* y);
int __VIResetRFIdle(void);
int __VIResetSIIdle(void);

#ifdef __cplusplus
}
#endif

#endif
