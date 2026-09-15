#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/Render/BulletBill.h"
#include "Game/ReplayManager.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBulletBill.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "types.h"

extern "C" DrawableBulletBill& fn_8018755C(RenderSnapshot*, unsigned int);
extern "C" void fn_801B8FF8(cFielder*);
extern "C" void fn_801B91F8(cFielder*);
extern "C" void fn_800367B4(cFielder*);

static const nlVector3 lbl_804DCDB8 = { 0.0f, 0.0f, 0.0f };
static const nlVector3 lbl_804DCDC4 = { 0.0f, -20.0f, -10.0f };
static float lbl_806DCD80 = 1.25f;

BulletBillObject::BulletBillObject(
    RenderObject* pDrawable, u32 nIndex, float radius, float parameter)
{
    index = nIndex;
    drawable = pDrawable;
    scale = 1.0f;
    targetScale = 1.0f;
    scaleTimer = 0.0f;
    active = false;
    target = 0;
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 0.0f;
    orientation.z = 0.0f;
    orientation.y = 0.0f;
    orientation.x = 0.0f;
    orientation.w = 1.0f;

    PhysicsObject* pPhysics = new (8, false)
        PhysicsBulletBill(this, radius, parameter);
    physics = pPhysics;
    pPhysics->DisableCollisions();
}

BulletBillObject::~BulletBillObject()
{
    delete physics;
}

void BulletBillObject::Update(float deltaTime)
{
    if (!active)
    {
        return;
    }

    const nlVector3& currentPosition = position;
    const nlVector3& currentVelocity = velocity;

    nlVector3 newPosition;
    newPosition.z = currentPosition.z + deltaTime * currentVelocity.z;
    newPosition.y = currentPosition.y + deltaTime * currentVelocity.y;
    newPosition.x = currentPosition.x + deltaTime * currentVelocity.x;
    position = newPosition;
    physics->SetPosition(
        position, PhysicsObject::WORLD_COORDINATES);

    if (scaleTimer > 0.0f)
    {
        scaleTimer -= deltaTime;
        if (scaleTimer <= 0.0f)
        {
            scaleTimer = 0.0f;
            scale = targetScale;
        }
        else
        {
            float percent = deltaTime / scaleTimer;
            if (percent > 1.0f)
            {
                percent = 1.0f;
            }
            scale = Interpolate(scale, targetScale, percent);
        }
    }

    if (target != 0)
    {
        fn_802B549C(
            orientation, target->mUnidentified024.m_aActualFacingDirection);
    }
}

void UpdateBulletBillEmitter(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }
    if (!controller.m_Replaying && ReplayManager::Instance()->mRender != 0)
    {
        BulletBillObject* object = (BulletBillObject*)controller.m_uUserData;
        u32 index = object->index;
        RenderSnapshot* snapshot = ReplayManager::Instance()->mRender;
        DrawableBulletBill& bill = fn_8018755C(snapshot, index);
        if (bill.mVisible)
        {
            controller.SetPosition(bill.mPosition);
        }
    }
}

void BulletBillObject::Show(cFielder* fielder)
{
    target = fielder;
    if (fielder != 0)
    {
        fn_802B549C(orientation, fielder->mUnidentified024.m_aActualFacingDirection);
    }

    nlVector3 newPosition = g_pBall->m_v3Position;
    newPosition.z = ((PhysicsBulletBill*)physics)->GetRadius();
    position = newPosition;
    active = true;
    physics->EnableCollisions();
    physics->SetPosition(position, PhysicsObject::WORLD_COORDINATES);
    targetScale = 0.2f;
    scaleTimer = 0.0f;
    scale = targetScale;
    targetScale = 1.0f;
    scaleTimer = 0.52f;

    float speed = fielder->mUnidentified024.m_fActualSpeed;
    nlVector3 newVelocity;
    nlPolarToCartesian(newVelocity.x, newVelocity.y,
        fielder->mUnidentified024.m_aActualFacingDirection,
        speed);
    newVelocity.z = 0.0f;
    velocity = newVelocity;

    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("bulletbill_trail");
    EmissionController* controller = EmissionManager::Instance()->Create(group, 3, true, 0);
    controller->SetPosition(position);
    controller->m_uUserData = (unsigned long)this;
    controller->SetUpdateCallback(Function<EmissionController&>(UpdateBulletBillEmitter));

    fn_801B8FF8(fielder);
    g_pBall->m_bVisible = false;
    g_pBall->m_pPhysicsBall->DisableCollisions();
}

void BulletBillObject::Hide(bool destroyEffect)
{
    if (active)
    {
        if (target == g_pBall->m_pOwner)
        {
            g_pBall->m_pPhysicsBall->EnableCollisions();
            g_pBall->m_bVisible = true;
            target->ReleaseBall(0);
            target->SetNoPickUpTime(0.2f);
        }
        if (!destroyEffect)
        {
            if (!target->IsFallenDown())
            {
                target->InitActionSlideAttackReact(target, false);
                target->m_pCurrentAnimController->m_fPlaybackSpeedScale = lbl_806DCD80;
            }
            fn_801B91F8(target);
            fn_800367B4(target);
        }
        position = lbl_804DCDC4;
        velocity = lbl_804DCDB8;
        physics->SetPosition(position, PhysicsObject::WORLD_COORDINATES);
        active = false;
    }
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("bulletbill_trail");
    if (!destroyEffect)
    {
        EmissionManager::Instance()->Kill((unsigned long)this, group);
    }
    else
    {
        EmissionManager::Instance()->Destroy((unsigned long)this, group);
    }
    physics->DisableCollisions();
}

void BulletBillObject::Reset()
{
    Hide(true);

    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 0.0f;
    orientation.z = 0.0f;
    orientation.y = 0.0f;
    orientation.x = 0.0f;
    orientation.w = 1.0f;
    scale = 1.0f;
    targetScale = 1.0f;
    scaleTimer = 0.0f;
    target = 0;
}
