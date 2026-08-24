#ifndef GAME_DRAWABLE_SHADOW_PROP_H
#define GAME_DRAWABLE_SHADOW_PROP_H

#include "NL/nlMath.h"

// Shared by the drawable shadow props. The quad is the payload handed to the
// immediate-mode draw call; the ground query object is not reconstructed, so
// only the field the props read is modelled.
struct ShadowQuad
{
    nlVector3 corners[4];
    float uv[4][2];
    u32 colors[4];
};

struct GroundInfo
{
    char _000[0x98];
    /* 0x98 */ float height;
};

extern "C" GroundInfo* fn_802772BC();
extern "C" void* fn_8027262C();
extern "C" void fn_802C9664(const ShadowQuad*, void*, int);

#endif // GAME_DRAWABLE_SHADOW_PROP_H
