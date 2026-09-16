#include "Game/Render/HammerObject.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/Fielder.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlSlotPool.h"
#include "ode/objects.h"

extern "C"
{
    extern float gHammerGrowScale;
    extern float gHammerGrowDuration;
    float gHammerSpinSpeed = 25.0f;
    float gHammerLandedDuration = 5.0f;
    float gHammerMinLandingAngle = 105.0f;
    float gHammerMaxLandingAngle = 165.0f;

    extern const float sHammerOne;
    extern const float sHammerZero;
    extern const float sHammerLengthEpsilon;
    extern const float sHammerPi;
    extern const float sHammerHalf;
    extern const float sHammerDegreesPerHalfTurn;
    extern const float sHammerRadiansToAngleUnits;
    extern const float sHammerQuarter;

    nlVector4 sHammerForward = { 1.0f, 0.0f, 0.0f, 0.0f };

    extern void fn_801B8C7C(const nlVector3&);
    extern void fn_801BCA5C(const nlVector3*);
    extern void fn_802B5444(nlQuaternion&, unsigned short);
}

bool PlaySound(int, unsigned long, const void*, void*);

inline AvoidablePoint::AvoidablePoint(
    int type, float radius, const nlVector2& position)
    : AvoidableObject(type)
    , mRadius(radius)
{
    mPosition.x = position.x;
    mPosition.y = position.y;
    mPosition.z = sHammerZero;
}

static const nlVector3 sHammerInactivePosition = { 0.0f, -20.0f, -5.0f };

static inline float ToRadians(float degrees)
{
    return sHammerPi * degrees / sHammerDegreesPerHalfTurn;
}

static inline void SetVelocity(HammerObject* object, const nlVector3& velocity)
{
    object->mPhysics->SetLinearVelocity(velocity);

    dQuaternion bodyOrientation;
    nlVector3 forward;
    nlVector3 planarVelocity;
    nlVector3 angularVelocity;
    nlQuaternion orientation;

    float velocityY = velocity.y;
    float velocityX = velocity.x;
    float spinSpeed = gHammerSpinSpeed;
    nlVec3Set(angularVelocity,
        sHammerZero,
        spinSpeed,
        sHammerZero);
    nlVec3Set(planarVelocity,
        velocityX,
        velocityY,
        sHammerZero);

    if (planarVelocity.GetLengthSq3D() > sHammerLengthEpsilon)
    {
        forward = *(nlVector3*)&sHammerForward;
        GetRotationBetweenVectors(orientation, forward, planarVelocity);
    }
    else
    {
        orientation.z = sHammerZero;
        orientation.y = sHammerZero;
        orientation.x = sHammerZero;
        orientation.w = sHammerOne;
    }

    bodyOrientation[0] = orientation.x;
    bodyOrientation[1] = orientation.y;
    bodyOrientation[2] = orientation.z;
    bodyOrientation[3] = orientation.w;
    dBodySetQuaternion(object->mPhysics->m_bodyID, bodyOrientation);
    object->mPhysics->SetAngularVelocity(angularVelocity);
    object->mOrientation = orientation;
}

static inline void Reset(HammerObject* object)
{
    object->mPendingReset = false;
    object->mPhysics->SetPosition(
        sHammerInactivePosition, PhysicsObject::WORLD_COORDINATES);
    SetVelocity(object, v3Zero);
    object->mStoredVelocity = object->mPhysics->GetLinearVelocity();
    object->mPhysics->DisableCollisions();
    object->mPhysics->EnableGravity();

    object->mActive = false;
    object->mTargetRadiusScale = sHammerOne;
    object->mRadiusTimer = sHammerZero;
    object->mRadiusScale = sHammerOne;
    object->mPhysics->SetRadius(object->mBaseRadius * sHammerOne);
    object->mOwner = 0;
    object->mFreezeTimer = sHammerZero;
    object->mLandedTimer = sHammerZero;
    delete object->mAvoidable;
    object->mAvoidable = 0;
}

