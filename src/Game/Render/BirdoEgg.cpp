#include "Game/Physics/PhysicsBirdoEgg.h"

#include "Game/Render/BirdoEgg.h"

#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "types.h"

extern const nlVector3 lbl_804DCD90;
extern const nlVector3 lbl_804DCD9C;
extern const float lbl_806DCD7C;
extern const float lbl_806E4ED0;
extern const float lbl_806E4ED4;
extern const float lbl_806E4EDC;

BirdoEggObject::BirdoEggObject(RenderObject* drawable)
{
    mRadius = lbl_806DCD7C;
    mDrawable = drawable;
    mTargetRadius = lbl_806DCD7C;
    mRadiusTimer = lbl_806E4ED0;
    mActiveTimer = lbl_806E4ED0;
    mVisible = false;
    mShooter = 0;
    mVelocity = lbl_804DCD90;
    mPosition = lbl_804DCD9C;
    mSpinSpeed = lbl_806E4ED0;
    mOrientation.z = lbl_806E4ED0;
    mOrientation.y = lbl_806E4ED0;
    mOrientation.x = lbl_806E4ED0;
    mOrientation.w = lbl_806E4ED4;

    PhysicsBirdoEgg* physics = new (8, false) PhysicsBirdoEgg(
        this, lbl_806DCD7C);
    mPhysics = physics;
    physics->SetPosition(
        lbl_804DCD9C, PhysicsObject::WORLD_COORDINATES);
    physics->DisableCollisions();
}

BirdoEggObject::~BirdoEggObject()
{
    delete mPhysics;
}

void BirdoEggObject::SetPosition(const nlVector3& position)
{
    mPosition = position;
    mPhysics->SetPosition(
        mPosition, PhysicsObject::WORLD_COORDINATES);
}

float BirdoEggObject::GetScale() const
{
    return mRadius / lbl_806DCD7C;
}

void BirdoEggObject::Reset()
{
    Hide(true);

    mSpinSpeed = lbl_806E4ED0;
    mPosition = lbl_804DCD9C;
    mOrientation.z = lbl_806E4ED0;
    mOrientation.y = lbl_806E4ED0;
    mOrientation.x = lbl_806E4ED0;
    mOrientation.w = lbl_806E4ED4;
    mRadius = lbl_806E4ED4;
    mTargetRadius = lbl_806E4ED4;
    mRadiusTimer = lbl_806E4ED0;
    mShooter = 0;
    mActiveTimer = lbl_806E4ED0;

    mPhysics->SetRadius(lbl_806E4EDC);
    mPhysics->SetPosition(
        mPosition, PhysicsObject::WORLD_COORDINATES);
}
