#include <math.h>
#include "Game/Render/KoopaShellObject.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Drawable/DrawableKoopaShell.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsKoopaShell.h"
#include "Game/Physics/PhysicsSphere.h"
#include "Game/ReplayManager.h"
#include "Game/RenderSnapshot.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

extern "C"
{
    void fn_801BC6E4(cFielder*);
    void fn_801BC96C(const nlVector3*);
}

static const nlVector3 sInitialVelocity = { 0.0f, 0.0f, 0.0f };
static const nlVector3 sHiddenPosition = { 0.0f, 20.0f, -10.0f };

static char sKoopaShellTrail[] = "koopa_shell_trail";

static float sActiveTime = 1.1f;
static unsigned int sSpinStep = 3500;
static float sInitialRadius = 0.02f;
static float sRadiusTransitionTime = 0.25f;
static float sActiveRadius = 1.33f;
static float sDefaultRadius = 0.35f;

static inline void SetPosition(
    KoopaShellObject* object, const nlVector3& position)
{
    object->mPosition = position;
    object->mPhysics->SetPosition(
        object->mPosition, PhysicsObject::WORLD_COORDINATES);
}

static inline void Deactivate(KoopaShellObject* object, bool destroyEffect)
{
    if (object->mVisible)
    {
        if (object->mOwner != 0 && object->mOwner->m_pBall != 0)
        {
            object->mOwner->ReleaseBall(0);
        }

        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(sKoopaShellTrail);
        if (!destroyEffect)
        {
            EmissionManager::Instance()->Kill(
                (unsigned long)object, group);
        }
        else
        {
            EmissionManager::Instance()->Destroy(
                (unsigned long)object, group);
        }

        if (!destroyEffect)
        {
            fn_801BC96C(&object->mPosition);
        }

        SetPosition(object, sHiddenPosition);
        object->mPhysics->SetPosition(
            sHiddenPosition, PhysicsObject::WORLD_COORDINATES);
        object->mVisible = false;
    }

    object->mPhysics->DisableCollisions();
    g_pBall->m_pPhysicsBall->EnableCollisions();
    object->mRadiusTimer = 0.0f;
    object->mActiveTimer = 0.0f;
}

KoopaShellObject::KoopaShellObject(RenderObject* drawable)
{
    mRadius = sDefaultRadius;
    mTargetRadius = sDefaultRadius;
    mDrawable = drawable;
    mRadiusTimer = 0.0f;
    mActiveTimer = 0.0f;
    mVisible = false;
    mOwner = 0;
    mVelocity = sInitialVelocity;
    mPosition = sHiddenPosition;
    mSpin = 0;

    mPhysics = new (8, false) PhysicsKoopaShell(this, sDefaultRadius);
    mPhysics->SetPosition(
        sHiddenPosition, PhysicsObject::WORLD_COORDINATES);
    mPhysics->DisableCollisions();
}

KoopaShellObject::~KoopaShellObject()
{
    delete mPhysics;
}

void KoopaShellObject::Update(float deltaTime)
{
    if (!mVisible)
    {
        return;
    }

    if (mActiveTimer > 0.0f)
    {
        mActiveTimer -= deltaTime;
        if (mActiveTimer <= 0.0f)
        {
            ::Deactivate(this, false);
            return;
        }
    }

    mSpin += sSpinStep;

    if (mRadiusTimer > 0.0f)
    {
        mRadiusTimer -= deltaTime;
        if (mRadiusTimer <= 0.0f)
        {
            mRadiusTimer = 0.0f;
            mRadius = mTargetRadius;
        }
        else
        {
            float fraction = deltaTime / mRadiusTimer;
            if (fraction > 1.0f)
            {
                fraction = 1.0f;
            }
            mRadius = Interpolate(
                mRadius, mTargetRadius, fraction);
        }
        mPhysics->SetRadius(mRadius);
    }

    // Retail reads the new position's uninitialized height before clamping it.
    nlVector3 position = {
        mPosition.x + deltaTime * mVelocity.x,
        mPosition.y + deltaTime * mVelocity.y,
        position.z
    };
    if (position.z < mPhysics->GetRadius())
    {
        position.z = mPhysics->GetRadius();
    }
    ::SetPosition(this, position);

    if (fabsf(mPosition.y) > cField::GetSidelineY(1))
    {
        EmissionManager::Instance()->Kill(
            (unsigned long)this,
            EmissionManager::Instance()->GetEffectsGroup(sKoopaShellTrail));
    }
}

void KoopaShellObject::UpdateTrailEffect(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_Replaying == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        RenderSnapshot* snapshot = ReplayManager::Instance()->mRender;
        if (snapshot->_1BE8.mVisible)
        {
            controller.SetPosition(snapshot->_1BE8.mPosition);
        }
    }
}

void KoopaShellObject::Activate(cFielder* owner)
{
    mOwner = owner;
    float radius = sInitialRadius;
    mTargetRadius = radius;
    mRadiusTimer = 0.0f;
    mRadius = radius;
    mPhysics->SetRadius(radius);

    float transitionTime = sRadiusTransitionTime;
    radius = sActiveRadius;
    mTargetRadius = radius;
    mRadiusTimer = transitionTime;
    if (transitionTime <= 0.0f)
    {
        mRadius = radius;
        mPhysics->SetRadius(radius);
    }

    mSpin = nlRandom(0xFFFF);
    mActiveTimer = sActiveTime;
    mVisible = true;
    mPhysics->EnableCollisions();

    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(sKoopaShellTrail);
    EmissionController* controller = EmissionManager::Instance()->Create(group, 3, true, 0);
    controller->SetPosition(mPosition);
    controller->m_uUserData = (u32)this;
    controller->SetUpdateCallback(
        Function1<void, EmissionController&>(UpdateTrailEffect));

    fn_801BC6E4(owner);
    g_pBall->m_bVisible = false;
    g_pBall->m_pPhysicsBall->DisableCollisions();
}

void KoopaShellObject::Deactivate(bool destroyEffect)
{
    ::Deactivate(this, destroyEffect);
}

float KoopaShellObject::GetRadiusScale() const
{
    return mRadius / sDefaultRadius;
}

void KoopaShellObject::SetPosition(const nlVector3& position)
{
    ::SetPosition(this, position);
}

void KoopaShellObject::Reset()
{
    ::Deactivate(this, true);

    mRadiusTimer = 0.0f;
    mActiveTimer = 0.0f;
    mPosition = sHiddenPosition;
    mSpin = 0;
    mRadius = 1.0f;
    mTargetRadius = 1.0f;
    mRadiusTimer = 0.0f;
    mOwner = 0;
    mActiveTimer = 0.0f;
    mPhysics->SetRadius(0.1f);
    mPhysics->SetPosition(
        mPosition, PhysicsObject::WORLD_COORDINATES);
}