HammerObject::HammerObject(int index, float radius)
{
    HammerObject* object = this;
    object->mBaseRadius = radius;
    object->mRadiusScale = sHammerOne;
    object->mTargetRadiusScale = sHammerOne;
    object->mRadiusTimer = sHammerZero;
    object->mIndex = index;
    object->mActive = false;
    object->mPendingReset = false;
    object->mOwner = 0;
    object->mFreezeTimer = sHammerZero;
    object->mLandedTimer = sHammerZero;

    PhysicsHammer* physics = new PhysicsHammer(radius);
    object->mPhysics = physics;
    physics->mHammer = object;
    object->mPhysics->SetPosition(
        sHammerInactivePosition, PhysicsObject::WORLD_COORDINATES);
    object->mDrawable = GetRenderObject(2, index);
    object->mAvoidable = 0;
    ::Reset(object);
}

HammerObject::~HammerObject()
{
    if (mAvoidable != 0)
    {
        delete mAvoidable;
    }
    if (mPhysics != 0)
    {
        delete mPhysics;
    }
}

const nlVector3* HammerObject::GetPosition() const
{
    return &((HammerObject*)this)->mPhysics->GetPosition();
}

void HammerObject::SetPosition(const nlVector3& position)
{
    mPhysics->SetPosition(position, PhysicsObject::WORLD_COORDINATES);
}

void HammerObject::SetVelocity(const nlVector3& velocity)
{
    ::SetVelocity(this, velocity);
}

const nlQuaternion* HammerObject::GetOrientation()
{
    HammerObject* object = this;
    if (object->mLandedTimer <= sHammerZero)
    {
        const float* orientation = dBodyGetQuaternion(object->mPhysics->m_bodyID);
        nlVec4Set(*(nlVector4*)&object->mOrientation, orientation[0], orientation[1], orientation[2], orientation[3]);
    }
    return &object->mOrientation;
}

void HammerObject::OnLanding()
{
    if (mLandedTimer <= sHammerZero)
    {
        mLandedTimer = gHammerLandedDuration;
        fn_801B8C7C(mPhysics->GetPosition());
        PlaySound(mOwner->mUnidentified318, 0x85EA466C, 0, 0);

        if (mLandedTimer <= sHammerZero)
        {
            const float* bodyOrientation
                = dBodyGetQuaternion(mPhysics->m_bodyID);
            float w;
            float z;
            float y;
            float x;
            w = bodyOrientation[3];
            z = bodyOrientation[2];
            y = bodyOrientation[1];
            x = bodyOrientation[0];
            nlVec4Set(*(nlVector4*)&mOrientation, x, y, z, w);
        }
        nlQuaternion orientation = mOrientation;
        nlQuaternion facing;
        nlQuaternion target;
        nlQuaternion tilt;
        fn_802B5444(tilt,
            (u16)(int)(sHammerRadiansToAngleUnits
                * ToRadians(sHammerHalf * (gHammerMaxLandingAngle + gHammerMinLandingAngle))));

        fn_802B549C(facing,
            mOwner->mUnidentified024.m_aActualFacingDirection);

        nlMultQuat(target, facing, tilt);

        float maximumDot = nlSin((u16)((u16)(int)(sHammerRadiansToAngleUnits
            * (sHammerQuarter * ToRadians(gHammerMaxLandingAngle - gHammerMinLandingAngle)))
            + 0x4000));
        float dot = nlQuatDot(orientation, target);
        if (dot < sHammerZero)
        {
            orientation.x = -orientation.x;
            orientation.y = -orientation.y;
            orientation.z = -orientation.z;
            orientation.w = -orientation.w;
            dot = nlQuatDot(orientation, target);
        }

        if (dot < maximumDot)
        {
            nlQuaternion lowTilt;
            nlQuaternion highTilt;
            float lowDegrees;
            float pi = sHammerPi;
            lowDegrees = gHammerMinLandingAngle;
            float lowAngle = pi * lowDegrees / sHammerDegreesPerHalfTurn;
            float highAngle = pi * gHammerMaxLandingAngle / sHammerDegreesPerHalfTurn;
            fn_802B5444(lowTilt,
                (u16)(int)(sHammerRadiansToAngleUnits * lowAngle));
            fn_802B5444(highTilt,
                (u16)(int)(sHammerRadiansToAngleUnits * highAngle));

            nlQuaternion lowOrientation;
            nlQuaternion highOrientation;
            nlMultQuat(lowOrientation, facing, lowTilt);
            nlMultQuat(highOrientation, facing, highTilt);

            if (nlQuatDot(orientation, lowOrientation)
                > nlQuatDot(orientation, highOrientation))
            {
                mOrientation = lowOrientation;
            }
            else
            {
                mOrientation = highOrientation;
            }
        }

        AvoidableObject* avoidable = new (8, false)
            AvoidablePoint(AVOID_BOWSER, mPhysics->GetRadius(),
                (const nlVector2&)mPhysics->GetPosition());
        mAvoidable = avoidable;
    }
}

