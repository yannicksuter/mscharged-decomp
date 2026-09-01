#include "Game/Physics/PhysicsNet.h"

#include "Game/FixedUpdateTask.h"
#include "Game/Ball.h"
#include "Game/Field.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Team.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/platvmath.h"
#include "NL/utility.h"
#include "math.h"
#include "types.h"

extern cTeam* g_pTeams[];

struct CollisionBallGoalpostData
{
    nlVector3 v3CollisionVelocity;
    nlVector3 v3CollisionPosition;
    unsigned int uTeamIndex;
};

extern SlotPool<CollisionBallGoalpostData> lbl_80571758;
extern "C" void fn_8014681C(CollisionBallGoalpostData*);

PhysicsNet* PhysicsNet::spPhysNetNegativeX = 0;
PhysicsNet* PhysicsNet::spPhysNetPositiveX = 0;
float PhysicsNet::sfPhysicsNetWidth = 0.0f;
float PhysicsNet::sfPhysicsNetHeight = 0.0f;
float PhysicsNet::sfPhysicsNetDepth = 0.0f;
static bool sbTestLowerHorizontalGoalpost = false;

bool PhysicsNet::sbSweepTestEnabled = true;
float PhysicsNet::sfWallSoftness = 0.01f;

PhysicsNet::PhysicsNet(CollisionSpace* space, bool positive_x)
{
    float physicsNetHeight;
    float goalPostRadius;
    float sideSign;
    float netHeight;
    float netWidth;
    float goalLineX;
    float zero;
    float halfDepth;
    float halfHeight;
    float planeX;
    float halfWidth;
    float halfWidthWithError;
    float backX;
    float angledX;
    float angledHeight;
    float sin45;
    float cos45;

    mpBackWall = 0;
    mpTopWall = 0;
    mpAngledWall = 0;
    mpGoalWallA1 = 0;
    mpGoalWallA2 = 0;
    mpGoalWallB1 = 0;
    mpGoalWallB2 = 0;
    mpSideGoalPost1 = 0;
    mpSideGoalPost2 = 0;
    mpTopGoalPost = 0;
    mpNetMesh = 0;
    errorCorrectionDepth = 0.0f;

    static float ballMaxMotionPerTick = PhysicsBall::GetBallMaxVelocity() * FixedUpdateTask::GetPhysicsUpdateTick();

    errorCorrectionDepth = ballMaxMotionPerTick;

    netHeight = cNet::m_fNetHeight;
    netWidth = cNet::m_fNetWidth;
    goalLineX = cField::GetGoalLineX((unsigned int)1);
    goalPostRadius = cNet::m_fNetPostRadius;
    if (positive_x)
    {
        sideSign = 1.0f;
    }
    else
    {
        sideSign = -1.0f;
    }
    zero = 0.0f;

    if (NetMesh::s_bAnimatedNetMeshEnabled)
    {
        if (positive_x)
        {
            nlVector3 netPosition = { 0.0f, 0.0f, 0.0f };
            netPosition.x = goalLineX;
            mpNetMesh = new (nlMalloc(sizeof(NetMesh), 8, false)) NetMesh(true);
        }
        else
        {
            nlVector3 netPosition = { 0.0f, 0.0f, 0.0f };
            netPosition.x = -goalLineX;
            mpNetMesh = new (nlMalloc(sizeof(NetMesh), 8, false)) NetMesh(false);
        }
    }

    nlVector3 center;
    nlVector3 vector1;
    nlVector3 vector2;
    nlVector3 postPosition;
    nlMatrix4 rotation;

    halfDepth = 0.5f * (sfPhysicsNetDepth + errorCorrectionDepth);
    physicsNetHeight = sfPhysicsNetHeight;
    halfHeight = 0.5f * (physicsNetHeight + errorCorrectionDepth);
    halfWidth = 0.5f * sfPhysicsNetWidth;
    halfWidthWithError = 0.5f * sfPhysicsNetWidth + errorCorrectionDepth;
    planeX = sideSign * ((goalLineX + halfDepth) - zero);
    backX = sideSign * ((goalLineX + sfPhysicsNetDepth) - zero);
    nlSinCos(&sin45, &cos45, 0x2000);
    angledX = sideSign * (((2.2f + goalLineX) - zero));
    angledHeight = 1.7f;

    nlVec3Set(center, planeX, halfWidth - 0.3f, halfHeight);
    nlVec3Set(vector1, halfDepth, 0.0f, 0.0f);
    nlVec3Set(vector2, 0.0f, 0.0f, halfHeight);
    mpGoalWallA1 = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false)) PhysicsFinitePlane(
        g_CollisionSpace, center, vector1, vector2, true, errorCorrectionDepth);

    nlVec3Set(center, planeX, 0.3f - halfWidth, halfHeight);
    nlVec3Set(vector1, halfDepth, 0.0f, 0.0f);
    nlVec3Set(vector2, 0.0f, 0.0f, halfHeight);
    mpGoalWallA2 = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false)) PhysicsFinitePlane(
        g_CollisionSpace, center, vector2, vector1, true, errorCorrectionDepth);

    nlVec3Set(center, planeX, halfWidth, halfHeight);
    nlVec3Set(vector1, halfDepth, 0.0f, 0.0f);
    nlVec3Set(vector2, 0.0f, 0.0f, halfHeight);
    mpGoalWallB1 = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false)) PhysicsFinitePlane(
        g_CollisionSpace, center, vector1, vector2, true, errorCorrectionDepth);

    nlVec3Set(center, planeX, -halfWidth, halfHeight);
    nlVec3Set(vector1, halfDepth, 0.0f, 0.0f);
    nlVec3Set(vector2, 0.0f, 0.0f, halfHeight);
    mpGoalWallB2 = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false)) PhysicsFinitePlane(
        g_CollisionSpace, center, vector2, vector1, true, errorCorrectionDepth);

    nlVec3Set(center, backX, 0.0f, halfHeight);
    nlVec3Set(vector1, 0.0f, sideSign * halfWidthWithError, 0.0f);
    nlVec3Set(vector2, 0.0f, 0.0f, halfHeight);
    mpBackWall = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false)) PhysicsFinitePlane(
        g_CollisionSpace, center, vector2, vector1, true, errorCorrectionDepth);

    nlVec3Set(center, planeX, 0.0f, physicsNetHeight);
    nlVec3Set(vector1, 0.0f, halfWidthWithError, 0.0f);
    nlVec3Set(vector2, halfDepth, 0.0f, 0.0f);
    mpTopWall = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false)) PhysicsFinitePlane(
        g_CollisionSpace, center, vector1, vector2, true, errorCorrectionDepth);

    nlVec3Set(center, angledX, 0.0f, angledHeight);
    nlVec3Set(vector1, 0.0f, sideSign * halfWidthWithError, 0.0f);
    nlVec3Set(vector2, sin45 * (-sideSign * halfHeight), 0.0f, halfHeight * cos45);
    mpAngledWall = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false)) PhysicsFinitePlane(
        g_CollisionSpace, center, vector2, vector1, true, errorCorrectionDepth);

    mpBackWall->SetCategory(0x200);
    mpBackWall->SetCollide(0x20);
    mpTopWall->SetCategory(0x200);
    mpTopWall->SetCollide(0x20);
    mpAngledWall->SetCategory(0x200);
    mpAngledWall->SetCollide(0x20);
    mpGoalWallA1->SetCategory(0x200);
    mpGoalWallA1->SetCollide(0x20);
    mpGoalWallA2->SetCategory(0x200);
    mpGoalWallA2->SetCollide(0x20);
    mpGoalWallB1->SetCategory(0x200);
    mpGoalWallB1->SetCollide(0x20);
    mpGoalWallB2->SetCategory(0x200);
    mpGoalWallB2->SetCollide(0x20);

    if (!PhysicsNet::sbSweepTestEnabled)
    {
        double f0 = 0.2;
        goalPostRadius = (float)(goalPostRadius + f0);

        mpSideGoalPost1 = new (nlMalloc(sizeof(PhysicsCapsule), 8, false)) PhysicsCapsule(g_CollisionSpace, 0, goalPostRadius, netHeight);
        mpSideGoalPost2 = new (nlMalloc(sizeof(PhysicsCapsule), 8, false)) PhysicsCapsule(g_CollisionSpace, 0, goalPostRadius, netHeight);
        mpTopGoalPost = new (nlMalloc(sizeof(PhysicsCapsule), 8, false)) PhysicsCapsule(g_CollisionSpace, 0, goalPostRadius, netWidth);

        nlVec3Set(postPosition, (positive_x) ? goalLineX : -goalLineX, 0.5f * netWidth, 0.5f * netHeight);
        mpSideGoalPost1->SetPosition(postPosition, (PhysicsObject::CoordinateType)0);

        postPosition.y = -0.5f * netWidth;
        mpSideGoalPost2->SetPosition(postPosition, (PhysicsObject::CoordinateType)0);

        postPosition.y = 0.0f;
        postPosition.z = netHeight;
        mpTopGoalPost->SetPosition(postPosition, (PhysicsObject::CoordinateType)0);

        nlMakeRotationMatrixX(rotation, 1.5707964f);
        mpTopGoalPost->SetRotation(rotation);
    }

    mpNet = g_pTeams[0]->m_pNet;
    const float sign = mpNet->m_fDirection;
    if (((sign < 0.0f) && (positive_x != 0)) || ((sign > 0.0f) && (positive_x == 0)))
    {
        mpNet = g_pTeams[1]->m_pNet;
    }
}

