#include "Game/Physics/PhysicsBall.h"
#include "Game/Terrain.h"

#include "Game/AI/AiUtil.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/Field.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "math.h"
#include "types.h"
#include "Game/UnidentifiedStaticStorage.h"

float g_BallFriction = 5.0f;
float g_BallFrictionWall = 4.0f;
float g_BallBounce = 0.25f;
float g_BallBounceGround = 0.35f;
float g_BallBounceWall = 0.35f;
float g_BallRollingResistance = 5.0f;
float g_BallAirResistance = 0.1f;
float lbl_806DCA7C = 0.01f;
float lbl_806DCA80 = 0.01f;
float lbl_806DCA84 = 0.07f;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

extern "C" void fn_8013F854(const char*, ...);
extern float lbl_806E11E8;

PhysicsBall::PhysicsBall(
    CollisionSpace* space, PhysicsWorld* world, float radius)
    : PhysicsSphere(space, world, radius)
    , mbUseTiltForce(false)
    , mbUseWindForce(false)
    , mbIsSupportedByGround(false)
    , mbUseAngularVel(false)
    , mbUseMagnusEffect(false)
    , mbIgnoreForces(false)
    , mbCanFreeFall(false)
    , mbCanGoThroughGround(false)
    , mbPassLockedIn(false)
    , mfSpinTimer(0.0f)
    , mfBallAirResistance(0.0f)
    , mfChargeBonus(0.0f)
{
    SetCategory(0x20);
    SetCollide(0xAF);

    mbIgnoreForces = false;
    m_gravity = -22.5f;

    mv3TiltForce.x = 0.0f;
    mv3TiltForce.y = 0.0f;
    mv3TiltForce.z = 0.0f;
    mv3WindForce.x = 0.0f;
    mv3WindForce.y = 0.0f;
    mv3WindForce.z = 0.0f;
    mfBallAirResistance = g_BallAirResistance;
}

float PhysicsBall::GetBallMaxVelocity()
{
    return 90.0f;
}

void PhysicsBall::fn_8013FE00()
{
    mbIgnoreForces = true;
    m_gravity = 0.0f;
}

void PhysicsBall::fn_8013FE14()
{
    mbIgnoreForces = false;
    m_gravity = -22.5f;
}

void PhysicsBall::Unknown0()
{
    PhysicsObject::Unknown0();
    SetCategory(0x20);
    SetCollide(0xAF);

    mbIgnoreForces = false;
    m_gravity = -22.5f;
    mv3TiltForce.x = 0.0f;
    mv3TiltForce.y = 0.0f;
    mv3TiltForce.z = 0.0f;
    mv3WindForce.x = 0.0f;
    mv3WindForce.y = 0.0f;
    mv3WindForce.z = 0.0f;
    mbUseTiltForce = false;
    mbUseWindForce = false;
    mbIsSupportedByGround = false;
    mbUseAngularVel = false;
    mbUseMagnusEffect = false;
    mbIgnoreForces = false;
    mbCanFreeFall = false;
    mbCanGoThroughGround = false;
    mbPassLockedIn = false;
    mfSpinTimer = 0.0f;
    mfChargeBonus = 0.0f;
    mfBallAirResistance = g_BallAirResistance;
}

bool PhysicsBall::SetContactInfo(
    dContact* contact, PhysicsObject* other, bool param)
{
    if (m_parentObject != 0)
    {
        return m_parentObject->SetContactInfo(contact, other, param);
    }

    if (param)
    {
        SetDefaultContactInfo(contact);
    }

    int objType = other->GetObjectType();
    if (objType != 8)
    {
        if (objType == 0x12)
        {
            contact->surface.bounce = g_pGame->mpTerrain->GetRestitution(g_BallBounceGround);
        }
        else if (objType == 0x16 || objType == 0x17)
        {
            contact->surface.bounce = g_BallBounceWall;
            contact->surface.mu = g_BallFrictionWall;
        }
        else
        {
            contact->surface.bounce = g_BallBounce;
            contact->surface.mu = g_BallFriction;
        }
        contact->surface.bounce_vel = 0.0f;
    }
    return true;
}

void PhysicsBall::PreCollide()
{
    mbIsSupportedByGround = false;
}

