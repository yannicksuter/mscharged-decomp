#include "Game/Physics/PhysicsFakeBall.h"

#include "Game/AI/AiUtil.h"
#include "Game/Field.h"
#include "Game/MathHelpers.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Player.h"
#include "NL/nlDLRing.h"
#include "math.h"
#include "types.h"

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

static void ClearBallCache();

SlotPool<BallCacheInfo> BallCacheInfo::mBallCacheInfoSlotPool(16, 16);
FakeBallWorld* FakeBallWorld::mpPredictWorld;
nlDLListSlotPool<BallCacheInfo*> FakeBallWorld::mBallCacheList;
float FakeBallWorld::mfLastCacheTime = -1.0f;
nlDLListIterator<BallCacheInfo*>* FakeBallWorld::mpCacheIterator;


class SimpleCollisionSpace : public CollisionSpace
{
public:
    SimpleCollisionSpace(PhysicsWorld*, bool);
    virtual ~SimpleCollisionSpace() { }
};

void FakeBallWorld::Init(cBall* pBall)
{
    if (mpPredictWorld == 0)
    {
        mpPredictWorld = new (nlMalloc(sizeof(FakeBallWorld), 8, false))
            FakeBallWorld(pBall);
    }

    ClearBallCache();
}

void FakeBallWorld::Destroy()
{
    if (mpPredictWorld != 0)
    {
        delete mpPredictWorld;
        mpPredictWorld = 0;
    }

    ClearBallCache();
    mBallCacheList.m_Allocator.FreeBlocks();
    BallCacheInfo::mBallCacheInfoSlotPool.FreeBlocks();
}

static void ClearBallCache()
{
    if (!FakeBallWorld::mBallCacheList.IsEmpty())
    {
        nlDLListIterator<BallCacheInfo*> iter
            = FakeBallWorld::mBallCacheList.Begin();
        while (iter.hasNext())
        {
            BallCacheInfo::mBallCacheInfoSlotPool.Free(*iter);
            iter.next();
        }
        FakeBallWorld::mBallCacheList.Clear();
    }

    FakeBallWorld::mfLastCacheTime = -1.0f;
    if (FakeBallWorld::mpPredictWorld != 0)
    {
        FakeBallWorld::mpPredictWorld->mbHitSuccess = false;
        FakeBallWorld::mpPredictWorld->mUnidentified1D = false;
    }
}

void FakeBallWorld::InvalidateBallCache()
{
    ClearBallCache();
}

BallCacheInfo* FakeBallWorld::AddCacheEntry(
    float fTime, PhysicsBall* pPhysicsBall)
{
    DLListEntry<BallCacheInfo*>* pNewEntry;
    BallCacheInfo* pNewInfo;
    BallCacheInfo::mBallCacheInfoSlotPool.AllocateForReturn(pNewInfo);
    pNewInfo->mfTime = FakeBallWorld::mfLastCacheTime;
    pNewInfo->mv3Position = ((PhysicsObject*)pPhysicsBall)->GetPosition();
    pNewInfo->mv3LinearVelocity
        = ((PhysicsObject*)pPhysicsBall)->GetLinearVelocity();

    pNewEntry = mBallCacheList.m_Allocator.Allocate();
    if (pNewEntry != 0)
    {
        pNewEntry->m_next = 0;
        pNewEntry->m_prev = 0;
        pNewEntry->entry = pNewInfo;
    }
    nlDLRingAddEnd(&mBallCacheList.m_Head, pNewEntry);

    float fUnidentified0 = (float)fabs(pPhysicsBall->GetPosition().x);
    float fUnidentified1
        = cField::GetGoalLineX(1U) - pPhysicsBall->GetRadius();
    if (fUnidentified0 >= fUnidentified1)
    {
        mpPredictWorld->mUnidentified1D = true;
    }

    return pNewInfo;
}