PhysicsNet::~PhysicsNet()
{
    delete mpBackWall;
    delete mpTopWall;
    delete mpAngledWall;
    delete mpGoalWallA1;
    delete mpGoalWallA2;
    delete mpGoalWallB1;
    delete mpGoalWallB2;
    delete mpSideGoalPost1;
    delete mpSideGoalPost2;
    delete mpTopGoalPost;

    if (mpNetMesh != 0)
    {
        delete mpNetMesh;
    }
}

bool PhysicsNet::IsAGoalPost(PhysicsObject* obj)
{
    if ((obj == spPhysNetNegativeX->mpSideGoalPost1)
        || (obj == spPhysNetNegativeX->mpSideGoalPost2)
        || (obj == spPhysNetNegativeX->mpTopGoalPost)
        || (obj == spPhysNetPositiveX->mpSideGoalPost1)
        || (obj == spPhysNetPositiveX->mpSideGoalPost2)
        || (obj == spPhysNetPositiveX->mpTopGoalPost))
    {
        return true;
    }
    return false;
}

bool PhysicsNet::IsAGoalWall(PhysicsObject* obj)
{
    if ((obj == spPhysNetNegativeX->mpBackWall)
        || (obj == spPhysNetNegativeX->mpTopWall)
        || (obj == spPhysNetNegativeX->mpAngledWall)
        || (obj == spPhysNetPositiveX->mpBackWall)
        || (obj == spPhysNetPositiveX->mpTopWall)
        || (obj == spPhysNetPositiveX->mpAngledWall))
    {
        return true;
    }
    return false;
}