void PhysicsBall::PreUpdate()
{
    nlVector3 vec;
    GetLinearVelocity(&vec);

    float l = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    if (l > 8100.0f)
    {
        float n = 90.0f / nlSqrt(l, true);
        nlVec3Set(vec, n * vec.x, n * vec.y, n * vec.z);
        SetLinearVelocity(vec);
    }
    PhysicsObject::PreUpdate();
    if (!mbIgnoreForces)
    {
        AddResistanceForces();
    }
}

void PhysicsBall::PostUpdate()
{
    PhysicsObject::PostUpdate();
    if (m_parentObject != 0)
    {
        return;
    }

    nlVector3 linVel;
    GetLinearVelocity(&linVel);
    float l = linVel.x * linVel.x + linVel.y * linVel.y
            + linVel.z * linVel.z;
    if (l > 8100.0f)
    {
        float f = 90.0f / nlSqrt(l, true);
        nlVec3Set(linVel, f * linVel.x, f * linVel.y, f * linVel.z);
        SetLinearVelocity(linVel);
    }

    if (GetPosition().z > 20.0f && linVel.z > 0.0f)
    {
        linVel.z *= 0.9f;
        SetLinearVelocity(linVel);
    }

    if (GetPosition().z < GetRadius() && !mbCanFreeFall
        && !mbCanGoThroughGround)
    {
        nlVector3 pos;
        GetPosition(&pos);
        pos.z = GetRadius();
        SetPosition(pos, WORLD_COORDINATES);

        if (linVel.z < 0.0f)
        {
            linVel.z *= -g_pGame->mpTerrain->GetRestitution(g_BallBounceGround);
            SetLinearVelocity(linVel);
            mfBallAirResistance = g_BallAirResistance;
        }
    }
}

void PhysicsBall::CloneBall(const PhysicsBall& other)
{
    CloneObject(other);

    u32* src = (u32*)&other.mv3TiltForce;
    u32* dst = (u32*)&mv3TiltForce;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    mbUseTiltForce = other.mbUseTiltForce;
    mbUseWindForce = other.mbUseWindForce;
    mbIsSupportedByGround = other.mbIsSupportedByGround;
    mbUseAngularVel = other.mbUseAngularVel;
    mbUseMagnusEffect = other.mbUseMagnusEffect;
    mbIgnoreForces = other.mbIgnoreForces;
    mbCanFreeFall = other.mbCanFreeFall;
    mbCanGoThroughGround = other.mbCanGoThroughGround;
    mbPassLockedIn = other.mbPassLockedIn;
    mfSpinTimer = other.mfSpinTimer;
    mfBallAirResistance = other.mfBallAirResistance;
    mfChargeBonus = other.mfChargeBonus;
}

