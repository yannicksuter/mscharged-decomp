#include "Game/Physics/PhysicsGoalie.h"
#include "Game/AI/AiUtil.h"
#include "Game/Ball.h"
#include "Game/Field.h"
#include "Game/Goalie.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Team.h"
#include "NL/utility.h"
#include "math.h"

#include "types.h"

static const f32 CANT_COLLIDE = 3.402823466e+38F;

// Retail addresses this runtime-initialised constant through the small
// read-only data base (r2), so it lives in .sbss2 rather than .sbss.
__declspec(section ".sdata2") static const float ballMaxMotionPerTick =
    PhysicsBall::GetBallMaxVelocity()
    * FixedUpdateTask::GetPhysicsUpdateTick();

void PhysicsGoalie::PostUpdate()
{
    PhysicsCharacter::PostUpdate();
    CollideGoalieWithPost();
}

bool PhysicsGoalie::SweepTestForBallContact(
    const nlVector3& ballPrevPosition,
    const nlVector3& ballCurrentPosition, const nlVector3& velocity,
    float ballRadius, nlVector3& positionWhenHit,
    nlVector3& contactNormal) const
{
    int testsPassed = 0;
    float goalieRadius = 4.0f * m_CentreOfMassHeight;

    nlVector3 goaliePos;
    GetPosition(&goaliePos);
    goaliePos.z = (2.0 * m_CentreOfMassHeight) + goaliePos.z;

    if (IsBallNearGoalie(
            goalieRadius, ballRadius, goaliePos, ballPrevPosition))
    {
        testsPassed = 1;
        if (BigBallSweepTest(goalieRadius, ballRadius, goaliePos,
                ballPrevPosition, ballCurrentPosition))
        {
            testsPassed = 2;
            if (SweepTestEveryBone(ballRadius, ballPrevPosition,
                    ballCurrentPosition, contactNormal,
                    positionWhenHit))
            {
                testsPassed = 3;
            }
        }
    }

    return testsPassed == 3;
}

bool PhysicsGoalie::IsBallNearGoalie(float goalieRadius,
    float ballRadius, const nlVector3& goaliePos,
    const nlVector3& ballPrevPosition)
{
    // Retail writes a zero to one axis of each of these vectors and never
    // reads them back.
    nlVector3 clearedY;
    nlVector3 clearedZ;
    nlVector3 clearedX;
    clearedX.x = 0.0f;
    clearedY.y = 0.0f;
    clearedZ.z = 0.0f;

    nlVector3 diff;
    nlVec3Sub(diff, ballPrevPosition, goaliePos);
    return (nlVec3Length(diff)
               - (goalieRadius + (ballRadius + ballMaxMotionPerTick)))
        <= 0.0f;
}

bool PhysicsGoalie::BigBallSweepTest(float goalieRadius,
    float ballRadius, const nlVector3& goaliePos,
    const nlVector3& ballPrevPosition,
    const nlVector3& ballCurrentPosition)
{
    float time = SweepSpheres(ballRadius, ballPrevPosition,
        ballCurrentPosition, goalieRadius, goaliePos, goaliePos);

    if ((time == CANT_COLLIDE) || (time < 0.0f) || (time > 1.0f))
    {
        return false;
    }
    return true;
}

