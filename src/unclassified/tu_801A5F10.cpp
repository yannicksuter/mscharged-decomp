#include "unclassified/tu_801A5F10.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Drawable/DrawableKoopaShell.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsSphere.h"
#include "Game/ReplayManager.h"
#include "Game/RenderSnapshot.h"
#include "NL/nlMemory.h"

extern "C"
{
    PhysicsSphere* fn_801709D0(
        PhysicsSphere*, KoopaShellObject*, float);
    EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
    EmissionController* fn_802E7FE4(
        EmissionManager*, EffectsGroup*, int, bool, bool);
    void fn_801BC6E4(cFielder*);
    void fn_801BC96C(const nlVector3*);
}

extern const nlVector3 lbl_804DCED0;
extern const nlVector3 lbl_804DCEDC;

static char sKoopaShellTrail[] = "koopa_shell_trail";

static float sActiveTime = 1.1f;
static unsigned int sSpinStep = 3500;
static float sInitialRadius = 0.02f;
static float sRadiusTransitionTime = 0.25f;
static float sActiveRadius = 1.33f;
static float sDefaultRadius = 0.35f;

extern "C" void fn_801A5F10()
{
}

extern "C" void fn_801A5F14()
{
}

extern "C" void fn_801A5F18()
{
}

extern "C" void fn_801A5F1C()
{
}

extern "C" void fn_801A5F20()
{
}

extern "C" void fn_801A5F24()
{
}

extern "C" void fn_801A5F28()
{
}

extern "C" void fn_801A5F2C()
{
}

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

        EffectsGroup* group = fn_802E7CDC(
            EmissionManager::Instance(), sKoopaShellTrail);
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

        SetPosition(object, lbl_804DCEDC);
        object->mPhysics->SetPosition(
            lbl_804DCEDC, PhysicsObject::WORLD_COORDINATES);
        object->mVisible = false;
    }

    object->mPhysics->DisableCollisions();
    g_pBall->m_pPhysicsBall->EnableCollisions();
    object->mRadiusTimer = 0.0f;
    object->mActiveTimer = 0.0f;
}

extern "C" KoopaShellObject* fn_801A5F30(
    KoopaShellObject* object, RenderObject* drawable)
{
    object->mRadius = sDefaultRadius;
    object->mDrawable = drawable;
    object->mTargetRadius = sDefaultRadius;
    object->mRadiusTimer = 0.0f;
    object->mActiveTimer = 0.0f;
    object->mVisible = false;
    object->mOwner = 0;
    object->mVelocity = lbl_804DCED0;
    object->mPosition = lbl_804DCEDC;
    object->mSpin = 0;

    PhysicsSphere* physics =
        (PhysicsSphere*)nlMalloc(0x40, 8, false);
    if (physics != 0)
    {
        physics = fn_801709D0(
            physics, object, sDefaultRadius);
    }
    object->mPhysics = physics;
    object->mPhysics->SetPosition(
        lbl_804DCEDC, PhysicsObject::WORLD_COORDINATES);
    object->mPhysics->DisableCollisions();
    return object;
}

extern "C" KoopaShellObject* fn_801A6004(
    KoopaShellObject* object, int shouldDelete)
{
    if (object != 0)
    {
        delete object->mPhysics;
        if (shouldDelete > 0)
        {
            ::operator delete(object);
        }
    }
    return object;
}

