#ifndef GAME_GL_GLTEXTUREANIM_H
#define GAME_GL_GLTEXTUREANIM_H

#include "types.h"

enum eGLTexAnimMode
{
    GLAnimMode_Loop = 0,
    GLAnimMode_PingPong = 1,
    GLAnimMode_Hold = 2,
    GLAnimMode_Num = 3,
};

struct GLAnimTex
{
    /* 0x00 */ unsigned long m_TexHandle;
    /* 0x04 */ f32 m_fTime;
};

class GLResourcePool;

class GLTextureAnim
{
public:
    void SetTexture(int index, const GLAnimTex& animTex)
    {
        m_pAnimTex[index] = animTex;
    }

    u32 GetTextureIndex() const { return m_textureIndex; }

    GLAnimTex& GetTexture(int index);
    void Update(float dt);

    /* 0x00 */ s32 m_nFrame;
    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ s32 m_nNumTextures;
    /* 0x0C */ eGLTexAnimMode m_ePlayMode;
    /* 0x10 */ s32 m_nPlayDir;
    /* 0x14 */ bool m_bPaused;
    /* 0x15 */ u8 m_pad15[3];
    /* 0x18 */ u32 m_textureIndex;
    /* 0x1C */ f32 m_fTime;
    /* 0x20 */ GLAnimTex* m_pAnimTex;
};

GLTextureAnim* glGetTextureAnim(unsigned long texture);
bool glIsTextureAnim(const void* data, unsigned long size);
void glAddTextureAnim(void* data, unsigned long size,
    GLResourcePool* resource);
void glReleaseTextureAnim(GLTextureAnim* anim);

#endif // GAME_GL_GLTEXTUREANIM_H
