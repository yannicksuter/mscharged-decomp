#include "Game/Physics/PhysicsAIBall.h"

#include "Game/Ball.h"
#include "Game/CharacterTemplate.h"
#include "Game/Field.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Goalie.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsGoalie.h"
#include "Game/Physics/PhysicsNet.h"
#include "math.h"
#include "types.h"

extern CollisionSpace* g_CollisionSpace;
extern PhysicsWorld* g_PhysicsWorld;

extern "C" void fn_8013F854(const char*, ...)
{
}

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);

static unsigned short sPhysicsAIBallType = 0xFFFF;
static float sfMaxBallBounceSpeed = 20.0f;
static float sfBallGoalieSweepTestVelocityThreshold = 5.0f;

PhysicsAIBall::PhysicsAIBall(float radius)
    : PhysicsBall(g_CollisionSpace, g_PhysicsWorld, radius)
    , m_pAIBall(0)
    , m_goalieContactFramesAgo(9999)
    , mbIsInsideNet(false)
    , mbGoalPlaneContact(false)
    , mbBallSpeedBelowSweepTestThreshold(false)
    , mbHasHitPlayer(false)
    , mbCanCollidePlayer(true)
    , mbCanCollideGoalie(true)
{
    m_prevPosition.x = 0.0f;
    m_prevPosition.y = 0.0f;
    m_prevPosition.z = 0.0f;
}

void PhysicsAIBall::Unknown0()
{
    PhysicsBall::Unknown0();
    m_goalieContactFramesAgo = 9999;
    mbIsInsideNet = false;
    mbGoalPlaneContact = false;
    mbBallSpeedBelowSweepTestThreshold = false;
    mbHasHitPlayer = false;
    mbCanCollidePlayer = true;
    mbCanCollideGoalie = true;
    m_prevPosition.x = 0.0f;
    m_prevPosition.y = 0.0f;
    m_prevPosition.z = 0.0f;
}

ContactType PhysicsAIBall::Contact(
    PhysicsObject* obj, dContact* info, int numContacts)
{
    int objID = obj->GetObjectType();
    fn_8013F854(
        "AIBall Contact objID %d numContacts %d\n", objID, numContacts);

    if ((objID == 0xD || objID == 0xE)
        && !mbCanCollidePlayer)
    {
        return NO_CONTACT;
    }

    if (objID == 0x1D && m_pAIBall != 0)
    {
        return NO_CONTACT;
    }

    if (PhysicsNet::sbSweepTestEnabled && PhysicsNet::IsAGoalPost(obj))
    {
        return NO_CONTACT;
    }

    if (m_parentObject == 0 && (objID == 0x17 || objID == 5))
    {
        if (mbIsInsideNet)
        {
            return NO_CONTACT;
        }

        nlVector3 ballPosition;
        GetPosition(&ballPosition);
        float radius = GetRadius();
        if (fabsf(ballPosition.x)
                > cField::GetGoalLineX(1U) - 2.0f * radius
            && fabsf(ballPosition.y) < 0.5f * cNet::m_fNetWidth - radius
            && fabsf(ballPosition.z) < cNet::m_fNetHeight - radius)
        {
            return NO_CONTACT;
        }
        mbGoalPlaneContact = true;
    }

    return PhysicsBall::Contact(obj, info, numContacts);
}

void PhysicsAIBall::PreUpdate()
{
    mbHasHitPlayer = false;
    PhysicsBall::PreUpdate();
    m_prevPosition = GetPosition();
}

void PhysicsAIBall::PostUpdate()
{
    nlVector3 v3IncidentVel;
    GetLinearVelocity(&v3IncidentVel);
    PhysicsBall::PostUpdate();

    extern bool gbEnableBallGoalieSweepTest;
    if (gbEnableBallGoalieSweepTest)
    {
        CheckIfBallWentThroughGoalie();
    }
    if (PhysicsNet::sbSweepTestEnabled)
    {
        CheckIfBallWentThroughGoalPost();
    }

    mbGoalPlaneContact = false;

    nlVector3 ballPosition;
    GetPosition(&ballPosition);
    float radius = GetRadius();
    float threshold = cField::GetGoalLineX(1U) + radius - 0.08f;
    if (fabsf(ballPosition.x) < threshold)
    {
        mbIsInsideNet = false;
    }
    else if (m_pAIBall != 0
             && DidBallJustEnterNet(m_pAIBall->m_v3PrevPosition, ballPosition))
    {
        mbIsInsideNet = true;
    }

    const nlVector3& v3Vel = GetLinearVelocity();
    float velocitySq = nlVec3DotProduct(v3Vel, v3Vel);
    mbBallSpeedBelowSweepTestThreshold
        = velocitySq
        < sfBallGoalieSweepTestVelocityThreshold
              * sfBallGoalieSweepTestVelocityThreshold;
}