bool FakeBallWorld::GetPredictedBallPosition(float fDeltaTime,
    nlVector3& v3Position, nlVector3& v3Velocity)
{
    FakeBallWorld* predictWorld = mpPredictWorld;
    cBall* pBall = predictWorld->GetBall();
    if (pBall->m_pOwner != 0)
    {
        v3Position = pBall->m_v3Position;
        v3Velocity
            = predictWorld->mpBall->m_pOwner->mUnidentified024.m_v3Velocity;
        return false;
    }

    float fSimTime;
    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    fSimTime = GetFixedUpdateTask()->mSimulationTime;
    if (mfLastCacheTime < fSimTime)
    {
        ClearBallCache();
    }
    else if (!mBallCacheList.IsEmpty())
    {
        BallCacheInfo* pLast = 0;
        nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
        while (iter.hasNext())
        {
            BallCacheInfo* pCur = *iter;
            if (fSimTime >= pCur->mfTime)
            {
                if (pLast != 0)
                {
                    mBallCacheList.RemoveStart(&pLast);
                    BallCacheInfo::mBallCacheInfoSlotPool.Free(pLast);
                }
                pLast = pCur;
                iter.next();
            }
            else
            {
                if (pLast != 0
                    && fSimTime - pLast->mfTime
                           < pCur->mfTime - fSimTime)
                {
                    pCur = pLast;
                }

                float distSq = CalculateDistanceSquared(
                    pCur->mv3Position,
                    mpPredictWorld->mpBall->m_v3Position);
                if (!(distSq > 0.0025f))
                {
                    break;
                }
                ClearBallCache();
                break;
            }
        }
    }

    float fTargetTime = fSimTime + fDeltaTime;
    if (mfLastCacheTime < fTargetTime)
    {
        while (mfLastCacheTime < fTargetTime)
        {
            if (mfLastCacheTime < fSimTime)
            {
                mpPredictWorld->mpPhysicsBall->CloneBall(
                    *mpPredictWorld->mpBall->m_pPhysicsBall);
                mfLastCacheTime = fSimTime;
            }
            else
            {
                PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
                mfLastCacheTime += fPhysicsTick;
            }
            AddCacheEntry(mfLastCacheTime,
                (PhysicsBall*)mpPredictWorld->mpPhysicsBall);
        }
    }

    float overshoot = mfLastCacheTime - fTargetTime;
    BallCacheInfo* pPrev;
    BallCacheInfo* pNext;
    if (fDeltaTime < overshoot)
    {
        nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
        pNext = *iter;
        pPrev = pNext;
        while (!nlDLRingIsEnd(iter.m_Head, iter.m_Curr)
               && pNext->mfTime < fTargetTime)
        {
            pPrev = pNext;
            iter.next();
            pNext = *iter;
        }
    }
    else
    {
        nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.End();
        pNext = *iter;
        pPrev = pNext;
        while (!nlDLRingIsStart(iter.m_Head, iter.m_Curr)
               && pPrev->mfTime >= fTargetTime)
        {
            pNext = pPrev;
            iter.Retreat();
            pPrev = *iter;
        }
    }

    if (pNext != pPrev && fTargetTime > pPrev->mfTime)
    {
        float fPercent = (fTargetTime - pPrev->mfTime)
                       / (pNext->mfTime - pPrev->mfTime);
        if (fPercent < 1.0f)
        {
            nlVecLerp(v3Position,
                pPrev->mv3Position,
                pNext->mv3Position,
                fPercent);
            nlVecLerp(v3Velocity, pPrev->mv3LinearVelocity, pNext->mv3LinearVelocity, fPercent);
        }
        else
        {
            v3Position = pNext->mv3Position;
            v3Velocity = pNext->mv3LinearVelocity;
        }
    }
    else
    {
        v3Position = pPrev->mv3Position;
        v3Velocity = pPrev->mv3LinearVelocity;
    }
    return true;
}

