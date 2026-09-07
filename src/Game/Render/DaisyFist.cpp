#include "Game/Render/DaisyFist.h"
#include "unclassified/tu_80175F8C.h"
#include "Game/Drawable/RenderObject.h"

#include "Game/AI/AiUtil.h"
#include "Game/Character.h"

// Render object table type 6 is "art/objects/gameplay/daisy_fist".

static float sSpawnScale = 0.5f;
static float sFullScale = 1.25f;
static float sBaseScale = 0.22f;
static float sImpactHeightOffset = 2.5f;
static float sForwardOffset = 3.2f;
static float sHiddenHeight = -3.5f;
static float sRaisedHeight = -0.6f;
static float sImpactHeight = -2.0f;
static float sRiseTime = 0.3f;
static float sScaleTime = 0.15f;

static const nlVector3 sHiddenPosition = { 0.0f, -20.0f, -18.0f };

DaisyFistObject::DaisyFistObject(int index)
{
    mScale = sBaseScale;
    mTargetScale = sBaseScale;
    mScaleTimer = 0.0f;
    mRiseTimer = 0.0f;
    mVisible = false;
    mOwner = 0;
    mDelayTimer = 0.0f;
    mPosition = sHiddenPosition;
    mOrientation = 0;
    mDrawable = GetRenderObject(6, index);
}

DaisyFistObject::~DaisyFistObject()
{
}

void DaisyFistObject::Update(float dt)
{
    if (!mVisible)
    {
        return;
    }

    if (mDelayTimer > 0.0f)
    {
        mDelayTimer -= dt;
        if (!(mDelayTimer <= 0.0f))
        {
            return;
        }
    }

    if (mRiseTimer > 0.0f)
    {
        float oldZ = mPosition.z;
        mRiseTimer -= dt;
        mPosition.z += dt * (sRaisedHeight - sHiddenHeight) / sRiseTime;
        if (oldZ < sImpactHeight && mPosition.z >= sImpactHeight)
        {
            nlVector3 impactPosition = mPosition;
            impactPosition.z += sImpactHeightOffset;
            CreateDaisyFistImpact(&impactPosition, mOwner);
        }
    }
    else if (mPosition.z > sHiddenHeight)
    {
        mPosition.z -= 0.5f * dt * (sRaisedHeight - sHiddenHeight) / sRiseTime;
    }
    else
    {
        if (mVisible)
        {
            mVisible = false;
            mPosition = sHiddenPosition;
        }
        mScaleTimer = 0.0f;
        mRiseTimer = 0.0f;
        mDelayTimer = 0.0f;
    }

    if (mScaleTimer > 0.0f)
    {
        mScaleTimer -= dt;
        if (mScaleTimer <= 0.0f)
        {
            mScaleTimer = 0.0f;
            mScale = mTargetScale;
            return;
        }

        float percent = dt / mScaleTimer;
        if (percent > 1.0f)
        {
            percent = 1.0f;
        }
        mScale = Interpolate(mScale, mTargetScale, percent);
    }
}

void DaisyFistObject::Spawn(cCharacter* owner, u16 orientation)
{
    mOwner = owner;

    float targetScale = sSpawnScale;
    mTargetScale = targetScale;
    mScaleTimer = 0.0f;
    mScale = targetScale;

    targetScale = sFullScale;
    float scaleTime = sScaleTime;
    mTargetScale = targetScale;
    mScaleTimer = scaleTime;
    if (scaleTime <= 0.0f)
    {
        mScale = targetScale;
    }

    mRiseTimer = sRiseTime;
    mOrientation = orientation;

    nlVector3 local = { sForwardOffset, 0.0f, sHiddenHeight };
    nlVector3 world;
    GetWorldPoint(world, local, owner->m_v3Position, orientation);
    mPosition = world;
    mVisible = true;
}

float DaisyFistObject::GetScale() const
{
    return mScale / sBaseScale;
}

void DaisyFistObject::Reset()
{
    if (mVisible)
    {
        mVisible = false;
        mPosition = sHiddenPosition;
    }

    mScaleTimer = 0.0f;
    mRiseTimer = 0.0f;
    mDelayTimer = 0.0f;
    mPosition = sHiddenPosition;
    mOrientation = 0;
    mScale = 1.0f;
    mTargetScale = 1.0f;
    mScaleTimer = 0.0f;
    mOwner = 0;
    mDelayTimer = 0.0f;
    mRiseTimer = 0.0f;
}
