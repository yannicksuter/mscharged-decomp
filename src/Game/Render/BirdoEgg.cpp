#include "Game/Physics/PhysicsBirdoEgg.h"

#include "Game/Render/BirdoEgg.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/ReplayManager.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "types.h"
#include <math.h>

extern "C" void fn_8013F854(const char*, ...);
extern "C" void fn_801BC828(cCharacter*);
extern "C" void fn_801BC9E4(const nlVector3&);
extern "C" void fn_802B5370(nlQuaternion&, const nlVector3&, unsigned short);

static const nlVector3 lbl_804DCD90 = { 0.0f, 0.0f, 0.0f };
static const nlVector3 lbl_804DCD9C = { 0.0f, 20.0f, -15.0f };
static float lbl_806DCD68 = 1.05f;
static float lbl_806DCD6C = 0.35f;
static float lbl_806DCD70 = 27.5f;
static float lbl_806DCD74 = 0.02f;
static float lbl_806DCD78 = 1.25f;
static float lbl_806DCD7C = 0.3f;

BirdoEggObject::BirdoEggObject(RenderObject* drawable)
{
    mRadius = lbl_806DCD7C;
    mTargetRadius = lbl_806DCD7C;
    mDrawable = drawable;
    mRadiusTimer = 0.0f;
    mActiveTimer = 0.0f;
    mVisible = false;
    mShooter = 0;
    mVelocity = lbl_804DCD90;
    mPosition = lbl_804DCD9C;
    mSpinSpeed = 0.0f;
    mOrientation.z = 0.0f;
    mOrientation.y = 0.0f;
    mOrientation.x = 0.0f;
    mOrientation.w = 1.0f;

    mPhysics = new (8, false) PhysicsBirdoEgg(
        this, lbl_806DCD7C);
    mPhysics->SetPosition(
        lbl_804DCD9C, PhysicsObject::WORLD_COORDINATES);
    mPhysics->DisableCollisions();
}

BirdoEggObject::~BirdoEggObject()
{
    delete mPhysics;
}

void BirdoEggObject::Update(float deltaTime)
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
            Hide(false);
            return;
        }
    }

    nlQuaternion rotation;
    nlVector3 axis = { 1.0f, 0.0f, 0.0f };
    unsigned short angle = (unsigned short)(int)(10430.378f * (mSpinSpeed * deltaTime));
    fn_802B5370(rotation, axis, angle);
    nlMultQuat(mOrientation, mOrientation, rotation);

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
            float percent = deltaTime / mRadiusTimer;
            if (percent > 1.0f)
            {
                percent = 1.0f;
            }
            mRadius = Interpolate(mRadius, mTargetRadius, percent);
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
    SetPosition(position);

    if (fabsf(mPosition.y) > cField::GetSidelineY(1))
    {
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("birdo_egg_trail");
        EmissionManager::Instance()->Kill((unsigned long)this, group);
    }
}

void UpdateBirdoEggEmitter(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }
    if (!controller.m_Replaying && ReplayManager::Instance()->mRender != 0)
    {
        DrawableBirdoEgg& egg = ReplayManager::Instance()->mRender->_1BC4;
        if (egg.mVisible)
        {
            controller.SetPosition(egg.mPosition);
        }
    }
}

void BirdoEggObject::SetPosition(const nlVector3& position)
{
    mPosition = position;
    mPhysics->SetPosition(
        mPosition, PhysicsObject::WORLD_COORDINATES);
}

void BirdoEggObject::Show(cFielder* shooter)
{
    fn_8013F854("BirdoEggShow\n");
    mShooter = shooter;
    cNet* net = shooter->m_pTeam->GetOtherNet();
    float x = shooter->mUnidentified024.m_v3Position.x - net->m_v3NetLocation.x;
    float y = shooter->mUnidentified024.m_v3Position.y - net->m_v3NetLocation.y;
    float angle = nlATan2f(y, x);
    fn_802B549C(mOrientation, (unsigned short)(int)(10430.378f * angle));
    mSpinSpeed = lbl_806DCD70;
    mTargetRadius = lbl_806DCD74;
    mRadiusTimer = 0.0f;
    mRadius = mTargetRadius;
    mPhysics->SetRadius(mRadius);

    float time = lbl_806DCD6C;
    float radius = lbl_806DCD78;
    mTargetRadius = radius;
    mRadiusTimer = time;
    if (time <= 0.0f)
    {
        mRadius = radius;
        mPhysics->SetRadius(mRadius);
    }
    mActiveTimer = lbl_806DCD68;
    fn_8013F854("BirdoEggSetActive\n");
    mVisible = true;
    mPhysics->EnableCollisions();

    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("birdo_egg_trail");
    EmissionController* controller = EmissionManager::Instance()->Create(group, 3, true, 0);
    controller->SetPosition(mPosition);
    controller->m_uUserData = (unsigned long)this;
    controller->SetUpdateCallback(Function<EmissionController&>(UpdateBirdoEggEmitter));

    fn_801BC828(shooter);
    g_pBall->m_bVisible = false;
    g_pBall->ClearBallBlur();
    g_pBall->m_pPhysicsBall->DisableCollisions();
}

void BirdoEggObject::Hide(bool destroyEffect)
{
    fn_8013F854("BirdoEggHide\n");
    if (mVisible)
    {
        fn_8013F854("BirdoEggHide SetVis(false)\n");
        if (!destroyEffect)
        {
            PlaySound(mShooter->mUnidentified318, 0x52641B7B, 0, 0);
        }
        EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("birdo_egg_trail");
        if (!destroyEffect)
        {
            EmissionManager::Instance()->Kill((unsigned long)this, group);
        }
        else
        {
            EmissionManager::Instance()->Destroy((unsigned long)this, group);
        }
        if (!destroyEffect)
        {
            fn_801BC9E4(mPosition);
        }
        SetPosition(lbl_804DCD9C);
        mPhysics->SetPosition(lbl_804DCD9C, PhysicsObject::WORLD_COORDINATES);
        mVisible = false;
    }
    mPhysics->DisableCollisions();
    g_pBall->m_pPhysicsBall->EnableCollisions();
    mRadiusTimer = 0.0f;
    mActiveTimer = 0.0f;
}

float BirdoEggObject::GetScale() const
{
    return mRadius / lbl_806DCD7C;
}

void BirdoEggObject::Reset()
{
    Hide(true);

    mSpinSpeed = 0.0f;
    mPosition = lbl_804DCD9C;
    mOrientation.z = 0.0f;
    mOrientation.y = 0.0f;
    mOrientation.x = 0.0f;
    mOrientation.w = 1.0f;
    mRadius = 1.0f;
    mTargetRadius = 1.0f;
    mRadiusTimer = 0.0f;
    mShooter = 0;
    mActiveTimer = 0.0f;

    mPhysics->SetRadius(0.1f);
    mPhysics->SetPosition(
        mPosition, PhysicsObject::WORLD_COORDINATES);
}
