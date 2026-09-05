#ifndef NL_GL_GLTARGET_H
#define NL_GL_GLTARGET_H

#include "types.h"

class TargetPlatform_8036DE50;

struct TargetInfo_8036DE50
{
    TargetInfo_8036DE50()
        : unknown08(0)
        , unknown0C(0)
    {
    }

    /* 0x00 */ unsigned long height;
    /* 0x04 */ unsigned long width;
    /* 0x08 */ unsigned long unknown08;
    /* 0x0C */ unsigned long unknown0C;
    /* 0x10 */ unsigned long unknown10;
    /* 0x14 */ int format;
    /* 0x18 */ unsigned long unknown18;
    /* 0x1C */ unsigned long unknown1C;
    /* 0x20 */ unsigned long unknown20;
    /* 0x24 */ unsigned char colour[4];
}; // size: 0x28

struct GLRenderPair
{
    unsigned long hash;
    TargetPlatform_8036DE50* target;

    GLRenderPair()
        : hash(0)
        , target(0)
    {
    }

    GLRenderPair(unsigned long targetHash, TargetPlatform_8036DE50* platformTarget)
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

extern "C" GLRenderPair fn_802CD82C();
extern "C" GLRenderPair fn_802CD884(const char* name, const TargetInfo_8036DE50* targetInfo);
extern "C" void fn_802CDA14(GLRenderPair* target);
extern "C" unsigned long fn_802CDAA8(GLRenderPair target);

#endif // NL_GL_GLTARGET_H
