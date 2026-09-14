#include "Game/Physics/PhysicsAIBall.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/CharacterTemplate.h"
#include "Game/DebugWriteCache.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsGoalie.h"
#include "Game/Physics/PhysicsNet.h"
#include "NL/nlSlotPool.h"
#include "math.h"
#include "types.h"

#include "Game/UnidentifiedStaticStorage.h"

extern CollisionSpace* g_CollisionSpace;
extern PhysicsWorld* g_PhysicsWorld;
extern bool gbEnableBallGoalieSweepTest;

extern "C" void fn_8013F854(const char*, ...);
extern "C" bool fn_800167A8(cBall*);
extern "C" void fn_80145C9C();
extern "C" void fn_801462DC(CollisionPlayerBallData*);
extern "C" void fn_8014658C(CollisionBallGroundData*);
extern "C" void fn_801466D4(CollisionBallWallData*);

static unsigned short sPhysicsAIBallType = 0xFFFF;
float lbl_806DCA0C = 0.05f;
float lbl_806DCA10 = 1.0f;
float lbl_806DCA14 = 10.0f;
float lbl_806DCA18 = 60.0f;
float lbl_806DCA1C = 37.5f;
float lbl_806DCA20 = 1.9f;
float lbl_806DCA24 = 0.5f;
float lbl_806DCA28 = 2.0f;
float lbl_806DCA2C = 5.0f;
float lbl_806DCA30 = 6.0f;
float lbl_806DCA34 = 0.7f;
float lbl_806DCA38 = 0.7f;
float lbl_806DCA3C = 0.3f;
float lbl_806DCA40 = 0.3f;
float lbl_806DCA44 = 0.35f;
float lbl_806DCA48 = 1.0f;
float lbl_806DCA4C = 0.005f;
float lbl_806DCA50 = 0.3f;
static float sfMaxBallBounceSpeed = 15.0f;
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

    if (objID == 0x1D)
    {
        return fn_800167A8(m_pAIBall) ? NO_CONTACT : ONE_WAY_CONTACT_THIS;
    }

    if (objID == 0xD || objID == 0xE)
    {
        if (gbEnableBallGoalieSweepTest)
        {
            PhysicsCharacter* physicsCharacter
                = (PhysicsCharacter*)obj->m_parentObject;
            cFielder* pFielder = (cFielder*)physicsCharacter->m_pAICharacter;

            if (pFielder->m_eClassType == 3)
            {
                if (!mbBallSpeedBelowSweepTestThreshold || !mbCanCollideGoalie)
                {
                    return NO_CONTACT;
                }
            }
            else if (pFielder->m_eClassType == 2)
            {
                if (!mbHasHitPlayer && pFielder != m_pAIBall->m_pShooter
                    && !mbCanCollidePlayer)
                {
                    return NO_CONTACT;
                }
            }
        }
    }

    if (PhysicsNet::sbSweepTestEnabled)
    {
        if (PhysicsNet::IsAGoalPost(obj))
        {
            return NO_CONTACT;
        }
    }

    if (m_parentObject == 0)
    {
        if (mbCanFreeFall)
        {
            return NO_CONTACT;
        }

        if (objID == 0x17 || objID == 5)
        {
            if (mbIsInsideNet)
            {
                return NO_CONTACT;
            }

            nlVector3 ballPosition;
            GetPosition(&ballPosition);
            float radius = GetRadius();

            if (fabsf(ballPosition.x) > cField::GetGoalLineX(1U) - 2.0f * radius)
            {
                if (fabsf(ballPosition.y) < 0.5f * cNet::GetNetWidth() - radius
                    && fabsf(ballPosition.z) < cNet::GetNetHeight() - radius)
                {
                    return NO_CONTACT;
                }
                mbGoalPlaneContact = true;
            }

            if (m_pAIBall->m_tNoPickupTimer.m_uPackedTime != 0)
            {
                if (m_pAIBall->HasActivePassTarget()
                    || m_pAIBall->m_tLightningTimer.m_uPackedTime != 0
                    || m_pAIBall->meBallState == 1)
                {
                    return NO_CONTACT;
                }
            }

            dContact* pContact = info;
            bool hitWall = false;
            for (int i = 0; i < numContacts; i++)
            {
                if (pContact->geom.normal[2] < 0.08f)
                {
                    hitWall = true;
                    break;
                }
                pContact++;
            }

            if (hitWall || objID == 5)
            {
                nlVector3 contactPos;
                nlVec3Set(contactPos, info->geom.pos[0], info->geom.pos[1],
                    info->geom.pos[2]);

                if (GameInfoManager::Instance()->GetStadium() == 11
                    && objID == 0x17)
                {
                    bool bAboveHeight = contactPos.z > 0.36f;
                    float sideDist = fabsf(contactPos.y) - GetRadius();
                    bool bOutsideSideline = sideDist > cField::GetSidelineY(1U);
                    bool bInsideGoalLine = fabsf(contactPos.x)
                                         < cField::GetGoalLineX(1U) - 1.05f;
                    if (bInsideGoalLine && bAboveHeight)
                    {
                        if (bOutsideSideline)
                        {
                            fn_80145C9C();
                        }
                        return NO_CONTACT;
                    }
                }

                nlVector3 ballVelocity = GetLinearVelocity();
                nlPolar aBallSpeed;
                nlCartesianToPolar(aBallSpeed, ballVelocity.x, ballVelocity.y);

                if (aBallSpeed.r > 1.0f)
                {
                    CollisionBallWallData* pEventData
                        = g_CollisionBallWallDataPool.Allocate();
                    pEventData->pBall = m_pAIBall;
                    pEventData->bIsPerfect = false;
                    float speedSq = nlVec3LengthSquared(ballVelocity);
                    pEventData->bIsShot
                        = m_pAIBall->m_tLightningTimer.m_uPackedTime != 0;
                    nlVec3Set(pEventData->position, contactPos.x, contactPos.y,
                        contactPos.z);
                    nlVec3Set(pEventData->normal, info->geom.normal[0],
                        info->geom.normal[1], info->geom.normal[2]);
                    pEventData->fCollisionVecLen = nlSqrt(speedSq, true);
                    fn_801466D4(pEventData);
                    ScaleAngularVelocity(0.9f);
                }

                if (aBallSpeed.r > sfMaxBallBounceSpeed)
                {
                    float scale = sfMaxBallBounceSpeed / aBallSpeed.r;
                    nlVector3 vel;
                    nlVec3Scale(vel, ballVelocity, scale);
                    SetLinearVelocity(vel);
                }
            }
        }
        else if (objID == 7)
        {
            if (!mbIsInsideNet)
            {
                return NO_CONTACT;
            }

            if (PhysicsNet::IsAGoalWall(obj))
            {
                info->surface.soft_cfm = lbl_806DCAB4;
            }
            else if (PhysicsNet::IsGoalWallSetA(obj))
            {
                info->surface.soft_cfm = lbl_806DCAB0;
            }
            else if (PhysicsNet::IsGoalWallSetB(obj))
            {
                info->surface.soft_cfm = PhysicsNet::sfWallSoftness;
            }
        }
        else if (objID == 0x12)
        {
            nlVector3 v3IncidentVel = GetLinearVelocity();
            if (m_pAIBall->m_tNoPickupTimer.m_uPackedTime == 0
                && v3IncidentVel.z > 1.0f)
            {
                CollisionBallGroundData* pEventData
                    = g_CollisionBallGroundDataPool.Allocate();
                pEventData->pBall = g_pBall;
                if (g_pBall->m_tLightningTimer.m_uPackedTime != 0)
                {
                    pEventData->bIsShot = true;
                }
                else
                {
                    pEventData->bIsShot = false;
                }
                GetPosition(&pEventData->position);
                pEventData->normal.x = 0.0f;
                pEventData->normal.y = 0.0f;
                pEventData->normal.z = 1.0f;
                pEventData->fVecZComponent = v3IncidentVel.z;
                fn_8014658C(pEventData);
            }
        }
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

    if (gbEnableBallGoalieSweepTest && mbCanCollideGoalie)
    {
        CheckIfBallWentThroughGoalie();
    }
    if (PhysicsNet::sbSweepTestEnabled)
    {
        CheckIfBallWentThroughGoalPost();
    }

    mbGoalPlaneContact = false;

    if (m_pAIBall->GetOwnerGoalie() != 0)
    {
        ((Goalie*)m_pAIBall->GetOwnerGoalie())
            ->GetPhysicsGoalie()
            ->CollideHeldBallWithGoalLine();
    }

    GetRadius();
    nlVector3 ballPosition;
    GetPosition(&ballPosition);

    if (IsBallOutsideNet(ballPosition))
    {
        mbIsInsideNet = false;
    }
    else
    {
        nlVector3 oldPosition;
        nlVector3 newPosition;
        GetPosition(&newPosition);
        oldPosition = m_pAIBall->m_v3PrevPosition;
        if (DidBallJustEnterNet(oldPosition, newPosition))
        {
            mbIsInsideNet = true;
        }
    }

    const nlVector3& v3Vel = GetLinearVelocity();
    mbBallSpeedBelowSweepTestThreshold
        = nlVec3LengthSquared(v3Vel)
        < sfBallGoalieSweepTestVelocityThreshold
              * sfBallGoalieSweepTestVelocityThreshold;
}

