#include "Game/Drawable/DrawableCharacter.h"

#include "Game/BasicStadium.h"
#include "Game/BulletBill.h"
#include "Game/Drawable/DrawableBulletBill.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/Drawable/ShadowProp.h"
#include "NL/gl/glState.h"
#include "NL/platqmath.h"

// Charged-only shadow prop. It follows the same snapshot idiom as
// DrawableBall: a compact replay-safe state that is grabbed from the live
// object, blended between snapshots, and rendered through the shared render
// object. The live object and the material/texture services it calls are not
// reconstructed yet and stay address-named.

extern "C"
{
    void* fn_8027267C(int);
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

    GroundInfo* ground =
        reinterpret_cast<GroundInfo*>(BasicStadium::GetCurrentStadium());
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
    glSetCurrentTexture(glGetTexture("global/bulletbillshadow"), GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());
    quad.Attach((eGLView)(u32)fn_8027262C(), 0);
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

    mVisible = object->active;
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

    nlQuatToMatrix(matrix, mOrientation, true);

    if (mScale < 1.0f)
    {
        nlVec3Scale(*(nlVector3*)matrix.e2[0], mScale);
        nlVec3Scale(*(nlVector3*)matrix.e2[1], mScale);
        nlVec3Scale(*(nlVector3*)matrix.e2[2], mScale);
    }

    matrix.SetRow4_(3, mPosition.x, mPosition.y, mPosition.z, 1.0f);

    drawable->SetWorldMatrix(&matrix);
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
    nlQuatNLerp(mOrientation, lhs.mOrientation, rhs.mOrientation, t);
    mPosition.x = (1.0f - t) * lhs.mPosition.x + t * rhs.mPosition.x;
    mPosition.y = (1.0f - t) * lhs.mPosition.y + t * rhs.mPosition.y;
    mPosition.z = (1.0f - t) * lhs.mPosition.z + t * rhs.mPosition.z;
}