bool PhysicsNet::IsGoalWallSetA(PhysicsObject* obj)
{
    if ((obj == spPhysNetNegativeX->mpGoalWallA1)
        || (obj == spPhysNetNegativeX->mpGoalWallA2)
        || (obj == spPhysNetPositiveX->mpGoalWallA1)
        || (obj == spPhysNetPositiveX->mpGoalWallA2))
    {
        return true;
    }
    return false;
}

bool PhysicsNet::IsGoalWallSetB(PhysicsObject* obj)
{
    if ((obj == spPhysNetNegativeX->mpGoalWallB1)
        || (obj == spPhysNetNegativeX->mpGoalWallB2)
        || (obj == spPhysNetPositiveX->mpGoalWallB1)
        || (obj == spPhysNetPositiveX->mpGoalWallB2))
    {
        return true;
    }
    return false;
}

bool PhysicsNet::SweepTestForBallContact(const nlVector3& startPos, const nlVector3& endPos, const nlVector3& ballVelocity, float ballRadius, nlVector3& contactPos, nlVector3& contactNormal, PhysicsObject** hitObject) const
{
    nlVector3 leftPostPos, rightPostPos;
    nlVector3 ballLinVelocity;
    nlVector3 ballAngularVelocity;
    CollisionBallGoalpostData* pEventData;
    nlVector3 goalPost0Location;
    nlVector3 goalPost1Location;
    nlVector3 goalPostSpherePos0;
    nlVector3 ballLinearVelocity;
    nlVector3 goalPostSpherePos1;
    float height;
    bool hitHorizontalGoalpost;
    bool hitLeftVerticalGoalpost;
    bool hitRightVerticalGoalpost;
    float absEndX;
    float absLeftPostX;
    float startZ;
    float endZ;
    float normalY;
    float normalX;
    float normalZ;

    cField::GetGoalLineX((unsigned int)1);
    float netPostRadius = cNet::m_fNetPostRadius;
    float radiusSum = netPostRadius + ballRadius;

    hitHorizontalGoalpost = false;
    hitLeftVerticalGoalpost = false;
    hitRightVerticalGoalpost = false;

    height = cNet::m_fNetHeight;

    mpNet->GetPostLocation(leftPostPos, 0, 0.0f);
    mpNet->GetPostLocation(rightPostPos, 1, 0.0f);

    absEndX = (float)fabs(endPos.x);
    absLeftPostX = (float)fabs(leftPostPos.x);

    if (absEndX < (absLeftPostX - radiusSum))
    {
        return false;
    }

    float absEndY = (float)fabs(endPos.y);
    float absStartY = (float)fabs(startPos.y);
    float minAbsY = absEndY <= absStartY ? absEndY : absStartY;
    if (minAbsY > (radiusSum + (float)fabs(leftPostPos.y)))
    {
        return false;
    }

    startZ = startPos.z;
    endZ = endPos.z;
    float minZ = endZ <= startZ ? endZ : startZ;
    if (minZ > (height + radiusSum))
    {
        return false;
    }

    if (absEndX > (absLeftPostX + netPostRadius))
    {
        float halfNetWidth = 0.5f * sfPhysicsNetWidth;
        float outerNetWidth = halfNetWidth + (0.1f + 2.0f * ballRadius);

        if ((absStartY <= halfNetWidth) && (absEndY > halfNetWidth))
        {
            nlVec3Set(contactNormal, 0.0f, 0.0f, 0.0f);
            if (absEndY > (absStartY + 0.0001f))
            {
                float alpha = (halfNetWidth - absStartY) / (absEndY - absStartY);
                nlVecLerp(contactPos, startPos, endPos, alpha);
            }
            else
            {
                contactPos = startPos;
            }
            return true;
        }

        if ((absStartY >= outerNetWidth) && (absEndY < outerNetWidth))
        {
            if (absEndY < (absStartY - 0.0001f))
            {
                float alpha = (absStartY - outerNetWidth) / (absStartY - absEndY);
                nlVecLerp(contactPos, startPos, endPos, alpha);
            }
            else
            {
                contactPos = startPos;
            }

            g_pBall->SetPosition(contactPos);

            ballLinVelocity.x = 0.0f;
            ballLinVelocity.y = -0.2f * ballVelocity.y;
            ballLinVelocity.z = 0.2f * ballVelocity.z;
            g_pBall->m_pPhysicsBall->GetAngularVelocity(&ballAngularVelocity);
            nlVec3Scale(ballAngularVelocity, 0.3f);
            g_pBall->SetVelocity(
                ballLinVelocity, SPINTYPE_PARAMETER, &ballAngularVelocity);
            PhysicsBall* physicsBall = g_pBall->m_pPhysicsBall;
            physicsBall->mUnidentified054 = false;
            physicsBall->mUnidentified064 = 0.0f;
            return false;
        }
    }

    goalPost0Location = leftPostPos;
    goalPost0Location.z = startPos.z;
    goalPost1Location = leftPostPos;
    goalPost1Location.z = endPos.z;

    float sweepResult = SweepSpheres(ballRadius, startPos, endPos, netPostRadius, goalPost0Location, goalPost1Location);
    if (sweepResult >= 0.0f && sweepResult <= 1.0f)
    {
        hitLeftVerticalGoalpost = true;
    }

    if ((sweepResult < 0.0f) || (sweepResult > 1.0f))
    {
        goalPost0Location = rightPostPos;
        goalPost0Location.z = startPos.z;
        goalPost1Location = rightPostPos;
        goalPost1Location.z = endPos.z;

        sweepResult = SweepSpheres(ballRadius, startPos, endPos, netPostRadius, goalPost0Location, goalPost1Location);
        if (sweepResult >= 0.0f && sweepResult <= 1.0f)
        {
            hitRightVerticalGoalpost = true;
        }
    }

    float horizontalPostHeight = cNet::m_fNetHeight;
    if (sbTestLowerHorizontalGoalpost)
    {
        horizontalPostHeight = 0.5f * cNet::m_fNetHeight;
    }

    goalPost0Location = rightPostPos;
    goalPost0Location.y = startPos.y;
    goalPost0Location.z = horizontalPostHeight;
    goalPost1Location = rightPostPos;
    goalPost1Location.y = endPos.y;
    goalPost1Location.z = horizontalPostHeight;

    float sweepResult3 = SweepSpheres(ballRadius, startPos, endPos, netPostRadius, goalPost0Location, goalPost1Location);
    if (sweepResult3 >= 0.0f && sweepResult3 <= 1.0f)
    {
        hitHorizontalGoalpost = true;
    }

    float finalSweepResult = -1.0f;

    if ((hitHorizontalGoalpost != 0) && ((hitLeftVerticalGoalpost != 0) || (hitRightVerticalGoalpost != 0)))
    {
        if (sweepResult3 < sweepResult)
        {
            finalSweepResult = sweepResult3;
            hitLeftVerticalGoalpost = 0;
            hitRightVerticalGoalpost = 0;
        }
        else
        {
            finalSweepResult = sweepResult;
            hitHorizontalGoalpost = 0;
        }
    }
    else if (hitHorizontalGoalpost != 0)
    {
        finalSweepResult = sweepResult3;
    }
    else if ((hitLeftVerticalGoalpost != 0) || (hitRightVerticalGoalpost != 0))
    {
        finalSweepResult = sweepResult;
    }

    if ((hitLeftVerticalGoalpost) || (hitRightVerticalGoalpost) || (hitHorizontalGoalpost))
    {

        if (hitLeftVerticalGoalpost != 0)
        {
            goalPost0Location = leftPostPos;
            goalPost0Location.z = startPos.z;

            goalPost1Location = leftPostPos;
            goalPost1Location.z = endPos.z;
        }
        else if (hitRightVerticalGoalpost != 0)
        {
            goalPost0Location = rightPostPos;
            goalPost0Location.z = startPos.z;

            goalPost1Location = rightPostPos;
            goalPost1Location.z = endPos.z;
        }
        else if (hitHorizontalGoalpost != 0)
        {
            float endY = endPos.y;
            float startY = startPos.y;
            float goalHeight = cNet::m_fNetHeight;
            goalPost0Location = rightPostPos;
            goalPost0Location.y = startY;
            goalPost0Location.z = goalHeight;

            goalPost1Location = rightPostPos;
            goalPost1Location.y = endY;
            goalPost1Location.z = goalHeight;
        }

        nlVec3Sub(goalPostSpherePos0, endPos, startPos);
        nlVec3ScaleAdd(contactPos, finalSweepResult, goalPostSpherePos0, startPos);

        if (hitHorizontalGoalpost != 0)
        {
            goalPost0Location.y = contactPos.y;
        }
        else
        {
            goalPost0Location.z = contactPos.z;
        }

        normalY = contactPos.y - goalPost0Location.y;
        normalX = contactPos.x - goalPost0Location.x;
        normalZ = contactPos.z - goalPost0Location.z;
        nlVec3Set(contactNormal, normalX, normalY, normalZ);

        float normalLength = nlRecipSqrt(contactNormal.x * contactNormal.x + contactNormal.y * contactNormal.y + contactNormal.z * contactNormal.z, true);

        height = ballRadius + netPostRadius;
        nlVec3Scale(contactNormal, normalLength);
        nlVec3Sub(goalPostSpherePos1, contactPos, goalPost0Location);
        if (nlGetLengthSquared3D(goalPostSpherePos1.x, goalPostSpherePos1.y, goalPostSpherePos1.z) < (height * height))
        {
            nlVec3ScaleAdd(contactPos, height, contactNormal, goalPost0Location);
        }

        ballLinearVelocity = g_pBall->m_pPhysicsBall->GetLinearVelocity();

        float velocitySquared = ballLinearVelocity.x * ballLinearVelocity.x + ballLinearVelocity.y * ballLinearVelocity.y + ballLinearVelocity.z * ballLinearVelocity.z;

        if (velocitySquared > 25.0f)
        {
            pEventData = 0;
            lbl_80571758.Allocate(pEventData);
            pEventData->v3CollisionVelocity = ballLinearVelocity;
            pEventData->v3CollisionPosition = contactPos;
            pEventData->uTeamIndex = (g_pBall->m_v3Position.x < 0.0f) ? 0 : 1;
            fn_8014681C(pEventData);
        }
        return true;
    }
    return false;
}

void PhysicsNet::StaticInit(CollisionSpace* pCollisionSpace)
{
    spPhysNetPositiveX = new (nlMalloc(sizeof(PhysicsNet), 8, false)) PhysicsNet(g_CollisionSpace, true);
    spPhysNetNegativeX = new (nlMalloc(sizeof(PhysicsNet), 8, false)) PhysicsNet(g_CollisionSpace, false);
}

void PhysicsNet::StaticDestroy()
{
    delete spPhysNetNegativeX;
    delete spPhysNetPositiveX;
}