void PhysicsAIBall::CheckIfBallWentThroughGoalie()
{
    if (mbBallSpeedBelowSweepTestThreshold)
    {
        return;
    }

    nlVector3 newPosition;
    GetPosition(&newPosition);

    cPlayer* pGoaliePlayer = (cPlayer*)g_pCharacters[8];
    if (pGoaliePlayer == 0)
    {
        return;
    }
    Goalie* pGoalie = (Goalie*)pGoaliePlayer;

    if (newPosition.x * pGoaliePlayer->m_v3Position.x < 0.0f)
    {
        pGoalie = (Goalie*)g_pCharacters[9];
    }
    if (pGoalie == 0 || pGoalie->m_pBall != 0
        || pGoalie->m_tNoPickupTimer.m_uPackedTime != 0)
    {
        return;
    }

    nlVector3 ballPosition = { 0.0f, 0.0f, 0.0f };
    nlVector3 contactNormal = { 0.0f, 0.0f, 0.0f };
    bool contact = false;
    if (m_goalieContactFramesAgo > 3)
    {
        contact = pGoalie->GetPhysicsGoalie()->SweepTestForBallContact(
            m_prevPosition, newPosition, GetLinearVelocity(), GetRadius(), ballPosition, contactNormal);
    }

    if (contact)
    {
        SetPosition(ballPosition, WORLD_COORDINATES);

        const nlVector3& v3Vel = GetLinearVelocity();
        float normalLengthSq
            = nlVec3DotProduct(contactNormal, contactNormal);
        if (normalLengthSq > 0.0f)
        {
            float reflectScale
                = nlVec3DotProduct(v3Vel, contactNormal) / normalLengthSq;
            nlVector3 v3ExitVel;
            v3ExitVel.x
                = v3Vel.x - 2.0f * reflectScale * contactNormal.x;
            v3ExitVel.y
                = v3Vel.y - 2.0f * reflectScale * contactNormal.y;
            v3ExitVel.z
                = v3Vel.z - 2.0f * reflectScale * contactNormal.z;
            v3ExitVel.x *= 0.175f;
            v3ExitVel.y *= 0.175f;
            v3ExitVel.z *= 0.175f;
            SetLinearVelocity(v3ExitVel);
        }

        mbUseMagnusEffect = false;
        FakeBallWorld::InvalidateBallCache();
        if (m_pAIBall != 0)
        {
            m_pAIBall->ClearBallBlur();
            ++m_pAIBall->m_bBallPathChangeCount;
        }
        m_goalieContactFramesAgo = 0;
    }

    ++m_goalieContactFramesAgo;
}

void PhysicsAIBall::CheckIfBallWentThroughGoalPost()
{
    if (m_parentObject != 0)
    {
        return;
    }

    nlVector3 oldPosition = m_prevPosition;
    nlVector3 newPosition;
    GetPosition(&newPosition);
    nlVector3 ballPosition = { 0.0f, 0.0f, 0.0f };
    nlVector3 contactNormal = { 0.0f, 0.0f, 0.0f };
    PhysicsObject* physicsObject = 0;

    PhysicsNet* pNet = oldPosition.x > 0.0f
                         ? PhysicsNet::spPhysNetPositiveX
                         : PhysicsNet::spPhysNetNegativeX;
    if (pNet == 0)
    {
        return;
    }

    const nlVector3& v3BallVel = GetLinearVelocity();
    bool contact = pNet->SweepTestForBallContact(oldPosition, newPosition, v3BallVel, GetRadius(), ballPosition, contactNormal, &physicsObject);
    if (!contact || mbGoalPlaneContact)
    {
        return;
    }

    ballPosition.x += 0.005f * contactNormal.x;
    ballPosition.y += 0.005f * contactNormal.y;
    ballPosition.z += 0.005f * contactNormal.z;

    float normalLengthSq = nlVec3DotProduct(contactNormal, contactNormal);
    if (normalLengthSq > 0.0f)
    {
        float reflectScale
            = nlVec3DotProduct(v3BallVel, contactNormal) / normalLengthSq;
        nlVector3 v3ExitVel;
        v3ExitVel.x
            = v3BallVel.x - 2.0f * reflectScale * contactNormal.x;
        v3ExitVel.y
            = v3BallVel.y - 2.0f * reflectScale * contactNormal.y;
        v3ExitVel.z
            = v3BallVel.z - 2.0f * reflectScale * contactNormal.z;
        v3ExitVel.x *= 0.35f;
        v3ExitVel.y *= 0.35f;
        v3ExitVel.z *= 0.35f;

        float velocitySq = nlVec3DotProduct(v3BallVel, v3BallVel);
        if (velocitySq < 1.0f)
        {
            if (ballPosition.x > 0.0f)
            {
                v3ExitVel.x -= 0.3f;
            }
            else
            {
                v3ExitVel.x += 0.3f;
            }

            float physicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
            float dt = 0.3f * physicsTick;
            ballPosition.x += dt * v3ExitVel.x;
            ballPosition.y += dt * v3ExitVel.y;
            ballPosition.z += dt * v3ExitVel.z;
        }

        nlVector3 v3AngVel;
        GetAngularVelocity(&v3AngVel);
        v3AngVel.x *= 0.8f;
        v3AngVel.y *= 0.8f;
        v3AngVel.z *= 0.8f;

        SetPosition(ballPosition, WORLD_COORDINATES);
        SetLinearVelocity(v3ExitVel);
        SetAngularVelocity(v3AngVel);
        mbUseMagnusEffect = false;
        FakeBallWorld::InvalidateBallCache();
        if (m_pAIBall != 0)
        {
            ++m_pAIBall->m_bBallPathChangeCount;
        }
    }
}

