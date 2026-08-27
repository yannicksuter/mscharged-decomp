#ifndef NL_GL_GLPLAT_H
#define NL_GL_GLPLAT_H

#include "NL/gl/glStruct.h"

struct GXRenderModeObj
{
    u32 tvInfo;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    u32 xfbMode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
};

extern GXRenderModeObj glx_rmode;

bool glplatPreStartup();
bool glplatStartup(gl_ScreenInfo* screenInfo);
bool glplatPostStartup();
void glplatBeginFrame();
void glplatEndFrame();
void glplatSendFrame();
void glplatAbortFrame();
void glplatFinish();
void glx_ClearXFB(void* framebuffer);

#endif // NL_GL_GLPLAT_H
