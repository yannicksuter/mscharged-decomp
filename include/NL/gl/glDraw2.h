#ifndef NL_GL_GLDRAW2_H
#define NL_GL_GLDRAW2_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"

class GLView;

class glPoly2
{
public:
    bool Attach(GLView* view, int layer, unsigned long* pMatrixHandle);
    void FullCoverage(const nlColour& col, float z);
    void SetupRectangle(float x, float y, float w, float h, float z);
    void SetupRotatedRectangle(float cx, float cy, float w, float h, float angle, float z);
    void SetColour(const nlColour& col);

    /* 0x00 */ nlVector2 m_pos[4];
    /* 0x20 */ nlVector2 m_uv[4];
    /* 0x40 */ nlColour m_colour[4];
    /* 0x50 */ float depth;
}; // size: 0x54

bool glAttachPoly2(GLView* view, unsigned long numPolys, glPoly2* pPolys,
    unsigned long* pMatrixHandle);
bool glAttachPoly2(GLView* view, int layer, unsigned long numPolys,
    glPoly2* pPolys, unsigned long* pMatrixHandle);
bool glAttachPoly2(GLView* view, int layer, unsigned long numPolys,
    glPoly2* pPolys, const void* pUserData, unsigned long* pMatrixHandle);

#endif // NL_GL_GLDRAW2_H
