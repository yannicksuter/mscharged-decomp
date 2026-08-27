#include "Game/Drawable/DrawableCharacter.h"

#include "Game/Drawable/RenderObject.h"
#include "Game/Drawable/ShadowProp.h"
#include "NL/gl/glState.h"
#include "NL/nlString.h"
#include "NL/platqmath.h"

// Charged-only shadow prop, sixth of the run described beside
// DrawableBulletBill. This one carries the index of its live camera instead of
// a pointer, and owns a lowercase hash of the flying-camera model name. The
// live object and the render-object lookup stay address-named.

// The original type identity of this common weak static is not yet known.
struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

struct FlyingCameraObject
{
    /* 0x00 */ nlQuaternion orientation;
    /* 0x10 */ nlVector3 position;
    char _01C[0x3E];
    /* 0x5A */ bool visible;
};

class DrawableFlyingCamera
{
public:
    DrawableFlyingCamera();
    void Grab();
    void Render() const;
    void Blend(const float*, const DrawableFlyingCamera&, const DrawableFlyingCamera&);

    /* 0x00 */ nlQuaternion mOrientation;
    /* 0x10 */ nlVector3 mPosition;
    /* 0x1C */ float mScale;
    /* 0x20 */ int mIndex;
    /* 0x24 */ bool mVisible;
    char _025[3];
};

extern "C"
{
    void* fn_8027267C(int);
    FlyingCameraObject* fn_801A0C44(int);
    RenderObject* fn_80276360(int, int);
    void fn_802B5544(nlQuaternion&, const nlQuaternion&, const nlQuaternion&, float);
}

static float gShadowScaleHigh = 0.5f;
static int gShadowAlphaLow = 100;
static int gShadowAlphaHigh = 10;
static float gShadowFadeHeight = 11.0f;
static float gShadowScaleIn = 0.7f;

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
    glSetCurrentTexture(glGetTexture("global/camera_shadow"), GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());
    quad.Attach((eGLView)(u32)fn_8027262C(), 0);
}

DrawableFlyingCamera::DrawableFlyingCamera()
{
    mIndex = 0;
    mScale = 1.0f;
    mVisible = false;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mOrientation.z = 0.0f;
    mOrientation.y = 0.0f;
    mOrientation.x = 0.0f;
    mOrientation.w = 1.0f;
}

void DrawableFlyingCamera::Grab()
{
    mVisible = fn_801A0C44(mIndex)->visible;
    mPosition = fn_801A0C44(mIndex)->position;
    mOrientation = fn_801A0C44(mIndex)->orientation;
}

void DrawableFlyingCamera::Render() const
{
    nlMatrix4 matrix;
    RenderObject* drawable = fn_80276360(7, mIndex);

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

    nlVec3Scale(*(nlVector3*)matrix.e2[0], mScale);
    nlVec3Scale(*(nlVector3*)matrix.e2[1], mScale);
    nlVec3Scale(*(nlVector3*)matrix.e2[2], mScale);

    matrix.m41 = mPosition.x;
    matrix.m42 = mPosition.y;
    matrix.m43 = mPosition.z;
    matrix.m44 = 1.0f;

    drawable->V3(&matrix);
    drawable->Draw();

    DrawShadow(matrix, gShadowScaleIn);
}

void DrawableFlyingCamera::Blend(const float* factors, const DrawableFlyingCamera& lhs, const DrawableFlyingCamera& rhs)
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

static u32 gFlyingCameraNameHash = nlStringLowerHash("gameplay/flyingcamera3");

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