ContactType PhysicsBall::Contact(
    PhysicsObject* other, dContact* contact, int numContacts)
{
    nlVector3 normal = v3Zero;
    nlVector3 myPos = v3Zero;
    nlVector3 pos;

    int objID = other->GetObjectType();
    fn_8013F854(
        "Ball Contact objID %d numContacts %d\n", objID, numContacts);
    GetPosition(&myPos);

    if (objID == 0x1C)
    {
        return NO_CONTACT;
    }

    if (objID == 0x12 && !mbCanFreeFall && !mbCanGoThroughGround)
    {
        dContact* c = contact;
        for (int i = 0; i < numContacts; ++i, ++c)
        {
            if (c->geom.pos[2] <= myPos.z && c->geom.normal[2] > 0.9f)
            {
                mbIsSupportedByGround = true;
                break;
            }
        }
    }

    if (m_parentObject != 0)
    {
        if (objID == 0x12)
        {
            if (mbCanFreeFall)
            {
                fn_8013F854("Ball Contact IsFreeFall\n");
                return NO_CONTACT;
            }

            GetPosition(&myPos);
            if (contact->geom.normal[2] > 0.0f
                && contact->geom.pos[2] + GetRadius() < myPos.z)
            {
                pos = GetPosition();
                float zPosAdjust = contact->geom.normal[2]
                                 * contact->geom.depth;
                pos.z += zPosAdjust;
                SetPosition(pos, WORLD_COORDINATES);

                if (contact->geom.normal[2] > 0.95f)
                {
                    fn_8013F854("Ball Contact normal[2] > 0.95\n");
                    return NO_CONTACT;
                }

                normal.x = contact->geom.normal[0];
                normal.y = contact->geom.normal[1];
                normal.z = 0.0f;
                float invLength = nlRecipSqrt(normal.GetLengthSq3D(), true);
                nlVec3Set(normal, invLength * normal.x, invLength * normal.y,
                    invLength * normal.z);
                contact->geom.normal[0] = normal.x;
                contact->geom.normal[1] = normal.y;
                contact->geom.normal[2] = normal.z;
                contact->geom.depth -= zPosAdjust;
            }
            fn_8013F854(
                "Ball contact call parent depth %f\n", contact->geom.depth);
        }
        return m_parentObject->Contact(other, contact, numContacts);
    }

    if (objID != 0x12 && objID != 0xD && objID != 0xE && objID != 8)
    {
        if (objID == 0x16)
        {
            fn_8013F854("Ball contact PHYSOBJ_GOALIE_PLANE\n");
            return TWO_WAY_CONTACT;
        }

        mbUseMagnusEffect = false;
        mfChargeBonus = 0.0f;
        if (objID == 0x17)
        {
            float absX = fabsf(GetPosition().x);
            float absY = fabsf(GetPosition().y);
            if (absX > cField::GetGoalLineX(1U) - 0.4f
                && absY < cField::GetSidelineY(1U) - 2.2f)
            {
                fn_8013F854(
                    "Ball contact PHYSOBJ_WALL %f %f\n", absX, absY);
                return TWO_WAY_CONTACT;
            }
        }

        FakeBallWorld::InvalidateBallCache();
        fn_8013F854("Ball contact IncrementBall\n");
        ++g_pBall->m_bBallPathChangeCount;
        ++g_pBall->m_bBallDeflectCount;
    }
    return TWO_WAY_CONTACT;
}

