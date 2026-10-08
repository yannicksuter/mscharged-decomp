#ifndef NL_GL_GLPLAT_H
#define NL_GL_GLPLAT_H

#include "NL/gl/glStruct.h"
#include "NL/glx/glxModel.h"

#include <revolution/gx/GXRenderModeObj.h>

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
void glplatInitializeMaterialPrograms();

bool glplatPreStartup();
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
