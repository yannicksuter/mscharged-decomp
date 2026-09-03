#ifndef GAME_GL_GLVERTEXANIM_H
#define GAME_GL_GLVERTEXANIM_H

#include "NL/gl/glModel.h"
#include "types.h"

enum eGLVertAnimMode
{
    GLVAnimMode_Loop = 0,
    GLVAnimMode_Hold = 1,
    GLVAnimMode_Num = 2,
};

class GLVertexAnim
{
public:
    GLVertexAnim(const void* data, const void* extraData);
    ~GLVertexAnim();

    glModel* GetModel(int frame);
    void Update(float dt);

    /* 0x00 */ u32 m_uHashID;
    /* 0x04 */ u32 m_nNumFrames;
    /* 0x08 */ s32 m_nNumVertices;
    /* 0x0C */ u32 m_Unknown0C;
    /* 0x10 */ u32 m_Unknown10;
    /* 0x14 */ u32 m_Unknown14;
    /* 0x18 */ s32* m_Unknown18;
    /* 0x1C */ eGLVertAnimMode m_eMode;
    /* 0x20 */ bool m_bDone;
    /* 0x21 */ u8 m_pad21[3];
    /* 0x24 */ f32 m_fTimeScale;
    /* 0x28 */ f32 m_fFrame;
    /* 0x2C */ f32 m_fFrameRate;
    /* 0x30 */ u8* m_pVertices;
    /* 0x34 */ glModel* m_pModel;
}; // total size: 0x38

#endif // GAME_GL_GLVERTEXANIM_H