bool PhysicsGoalie::SweepTestEveryBone(float ballRadius,
    const nlVector3& ballPrevPosition,
    const nlVector3& ballCurrentPosition, nlVector3& contactNormal,
    nlVector3& positionWhenHit) const
{
    nlVector3 normalAccumulator = { 0.0f, 0.0f, 0.0f };
    float smallestTime = 99999.0f;

    nlListConstIterator<PhysicsBoneVolume*> boneVolumeIterator
        = m_BoneVolumes.Begin();
    PhysicsBoneVolume* boneVolume;
    const nlVector3* bonePreviousPosition;
    bool detectedContact = false;
    int numContactsDetected = 0;
    if (!boneVolumeIterator.IsValid())
    {
        return false;
    }

    while (boneVolumeIterator.IsValid())
    {
        boneVolume = boneVolumeIterator.Current();
        PhysicsSphere* physSphere = (PhysicsSphere*)boneVolume->m_pObject;
        const nlVector3& boneCurrentPosition = physSphere->GetPosition();
        bonePreviousPosition = &boneVolume->m_PrevPosition;
        float time = SweepSpheres(ballRadius, ballPrevPosition,
            ballCurrentPosition, physSphere->GetRadius(),
            *bonePreviousPosition, boneCurrentPosition);

        if ((time != CANT_COLLIDE) && (time > 0.0f) && (time < 1.0f))
        {
            if (time < smallestTime)
            {
                smallestTime = time;
            }

            const float oneMinusTime = 1.0f - time;
            nlVector3 bonePositionWhenHit;

            nlVec3WeightedSum(positionWhenHit, oneMinusTime,
                ballPrevPosition, time, ballCurrentPosition);
            nlVec3WeightedSum(bonePositionWhenHit, oneMinusTime,
                *bonePreviousPosition, time, boneCurrentPosition);
            nlVec3Sub(contactNormal, positionWhenHit, bonePositionWhenHit);
            float invLength
                = nlRecipSqrt(contactNormal.GetLengthSq3D(), true);
            nlVec3Set(contactNormal, invLength * contactNormal.x,
                invLength * contactNormal.y, invLength * contactNormal.z);

            detectedContact = true;
            numContactsDetected += 1;

            nlVec3Add(normalAccumulator, normalAccumulator, contactNormal);
        }

        boneVolumeIterator.Next();
    }

    if (detectedContact)
    {
        float oneMinusTime = 1.0f - smallestTime;
        contactNormal.z
            = (1.0f / (float)numContactsDetected) * normalAccumulator.z;
        contactNormal.y
            = (1.0f / (float)numContactsDetected) * normalAccumulator.y;
        contactNormal.x
            = (1.0f / (float)numContactsDetected) * normalAccumulator.x;
        nlVec3WeightedSum(positionWhenHit, oneMinusTime,
            ballPrevPosition, smallestTime, ballCurrentPosition);
    }

    return detectedContact;
}

