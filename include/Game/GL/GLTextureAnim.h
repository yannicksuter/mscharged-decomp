#ifndef GAME_GL_GLTEXTUREANIM_H
#define GAME_GL_GLTEXTUREANIM_H

#include "types.h"

struct GLAnimTex
{
    /* 0x00 */ u32 textureHandle;
    /* 0x04 */ f32 time;
};

class GLTextureAnim
{
public:
    void SetTexture(int frameIndex, const GLAnimTex& animTex)
    {
        GLAnimTex* textureArray = m_frames;
        u32 offset = frameIndex * sizeof(GLAnimTex);
        GLAnimTex* targetTex = (GLAnimTex*)((u8*)textureArray + offset);

        targetTex->textureHandle = animTex.textureHandle;
        targetTex->time = animTex.time;
    }

    GLAnimTex* GetTexture(int frameIndex);
    void Update(float dt);

    /* 0x00 */ s32 m_currentFrame;
    /* 0x04 */ u32 m_unk_0x04;
    /* 0x08 */ s32 m_frameCount;
    /* 0x0C */ u32 m_mode;
    /* 0x10 */ s32 m_direction;
    /* 0x14 */ bool m_isStopped;
    /* 0x15 */ u8 m_pad15[3];
    /* 0x18 */ u32 m_unk_0x18;
    /* 0x1C */ f32 m_currentTime;
    /* 0x20 */ GLAnimTex* m_frames;
};

#endif // GAME_GL_GLTEXTUREANIM_H
