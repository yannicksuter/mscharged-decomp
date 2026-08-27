#ifndef NL_GL_GLDRAW3_H
#define NL_GL_GLDRAW3_H

#include "NL/gl/gl.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

struct glModel;

class glQuad3
{
public:
    const glModel* GetModel() const;
    bool Attach(eGLView view, int stream);
    void SetupRotatedRectangle(
        float w, float h, const nlMatrix4& mtx, bool flipU, bool flipV);
    void SetupRotatedRectangle(float w, float h0, float h1,
        const nlMatrix4& mtx, bool flipU, bool flipV);
    void SetColour(const nlColour& c);
    void SetColour(
        unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    /* 0x00..0x2F */ nlVector3 m_pos[4];
    /* 0x30..0x4F */ nlVector2 m_uv[4];
    /* 0x50..0x5F */ nlColour m_colour[4];
};

bool glAttachQuad3(eGLView view, unsigned long count, glQuad3* quads);
bool glAttachQuad3(
    eGLView view, int stream, unsigned long count, glQuad3* quads);

#endif // NL_GL_GLDRAW3_H