void PhysicsAIBall::CheckIfBallWentThroughGoalie()
{
    if (mbBallSpeedBelowSweepTestThreshold)
    {
        return;
    }

    nlVector3 oldPosition;
    nlVector3 newPosition;
    GetPosition(&newPosition);
    oldPosition = m_prevPosition;

    cCharacter* pGoalie0 = g_pCharacters[8];
    cCharacter* pGoalie1 = g_pCharacters[9];
    bool bCanCollide0 = pGoalie0->m_pPhysicsCharacter->m_CanCollideWithBall;
    bool bCanCollide1 = pGoalie1->m_pPhysicsCharacter->m_CanCollideWithBall;

    if (!bCanCollide0
        || newPosition.x * pGoalie0->mUnidentified024.m_v3Position.x < 0.0f)
    {
        pGoalie0 = 0;
    }
    if (!bCanCollide1
        || newPosition.x * pGoalie1->mUnidentified024.m_v3Position.x < 0.0f)
    {
        pGoalie1 = 0;
    }

    Goalie* pGoalie;
    if (pGoalie0 == 0)
    {
        if (pGoalie1 == 0)
        {
            return;
        }
        pGoalie = (Goalie*)pGoalie1;
    }
    else if (pGoalie1 == 0)
    {
        pGoalie = (Goalie*)pGoalie0;
    }
    else if (CalculateDistanceSquared(
                 pGoalie0->mUnidentified024.m_v3Position, newPosition)
             < CalculateDistanceSquared(
                 pGoalie1->mUnidentified024.m_v3Position, newPosition))
    {
        pGoalie = (Goalie*)pGoalie0;
    }
    else
    {
        pGoalie = (Goalie*)pGoalie1;
    }

    if (pGoalie->m_pBall != 0)
    {
        return;
    }

    nlVector3 ballPosition = { 0.0f, 0.0f, 0.0f };
    nlVector3 contactNormal = { 0.0f, 0.0f, 0.0f };
    bool contact = false;

    if (m_goalieContactFramesAgo > 3)
    {
        float radius = GetRadius();
        contact = pGoalie->GetPhysicsGoalie()->SweepTestForBallContact(
            oldPosition, newPosition, GetLinearVelocity(), radius,
            ballPosition, contactNormal);
    }

    if (contact)
    {
        SetPosition(ballPosition, WORLD_COORDINATES);

        dContact contactInfo;
        SetContactInfo(&contactInfo, pGoalie->m_pPhysicsCharacter, true);

        contactInfo.geom.normal[0] = contactNormal.x;
        contactInfo.geom.normal[1] = contactNormal.y;
        contactInfo.geom.normal[2] = contactNormal.z;
        contactInfo.geom.g1 = m_geomID;
        contactInfo.geom.g2 = 0;

        nlVector3 contactPos;
        nlVec3ScaleAdd(contactPos, -GetRadius(), contactNormal, ballPosition);

        contactInfo.geom.pos[0] = contactPos.x;
        contactInfo.geom.pos[1] = contactPos.y;
        contactInfo.geom.pos[2] = contactPos.z;
        contactInfo.geom.depth = 0.0f;

        if (!pGoalie->PreCollideWithBallCallback(contactInfo))
        {
            return;
        }

        FakeBallWorld::InvalidateBallCache();
        mbUseMagnusEffect = false;
        mfChargeBonus = 0.0f;

        nlVector3 v3Vel;
        GetLinearVelocity(&v3Vel);

        CollisionPlayerBallData* pEventData
            = g_CollisionPlayerBallDataPool.Allocate();
        pEventData->pPlayer = pGoalie;
        pEventData->pBall = m_pAIBall;
        pEventData->velocity = v3Vel;
        pEventData->boneID = 0;
        fn_801462DC(pEventData);

        m_pAIBall->ClearBallBlur();
        m_goalieContactFramesAgo = 0;

        nlVector3 v3ExitVel;
        nlVector3 v3Projection;
        nlVec3Project(v3Projection, v3Vel, contactNormal);
        nlVec3ScaleAdd(v3ExitVel, -2.0f, v3Projection, v3Vel);

        SaveData* pSaveData = pGoalie->mpSaveData;
        if (pSaveData != 0 && (pSaveData->muSaveType & 0x38) != 0)
        {
            nlVector3 v3DeflectFudge;
            RotateVectorZAxis(v3DeflectFudge, v3ExitVel,
                (u16)-pGoalie->mUnidentified024.m_aActualFacingDirection);

            float exitSpeed = nlVec3Length(v3ExitVel);
            exitSpeed = nlMinEquals(
                nlMaxEquals(exitSpeed, lbl_806DCA14), lbl_806DCA18);

            float scale;
            if (exitSpeed <= lbl_806DCA1C)
            {
                scale = InterpolateRangeClamped(exitSpeed * lbl_806DCA20,
                    exitSpeed, lbl_806DCA14, lbl_806DCA1C, exitSpeed);
            }
            else
            {
                scale = InterpolateRangeClamped(exitSpeed,
                    exitSpeed * lbl_806DCA24, lbl_806DCA1C, lbl_806DCA18,
                    exitSpeed);
            }

            v3DeflectFudge.x += lbl_806DCA28;

            float saveY = pSaveData->mv3SavePos.y;
            v3DeflectFudge.y = saveY;
            if (saveY > 0.0f)
            {
                v3DeflectFudge.y += lbl_806DCA2C;
            }
            else
            {
                v3DeflectFudge.y -= lbl_806DCA2C;
            }

            v3DeflectFudge.z = lbl_806DCA30 + pSaveData->mv3SavePos.z;
            v3DeflectFudge.y = v3DeflectFudge.y
                             * (scale * (lbl_806DCA34 + nlRandomf(lbl_806DCA3C)));
            v3DeflectFudge.z = v3DeflectFudge.z
                             * (scale * (lbl_806DCA38 + nlRandomf(lbl_806DCA40)));

            RotateVectorZAxis(v3DeflectFudge, v3DeflectFudge,
                pGoalie->mUnidentified024.m_aActualFacingDirection);

            nlVecLerp(v3ExitVel, v3ExitVel, v3DeflectFudge, lbl_806DCA10);
        }

        nlVec3Scale(v3ExitVel, lbl_806DCA0C);
        SetLinearVelocity(v3ExitVel);
    }

    ++m_goalieContactFramesAgo;
}

