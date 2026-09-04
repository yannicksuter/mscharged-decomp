#ifndef _SHAPERENDER_H_
#define _SHAPERENDER_H_

#include "NL/nlMath.h"
#include "NL/nlColour.h"
#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"

class GLView;

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
    ShapeRender()
        : m_Unknown00(0)
        , m_Initialized(false)
    {
    }

    void CreateBoxGeometry(PrimitiveShape& prim);
    void CreateHemisphereGeometry(PrimitiveShape& prim);
    void CreateFlatCylinderEndGeometry(PrimitiveShape& prim);
    void CreateCylinderGeometry(PrimitiveShape& prim);
    void DrawSpherePrimitive(const nlMatrix4& mat_world, float radius,
        const nlColour& colour) const;
    void DrawLine3D(
        const nlVector3& p0,
        const nlVector3& p1,
        const nlColour& colour,
        bool bWithDepth) const;
    void DrawRectangle2D(float x, float y, float w, float h, float z,
        const nlColour& colour, int view) const;
    void DrawSphere(const nlVector3& position, const nlColour& colour,
        float radius) const;
    void Initialize(void* resource);

    /* 0x00 */ void* m_Unknown00;
    /* 0x04 */ bool m_Initialized;
    /* 0x08 */ PrimitiveShape m_Box;
    /* 0x1C */ PrimitiveShape m_Hemisphere;
    /* 0x30 */ PrimitiveShape m_FlatCylinderEnd;
    /* 0x44 */ PrimitiveShape m_Cylinder;
    /* 0x58 */ void* m_pLightUserData;
    /* 0x5C */ GLView* m_eView;
}; // total size: 0x60

extern ShapeRender g_ShapeRenderer;

#endif // _SHAPERENDER_H_