extern "C" void fn_801A6074(
    KoopaShellObject* object, float deltaTime)
{
    if (!object->mVisible)
    {
        return;
    }

    if (object->mActiveTimer > 0.0f)
    {
        object->mActiveTimer -= deltaTime;
        if (object->mActiveTimer <= 0.0f)
        {
            Deactivate(object, false);
            return;
        }
    }

    object->mSpin += sSpinStep;

    if (object->mRadiusTimer > 0.0f)
    {
        object->mRadiusTimer -= deltaTime;
        if (object->mRadiusTimer <= 0.0f)
        {
            object->mRadiusTimer = 0.0f;
            object->mRadius = object->mTargetRadius;
        }
        else
        {
            float fraction = deltaTime / object->mRadiusTimer;
            if (fraction > 1.0f)
            {
                fraction = 1.0f;
            }
            object->mRadius = Interpolate(
                object->mRadius, object->mTargetRadius, fraction);
        }
        object->mPhysics->SetRadius(object->mRadius);
    }

    nlVector3 position;
    position.x = deltaTime * object->mVelocity.x
               + object->mPosition.x;
    position.y = deltaTime * object->mVelocity.y
               + object->mPosition.y;
    if (position.z < object->mPhysics->GetRadius())
    {
        position.z = object->mPhysics->GetRadius();
    }
    SetPosition(object, position);

    if (nlAbs(object->mPosition.y) > cField::GetSidelineY(1))
    {
        EmissionManager::Instance()->Kill(
            (unsigned long)object,
            fn_802E7CDC(
                EmissionManager::Instance(), sKoopaShellTrail));
    }
}

extern "C" void fn_801A62C4(EmissionController& controller)
{
    if (g_pGame != 0)
    {
        switch (g_pGame->m_eGameState)
        {
        case 4:
            break;
        default:
            if (controller.m_GlView == 0
                && ReplayManager::Instance()->mRender != 0)
            {
                RenderSnapshot* snapshot =
                    ReplayManager::Instance()->mRender;
                if (snapshot->_1BE8.mVisible)
                {
                    controller.SetPosition(
                        snapshot->_1BE8.mPosition);
                }
            }
            break;
        }
    }
}

extern "C" void fn_801A6344(
    KoopaShellObject* object, cFielder* owner)
{
    object->mOwner = owner;
    float radius = sInitialRadius;
    object->mTargetRadius = radius;
    object->mRadiusTimer = 0.0f;
    object->mRadius = radius;
    object->mPhysics->SetRadius(radius);

    float transitionTime = sRadiusTransitionTime;
    radius = sActiveRadius;
    object->mTargetRadius = radius;
    object->mRadiusTimer = transitionTime;
    if (transitionTime <= 0.0f)
    {
        object->mRadius = radius;
        object->mPhysics->SetRadius(radius);
    }

    object->mSpin = nlRandom(0xFFFF);
    object->mActiveTimer = sActiveTime;
    object->mVisible = true;
    object->mPhysics->EnableCollisions();

    EffectsGroup* group = fn_802E7CDC(
        EmissionManager::Instance(), sKoopaShellTrail);
    EmissionController* controller = fn_802E7FE4(
        EmissionManager::Instance(), group, 3, true, false);
    controller->SetPosition(object->mPosition);
    controller->m_uUserData = (u32)object;
    controller->SetUpdateCallback(
        Function1<void, EmissionController&>(fn_801A62C4));

    fn_801BC6E4(owner);
    g_pBall->m_bVisible = false;
    g_pBall->m_pPhysicsBall->DisableCollisions();
}

extern "C" void fn_801A64A4(
    KoopaShellObject* object, bool destroyEffect)
{
    Deactivate(object, destroyEffect);
}

extern "C" float fn_801A65C0(
    const KoopaShellObject* object)
{
    return object->mRadius / sDefaultRadius;
}

extern "C" void fn_801A65D0(
    KoopaShellObject* object, const nlVector3& position)
{
    SetPosition(object, position);
}

extern "C" void fn_801A65F8(KoopaShellObject* object)
{
    Deactivate(object, true);

    object->mRadiusTimer = 0.0f;
    object->mActiveTimer = 0.0f;
    object->mPosition = lbl_804DCEDC;
    object->mSpin = 0;
    object->mRadius = 1.0f;
    object->mTargetRadius = 1.0f;
    object->mRadiusTimer = 0.0f;
    object->mOwner = 0;
    object->mActiveTimer = 0.0f;
    object->mPhysics->SetRadius(0.1f);
    object->mPhysics->SetPosition(
        object->mPosition, PhysicsObject::WORLD_COORDINATES);
}