void PhysicsGoalie::CollideGoalieWithPost()
{
    Goalie* pGoalie = (Goalie*)m_pAICharacter;
    nlVector3 v3GoaliePos = GetPosition();
    v3GoaliePos.z = 0.0f;

    cNet* pNet = pGoalie->m_pTeam->m_pNet;
    nlVector3 v3PostPos;
    nlVector3 v3PrevHeadJointPos
        = pGoalie->GetPrevJointPosition(pGoalie->m_nHeadJointIndex);

    if (v3PrevHeadJointPos.y > 0.0f)
    {
        pNet->GetPostLocation(v3PostPos, 1, 0.0f);
    }
    else
    {
        pNet->GetPostLocation(v3PostPos, 0, 0.0f);
    }

    if (nlVec3DistanceSquared2D(v3PostPos, v3PrevHeadJointPos) < 4.0f)
    {
        float fJointRadius[3] = { 0.15f, 0.2f, 0.2f };
        float postRadius = cNet::GetPostRadius();
        float headDistLimitSq = nlGetLengthSquared1D(1.0f + postRadius);

        nlVector3 v3JointPos[3];

        v3JointPos[0]
            = pGoalie->GetJointPosition(pGoalie->m_nHeadJointIndex);
        v3JointPos[1]
            = pGoalie->GetJointPosition(pGoalie->m_nRightHandJointIndex);
        v3JointPos[2]
            = pGoalie->GetJointPosition(pGoalie->m_nLeftHandJointIndex);

        nlVector3* pJointPos = v3JointPos;
        float* pJointRadius = fJointRadius;
        u8 bMoved = 0;

        for (int i = 0; i < 3; i++, pJointPos++, pJointRadius++)
        {
            nlVector3 v3JointWorldPos = *pJointPos;
            v3JointWorldPos.z = v3PostPos.z;

            float fMinDist = postRadius + (*pJointRadius);
            float jointDistSq
                = nlVec3DistanceSquared2D(v3PostPos, v3JointWorldPos);

            if (i == 0)
            {
                if (jointDistSq < headDistLimitSq)
                {
                    nlVector3 v3Norm;
                    v3Norm.x = v3JointWorldPos.y - v3PrevHeadJointPos.y;
                    v3Norm.y = v3PrevHeadJointPos.x - v3JointWorldPos.x;
                    v3Norm.z = 0.0f;

                    if (v3Norm.GetLengthSq3D() < 0.00001f)
                    {
                        v3Norm.x = 0.0f;
                        v3Norm.y = v3PostPos.y;
                    }
                    else if ((v3PostPos.x * v3Norm.x) < 0.0f)
                    {
                        v3Norm.x *= -1.0f;
                        v3Norm.y *= -1.0f;
                    }

                    nlVector4 v4Plane;
                    MakePerpendicularPlane(
                        v3JointWorldPos, v3Norm, v4Plane, 0.0f);

                    float fCurDist = nlPlaneSide(v3PostPos, v4Plane);
                    float fCurDistAbs = (float)fabs(fCurDist);

                    if (fCurDistAbs < fMinDist)
                    {
                        float fJointDist = nlSqrt(jointDistSq, true);
                        float fMoveDist = InterpolateRangeClamped(0.0f,
                            fMinDist - fCurDistAbs, 1.0f + postRadius, 0.0f,
                            fJointDist);

                        if ((fCurDist > 0.0f) || m_CanCollideWithGoalLine)
                        {
                            fMoveDist *= -1.0f;
                        }

                        v3GoaliePos.z += fMoveDist * v4Plane.z;
                        v3GoaliePos.y += fMoveDist * v4Plane.y;
                        v3GoaliePos.x += fMoveDist * v4Plane.x;
                        bMoved = 1;
                    }
                }
            }
            else
            {
                if (jointDistSq < (fMinDist * fMinDist))
                {
                    nlVector3 v3JointDist;
                    nlVec3Sub(v3JointDist, v3JointWorldPos, v3PostPos);

                    if (jointDistSq > 0.00001f)
                    {
                        float fCurDist = nlSqrt(jointDistSq, true);
                        float fScale = (fMinDist - fCurDist) / fCurDist;

                        nlVec3ScaleAdd(
                            v3GoaliePos, fScale, v3JointDist, v3GoaliePos);
                        bMoved = 1;
                    }
                }
            }
        }

        if (bMoved)
        {
            SetCharacterPosition(v3GoaliePos);
        }
    }

    CollideHeldBallWithGoalLine();
}

void PhysicsGoalie::CollideHeldBallWithGoalLine()
{
    Goalie* pGoalie = (Goalie*)m_pAICharacter;
    if (m_CanCollideWithGoalLine && pGoalie->m_pBall != 0)
    {
        PhysicsAIBall* pPhysicsBall = pGoalie->m_pBall->m_pPhysicsBall;
        float fGoalLineLimit
            = cField::GetGoalLineX(1u) - pPhysicsBall->GetRadius();
        const nlVector3& v3BallPos = pPhysicsBall->GetPosition();
        float fOvershoot = (float)fabs(v3BallPos.x) - fGoalLineLimit;

        if (fOvershoot > 0.0f)
        {
            nlVector3 v3GoaliePos = GetPosition();
            nlVector3 v3NewBallPos = v3BallPos;

            fOvershoot = (v3BallPos.x > 0.0f) ? fOvershoot : -fOvershoot;

            v3GoaliePos.x -= fOvershoot;
            v3GoaliePos.z = 0.0f;
            SetCharacterPosition(v3GoaliePos);

            v3NewBallPos.x -= fOvershoot;
            pPhysicsBall->SetPosition(v3NewBallPos, WORLD_COORDINATES);
        }
    }
}
