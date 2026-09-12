#ifndef NL_GLX_GX_MATERIAL_PROGRAM_TEXTURE_ANIMATION_H
#define NL_GLX_GX_MATERIAL_PROGRAM_TEXTURE_ANIMATION_H

#include <revolution/gx.h>
#include "Game/Render/StadiumLoading.h"
#include "NL/nlMath.h"

static inline float glxWrapTextureOffset(float value)
{
    return (float)((int)(value * 1024.0f) % 1024) / 1024.0f;
}

static inline void glxLoadScrollingTextureMatrix(unsigned int matrix, const nlVector2& speed)
{
    float time = GetStadiumTime();
    nlVector2 offset;
    offset.x = glxWrapTextureOffset(time * speed.x);
    offset.y = glxWrapTextureOffset(time * speed.y);
    Mtx textureMatrix = {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
    };
    textureMatrix[0][3] = offset.x;
    textureMatrix[1][3] = offset.y;
    GXLoadTexMtxImm(textureMatrix, matrix, GX_MTX3x4);
}

#endif // NL_GLX_GX_MATERIAL_PROGRAM_TEXTURE_ANIMATION_H
