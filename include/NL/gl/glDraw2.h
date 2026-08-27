#ifndef NL_GL_GLDRAW2_H
#define NL_GL_GLDRAW2_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"

class glPoly2
{
public:
    void FullCoverage(const nlColour& col, float z);
    void SetupRectangle(float x, float y, float w, float h, float z);
    void SetupRotatedRectangle(float cx, float cy, float w, float h, float angle, float z);
    void SetColour(const nlColour& col);

    /* 0x00 */ nlVector2 m_pos[4];
    /* 0x20 */ nlVector2 m_uv[4];
    /* 0x40 */ nlColour m_colour[4];
    /* 0x50 */ float depth;
}; // size: 0x54

#endif // NL_GL_GLDRAW2_H