float FakeBallWorld::GetPredictedPlaneIntersectTime(
    const nlVector4& v4Plane, nlVector3& v3ContactPoint,
    nlVector3& v3ContactVelocity)
{
    cBall* pBall = mpPredictWorld->mpBall;
    float fDist = pBall->m_v3Position.x * v4Plane.x
                + pBall->m_v3Position.y * v4Plane.y
                + pBall->m_v3Position.z * v4Plane.z - v4Plane.w;
    if (fDist < 0.0f)
    {
        return -1.0f;
    }

    float fVelDot = pBall->m_v3Velocity.x * v4Plane.x
                  + pBall->m_v3Velocity.y * v4Plane.y
                  + pBall->m_v3Velocity.z * v4Plane.z;
    if (fVelDot >= 0.0f)
    {
        return -2.0f;
    }
    if (!GetPredictedBallPosition(
            0.0f, v3ContactPoint, v3ContactVelocity))
    {
        return -2.5f;
    }

    float fSimulationTime;
    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    fSimulationTime = GetFixedUpdateTask()->mSimulationTime;

    if (!mBallCacheList.IsEmpty())
    {
        nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
        BallCacheInfo* pPrev;
        BallCacheInfo* pNext = *iter;

        float fDistanceNext = pNext->mv3Position.x * v4Plane.x
                            + pNext->mv3Position.y * v4Plane.y
                            + pNext->mv3Position.z * v4Plane.z
                            - v4Plane.w;

        while (!nlDLRingIsEnd(iter.m_Head, iter.m_Curr))
        {
            iter.next();
            pPrev = pNext;
            pNext = *iter;
            float fDistancePrev = fDistanceNext;

            float fDistanceNew = pNext->mv3Position.x * v4Plane.x
                               + pNext->mv3Position.y * v4Plane.y
                               + pNext->mv3Position.z * v4Plane.z
                               - v4Plane.w;
            fDistanceNext = fDistanceNew;

            if (fDistanceNew < 0.0f)
            {
                float fPercent
                    = fDistancePrev / (fDistancePrev - fDistanceNew);
                float fTime
                    = Interpolate(pPrev->mfTime, pNext->mfTime, fPercent)
                    - fSimulationTime;

                float fInvPercent = 1.0f - fPercent;
                v3ContactPoint.x = fInvPercent * pPrev->mv3Position.x
                                 + fPercent * pNext->mv3Position.x;
                v3ContactPoint.y = fInvPercent * pPrev->mv3Position.y
                                 + fPercent * pNext->mv3Position.y;
                v3ContactPoint.z = fInvPercent * pPrev->mv3Position.z
                                 + fPercent * pNext->mv3Position.z;
                v3ContactVelocity.x
                    = fInvPercent * pPrev->mv3LinearVelocity.x
                    + fPercent * pNext->mv3LinearVelocity.x;
                v3ContactVelocity.y
                    = fInvPercent * pPrev->mv3LinearVelocity.y
                    + fPercent * pNext->mv3LinearVelocity.y;
                v3ContactVelocity.z
                    = fInvPercent * pPrev->mv3LinearVelocity.z
                    + fPercent * pNext->mv3LinearVelocity.z;

                return fTime;
            }

            if (fDistanceNew >= fDistancePrev)
            {
                v3ContactPoint = pPrev->mv3Position;
                v3ContactVelocity = pPrev->mv3LinearVelocity;
                return -3.0f;
            }
        }
    }

    BallCacheInfo* pLastCache;
    nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.End();
    BallCacheInfo* pCurCache = *iter;

    float fDistanceCur = pCurCache->mv3Position.x * v4Plane.x
                       + pCurCache->mv3Position.y * v4Plane.y
                       + pCurCache->mv3Position.z * v4Plane.z
                       - v4Plane.w;

    while (mfLastCacheTime < 6.0f + fSimulationTime)
    {
        pLastCache = pCurCache;
        float fDistanceLast = fDistanceCur;

        PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);

        mfLastCacheTime += fPhysicsTick;
        BallCacheInfo* pNewInfo = AddCacheEntry(
            mfLastCacheTime, (PhysicsBall*)mpPredictWorld->mpPhysicsBall);

        pCurCache = pNewInfo;

        float fDistanceNewCache
            = pNewInfo->mv3Position.x * v4Plane.x
            + pNewInfo->mv3Position.y * v4Plane.y
            + pNewInfo->mv3Position.z * v4Plane.z - v4Plane.w;
        fDistanceCur = fDistanceNewCache;

        if (fDistanceNewCache < 0.0f)
        {
            float fPercent
                = fDistanceLast / (fDistanceLast - fDistanceNewCache);
            float fTime
                = Interpolate(
                      pLastCache->mfTime, pNewInfo->mfTime, fPercent)
                - fSimulationTime;

            float fInvPercent = 1.0f - fPercent;
            v3ContactPoint.x = fInvPercent * pLastCache->mv3Position.x
                             + fPercent * pNewInfo->mv3Position.x;
            v3ContactPoint.y = fInvPercent * pLastCache->mv3Position.y
                             + fPercent * pNewInfo->mv3Position.y;
            v3ContactPoint.z = fInvPercent * pLastCache->mv3Position.z
                             + fPercent * pNewInfo->mv3Position.z;
            v3ContactVelocity.x
                = fInvPercent * pLastCache->mv3LinearVelocity.x
                + fPercent * pNewInfo->mv3LinearVelocity.x;
            v3ContactVelocity.y
                = fInvPercent * pLastCache->mv3LinearVelocity.y
                + fPercent * pNewInfo->mv3LinearVelocity.y;
            v3ContactVelocity.z
                = fInvPercent * pLastCache->mv3LinearVelocity.z
                + fPercent * pNewInfo->mv3LinearVelocity.z;

            return fTime;
        }

        if (fDistanceNewCache >= fDistanceLast)
        {
            v3ContactPoint = pLastCache->mv3Position;
            v3ContactVelocity = pLastCache->mv3LinearVelocity;
            return -4.0f;
        }
    }

    v3ContactPoint = pCurCache->mv3Position;
    v3ContactVelocity = pCurCache->mv3LinearVelocity;
    return -5.0f;
}

