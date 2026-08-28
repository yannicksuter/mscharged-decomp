#include "Game/Drawable/RenderObject.h"
#include "Game/Drawable/ShadowProp.h"
#include "Game/Physics/PhysicsObject.h"
#include "NL/gl/glState.h"
#include "NL/nlMath.h"
#include "NL/platqmath.h"

// Charged-only shadow prop, second of the run described beside
// DrawableBulletBill. This one is backed by a PhysicsObject rather than a
// plain transform, and scales its shadow by a per-object factor. The live
// object and the material services stay address-named.

struct ThwompObject;

class DrawableThwomp
{
public:
    DrawableThwomp();
    void Grab(const ThwompObject*);
    void Render(ThwompObject*) const;
    void Blend(const float*, const DrawableThwomp&, const DrawableThwomp&);

    /* 0x00 */ nlVector3 mPosition;
    /* 0x0C */ nlQuaternion mOrientation;
    /* 0x1C */ bool mVisible;
    char _01D[3];
};

struct ThwompObjectFields
{
    /* 0x00 */ int mState;
    char _004[4];
    /* 0x08 */ bool mVisible;
    char _009[3];
    /* 0x0C */ PhysicsObject* mPhysics;
    /* 0x10 */ RenderObject* mDrawable;
};

extern "C"
{
    void* fn_8027267C(int);
    const nlVector3* fn_801B327C(const ThwompObject*);
    float fn_801B3364(ThwompObject*);
    void fn_801B339C(ThwompObject*);
}

static float gShadowSizeLow = 1.7f;
static float gShadowSizeHigh = 0.8f;
static int gShadowAlphaLow = 175;
static int gShadowAlphaHigh = 100;
static float gShadowFadeHeight = 25.0f;
static u8 gShadowScalesWithObject = 1;

static void DrawShadow(ThwompObject* object, const nlMatrix4& matrix, void* material)
{
    u8 colour[4];
    nlVector3 extent;
    nlVector3 position;
    nlVector3 transformed;
    glQuad3 quad;

    float fade = matrix.m43 / gShadowFadeHeight;
    if (fade < 0.0f)
    {
        fade = 0.0f;
    }
    if (fade > 1.0f)
    {
        fade = 1.0f;
    }

    float size = (1.0f - fade) * gShadowSizeLow + fade * gShadowSizeHigh;
    float alpha = (1.0f - fade) * gShadowAlphaLow + fade * gShadowAlphaHigh;

    alpha = alpha * fn_801B3364(object);
    if (gShadowScalesWithObject == 1)
    {
        size = size * fn_801B3364(object);
    }

    int value = (int)alpha;
    if (value < 0)
    {
        value = 0;
    }
    if (value > 255)
    {
        value = 255;
    }

    GroundInfo* ground = fn_802772BC();
    float groundHeight = 0.0f;
    if (ground != 0)
    {
        groundHeight = ground->height;
    }

    position.x = matrix.m41;
    position.y = matrix.m42;
    position.z = 0.015625f + groundHeight;
    extent.x = size;
    extent.y = size;
    extent.z = 0.0f;
    nlMultDirVectorMatrix(transformed, extent, matrix);

    extent = transformed;
    colour[0] = 255;
    colour[1] = 255;
    colour[2] = 255;
    colour[3] = (u8)value;

    nlVec3Set(quad.m_pos[0], position.x + extent.y, position.y - extent.x, position.z);
    nlVec3Set(quad.m_pos[1], position.x - extent.x, position.y - extent.y, position.z);
    nlVec3Set(quad.m_pos[2], position.x - extent.y, position.y + extent.x, position.z);
    nlVec3Set(quad.m_pos[3], position.x + extent.x, position.y + extent.y, position.z);

    quad.m_uv[0].x = 1.0f;
    quad.m_uv[0].y = 1.0f;
    quad.m_uv[1].x = 0.0f;
    quad.m_uv[1].y = 1.0f;
    quad.m_uv[2].x = 0.0f;
    quad.m_uv[2].y = 0.0f;
    quad.m_uv[3].x = 1.0f;
    quad.m_uv[3].y = 0.0f;

    *(u32*)&quad.m_colour[3] = *(u32*)colour;
    *(u32*)&quad.m_colour[2] = *(u32*)colour;
    *(u32*)&quad.m_colour[1] = *(u32*)colour;
    *(u32*)&quad.m_colour[0] = *(u32*)colour;

    glSetDefaultState(true);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(glGetTexture("global/thwomp_shadow"), GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());

    if (material == 0)
    {
        material = fn_8027262C();
    }
    quad.Attach((eGLView)(u32)material, 0);
}

DrawableThwomp::DrawableThwomp()
{
    mVisible = false;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mOrientation.z = 0.0f;
    mOrientation.y = 0.0f;
    mOrientation.x = 0.0f;
    mOrientation.w = 1.0f;
}

void DrawableThwomp::Grab(const ThwompObject* object)
{
    nlMatrix4 rotation;

    if (object == 0)
    {
        mVisible = false;
        return;
    }

    mVisible = ((const ThwompObjectFields*)object)->mVisible;
    mPosition = *fn_801B327C(object);
    ((const ThwompObjectFields*)object)->mPhysics->GetRotation(&rotation);
    nlMatrixToQuat(mOrientation, rotation);
}

void DrawableThwomp::Render(ThwompObject* object) const
{
    nlMatrix4 matrix;
    RenderObject* drawable;
    void* material;

    if (object == 0)
    {
        return;
    }
    if (!mVisible)
    {
        return;
    }

    fn_801B339C(object);

    drawable = ((ThwompObjectFields*)object)->mDrawable;
    if (drawable == 0)
    {
        return;
    }

    if (mVisible)
    {
        drawable->m_uObjectFlags |= 1;
    }
    else
    {
        drawable->m_uObjectFlags &= ~1;
    }

    int state = ((ThwompObjectFields*)object)->mState;
    if (state == 1 || state == 0 || state == 6)
    {
        drawable->m_uObjectFlags &= ~1;
    }

    if (!mVisible)
    {
        return;
    }

    nlQuatToMatrix(matrix, mOrientation, true);
    matrix.m41 = mPosition.x;
    matrix.m42 = mPosition.y;
    matrix.m43 = mPosition.z;
    matrix.m44 = 1.0f;

    drawable->V3(&matrix);
    material = fn_8027267C(13);
    drawable->V8(material);

    DrawShadow(object, matrix, material);
}

void DrawableThwomp::Blend(const float* factors, const DrawableThwomp& lhs, const DrawableThwomp& rhs)
{
    bool visible = false;

    if (lhs.mVisible && rhs.mVisible)
    {
        visible = true;
    }

    mVisible = visible;
    if (!visible)
    {
        return;
    }

    float t = factors[2];
    nlQuatNLerp(mOrientation, lhs.mOrientation, rhs.mOrientation, t);
    mPosition.x = (1.0f - t) * lhs.mPosition.x + t * rhs.mPosition.x;
    mPosition.y = (1.0f - t) * lhs.mPosition.y + t * rhs.mPosition.y;
    mPosition.z = (1.0f - t) * lhs.mPosition.z + t * rhs.mPosition.z;
}
