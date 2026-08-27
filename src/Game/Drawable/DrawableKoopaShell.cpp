#include "Game/AI/AiUtil.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/Drawable/ShadowProp.h"
#include "NL/gl/glState.h"
#include "NL/nlMath.h"

// Charged-only shadow prop, third of the run described beside
// DrawableBulletBill. The live object's translation unit spells
// "koopa_shell_trail", and this snapshot spins the prop about Z from a 16-bit
// angle unit. The live object and the material services stay address-named.

struct KoopaShellObject;

struct KoopaShellObjectFields
{
    /* 0x00 */ u16 mSpin;
    char _002[2];
    /* 0x04 */ nlVector3 mPosition;
    char _010[0x10];
    /* 0x20 */ bool mVisible;
    char _021[7];
    /* 0x28 */ RenderObject* mDrawable;
};

class DrawableKoopaShell
{
public:
    DrawableKoopaShell();
    void Grab(const KoopaShellObject*);
    void Render(const KoopaShellObject*) const;
    void Blend(const float*, const DrawableKoopaShell&, const DrawableKoopaShell&);

    /* 0x00 */ u16 mSpin;
    char _002[2];
    /* 0x04 */ nlVector3 mPosition;
    /* 0x10 */ float mScale;
    /* 0x14 */ bool mVisible;
    char _015[3];
};

extern "C"
{
    float fn_8002D194(int);
    float fn_801A65C0(const KoopaShellObject*);
}

static float gShadowScaleIn = 0.125f;
static float gShadowScaleHigh = 0.125f;
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
    int value = (int)((1.0f - fade) * gShadowAlphaLow + fade * gShadowAlphaHigh);
    if (value < 0)
    {
        value = 0;
    }
    if (value > 255)
    {
        value = 255;
    }

    float distance = nlAbs(matrix.m42);
    float edge = fn_8002D194(1);
    if (distance > edge)
    {
        if (distance > 0.5f + edge)
        {
            value = 0;
        }
        else
        {
            value = (int)InterpolateRangeClamped(0.0f, value, 0.5f + edge, edge, distance);
        }
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
    glSetCurrentTexture(glGetTexture("global/ball_shadow"), GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());
    quad.Attach((eGLView)(u32)fn_8027262C(), 0);
}

DrawableKoopaShell::DrawableKoopaShell()
{
    mVisible = false;
    mScale = 1.0f;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mSpin = 0;
}

void DrawableKoopaShell::Grab(const KoopaShellObject* object)
{
    if (object == 0)
    {
        mVisible = false;
        return;
    }

    mVisible = ((const KoopaShellObjectFields*)object)->mVisible;
    if (!mVisible)
    {
        return;
    }

    mPosition = ((const KoopaShellObjectFields*)object)->mPosition;
    mSpin = ((const KoopaShellObjectFields*)object)->mSpin;
    mScale = fn_801A65C0(object);
}

void DrawableKoopaShell::Render(const KoopaShellObject* object) const
{
    nlMatrix4 matrix;
    RenderObject* drawable;

    if (object == 0)
    {
        return;
    }

    drawable = ((const KoopaShellObjectFields*)object)->mDrawable;
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

    nlMakeRotationMatrixZ(matrix, AngUnitsToRad_fromUnsignedShort(mSpin));

    if (1.0f != mScale)
    {
        nlVec3Scale(*(nlVector3*)matrix.e2[0], mScale);
        nlVec3Scale(*(nlVector3*)matrix.e2[1], mScale);
        nlVec3Scale(*(nlVector3*)matrix.e2[2], mScale);
    }

    matrix.m41 = mPosition.x;
    matrix.m42 = mPosition.y;
    matrix.m43 = mPosition.z;
    matrix.m44 = 1.0f;

    drawable->V3(&matrix);
    drawable->Draw();

    DrawShadow(matrix, mScale * gShadowScaleIn);
}

void DrawableKoopaShell::Blend(const float* factors, const DrawableKoopaShell& lhs, const DrawableKoopaShell& rhs)
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
    mPosition.x = (1.0f - t) * lhs.mPosition.x + t * rhs.mPosition.x;
    mPosition.y = (1.0f - t) * lhs.mPosition.y + t * rhs.mPosition.y;
    int offset = (short)(t * (short)(rhs.mSpin - lhs.mSpin));
    mSpin = lhs.mSpin + offset;
    mPosition.z = (1.0f - t) * lhs.mPosition.z + t * rhs.mPosition.z;
}
