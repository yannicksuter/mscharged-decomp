#ifndef NL_GL_GLTARGET_H
#define NL_GL_GLTARGET_H

#include "types.h"

class GLXTarget;

enum GLTargetFormat
{
    GLTargetFormat_RGB5A3 = 0,
    GLTargetFormat_RGB565 = 1,
    GLTargetFormat_RGBA8 = 5,
    GLTargetFormat_A8 = 6,
    GLTargetFormat_IA8 = 7
};

enum GLTargetInfoMode10
{
    GLTargetInfoMode10_0 = 0,
    GLTargetInfoMode10_1 = 1
};

struct GLTargetInfo
{
    GLTargetInfo()
        : unknown08(0)
        , unknown0C(0)
    {
    }

    /* 0x00 */ unsigned long height;
    /* 0x04 */ unsigned long width;
    /* 0x08 */ unsigned long unknown08;
    /* 0x0C */ unsigned long unknown0C;
    /* 0x10 */ GLTargetInfoMode10 unknown10;
    /* 0x14 */ GLTargetFormat format;
    /* 0x18 */ unsigned long clearFlags;
    /* 0x1C */ unsigned long clearDepthSetting;
    /* 0x20 */ unsigned long unknown20;
    /* 0x24 */ unsigned char colour[4];
}; // size: 0x28

struct GLRenderPair
{
    unsigned long hash;
    GLXTarget* target;

    GLRenderPair()
        : hash(0)
        , target(0)
    {
    }

    GLRenderPair(unsigned long targetHash, GLXTarget* platformTarget)
        : hash(targetHash)
        , target(platformTarget)
    {
    }

    operator bool() const
    {
        return hash != 0 && target != 0;
    }
};

void gl_TargetStartup();

GLRenderPair glGetBackBufferTarget();
GLRenderPair glCreateTarget(const char* name, const GLTargetInfo* targetInfo);
void glDestroyTarget(GLRenderPair* target);
unsigned long glGetTargetTexture(GLRenderPair target);

#endif // NL_GL_GLTARGET_H