void PhysicsBall::AddResistanceForces()
{
    nlVector3 velocity;
    nlVector3 resistance;
    nlVector3 v3Torque;
    nlVector3 v3CurAngularVel;
    nlVector3 v3BallSurfaceSpeed;
    nlVector3 v3CurBallSpeed;
    nlVector3 v3CurLinVel;
    nlVector3 v3CurAngVel;
    nlVector3 v3MagnusForce;

    velocity = GetLinearVelocity();
    if (m_parentObject == 0)
    {
        if (mbIsSupportedByGround && !mbUseAngularVel)
        {
            float speed = nlVec3Length(velocity);
            if (speed > 0.01f)
            {
                float factor = -g_pGame->mpTerrain->GetRollingResistance(
                                   g_BallRollingResistance)
                             / speed;
                nlVec3Scale(resistance, velocity, factor);
                AddForceAtCentreOfMass(resistance);
            }
        }

        float drag = -mfBallAirResistance;
        resistance.x = velocity.x * drag;
        resistance.y = velocity.y * drag;
        if (velocity.z > 0.0f)
        {
            resistance.z = velocity.z * drag;
        }
        else
        {
            resistance.z = velocity.z * g_BallAirResistance;
        }
        AddForceAtCentreOfMass(resistance);
    }

    if (!mbPassLockedIn && !mbCanFreeFall)
    {
        if (mbUseTiltForce)
        {
            AddForceAtCentreOfMass(mv3TiltForce);
        }
        if (mbUseWindForce)
        {
            AddForceAtCentreOfMass(mv3WindForce);
        }
    }

    if (mbCanFreeFall)
    {
        return;
    }

    if (mfSpinTimer > 0.0f)
    {
        mfSpinTimer -= FixedUpdateTask::GetPhysicsUpdateTick();
        if (mfSpinTimer <= 0.0f)
        {
            mbUseAngularVel = true;
        }
    }

    if (m_parentObject == 0 && mbUseAngularVel && !mbCanGoThroughGround)
    {
        float threshold = 0.02f + GetRadius();
        if (GetPosition().z < threshold)
        {
            CalcAngularFromLinearVelocity(v3Torque);
            GetAngularVelocity(&v3CurAngularVel);
            nlVec3Sub(v3Torque, v3Torque, v3CurAngularVel);
            nlVec3Scale(v3Torque, 0.25f);
            dBodyAddTorque(m_bodyID, v3Torque.x, v3Torque.y, v3Torque.z);

            CalcSurfaceVelocity(v3BallSurfaceSpeed);
            GetLinearVelocity(&v3CurBallSpeed);
            nlVec3Sub(v3BallSurfaceSpeed, v3BallSurfaceSpeed, v3CurBallSpeed);
            nlVec3Scale(v3BallSurfaceSpeed, 5.0f);
            AddForceAtCentreOfMass(v3BallSurfaceSpeed);
            v3BallSurfaceSpeed.z = 0.0f;

            if (v3Torque.GetLengthSq3D() < 0.0001f
                && v3BallSurfaceSpeed.GetLengthSq3D() < 0.00003f)
            {
                mbUseAngularVel = false;
            }
        }
    }

    if (m_parentObject == 0 && mbUseMagnusEffect)
    {
        GetLinearVelocity(&v3CurLinVel);
        if (v3CurLinVel.GetLengthSq3D() > 1.0f)
        {
            GetAngularVelocity(&v3CurAngVel);
            float airResistance = mfBallAirResistance;
            float angularScale = 1.0f - lbl_806DCA7C * airResistance;
            if (angularScale > 0.0001f)
            {
                nlVec3Scale(v3CurAngVel, angularScale);
            }
            else
            {
                v3CurAngVel.x = 0.0f;
                v3CurAngVel.y = 0.0f;
                v3CurAngVel.z = 0.0f;
            }
            SetAngularVelocity(v3CurAngVel);

            if (v3CurAngVel.GetLengthSq3D() > 1.0f)
            {
                float speed = nlVec3Length(v3CurLinVel);
                float chargeValue = mfChargeBonus * lbl_806E11E8;
                float speedScale = InterpolateRangeClamped(
                    0.0f, 1.0f, 30.0f, 55.0f, speed);
                float speedValue = speedScale * (1.0f - lbl_806E11E8);
                float magnusScale = InterpolateClamped(
                    lbl_806DCA80, lbl_806DCA84, chargeValue + speedValue);

                nlVec3CrossProduct(v3MagnusForce, v3CurAngVel, v3CurLinVel);
                v3MagnusForce.x *= magnusScale;
                v3MagnusForce.y *= magnusScale;
                v3MagnusForce.z *= magnusScale;
                AddForceAtCentreOfMass(v3MagnusForce);
            }
        }
    }
}

void PhysicsBall::fn_80140C30()
{
    mfBallAirResistance = g_BallAirResistance;
}

float PhysicsBall::fn_80140C3C()
{
    return g_BallAirResistance;
}

void PhysicsBall::CalcSurfaceVelocity(nlVector3& v3VelocityOut)
{
    nlVector3 v3AngVelocity;
    GetAngularVelocity(&v3AngVelocity);
    v3AngVelocity.z = 0.0f;

    nlVector3 v3Up = { 0.0f, 0.0f, 0.0f };
    v3Up.z = GetRadius();

    nlVec3CrossProduct(v3VelocityOut, v3AngVelocity, v3Up);
}

void PhysicsBall::ScaleAngularVelocity(float scale)
{
    nlVector3 v;
    if (mbUseAngularVel)
    {
        GetAngularVelocity(&v);
        nlVec3Scale(v, scale);
        SetAngularVelocity(v);
    }
}

void PhysicsBall::SetUseAngularVelocity(bool param_1)
{
    mbUseAngularVel = false;
    if (param_1)
    {
        mfSpinTimer = 0.08f;
        return;
    }
    mfSpinTimer = 0.0f;
}

void PhysicsBall::CalcAngularFromLinearVelocity(nlVector3& v3AngularVel)
{
    nlVector3 v3Velocity;
    GetLinearVelocity(&v3Velocity);

    nlVector3 v3Up = { 0.0f, 0.0f, 0.0f };
    v3Up.z = 1.0f / GetRadius();
    nlVector3 v3Look = { 0.0f, 0.0f, 0.0f };
    v3Look.x = v3Velocity.x;
    v3Look.y = v3Velocity.y;

    nlVec3CrossProduct(v3AngularVel, v3Up, v3Look);
}
