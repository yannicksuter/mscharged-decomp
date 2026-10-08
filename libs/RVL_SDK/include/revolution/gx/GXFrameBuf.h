#ifndef RVL_SDK_GX_FRAMEBUF_H
#define RVL_SDK_GX_FRAMEBUF_H
#include <revolution/types.h>
#include <revolution/gx/GXRenderModeObj.h>
#include <revolution/gx/GXFrameBuf_fwd.h>
#ifdef __cplusplus
extern "C" {
#endif

extern GXRenderModeObj GXNtsc480IntDf;
extern GXRenderModeObj GXNtsc480Prog;
extern GXRenderModeObj GXNtsc480ProgSoft;
extern GXRenderModeObj GXPal528IntDf;
extern GXRenderModeObj GXEurgb60Hz480IntDf;
extern GXRenderModeObj GXEurgb60Hz480Prog;
extern GXRenderModeObj GXEurgb60Hz480ProgSoft;
extern GXRenderModeObj GXMpal480IntDf;

void GXAdjustForOverscan(GXRenderModeObj* rIn, GXRenderModeObj* rOut, u16 horiz,
                         u16 vert);

void GXSetDispCopySrc(u16 x, u16 y, u16 w, u16 h);
void GXSetTexCopySrc(u16 x, u16 y, u16 w, u16 h);

void GXSetDispCopyDst(u16 w, u16 numXfbLines);
void GXSetTexCopyDst(u16 w, u16 h, GXTexFmt fmt, GXBool mipmap);
void GXSetDispCopyFrame2Field(GXCopyMode mode);

void GXSetCopyClamp(GXCopyClamp clamp);

void GXSetDispCopyGamma(GXGamma gamma);

u32 GXGetNumXfbLines(u16 efbHeight, f32 scaleY);
f32 GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight);
u32 GXSetDispCopyYScale(f32 scaleY);


void GXCopyTex(void*, GXBool);
void GXClearBoundingBox(void);

#ifdef __cplusplus
}
#endif
#endif