void HammerObject::Freeze(float time)
{
    HammerObject* object = this;
    object->mFreezeTimer = time;
    object->mStoredVelocity = object->mPhysics->GetLinearVelocity();
    object->mPhysics->DisableCollisions();
}

void HammerObject::Update(float dt)
{
    HammerObject* object = this;
    if (!object->mActive)
    {
        return;
    }

    if (object->mPendingReset)
    {
        ::Reset(object);
        return;
    }

    if (object->mFreezeTimer > sHammerZero)
    {
        object->mFreezeTimer -= dt;
        if (object->mFreezeTimer <= sHammerZero)
        {
            object->mPhysics->EnableCollisions();
            object->mPhysics->EnableGravity();
            object->mPhysics->SetLinearVelocity(object->mStoredVelocity);
        }
        else
        {
            object->mPhysics->Freeze();
            return;
        }
    }

    if (object->mLandedTimer > sHammerZero)
    {
        object->mLandedTimer -= dt;
        if (object->mLandedTimer <= sHammerZero)
        {
            fn_801BCA5C(&object->mPhysics->GetPosition());
            ::Reset(object);
            return;
        }
        object->mPhysics->Freeze();
    }

    if (object->mRadiusTimer > sHammerZero)
    {
        object->mRadiusTimer -= dt;
        if (object->mRadiusTimer <= sHammerZero)
        {
            object->mRadiusScale = object->mTargetRadiusScale;
            object->mRadiusTimer = sHammerZero;
            object->mPhysics->SetRadius(object->mBaseRadius * object->mTargetRadiusScale);
        }
        else
        {
            float fraction = dt / object->mRadiusTimer;
            if (fraction > sHammerOne)
            {
                fraction = sHammerOne;
            }
            object->mRadiusScale = Interpolate(
                object->mRadiusScale, object->mTargetRadiusScale, fraction);
            object->mPhysics->SetRadius(object->mBaseRadius * object->mRadiusScale);
        }
    }
}

void HammerObject::Activate(cFielder* fielder)
{
    HammerObject* object = this;
    object->mOwner = fielder;
    object->mPhysics->SetPosition(
        fielder->mUnidentified024.m_v3Position, PhysicsObject::WORLD_COORDINATES);
    object->mPendingReset = false;
    object->mActive = true;
    object->mPhysics->EnableCollisions();

    object->mTargetRadiusScale = sHammerOne;
    object->mRadiusTimer = sHammerZero;
    object->mRadiusScale = sHammerOne;
    object->mPhysics->SetRadius(object->mBaseRadius * sHammerOne);

    float targetScale = gHammerGrowScale;
    float duration = gHammerGrowDuration;
    object->mTargetRadiusScale = targetScale;
    object->mRadiusTimer = duration;
    if (duration <= sHammerZero)
    {
        object->mRadiusScale = object->mTargetRadiusScale;
        object->mPhysics->SetRadius(object->mBaseRadius * object->mTargetRadiusScale);
    }

    nlVector3 velocity = { 0, 0, 0 };
    object->mPhysics->SetLinearVelocity(velocity);

    dQuaternion bodyOrientation;
    nlVector3 forward;
    nlVector3 planarVelocity;
    nlVector3 angularVelocity;
    nlQuaternion orientation;

    float velocityY = velocity.y;
    float velocityX = velocity.x;
    float spinSpeed = gHammerSpinSpeed;
    nlVec3Set(angularVelocity,
        sHammerZero,
        spinSpeed,
        sHammerZero);
    nlVec3Set(planarVelocity,
        velocityX,
        velocityY,
        sHammerZero);

    if (planarVelocity.GetLengthSq3D() > sHammerLengthEpsilon)
    {
        forward = *(nlVector3*)&sHammerForward;
        GetRotationBetweenVectors(orientation, forward, planarVelocity);
    }
    else
    {
        orientation.z = sHammerZero;
        orientation.y = sHammerZero;
        orientation.x = sHammerZero;
        orientation.w = sHammerOne;
    }

    bodyOrientation[0] = orientation.x;
    bodyOrientation[1] = orientation.y;
    bodyOrientation[2] = orientation.z;
    bodyOrientation[3] = orientation.w;
    dBodySetQuaternion(object->mPhysics->m_bodyID, bodyOrientation);
    object->mPhysics->SetAngularVelocity(angularVelocity);
    object->mOrientation = orientation;
}

