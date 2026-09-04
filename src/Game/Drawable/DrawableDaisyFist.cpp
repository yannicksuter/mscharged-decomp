#include "Game/Drawable/DrawableDaisyFist.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/Render/RLView.h"
#include "NL/gl/glView.h"
#include "NL/nlMath.h"
#include "NL/platvmath.h"
#include "unclassified/tu_8019D6B4.h"

// Charged-only render snapshot of one Daisy crystal fist. Like the other
// gameplay-object snapshots it spins the prop about Z from a 16-bit angle unit,
// but it draws through a dedicated view instead of the default one.

extern "C"
{
}

DrawableDaisyFist::DrawableDaisyFist()
{
    mVisible = false;
    mScale = 1.0f;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mOrientation = 0;
}

void DrawableDaisyFist::Grab(const DaisyFistObject* object)
{
    if (object == 0)
    {
        mVisible = false;
        return;
    }

    mVisible = object->mVisible;
    if (mVisible)
    {
        mPosition = object->mPosition;
        mOrientation = object->mOrientation;
        mScale = object->GetScale();
    }
}

void DrawableDaisyFist::Render(const DaisyFistObject* object) const
{
    nlMatrix4 matrix;
    RenderObject* drawable = object->mDrawable;
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

    nlMakeRotationMatrixZ(matrix,
        AngUnitsToRad_fromUnsignedShort(mOrientation));
    nlVec3Scale(*(nlVector3*)matrix.e2[0], mScale);
    nlVec3Scale(*(nlVector3*)matrix.e2[1], mScale);
    nlVec3Scale(*(nlVector3*)matrix.e2[2], mScale);
    matrix.m41 = mPosition.x;
    matrix.m42 = mPosition.y;
    matrix.m43 = mPosition.z;
    matrix.m44 = 1.0f;

    UnidentifiedDrawableViewState* state = drawable->mUnidentified10;
    GLView* oldView68 = state->mView68;
    GLView* oldView6C = state->mView6C;
    state->mView68 = GetLayerView(eCLV_ElectricFence);
    state->mView6C = state->mView68;
    drawable->SetWorldMatrix(&matrix);
    drawable->Draw();
    state->mView68 = oldView68;
    state->mView6C = oldView6C;
}

void DrawableDaisyFist::Blend(const float* factors,
    const DrawableDaisyFist& lhs, const DrawableDaisyFist& rhs)
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
