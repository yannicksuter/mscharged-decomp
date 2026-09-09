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
class GLView;
class nlVector3;

void glplatViewProjectPoint(GLView* view, const nlVector3& v3world, nlVector3& v3NDC);

struct PlatformViewport
{
    int x;
    int y;
    int width;
    int height;
};

PlatformViewport* glplatGetViewport();

extern GXRenderModeObj glx_rmode;
void glplatFinalizePacket(glModelPacket* packet, bool permanent, void* allocator);

void glplatInitializeMaterialPrograms();

bool glplatPreStartup();
void glplatInitializeMaterialPrograms();
bool glplatStartup(gl_ScreenInfo* screenInfo);
bool glplatPostStartup();
void glplatBeginFrame();
void glplatEndFrame();
void glplatSendFrame();
void glplatAbortFrame();
void glplatFinish();
void glx_ClearXFB(void* framebuffer);

u32 glplatGetDefaultTargetWidth();
u32 glplatGetDefaultTargetHeight();
u32 glx_GetScaledXFBWidth();
u32 glplatGetFrameBufferWidth();
u32 glplatGetFrameBufferHeight();
s32 glx_GetVideoMode();
u32 glplatGetOrthographicWidth();
u32 glplatGetOrthographicHeight();

#endif // NL_GL_GLPLAT_H