void HammerObject::Reset(bool)
{
    mPendingReset = false;
    mPhysics->SetPosition(
        sHammerInactivePosition, PhysicsObject::WORLD_COORDINATES);
    const nlVector3& velocity = v3Zero;
    mPhysics->SetLinearVelocity(velocity);

    dQuaternion bodyOrientation;
    nlVector3 forward;
    nlVector3 planarVelocity;
    nlVector3 angularVelocity;
    nlQuaternion orientation;

    float velocityY = velocity.y;
    float velocityX = velocity.x;
    float spinSpeed = gHammerSpinSpeed;
    nlVec3Set(angularVelocity,
        sHammerZero,
        spinSpeed,
        sHammerZero);
    nlVec3Set(planarVelocity,
        velocityX,
        velocityY,
        sHammerZero);

    if (planarVelocity.GetLengthSq3D() > sHammerLengthEpsilon)
    {
        forward = *(nlVector3*)&sHammerForward;
        GetRotationBetweenVectors(orientation, forward, planarVelocity);
    }
    else
    {
        orientation.z = sHammerZero;
        orientation.y = sHammerZero;
        orientation.x = sHammerZero;
        orientation.w = sHammerOne;
    }

    bodyOrientation[0] = orientation.x;
    bodyOrientation[1] = orientation.y;
    bodyOrientation[2] = orientation.z;
    bodyOrientation[3] = orientation.w;
    dBodySetQuaternion(mPhysics->m_bodyID, bodyOrientation);
    mPhysics->SetAngularVelocity(angularVelocity);
    mOrientation = orientation;
    mStoredVelocity = mPhysics->GetLinearVelocity();
    mPhysics->DisableCollisions();
    mPhysics->EnableGravity();

    mActive = false;
    mTargetRadiusScale = sHammerOne;
    mRadiusTimer = sHammerZero;
    mRadiusScale = sHammerOne;
    mPhysics->SetRadius(mBaseRadius * sHammerOne);
    mOwner = 0;
    mFreezeTimer = sHammerZero;
    mLandedTimer = sHammerZero;
    delete mAvoidable;
    mAvoidable = 0;
}

void HammerObject::Deactivate(bool emitEffect)
{
    HammerObject* object = this;
    if (object->mActive)
    {
        object->mPendingReset = true;
        if (emitEffect)
        {
            fn_801BCA5C(&object->mPhysics->GetPosition());
        }
    }
}

extern "C"
{
    extern const float sHammerOne = 1.0f;
    extern const float sHammerZero = 0.0f;
    extern const float sHammerLengthEpsilon = 0.001f;
    extern const float sHammerPi = 3.1415927f;
    extern const float sHammerHalf = 0.5f;
    extern const float sHammerDegreesPerHalfTurn = 180.0f;
    extern const float sHammerRadiansToAngleUnits = 10430.378f;
    extern const float sHammerQuarter = 0.25f;
}
