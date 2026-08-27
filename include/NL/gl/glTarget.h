#ifndef NL_GL_GLTARGET_H
#define NL_GL_GLTARGET_H

#include "types.h"

class TargetPlatform_8036DE50;

struct GLRenderPair
{
    unsigned long hash;
    TargetPlatform_8036DE50* target;

    GLRenderPair()
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
extern "C" GLRenderPair fn_802CD884(const char* name, void* targetInfo);
extern "C" void fn_802CDA14(GLRenderPair* target);
extern "C" unsigned long fn_802CDAA8(const GLRenderPair* target);

#endif // NL_GL_GLTARGET_H
