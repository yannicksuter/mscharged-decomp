#include "Game/Drawable/DrawableHammer.h"
#include "Game/Drawable/RenderObject.h"
#include "NL/gl/glModel.h"
#include "NL/nlMath.h"

struct HammerObject;

struct HammerObjectFields
{
    char _000[0x14];
    /* 0x14 */ float mScale;
    char _018[0x0C];
    /* 0x24 */ bool mVisible;
    char _025[7];
    /* 0x2C */ RenderObject* mDrawable;
};

extern "C"
{
    const nlVector3* fn_801A1168(const HammerObject*);
    const nlQuaternion* fn_801A1298(const HammerObject*);
    void fn_801869AC(void*, void*, int, int, const DrawableHammer*, float);
}

u8 lbl_806DCBE8 = 1;

DrawableHammer::DrawableHammer()
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

void DrawableHammer::Grab(const HammerObject* object)
{
    if (object == 0)
    {
        mVisible = false;
        return;
    }

    mVisible = ((const HammerObjectFields*)object)->mVisible;
    mScale = ((const HammerObjectFields*)object)->mScale;
    mPosition = *fn_801A1168(object);
    mOrientation = *fn_801A1298(object);
}

void DrawableHammer::Render(const HammerObject* object) const
{
    nlMatrix4 matrix;
    RenderObject* drawable;

    if (object == 0)
    {
        return;
    }

    drawable = ((const HammerObjectFields*)object)->mDrawable;
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

    if (lbl_806DCBE8 != 0)
    {
        nlMatrix4* source;
        glModel* model;
        model = drawable->m_pModel;
        source = &drawable->GetWorldMatrix();
        glModel* geometry = glModelDupNoStreams(model, false, 0);
        fn_801869AC(geometry, source, 1, 0, this, 0.5f);
    }
}

void DrawableHammer::Blend(const float* factors, const DrawableHammer& lhs, const DrawableHammer& rhs)
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
