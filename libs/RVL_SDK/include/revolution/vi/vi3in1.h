#include <decomp.h>
#ifndef NO_INLINE
#define NO_INLINE DECOMP_DONT_INLINE
#endif
#ifndef _RVL_SDK_VI_VI3IN1_H
#define _RVL_SDK_VI_VI3IN1_H

#include <revolution/types.h>
#include <revolution/vi/vitypes.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile u32 Vdac_Flag_Changed;

void VISetRGBModeImm(void);
void __VISetFilter4EURGB60(VITiming timing);
void __VISetCGMS(void);
void __VISetWSS(void);
void __VISetClosedCaption(void);
void __VISetMacrovision(void);
void __VISetGamma(void);
void VISetGamma(VIGamma gamma);
void __VISetTrapFilter(void);
void __VISetRGBOverDrive(void);
void __VISetRGBModeImm(void);

void __VISetRevolutionModeSimple(void);
void __VISetYUVSEL(VIBool outsel) NO_INLINE;

#ifdef __cplusplus
}
#endif

#endif