void PhysicsAIBall::CheckIfBallWentThroughGoalPost()
{
    if (m_parentObject == 0)
    {
        nlVector3 oldPosition;
        nlVector3 newPosition;
        GetPosition(&newPosition);
        oldPosition = m_prevPosition;

        nlVector3 ballPosition = { 0.0f, 0.0f, 0.0f };
        nlVector3 contactNormal = { 0.0f, 0.0f, 0.0f };
        bool contact;
        PhysicsObject* physicsObject = 0;

        if (oldPosition.x > 0.0f)
        {
            PhysicsNet* pNet = PhysicsNet::spPhysNetPositiveX;
            float radius = GetRadius();
            contact = pNet->SweepTestForBallContact(oldPosition, newPosition,
                GetLinearVelocity(), radius, ballPosition, contactNormal,
                &physicsObject);
        }
        else
        {
            PhysicsNet* pNet = PhysicsNet::spPhysNetNegativeX;
            float radius = GetRadius();
            contact = pNet->SweepTestForBallContact(oldPosition, newPosition,
                GetLinearVelocity(), radius, ballPosition, contactNormal,
                &physicsObject);
        }

        if (contact && !mbGoalPlaneContact)
        {
            if (nlVec3LengthSquared(contactNormal) < 0.001f)
            {
                newPosition.y = ballPosition.y;
                SetPosition(newPosition, WORLD_COORDINATES);
                mbUseMagnusEffect = false;
                mfChargeBonus = 0.0f;
                FakeBallWorld::InvalidateBallCache();
                ++m_pAIBall->m_bBallPathChangeCount;
                return;
            }

            nlVec3ScaleAdd(ballPosition, lbl_806DCA4C, contactNormal, ballPosition);

            const nlVector3& v3BallVel = GetLinearVelocity();
            nlVector3 v3ExitVel;
            nlVector3 v3Projection;
            nlVec3Project(v3Projection, v3BallVel, contactNormal);
            nlVec3ScaleAdd(v3ExitVel, -2.0f, v3Projection, v3BallVel);
            nlVec3Scale(v3ExitVel, lbl_806DCA44);

            if (nlVec3LengthSquared(v3BallVel) < 1.0f)
            {
                if (ballPosition.x > 0.0f)
                {
                    v3ExitVel.x -= lbl_806DCA50;
                }
                else
                {
                    v3ExitVel.x += lbl_806DCA50;
                }

                float dt = lbl_806DCA50 * FixedUpdateTask::GetPhysicsUpdateTick();
                nlVec3ScaleAdd(ballPosition, dt, v3ExitVel, ballPosition);
            }

            nlVector3 v3AngVel;
            GetAngularVelocity(&v3AngVel);
            nlVec3Scale(v3AngVel, lbl_806DCA48);

            SetPosition(ballPosition, WORLD_COORDINATES);
            SetLinearVelocity(v3ExitVel);
            SetAngularVelocity(v3AngVel);
            mbUseMagnusEffect = false;
            mfChargeBonus = 0.0f;
            FakeBallWorld::InvalidateBallCache();
            ++m_pAIBall->m_bBallPathChangeCount;
        }
    }
}

