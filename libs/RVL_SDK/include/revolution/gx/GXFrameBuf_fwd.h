#ifndef RVL_SDK_GX_FRAMEBUF_FWD_H
#define RVL_SDK_GX_FRAMEBUF_FWD_H

#include <revolution/gx/GXRenderModeObj.h>
#include <revolution/gx/GXPublicTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void GXSetCopyClear(GXColor color, unsigned long z);
void GXCopyDisp(void* destination, GXBool clear);

void GXSetCopyFilter(unsigned char aa, const unsigned char sample_pattern[12][2],
                     unsigned char vf, const unsigned char vfilter[GX_VFILTER_SZ]);

#ifdef __cplusplus
}
#endif

#endif
