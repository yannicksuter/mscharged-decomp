#include <math.h>
#include "Game/Render/YoshiEggObject.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsYoshiEgg.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/ReplayManager.h"
#include "Game/Sys/audio.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

extern "C"
{
    extern const nlVector3 gYoshiEggZeroDisplacement;
    extern const nlVector3 gYoshiEggHiddenPosition;
    float gYoshiEggRollScale = 1.0f;
    float gYoshiEggStartRadius = 0.5f;
    float gYoshiEggExpandedRadius = 1.4f;
    float gYoshiEggHeightOffset;

    void fn_802B5370(nlQuaternion&, const nlVector3&, unsigned short);
}

YoshiEggObject::YoshiEggObject(RenderObject* drawable)
{
    mDrawable = drawable;
    mRadius = 1.0f;
    mTargetRadius = 1.0f;
    mRadiusTime = 0.0f;
    mActive = false;
    mFielder = 0;
    mDelay = 0.0f;
    mDisplacementDirection = gYoshiEggZeroDisplacement;
    mDisplacementDistance = 0.0f;
    mPosition = gYoshiEggHiddenPosition;
    mOrientation.x = mOrientation.y = mOrientation.z = 0.0f;
    mOrientation.w = 1.0f;
    mPhysics = new (8, false) PhysicsYoshiEgg(this, 1.0f);
    mPhysics->SetPosition(gYoshiEggHiddenPosition, PhysicsObject::WORLD_COORDINATES);
    mPhysics->DisableCollisions();
}

YoshiEggObject::~YoshiEggObject()
{
    delete mPhysics;
}

void YoshiEggObject::Suspend(bool, float delay)
{
    if (mActive)
    {
        mDelay = delay;
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
        EmissionManager::Instance()->Destroy((unsigned long)this, group);
        mPhysics->EnableCollisions();
        if (mFielder != 0 && mFielder->m_pBall != 0)
        {
            mFielder->ReleaseBall(0);
            g_pBall->m_pPhysicsBall->EnableCollisions();
        }
    }
}

void YoshiEggObject::Update(float dt)
{
    if (!mActive)
        return;

    if (mDelay > 0.0f)
    {
        mDelay -= dt;
        if (mDelay <= 0.0f)
        {
            EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
            EmissionController* controller = EmissionManager::Instance()->Create(group, 0, true, 0);
            controller->SetPosition(mPosition);
            controller->m_uUserData = (unsigned long)this;
            controller->SetUpdateCallback(UpdateTrailEffect);
            mPhysics->EnableCollisions();
        }
        else
        {
            return;
        }
    }

    if (mRadiusTime > 0.0f)
    {
        mRadiusTime -= dt;
        if (mRadiusTime <= 0.0f)
        {
            mRadiusTime = 0.0f;
            mRadius = mTargetRadius;
        }
        else
        {
            float factor = dt / mRadiusTime;
            if (factor > 1.0f)
                factor = 1.0f;
            mRadius = Interpolate(mRadius, mTargetRadius, factor);
        }
        mPhysics->SetRadius(mRadius);
    }

    if (mDisplacementDistance != 0.0f)
    {
        nlVector3 position = mFielder->mUnidentified024.m_v3Position;
        nlVector3 offset;
        nlVec3Scale(offset, mDisplacementDirection, mDisplacementDistance);
        if (nlAbs(offset.x) <= 0.01f && nlAbs(offset.y) <= 0.01f)
            offset.y = -0.01f;
        position.y += offset.y;
        position.x += offset.x;
        position.z = 0.0f;
        mFielder->SetPosition(position);
        mDisplacementDistance = 0.0f;
    }

    UpdateTransform(dt);
    if (fabsf(mPosition.y) > cField::GetSidelineY(1))
    {
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
        EmissionManager::Instance()->Kill((unsigned long)this, group);
    }
}

void YoshiEggObject::UpdateTrailEffect(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
        return;
    if (controller.m_Replaying == 0 && ReplayManager::Instance()->mRender != 0)
    {
        RenderSnapshot* snapshot = ReplayManager::Instance()->mRender;
        if (snapshot->_1BA0.mVisible)
            controller.SetPosition(snapshot->_1BA0.mPosition);
    }
}