float FakeBallWorld::GetPredictedHeightLimitTime(float fHeight,
    float fMinTime, nlVector3& v3ContactPoint,
    nlVector3& v3ContactVelocity, float& fTargetHeight, bool bDownOnly)
{
    cBall* pBall = mpPredictWorld->mpBall;
    float speedSq = pBall->m_v3Velocity.GetLengthSq3D();
    if (speedSq < 0.0001f)
    {
        v3ContactPoint = pBall->m_v3Position;
        v3ContactVelocity = v3Zero;
        fTargetHeight = v3ContactPoint.z;
        return fMinTime;
    }

    bool freeball = GetPredictedBallPosition(
        fMinTime, v3ContactPoint, v3ContactVelocity);
    if (v3ContactPoint.z <= fHeight
        && (!bDownOnly || v3ContactVelocity.z <= 0.0f))
    {
        fTargetHeight = v3ContactPoint.z;
        return fMinTime;
    }
    if (!freeball)
    {
        return -2.0f;
    }

    float fSimulationTime;
    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    fSimulationTime = GetFixedUpdateTask()->mSimulationTime;
    float fTestTime = fSimulationTime + fMinTime;
    float fLastZVel = 0.0f;
    fTargetHeight = 0.0f;
    nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
    while (iter.hasNext())
    {
        BallCacheInfo* pCur = *iter;
        if (pCur->mfTime >= fTestTime)
        {
            float zVel = pCur->mv3LinearVelocity.z;
            if (fTargetHeight < pCur->mv3Position.z)
            {
                fTargetHeight = pCur->mv3Position.z;
            }
            if ((pCur->mv3Position.z <= fHeight
                    && (!bDownOnly || zVel <= 0.0f))
                || (fLastZVel < 0.0f && zVel > 0.0f))
            {
                v3ContactPoint = pCur->mv3Position;
                v3ContactVelocity = pCur->mv3LinearVelocity;
                return pCur->mfTime - fSimulationTime;
            }
        }
        fLastZVel = pCur->mv3LinearVelocity.z;
        iter.Step();
    }

    while (mfLastCacheTime < 6.0f + fSimulationTime)
    {
        PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
        mfLastCacheTime += fPhysicsTick;
        BallCacheInfo* pNewInfo = AddCacheEntry(
            mfLastCacheTime, mpPredictWorld->mpPhysicsBall);
        float zVel = pNewInfo->mv3LinearVelocity.z;
        if (fTargetHeight < pNewInfo->mv3Position.z)
        {
            fTargetHeight = pNewInfo->mv3Position.z;
        }
        if ((pNewInfo->mv3Position.z <= fHeight
                && (!bDownOnly || zVel <= 0.0f))
            || (fLastZVel < 0.0f
                && pNewInfo->mv3LinearVelocity.z > 0.0f))
        {
            v3ContactPoint = pNewInfo->mv3Position;
            v3ContactVelocity = pNewInfo->mv3LinearVelocity;
            return pNewInfo->mfTime - fSimulationTime;
        }
        fLastZVel = pNewInfo->mv3LinearVelocity.z;
    }
    return -1.0f;
}

