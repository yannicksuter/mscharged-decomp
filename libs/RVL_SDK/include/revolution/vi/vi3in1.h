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

void __VISetRGBModeImm(void);

void __VISetRevolutionModeSimple(void);
void __VISetYUVSEL(VIBool outsel) NO_INLINE;

#ifdef __cplusplus
}
#endif

#endif