bool PhysicsAIBall::DidBallJustEnterNet(
    const nlVector3& oldPosition, nlVector3 newPosition)
{
    float absOldX = fabsf(oldPosition.x);
    float absNewX = fabsf(newPosition.x);
    float goalLineX = cField::GetGoalLineX(1U)
                    + g_pBall->m_pPhysicsBall->GetRadius() - 0.08f;

    if (absOldX < goalLineX && absNewX >= goalLineX)
    {
        nlVector3 interpolatedPosition;
        float deltaX = newPosition.x - oldPosition.x;
        if (fabsf(deltaX) > 0.0001f)
        {
            float alpha = newPosition.x > 0.0f ? goalLineX : -goalLineX;
            alpha = (alpha - oldPosition.x) / deltaX;
            nlVecLerp(interpolatedPosition, oldPosition, newPosition, alpha);
        }
        else
        {
            interpolatedPosition = newPosition;
        }

        if (interpolatedPosition.z
                < cNet::GetNetHeight() - g_pBall->m_pPhysicsBall->GetRadius()
            && interpolatedPosition.y > 0.5f * -cNet::GetNetWidth()
            && interpolatedPosition.y < 0.5f * cNet::GetNetWidth())
        {
            return true;
        }
    }
    else if (absNewX >= goalLineX)
    {
        if (newPosition.z
                < cNet::GetNetHeight() - g_pBall->m_pPhysicsBall->GetRadius()
            && newPosition.y > 0.5f * -cNet::GetNetWidth()
            && newPosition.y < 0.5f * cNet::GetNetWidth())
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

void fn_8013F854(const char*, ...)
{
}

inline void PhysicsAIBall::RegisterDebugFields(
    unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("PhysicsAIBall");
    PhysicsBall::RegisterDebugFields(type, cache);

#define REGISTER_FIELD(kind, field) \
    cache->AddField(kind, gDebugFieldTypes[kind].size, \
        (unsigned char*)&field - (unsigned char*)&mv3TiltForce, #field)

    REGISTER_FIELD(22, m_prevPosition);
    REGISTER_FIELD(8, m_goalieContactFramesAgo);
    REGISTER_FIELD(16, mbIsInsideNet);
    REGISTER_FIELD(16, mbGoalPlaneContact);
    REGISTER_FIELD(16, mbBallSpeedBelowSweepTestThreshold);
    REGISTER_FIELD(16, mbHasHitPlayer);
    REGISTER_FIELD(16, mbCanCollidePlayer);
    REGISTER_FIELD(16, mbCanCollideGoalie);

#undef REGISTER_FIELD

    cache->EndType();
}

inline void PhysicsAIBall::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sPhysicsAIBallType == 0xFFFF)
    {
        RegisterDebugFields(&sPhysicsAIBallType, cache);
    }

    unsigned int offset
        = (unsigned char*)&mv3TiltForce - (unsigned char*)this;
    void* data = (unsigned char*)this + offset;
    cache->ChecksumData(sPhysicsAIBallType, data, context);
    cache->WriteData(sPhysicsAIBallType, data, sizeof(PhysicsAIBall) - offset);
}