float FakeBallWorld::GetPredictedPosAtDistance(float fDistance,
    nlVector3& v3Position, nlVector3& v3Velocity, bool bFreeBallOnly)
{
    cBall* pBall = mpPredictWorld->GetBall();
    if (pBall->m_v3Velocity.GetLengthSq3D() < 0.0001f || fDistance < 0.0001f)
    {
        v3Position = pBall->m_v3Position;
        v3Velocity = v3Zero;
        return -1.0f;
    }

    float fSimulationTime;
    float fDistanceTargetSq;
    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    fSimulationTime = GetFixedUpdateTask()->mSimulationTime;
    fDistanceTargetSq = fDistance * fDistance;

    if (bFreeBallOnly)
    {
        if (mpPredictWorld->mpBall->m_pOwner != 0)
        {
            return -1.5f;
        }

        nlVector3 v3LastPos;
        nlVector3 v3LastVel;
        float fDistanceLastSq;
        ClearBallCache();
        mpPredictWorld->mpGroundPlane->DisableCollisions();
        mpPredictWorld->mpPhysicsBall->CloneBall(
            *mpPredictWorld->mpBall->m_pPhysicsBall);

        float fTime = 0.0f;
        float fDistanceCurSq = 0.0f;
        mpPredictWorld->mpPhysicsBall->mbCanGoThroughGround = true;
        mpPredictWorld->mpPhysicsBall->GetLinearVelocity(&v3Velocity);
        mpPredictWorld->mpPhysicsBall->GetPosition(&v3Position);
        nlVector3 v3StartPos = v3Position;
        bool bFound = false;
        while (fTime < 6.0f)
        {
            v3LastPos = v3Position;
            v3LastVel = v3Velocity;
            fDistanceLastSq = fDistanceCurSq;
            PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
            mpPredictWorld->mpPhysicsBall->GetLinearVelocity(&v3Velocity);
            mpPredictWorld->mpPhysicsBall->GetPosition(&v3Position);
            fDistanceCurSq = CalculateDistanceSquared(v3Position, v3StartPos);
            if (fDistanceCurSq > fDistanceTargetSq)
            {
                float fDistLast = nlSqrt(fDistanceLastSq, true);
                float fDistCur = nlSqrt(fDistanceCurSq, true);
                float fPercent
                    = (fDistance - fDistLast) / (fDistCur - fDistLast);
                nlVecLerp(v3Position, v3LastPos, v3Position, fPercent);
                fTime += fPercent * fPhysicsTick;
                nlVecLerp(v3Velocity, v3LastVel, v3Velocity, fPercent);
                bFound = true;
                break;
            }
            if (fDistanceCurSq <= fDistanceLastSq)
            {
                v3Position = v3LastPos;
                v3Velocity = v3LastVel;
                bFound = true;
                fTime = -2.0f;
                break;
            }
            fTime += fPhysicsTick;
        }

        mpPredictWorld->mpPhysicsBall->mbCanGoThroughGround = false;
        mpPredictWorld->mpGroundPlane->EnableCollisions();
        if (bFound)
        {
            return fTime;
        }
        return -3.0f;
    }

    if (!GetPredictedBallPosition(0.0f, v3Position, v3Velocity))
    {
        return -1.5f;
    }

    if (!mBallCacheList.IsEmpty())
    {
        nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
        BallCacheInfo* pPrev;
        BallCacheInfo* pNext = *iter;
        float fDistanceNextSq
            = CalculateDistanceSquared(pNext->mv3Position, pBall->m_v3Position);

        while (!nlDLRingIsEnd(iter.m_Head, iter.m_Curr))
        {
            iter.next();
            pPrev = pNext;
            pNext = *iter;
            float fDistancePrevSq = fDistanceNextSq;
            fDistanceNextSq = CalculateDistanceSquared(
                pNext->mv3Position, pBall->m_v3Position);
            if (fDistanceNextSq > fDistanceTargetSq)
            {
                float fDistPrev = nlSqrt(fDistancePrevSq, true);
                float fDistNext = nlSqrt(fDistanceNextSq, true);
                float fPercent
                    = (fDistance - fDistPrev) / (fDistNext - fDistPrev);
                float fTime = Interpolate(pPrev->mfTime, pNext->mfTime, fPercent)
                            - fSimulationTime;
                nlVecLerp(v3Position,
                    pPrev->mv3Position,
                    pNext->mv3Position,
                    fPercent);
                nlVecLerp(v3Velocity, pPrev->mv3LinearVelocity, pNext->mv3LinearVelocity, fPercent);
                return fTime;
            }
            if (fDistanceNextSq <= fDistancePrevSq)
            {
                v3Position = pPrev->mv3Position;
                v3Velocity = pPrev->mv3LinearVelocity;
                return -2.0f;
            }
        }
    }

    BallCacheInfo* pLastCache;
    nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.End();
    BallCacheInfo* pCurCache = *iter;
    float fDistanceCurSq
        = CalculateDistanceSquared(pCurCache->mv3Position, pBall->m_v3Position);
    while (mfLastCacheTime < 6.0f + fSimulationTime)
    {
        pLastCache = pCurCache;
        float fDistanceLastSq = fDistanceCurSq;
        PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
        mfLastCacheTime += fPhysicsTick;
        pCurCache = AddCacheEntry(
            mfLastCacheTime, mpPredictWorld->mpPhysicsBall);
        fDistanceCurSq = CalculateDistanceSquared(
            pCurCache->mv3Position, pBall->m_v3Position);
        if (fDistanceCurSq > fDistanceTargetSq)
        {
            float fDistLast = nlSqrt(fDistanceLastSq, true);
            float fDistCur = nlSqrt(fDistanceCurSq, true);
            float fPercent
                = (fDistance - fDistLast) / (fDistCur - fDistLast);
            float fTime = Interpolate(
                              pLastCache->mfTime, pCurCache->mfTime, fPercent)
                        - fSimulationTime;
            nlVecLerp(v3Position, pLastCache->mv3Position, pCurCache->mv3Position, fPercent);
            nlVecLerp(v3Velocity,
                pLastCache->mv3LinearVelocity,
                pCurCache->mv3LinearVelocity,
                fPercent);
            return fTime;
        }
        if (fDistanceCurSq <= fDistanceLastSq)
        {
            v3Position = pLastCache->mv3Position;
            v3Velocity = pLastCache->mv3LinearVelocity;
            return -3.0f;
        }
    }

    v3Position = pCurCache->mv3Position;
    v3Velocity = pCurCache->mv3LinearVelocity;
    return -4.0f;
}