void YoshiEggObject::Activate(cFielder* fielder)
{
    mFielder = fielder;
    if (fielder != 0)
        fn_802B549C(mOrientation, fielder->mUnidentified024.m_aActualFacingDirection);
    mTargetRadius = gYoshiEggStartRadius;
    mRadiusTime = 0.0f;
    mRadius = mTargetRadius;
    mPhysics->SetRadius(mRadius);
    mTargetRadius = gYoshiEggExpandedRadius;
    mRadiusTime = 0.2f;
    if (mActive)
    {
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
        EmissionManager::Instance()->Kill((unsigned long)this, group);
    }
    mActive = true;
    mPhysics->EnableCollisions();
    UpdateTransform(0.0f);
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
    EmissionController* controller = EmissionManager::Instance()->Create(group, 0, true, 0);
    controller->SetPosition(mPosition);
    controller->m_uUserData = (unsigned long)this;
    controller->SetUpdateCallback(UpdateTrailEffect);
    if (fielder->m_pBall != 0)
    {
        g_pBall->m_pPhysicsBall->DisableCollisions();
        g_pBall->m_bVisible = false;
        fielder->m_pPhysicsCharacter->DisablePhysicsColumn();
    }
}

void YoshiEggObject::Deactivate(bool destroyEffects)
{
    if (mActive)
    {
        if (mFielder == g_pBall->m_pOwner)
        {
            g_pBall->m_pPhysicsBall->EnableCollisions();
            g_pBall->m_bVisible = true;
            mFielder->m_pPhysicsCharacter->EnablePhysicsColumn();
        }
        if (!destroyEffects)
        {
            mFielder->IsFallenDown();
            PlaySound(mFielder->mUnidentified318, 0x1B274A7A, 0, 0);
        }
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
        if (!destroyEffects)
            EmissionManager::Instance()->Kill((unsigned long)this, group);
        else
            EmissionManager::Instance()->Destroy((unsigned long)this, group);
        mPosition = gYoshiEggHiddenPosition;
        mPhysics->SetPosition(gYoshiEggHiddenPosition, PhysicsObject::WORLD_COORDINATES);
        if (mActive)
        {
            EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("yoshi_egg_trail");
            EmissionManager::Instance()->Kill((unsigned long)this, group);
        }
        mActive = false;
    }
    mPhysics->DisableCollisions();
    mRadiusTime = 0.0f;
    mDelay = 0.0f;
}

float YoshiEggObject::GetRadius() const
{
    return mRadius;
}

void YoshiEggObject::UpdateTransform(float dt)
{
    cFielder* player = mFielder;
    nlVector3 position = player->mUnidentified024.m_v3Position;
    position.z += mRadius + gYoshiEggHeightOffset;
    nlVector3 foot = player->GetJointPosition(player->m_nLeftFootJointIndex);
    nlVector3 head = player->GetJointPosition(player->m_nHeadJointIndex);
    float height = 0.5f * (foot.z + head.z);
    if (height > position.z)
        position.z = height;
    mPosition = position;
    mPhysics->SetPosition(position, PhysicsObject::WORLD_COORDINATES);
    if (dt > 0.0f)
    {
        nlQuaternion rotation;
        nlVector3 axis;
        nlVector3 displacement;
        float sine, cosine;
        nlSinCos(&sine, &cosine, player->mUnidentified024.m_aActualFacingDirection + 0x4000);
        axis.x = cosine;
        axis.y = sine;
        axis.z = 0.0f;
        nlVec3Scale(displacement, player->mUnidentified024.m_v3Velocity, dt);
        float length = nlVec3Length(displacement);
        float angle = length * gYoshiEggRollScale / mRadius;
        fn_802B5370(rotation, axis, (unsigned short)(int)(10430.378f * angle));
        nlMultQuat(mOrientation, rotation, mOrientation);
    }
    else
    {
        mOrientation.x = mOrientation.y = mOrientation.z = 0.0f;
        mOrientation.w = 1.0f;
    }
}

void YoshiEggObject::Reset()
{
    Deactivate(true);
    mPosition = gYoshiEggHiddenPosition;
    mOrientation.x = mOrientation.y = mOrientation.z = 0.0f;
    mOrientation.w = 1.0f;
    mRadius = 1.0f;
    mTargetRadius = 1.0f;
    mRadiusTime = 0.0f;
    mFielder = 0;
    mDelay = 0.0f;
    mPhysics->SetRadius(0.1f);
    mPhysics->SetPosition(mPosition, PhysicsObject::WORLD_COORDINATES);
    mDisplacementDistance = 0.0f;
    mDisplacementDirection = gYoshiEggZeroDisplacement;
}

void YoshiEggObject::Break()
{
    Deactivate(false);
}

void YoshiEggObject::SetPendingDisplacement(const nlVector3& direction, float distance)
{
    mDisplacementDirection = direction;
    mDisplacementDistance = distance;
}
