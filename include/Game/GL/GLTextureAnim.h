#ifndef GAME_GL_GLTEXTUREANIM_H
#define GAME_GL_GLTEXTUREANIM_H

#include "types.h"

struct GLAnimTex
{
    /* 0x00 */ unsigned long textureHandle;
    /* 0x04 */ f32 time;
};

class GLResourcePool;

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
    /* 0x04 */ u32 m_hashID;
    /* 0x08 */ s32 m_frameCount;
    /* 0x0C */ u32 m_mode;
    /* 0x10 */ s32 m_direction;
    /* 0x14 */ bool m_isStopped;
    /* 0x15 */ u8 m_pad15[3];
    /* 0x18 */ u32 m_textureIndex;
    /* 0x1C */ f32 m_currentTime;
    /* 0x20 */ GLAnimTex* m_frames;
};

GLTextureAnim* glGetTextureAnim(unsigned long texture);
bool glIsTextureAnim(const void* data, unsigned long size);
void glAddTextureAnim(const void* data, unsigned long size,
    GLResourcePool* resource);
void glReleaseTextureAnim(GLTextureAnim* anim);

#endif // GAME_GL_GLTEXTUREANIM_H