FakeBallWorld::FakeBallWorld(cBall* pBall)
    : mpBall(pBall)
{
    mpPhysicsWorld
        = new (nlMalloc(sizeof(PhysicsWorld), 8, false)) PhysicsWorld();
    mpCollisionSpace
        = new (nlMalloc(sizeof(SimpleCollisionSpace), 8, false))
            SimpleCollisionSpace(mpPhysicsWorld, true);
    mpPhysicsWorld->SetCFM(0.00001f);
    mpGroundPlane
        = new (nlMalloc(sizeof(PhysicsGroundPlane), 8, false))
            PhysicsGroundPlane(mpCollisionSpace);
    mpPhysicsBall = new (nlMalloc(sizeof(FakePhysicsBall), 8, false))
        FakePhysicsBall(0.18f, *this);
    nlVector4 plane;
    nlVec4Set(plane, 1.0f, 0.0f, 0.0f, 20.6f);
    mpGoaliePlane1
        = new (nlMalloc(sizeof(PhysicsGoaliePlane), 8, false))
            PhysicsGoaliePlane(plane, *this);
    plane.x = -1.0f;
    mpGoaliePlane2
        = new (nlMalloc(sizeof(PhysicsGoaliePlane), 8, false))
            PhysicsGoaliePlane(plane, *this);
    mpGoaliePlane1->DisableCollisions();
    mpGoaliePlane2->DisableCollisions();
    mbHitSuccess = false;
}

FakeBallWorld::~FakeBallWorld()
{
    delete mpPhysicsBall;
    delete mpGoaliePlane1;
    delete mpGoaliePlane2;
    delete mpGroundPlane;
    delete mpCollisionSpace;
    delete mpPhysicsWorld;
}

void FakeBallWorld::ResetBallIterator()
{
    nlVector3 v3Position;
    nlVector3 v3Velocity;
    GetPredictedBallPosition(0.0f, v3Position, v3Velocity);

    static nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
    iter = mBallCacheList.Begin();
    mpCacheIterator = &iter;

    if (mpCacheIterator->m_Curr != 0)
    {
        mpCacheIterator->next();
    }
}

void FakeBallWorld::GetNextBallPosition(nlVector3& v3BallPos)
{
    if (mpCacheIterator->m_Curr != 0)
    {
        v3BallPos = mpCacheIterator->m_Curr->entry->mv3Position;
        mpCacheIterator->next();
        return;
    }

    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
    mfLastCacheTime += fPhysicsTick;
    BallCacheInfo* newInfo
        = AddCacheEntry(mfLastCacheTime, mpPredictWorld->mpPhysicsBall);
    v3BallPos = newInfo->mv3Position;
}

