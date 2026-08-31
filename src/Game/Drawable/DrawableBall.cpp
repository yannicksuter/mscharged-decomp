#include "Game/Drawable/DrawableBall.h"

#include "Game/BallTrail.h"
#include "Game/CharacterTemplate.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/RenderSnapshot.h"

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

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

struct CharacterState
{
    char _000[0x24];
    int type;
};

struct BallObject
{
    u8 visible;
    char _001[0x53];
    nlVector3 position;
    char _060[0x0C];
    nlVector3 velocity;
    char _078[0x0C];
    nlQuaternion orientation;
    char _094[0x34];
    void* owner;
    void* previousOwner;
    void* lastTouch;
    void* passTarget;
    char _0D8[0x0C];
    RenderObject* drawable;
};

struct TaskManager
{
    char _000[8];
    u32 flags;
};

struct BallScaleData
{
    char _000[0xD8];
    float scale;
};

extern BallObject* g_pBall;
extern TaskManager* m_pInstance__13nlTaskManager;
extern "C" float fn_800155A0(BallObject*, int);
extern "C" LiveBallTrail* fn_8001B284(u32);
extern "C" u32 fn_8001B30C(BallObject*);
extern "C" BallScaleData* fn_80284A58(u32);

static float g_fBallTrailScale = 2.25f;

DrawableCharacter* DrawableBall::IndexToPlayer(int index) const
{
    if (index < 0 || index >= 10)
    {
        return 0;
    }
    return &mRenderSnapshot->mCharacters[index];
}

DrawableBall::DrawableBall(RenderSnapshot* renderSnapshot)
    : mRenderSnapshot(renderSnapshot)
    , mFlags(0)
    , mScale(0.0f)
    , mTrailCount(0)
{
    mFlags.value |= 0x80000000;
}

void DrawableBall::Grab()
{
    mOrientation = g_pBall->orientation;
    mPosition = g_pBall->position;
    mVelocity = g_pBall->velocity;
    mScale = fn_800155A0(g_pBall, 0);

    mFlags.bits.ownerIndex
        = GetCharacterIndex((cCharacter*)g_pBall->owner);
    mFlags.bits.previousOwnerIndex
        = GetCharacterIndex((cCharacter*)g_pBall->previousOwner);
    mFlags.bits.passTargetIndex
        = GetCharacterIndex((cCharacter*)g_pBall->passTarget);
    mFlags.bits.lastTouchIndex
        = GetCharacterIndex((cCharacter*)g_pBall->lastTouch);
    BallObject* ball = g_pBall;
    mFlags.bits.visible = ball->visible;
    mFlags.bits.transient = 0;

    mTrailCount = fn_8001B30C(ball);
    for (u32 i = 0; i < mTrailCount; ++i)
    {
        LiveBallTrail* trail = fn_8001B284(i);
        mTrail[i].visible = trail->visible;
        mTrail[i].position = trail->position;
        mTrail[i].orientation = trail->orientation;
    }
}

void DrawableBall::Render() const
{
    RenderObject* drawable = g_pBall->drawable;
    if (mFlags.bits.visible)
    {
        drawable->m_uObjectFlags |= 1;
    }
    else
    {
        drawable->m_uObjectFlags &= ~1;
    }

    if (mFlags.bits.visible)
    {
        drawable->orientation = mOrientation;
        drawable->worldMatrixUpToDate = false;
        drawable->translation = mPosition;
        drawable->worldMatrixUpToDate = false;

        if ((m_pInstance__13nlTaskManager->flags & 0x20018) == 0)
        {
            if (g_pBall->owner == 0)
            {
                drawable->modelScale = 1.5f;
            }
            else
            {
                drawable->modelScale = 1.25f;
            }
        }
        else
        {
            drawable->modelScale = 1.0f;
        }

        drawable->snapshotScale = mScale;
        drawable->renderFlags |= 2;

        const int ownerIndex = mFlags.bits.ownerIndex;
        bool useDefaultRendering = true;
        if (IndexToPlayer(ownerIndex) != 0)
        {
            if (((CharacterState*)IndexToPlayer(ownerIndex)->character)->type == 12)
            {
                useDefaultRendering = false;
            }
        }

        if (useDefaultRendering)
        {
            drawable->renderFlags |= 4;
        }
        else
        {
            drawable->renderFlags &= ~4;
        }
        drawable->Draw();
    }

    for (u32 i = 0; i < mTrailCount; ++i)
    {
        const float scale = mScale;
        RenderObject* trail = fn_8001B284(i)->drawable;

        if (mTrail[i].visible)
        {
            trail->m_uObjectFlags |= 1;
        }
        else
        {
            trail->m_uObjectFlags &= ~1;
        }

        if (mTrail[i].visible)
        {
            trail->snapshotScale = scale;
            trail->translation = mTrail[i].position;
            trail->worldMatrixUpToDate = false;
            trail->orientation = mTrail[i].orientation;
            trail->worldMatrixUpToDate = false;
            trail->modelScale = g_fBallTrailScale;
            trail->renderFlags &= ~6;
            trail->Draw();
        }
    }
}

void DrawableBall::Blend(
    const float* blendFactors, const DrawableBall& lhs, const DrawableBall& rhs)
{
    const float factor = *blendFactors;

    mPrevOrientation = mOrientation;
    nlQuatNLerp(mOrientation, lhs.mOrientation, rhs.mOrientation, factor);

    mPosition.x = (1.0f - factor) * lhs.mPosition.x + factor * rhs.mPosition.x;
    mPosition.y = (1.0f - factor) * lhs.mPosition.y + factor * rhs.mPosition.y;
    mPosition.z = (1.0f - factor) * lhs.mPosition.z + factor * rhs.mPosition.z;
    mVelocity.x = (1.0f - factor) * lhs.mVelocity.x + factor * rhs.mVelocity.x;
    mVelocity.y = (1.0f - factor) * lhs.mVelocity.y + factor * rhs.mVelocity.y;
    mVelocity.z = (1.0f - factor) * lhs.mVelocity.z + factor * rhs.mVelocity.z;
    mScale = lhs.mScale * (1.0f - factor) + factor * rhs.mScale;

    if (factor < 0.5f)
    {
        mFlags.value = lhs.mFlags.value;
    }
    else
    {
        mFlags.value = rhs.mFlags.value;
    }

    mTrailCount = lhs.mTrailCount <= rhs.mTrailCount ? lhs.mTrailCount : rhs.mTrailCount;
    for (u32 i = 0; i < mTrailCount; ++i)
    {
        mTrail[i].visible = lhs.mTrail[i].visible && rhs.mTrail[i].visible;
        if (mTrail[i].visible)
        {
            mTrail[i].position.x =
                (1.0f - factor) * lhs.mTrail[i].position.x + factor * rhs.mTrail[i].position.x;
            mTrail[i].position.y =
                (1.0f - factor) * lhs.mTrail[i].position.y + factor * rhs.mTrail[i].position.y;
            mTrail[i].position.z =
                (1.0f - factor) * lhs.mTrail[i].position.z + factor * rhs.mTrail[i].position.z;
            nlQuatNLerp(
                mTrail[i].orientation, lhs.mTrail[i].orientation, rhs.mTrail[i].orientation, factor);
        }
    }
}

void DrawableBall::EvaluateFrom(DrawableCharacter& character)
{
    mPosition = character.GetBallPosition();
    mOrientation = character.GetBallOrientation();
    mScale = fn_80284A58(mOrientation.as_u32[2])->scale;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