bool PhysicsAIBall::DidBallJustEnterNet(
    const nlVector3& oldPosition, nlVector3 newPosition)
{
    float goalLineX
        = cField::GetGoalLineX(1U) + g_pBall->m_pPhysicsBall->GetRadius()
        - 0.08f;
    float absOldX = fabsf(oldPosition.x);
    float absNewX = fabsf(newPosition.x);

    if (absOldX < goalLineX && absNewX >= goalLineX)
    {
        nlVector3 interpolatedPosition;
        float deltaX = newPosition.x - oldPosition.x;
        if (fabsf(deltaX) > 0.0001f)
        {
            float alpha = newPosition.x > 0.0f ? goalLineX : -goalLineX;
            alpha = (alpha - oldPosition.x) / deltaX;
            nlVecLerp(
                interpolatedPosition, oldPosition, newPosition, alpha);
        }
        else
        {
            interpolatedPosition = newPosition;
        }

        if (interpolatedPosition.z > 0.0f
            && interpolatedPosition.z < cNet::m_fNetHeight
            && interpolatedPosition.y > -0.5f * cNet::m_fNetWidth
            && interpolatedPosition.y < 0.5f * cNet::m_fNetWidth)
        {
            return true;
        }
    }
    return false;
}

bool PhysicsAIBall::IsBallOutsideNet(const nlVector3& ballPosition)
{
    float radius = g_pBall->m_pPhysicsBall->GetRadius();
    return fabsf(ballPosition.x)
         < cField::GetGoalLineX(1U) + radius - 0.08f;
}

void PhysicsAIBall::RegisterDebugFields(
    unsigned short* type, DebugWriteCache* cache)
{
    *type = fn_80338EBC(cache, "PhysicsAIBall");

#define REGISTER_FIELD(kind, field) \
    fn_80338F88(cache, kind, lbl_80533C98[kind].size, (unsigned char*)&field - (unsigned char*)&mv3TiltForce, #field)

    REGISTER_FIELD(22, mv3TiltForce);
    REGISTER_FIELD(22, mv3WindForce);
    REGISTER_FIELD(16, mbUseTiltForce);
    REGISTER_FIELD(16, mbUseWindForce);
    REGISTER_FIELD(16, mbIsSupportedByGround);
    REGISTER_FIELD(16, mbUseAngularVel);
    REGISTER_FIELD(16, mbUseMagnusEffect);
    REGISTER_FIELD(16, mbIgnoreForces);
    REGISTER_FIELD(16, mbCanFreeFall);
    REGISTER_FIELD(16, mbCanGoThroughGround);
    REGISTER_FIELD(16, mbPassLockedIn);
    REGISTER_FIELD(17, mfSpinTimer);
    REGISTER_FIELD(17, mfBallAirResistance);
    REGISTER_FIELD(17, mfChargeBonus);
    REGISTER_FIELD(22, m_prevPosition);
    REGISTER_FIELD(8, m_goalieContactFramesAgo);
    REGISTER_FIELD(16, mbIsInsideNet);
    REGISTER_FIELD(16, mbGoalPlaneContact);
    REGISTER_FIELD(16, mbBallSpeedBelowSweepTestThreshold);
    REGISTER_FIELD(16, mbHasHitPlayer);
    REGISTER_FIELD(16, mbCanCollidePlayer);
    REGISTER_FIELD(16, mbCanCollideGoalie);

#undef REGISTER_FIELD

    fn_80338F78(cache);
}

void PhysicsAIBall::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sPhysicsAIBallType == 0xFFFF)
    {
        RegisterDebugFields(&sPhysicsAIBallType, cache);
    }

    unsigned int offset
        = (unsigned char*)&mv3TiltForce - (unsigned char*)this;
    void* data = (unsigned char*)this + offset;
    fn_80339450(cache, sPhysicsAIBallType, data, context);
    fn_8033930C(cache, sPhysicsAIBallType, data, sizeof(PhysicsAIBall) - offset);
}