void FakeBallWorld::GetNextBallPosVel(
    nlVector3& v3BallPos, nlVector3& v3BallVel)
{
    if (mpCacheIterator->m_Curr != 0)
    {
        BallCacheInfo* pCur = mpCacheIterator->m_Curr->entry;
        v3BallPos = pCur->mv3Position;
        v3BallVel = pCur->mv3LinearVelocity;
        mpCacheIterator->next();
        return;
    }

    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
    mfLastCacheTime += fPhysicsTick;
    BallCacheInfo* newInfo
        = AddCacheEntry(mfLastCacheTime, mpPredictWorld->mpPhysicsBall);
    v3BallPos = newInfo->mv3Position;
    v3BallVel = newInfo->mv3LinearVelocity;
}

bool FakeBallWorld::FindBallIntercept(const nlVector3& v3PlayerPos,
    float fPlayerReach, float fPlayerSpeed, nlVector3& v3InterceptPos,
    nlVector3& v3InterceptVel, float& fInterceptTime,
    float& fClosestDist, float fMaxTime)
{
    fInterceptTime = 0.0f;
    fClosestDist = 10000.0f;
    unsigned char bDone = 0;
    float fPlayerDistPerTick
        = fPlayerSpeed * FixedUpdateTask::GetPhysicsUpdateTick();

    nlVector3 v3NewBallPos;
    nlVector3 v3NewBallVel;
    ResetBallIterator();

    float fPlayerDistanceFromStartingPoint = fPlayerReach;
    while (!bDone)
    {
        GetNextBallPosVel(v3NewBallPos, v3NewBallVel);
        fPlayerDistanceFromStartingPoint += fPlayerDistPerTick;
        float adjustedDist = (float)fabs(
            nlSqrt(nlVec3DistanceSquared2D(v3NewBallPos, v3PlayerPos), true)
            - fPlayerDistanceFromStartingPoint);
        if (adjustedDist >= fClosestDist)
        {
            bDone = true;
        }
        else
        {
            v3InterceptPos = v3NewBallPos;
            v3InterceptVel = v3NewBallVel;
            fClosestDist = adjustedDist;
        }

        fInterceptTime += FixedUpdateTask::GetPhysicsUpdateTick();
        if (fInterceptTime >= fMaxTime)
        {
            bDone = true;
        }
    }
    return fInterceptTime < fMaxTime;
}

extern "C" void fn_8016EEC8()
{
    if (!FakeBallWorld::mpPredictWorld->mpGoaliePlane1
            ->AreCollisionsEnabled())
    {
        FakeBallWorld::mpPredictWorld->mpGoaliePlane1
            ->EnableCollisions();
        FakeBallWorld::mpPredictWorld->mpGoaliePlane2
            ->EnableCollisions();
        if (FakeBallWorld::mpPredictWorld->mUnidentified1D)
        {
            FakeBallWorld::mpPredictWorld->mUnidentified1D = false;
            FakeBallWorld::InvalidateBallCache();
        }
    }
}

extern "C" void fn_8016F06C()
{
    if (FakeBallWorld::mpPredictWorld->mpGoaliePlane1
            ->AreCollisionsEnabled())
    {
        FakeBallWorld::mpPredictWorld->mpGoaliePlane1
            ->DisableCollisions();
        FakeBallWorld::mpPredictWorld->mpGoaliePlane2
            ->DisableCollisions();
        if (FakeBallWorld::mpPredictWorld->mbHitSuccess)
        {
            FakeBallWorld::mpPredictWorld->mbHitSuccess = false;
            FakeBallWorld::InvalidateBallCache();
        }
    }
}

FakePhysicsBall::FakePhysicsBall(
    float radius, FakeBallWorld& fakeBallWorld)
    : PhysicsBall(fakeBallWorld.mpCollisionSpace,
          fakeBallWorld.mpPhysicsWorld, radius)
    , mWorld(fakeBallWorld)
{
}

ContactType FakePhysicsBall::Contact(
    PhysicsObject* object, dContact* contact, int numContacts)
{
    if (object->GetObjectType() == 0x16)
    {
        mWorld.mbHitSuccess = true;
    }
    return PhysicsBall::Contact(object, contact, numContacts);
}

PhysicsGoaliePlane::PhysicsGoaliePlane(const nlVector4& plane,
    FakeBallWorld& fakeBallWorld)
    : PhysicsWall(fakeBallWorld.mpCollisionSpace, plane.x, plane.y, plane.w)
    , mWorld(fakeBallWorld)
{
}
