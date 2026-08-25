#include "Game/Drawable/DrawableCharacter.h"

#include "Game/Drawable/RenderObject.h"
#include "Game/Drawable/ShadowProp.h"
#include "NL/gl/glState.h"
#include "NL/platqmath.h"

// Charged-only shadow prop. It follows the same snapshot idiom as
// DrawableBall: a compact replay-safe state that is grabbed from the live
// object, blended between snapshots, and rendered through the shared render
// object. The live object and the material/texture services it calls are not
// reconstructed yet and stay address-named.

struct BulletBillObject
{
    /* 0x00 */ nlQuaternion orientation;
    /* 0x10 */ nlVector3 position;
    char _01C[0x10];
    /* 0x2C */ float scale;
    char _030[8];
    /* 0x38 */ bool visible;
    char _039[7];
    /* 0x40 */ RenderObject* drawable;
};

class DrawableBulletBill
{
public:
    DrawableBulletBill();
    void Grab(const BulletBillObject*);
    void Render(const BulletBillObject*) const;
    void Blend(const float*, const DrawableBulletBill&, const DrawableBulletBill&);

    /* 0x00 */ nlQuaternion mOrientation;
    /* 0x10 */ nlVector3 mPosition;
    /* 0x1C */ float mScale;
    /* 0x20 */ bool mVisible;
    char _021[3];
};

extern "C" {
void* fn_8027267C(int);
void fn_80368374(nlMatrix4*, const DrawableBulletBill*, int);
void fn_802B5544(nlQuaternion&, const nlQuaternion&, const nlQuaternion&, float);
}

static float gShadowAlphaScale = 0.4f;
static float gShadowScaleHigh = 0.5f;
static int gShadowAlphaLow = 130;
static int gShadowAlphaHigh = 10;
static float gShadowFadeHeight = 10.0f;

static void DrawShadow(const nlMatrix4& matrix, float scale)
{
    u8 colour[4];
    nlVector3 extent;
    nlVector3 position;
    nlVector3 transformed;
    ShadowQuad quad;

    float fade = matrix.m43 / gShadowFadeHeight;
    if (fade < 0.0f)
    {
        fade = 0.0f;
    }
    if (fade > 1.0f)
    {
        fade = 1.0f;
    }

    float size = (1.0f - fade) * (1.75 * scale) + fade * (gShadowScaleHigh * scale);
    int alpha = (int)((1.0f - fade) * gShadowAlphaLow + fade * gShadowAlphaHigh);
    if (alpha < 0)
    {
        alpha = 0;
    }
    if (alpha > 255)
    {
        alpha = 255;
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
    colour[3] = (u8)alpha;

    nlVec3Set(quad.corners[0], position.x + extent.y, position.y - extent.x, position.z);
    nlVec3Set(quad.corners[1], position.x - extent.x, position.y - extent.y, position.z);
    nlVec3Set(quad.corners[2], position.x - extent.y, position.y + extent.x, position.z);
    nlVec3Set(quad.corners[3], position.x + extent.x, position.y + extent.y, position.z);

    quad.uv[0][0] = 1.0f;
    quad.uv[0][1] = 1.0f;
    quad.uv[1][0] = 0.0f;
    quad.uv[1][1] = 1.0f;
    quad.uv[2][0] = 0.0f;
    quad.uv[2][1] = 0.0f;
    quad.uv[3][0] = 1.0f;
    quad.uv[3][1] = 0.0f;

    quad.colors[3] = *(u32*)colour;
    quad.colors[2] = *(u32*)colour;
    quad.colors[1] = *(u32*)colour;
    quad.colors[0] = *(u32*)colour;

    glSetDefaultState(true);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(glGetTexture("global/bulletbillshadow"), GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());
    fn_802C9664(&quad, fn_8027262C(), 0);
}

DrawableBulletBill::DrawableBulletBill()
{
    mVisible = false;
    mScale = 1.0f;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mOrientation.z = 0.0f;
    mOrientation.y = 0.0f;
    mOrientation.x = 0.0f;
    mOrientation.w = 1.0f;
}

void DrawableBulletBill::Grab(const BulletBillObject* object)
{
    if (object == 0)
    {
        mVisible = false;
        return;
    }

    mVisible = object->visible;
    if (!mVisible)
    {
        return;
    }

    mPosition = object->position;
    mOrientation = object->orientation;
    mScale = object->scale;
}

void DrawableBulletBill::Render(const BulletBillObject* object) const
{
    RenderObject* drawable = object->drawable;
    nlMatrix4 matrix;

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

    if (!mVisible)
    {
        return;
    }

    fn_80368374(&matrix, this, 1);

    if (mScale < 1.0f)
    {
        nlVec3Scale(*(nlVector3*)matrix.e2[0], mScale);
        nlVec3Scale(*(nlVector3*)matrix.e2[1], mScale);
        nlVec3Scale(*(nlVector3*)matrix.e2[2], mScale);
    }

    matrix.SetRow4_(3, mPosition.x, mPosition.y, mPosition.z, 1.0f);

    drawable->V3(&matrix);
    drawable->V8(fn_8027267C(13));

    DrawShadow(matrix, gShadowAlphaScale);
}

void DrawableBulletBill::Blend(const float* factors, const DrawableBulletBill& lhs, const DrawableBulletBill& rhs)
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
    mScale = (1.0f - t) * lhs.mScale + t * rhs.mScale;
    fn_802B5544(mOrientation, lhs.mOrientation, rhs.mOrientation, t);
    mPosition.x = (1.0f - t) * lhs.mPosition.x + t * rhs.mPosition.x;
    mPosition.y = (1.0f - t) * lhs.mPosition.y + t * rhs.mPosition.y;
    mPosition.z = (1.0f - t) * lhs.mPosition.z + t * rhs.mPosition.z;
}
