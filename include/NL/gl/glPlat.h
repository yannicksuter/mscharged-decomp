#ifndef NL_GL_GLPLAT_H
#define NL_GL_GLPLAT_H

#include "NL/gl/glStruct.h"

#ifndef RVL_SDK_GX_FRAMEBUF_H
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
#endif

struct glModelPacket;

extern GXRenderModeObj glx_rmode;
extern "C" void fn_8036E438(glModelPacket* packet, bool permanent, void* allocator);

bool glplatPreStartup();
bool glplatStartup(gl_ScreenInfo* screenInfo);
bool glplatPostStartup();
void glplatBeginFrame();
void glplatEndFrame();
void glplatSendFrame();
void glplatAbortFrame();
void glplatFinish();
void glx_ClearXFB(void* framebuffer);

u32 fn_80369394();
u32 fn_803693A4();
s32 fn_803693B4();
u32 fn_80369D5C();
u32 fn_80369D64();

#endif // NL_GL_GLPLAT_H
