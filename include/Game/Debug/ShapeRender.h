#ifndef _SHAPERENDER_H_
#define _SHAPERENDER_H_

#include "NL/nlMath.h"
#include "NL/nlColour.h"
#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"

struct PrimitiveShape
{
    /* 0x00 */ nlVector3* position;
    /* 0x04 */ nlVector3* normal;
    /* 0x08 */ nlVector2* texcoord;
    /* 0x0C */ int vertCount;
    /* 0x10 */ glModel* model;
}; // total size: 0x14

class ShapeRender
{
public:
    void DrawLine3D(
        const nlVector3& p0,
        const nlVector3& p1,
        const nlColour& colour,
        bool bWithDepth) const;

    /* 0x00 */ bool m_Initialized;
    /* 0x04 */ PrimitiveShape m_Box;
    /* 0x18 */ PrimitiveShape m_Hemisphere;
    /* 0x2C */ PrimitiveShape m_FlatCylinderEnd;
    /* 0x40 */ PrimitiveShape m_Cylinder;
    /* 0x54 */ void* m_pLightUserData;
    /* 0x58 */ eGLView m_eView;
}; // total size: 0x5C

extern ShapeRender g_ShapeRenderer;

#endif // _SHAPERENDER_H_
