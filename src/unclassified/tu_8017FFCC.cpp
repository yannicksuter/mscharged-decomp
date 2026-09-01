#include "Game/Drawable/Drawable_8017FFCC.h"
#include "Game/Drawable/RenderObject.h"
#include "NL/gl/glView.h"
#include "NL/nlMath.h"
#include "NL/platvmath.h"

struct RenderEntry_8018006C
{
    char _000[0x24];
    RenderObject* object;
};

extern "C"
{
    GLView* fn_8027267C(int index);
}

Drawable_8017FFCC::Drawable_8017FFCC()
{
    mVisible = false;
    mScale = 1.0f;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mOrientation = 0;
}

void Drawable_8017FFCC::Grab(const Object_8017FFF4* object)
{
    if (object == 0)
    {
        mVisible = false;
        return;
    }

    mVisible = object->visible;
    if (mVisible)
    {
        mPosition = object->position;
        mOrientation = object->orientation;
        mScale = object->fn_8019DA04();
    }
}

void Drawable_8017FFCC::Render(const RenderEntry_8018006C* entry) const
{
    nlMatrix4 matrix;
    RenderObject* object = entry->object;
    if (object == 0)
    {
        return;
    }

    if (mVisible)
    {
        object->m_uObjectFlags |= 1;
    }
    else
    {
        object->m_uObjectFlags &= ~1;
    }

    if (!mVisible)
    {
        return;
    }

    nlMakeRotationMatrixZ(matrix,
        AngUnitsToRad_fromUnsignedShort(mOrientation));
    nlVec3Scale(*(nlVector3*)matrix.e2[0], mScale);
    nlVec3Scale(*(nlVector3*)matrix.e2[1], mScale);
    nlVec3Scale(*(nlVector3*)matrix.e2[2], mScale);
    matrix.m41 = mPosition.x;
    matrix.m42 = mPosition.y;
    matrix.m43 = mPosition.z;
    matrix.m44 = 1.0f;

    UnidentifiedDrawableViewState* state = object->mUnidentified10;
    GLView* oldView68 = state->mView68;
    GLView* oldView6C = state->mView6C;
    state->mView68 = fn_8027267C(29);
    state->mView6C = state->mView68;
    object->SetWorldMatrix(&matrix);
    object->Draw();
    state->mView68 = oldView68;
    state->mView6C = oldView6C;
}

void Drawable_8017FFCC::Blend(const float* factors,
    const Drawable_8017FFCC& lhs, const Drawable_8017FFCC& rhs)
{
    mVisible = lhs.mVisible && rhs.mVisible;
    if (!mVisible)
    {
        return;
    }

    float factor = factors[2];
    mScale = (1.0f - factor) * lhs.mScale + factor * rhs.mScale;
    int orientationDelta = (int)(factor * (s16)(rhs.mOrientation - lhs.mOrientation));
    mOrientation = lhs.mOrientation + (s16)orientationDelta;
    nlVecLerp(mPosition, lhs.mPosition, rhs.mPosition, factor);
}
