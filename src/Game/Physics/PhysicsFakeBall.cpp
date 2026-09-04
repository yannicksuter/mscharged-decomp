#include "Game/Physics/PhysicsFakeBall.h"

#include "Game/AI/AiUtil.h"
#include "Game/Field.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Player.h"
#include "NL/nlDLRing.h"
#include "math.h"
#include "types.h"

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

SlotPool<BallCacheInfo> BallCacheInfo::mBallCacheInfoSlotPool(16, 16);
FakeBallWorld* FakeBallWorld::mpPredictWorld;
nlDLListSlotPool<BallCacheInfo*> FakeBallWorld::mBallCacheList;
float FakeBallWorld::mfLastCacheTime = -1.0f;
nlDLListIterator<BallCacheInfo*>* FakeBallWorld::mpCacheIterator;

extern "C" FixedUpdateTask* fn_8011166C();

class SimpleCollisionSpace : public CollisionSpace
{
public:
    SimpleCollisionSpace(PhysicsWorld*, bool);
    virtual ~SimpleCollisionSpace() { }
};

static inline void ResetBallIteratorState()
{
    static nlDLListIterator<BallCacheInfo*> iter
        = FakeBallWorld::mBallCacheList.Begin();

    iter.m_Curr
        = nlDLRingGetStart(FakeBallWorld::mBallCacheList.m_Head);
    iter.m_Head = FakeBallWorld::mBallCacheList.m_Head;
    FakeBallWorld::mpCacheIterator = &iter;

    if (iter.m_Curr != 0)
    {
        iter.next();
    }
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
    mpGoaliePlane1
        = new (nlMalloc(sizeof(PhysicsGoaliePlane), 8, false))
            PhysicsGoaliePlane(1.0f, 0.0f, 20.6f, *this);
    mpGoaliePlane2
        = new (nlMalloc(sizeof(PhysicsGoaliePlane), 8, false))
            PhysicsGoaliePlane(-1.0f, 0.0f, 20.6f, *this);
    mpGoaliePlane1->DisableCollisions();
    mpGoaliePlane2->DisableCollisions();
    mbHitSuccess = false;
    mUnidentified1D = false;
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

void FakeBallWorld::ClearBallCache()
{
    if (mBallCacheList.m_Head != 0)
    {
        nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
        while (iter.hasNext())
        {
            BallCacheInfo::mBallCacheInfoSlotPool.Free(*iter);
            iter.next();
        }
        mBallCacheList.Clear();
    }

    mfLastCacheTime = -1.0f;
    if (mpPredictWorld != 0)
    {
        mpPredictWorld->mbHitSuccess = false;
        mpPredictWorld->mUnidentified1D = false;
    }
}

void FakeBallWorld::InvalidateBallCache()
{
    ClearBallCache();
}

BallCacheInfo* FakeBallWorld::AddCacheEntry(
    float fTime, PhysicsBall* pPhysicsBall)
{
    BallCacheInfo* pNewInfo;
    BallCacheInfo::mBallCacheInfoSlotPool.AllocateForReturn(pNewInfo);
    pNewInfo->mfTime = fTime;
    pNewInfo->mv3Position = pPhysicsBall->GetPosition();
    pNewInfo->mv3LinearVelocity = pPhysicsBall->GetLinearVelocity();

    DLListEntry<BallCacheInfo*>* pNewEntry
        = mBallCacheList.m_Allocator.Allocate();
    if (pNewEntry != 0)
    {
        pNewEntry->m_next = 0;
        pNewEntry->m_prev = 0;
        pNewEntry->entry = pNewInfo;
    }
    nlDLRingAddEnd(&mBallCacheList.m_Head, pNewEntry);
    return pNewInfo;
}

bool FakeBallWorld::GetPredictedBallPosition(float fDeltaTime,
    nlVector3& v3Position, nlVector3& v3Velocity)
{
    cBall* pBall = mpPredictWorld->GetBall();
    if (pBall->m_pOwner != 0)
    {
        v3Position = pBall->m_v3Position;
        v3Velocity = pBall->m_pOwner->m_v3Velocity;
        return false;
    }

    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    float fSimTime = fn_8011166C()->mSimulationTime;
    if (mfLastCacheTime < fSimTime)
    {
        ClearBallCache();
    }
    else if (mBallCacheList.m_Head != 0)
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

                float distSq = nlGetLengthSquared3D(
                    pCur->mv3Position.x - pBall->m_v3Position.x,
                    pCur->mv3Position.y - pBall->m_v3Position.y,
                    pCur->mv3Position.z - pBall->m_v3Position.z);
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
    while (mfLastCacheTime < fTargetTime)
    {
        if (mfLastCacheTime < fSimTime)
        {
            mpPredictWorld->mbHitSuccess = false;
            mpPredictWorld->mUnidentified1D = false;
            mpPredictWorld->mpPhysicsBall->CloneBall(
                *mpPredictWorld->mpBall->m_pPhysicsBall);
            mfLastCacheTime = fSimTime;
        }
        else
        {
            PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
            mfLastCacheTime += fPhysicsTick;
        }
        AddCacheEntry(mfLastCacheTime, mpPredictWorld->mpPhysicsBall);
    }

    float overshoot = mfLastCacheTime - fTargetTime;
    BallCacheInfo* pPrev;
    BallCacheInfo* pNext;
    if (fDeltaTime < overshoot)
    {
        nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
        pNext = *iter;
        pPrev = pNext;
        while (!iter.IsEnd() && pNext->mfTime < fTargetTime)
        {
            pPrev = pNext;
            iter.next();
            pNext = *iter;
        }
    }
    else
    {
        DLListEntry<BallCacheInfo*>* pEntry
            = nlDLRingGetEnd(mBallCacheList.m_Head);
        nlDLListIterator<BallCacheInfo*> iter(
            mBallCacheList.m_Head, pEntry);
        pNext = *iter;
        pPrev = pNext;
        while (!nlDLRingIsStart(iter.m_Head, iter.m_Curr)
               && pPrev->mfTime >= fTargetTime)
        {
            pNext = pPrev;
            iter.m_Curr = iter.m_Curr->m_prev;
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

    float fSimulationTime = fn_8011166C()->mSimulationTime;
    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
    BallCacheInfo* pNext = *iter;
    float fDistanceNext = pNext->mv3Position.x * v4Plane.x
                        + pNext->mv3Position.y * v4Plane.y
                        + pNext->mv3Position.z * v4Plane.z - v4Plane.w;

    while (!iter.IsEnd())
    {
        BallCacheInfo* pPrev = pNext;
        float fDistancePrev = fDistanceNext;
        iter.next();
        pNext = *iter;
        fDistanceNext = pNext->mv3Position.x * v4Plane.x
                      + pNext->mv3Position.y * v4Plane.y
                      + pNext->mv3Position.z * v4Plane.z - v4Plane.w;
        if (fDistanceNext < 0.0f)
        {
            float fPercent
                = fDistancePrev / (fDistancePrev - fDistanceNext);
            nlVecLerp(v3ContactPoint,
                pPrev->mv3Position,
                pNext->mv3Position,
                fPercent);
            nlVecLerp(v3ContactVelocity, pPrev->mv3LinearVelocity, pNext->mv3LinearVelocity, fPercent);
            return Interpolate(
                       pPrev->mfTime, pNext->mfTime, fPercent)
                 - fSimulationTime;
        }
        if (fDistanceNext >= fDistancePrev)
        {
            v3ContactPoint = pPrev->mv3Position;
            v3ContactVelocity = pPrev->mv3LinearVelocity;
            return -3.0f;
        }
    }

    BallCacheInfo* pCurCache
        = nlDLRingGetEnd(mBallCacheList.m_Head)->entry;
    float fDistanceCur = pCurCache->mv3Position.x * v4Plane.x
                       + pCurCache->mv3Position.y * v4Plane.y
                       + pCurCache->mv3Position.z * v4Plane.z - v4Plane.w;
    float fMaxTime = fSimulationTime + 6.0f;
    while (mfLastCacheTime < fMaxTime)
    {
        BallCacheInfo* pLastCache = pCurCache;
        float fDistanceLast = fDistanceCur;
        PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
        mfLastCacheTime += fPhysicsTick;
        pCurCache = AddCacheEntry(
            mfLastCacheTime, mpPredictWorld->mpPhysicsBall);
        fDistanceCur = pCurCache->mv3Position.x * v4Plane.x
                     + pCurCache->mv3Position.y * v4Plane.y
                     + pCurCache->mv3Position.z * v4Plane.z - v4Plane.w;
        if (fDistanceCur < 0.0f)
        {
            float fPercent
                = fDistanceLast / (fDistanceLast - fDistanceCur);
            nlVecLerp(v3ContactPoint, pLastCache->mv3Position, pCurCache->mv3Position, fPercent);
            nlVecLerp(v3ContactVelocity,
                pLastCache->mv3LinearVelocity,
                pCurCache->mv3LinearVelocity,
                fPercent);
            return Interpolate(
                       pLastCache->mfTime, pCurCache->mfTime, fPercent)
                 - fSimulationTime;
        }
        if (fDistanceCur >= fDistanceLast)
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
    float speedSq = nlGetLengthSquared3D(pBall->m_v3Velocity.x,
        pBall->m_v3Velocity.y,
        pBall->m_v3Velocity.z);
    if (speedSq < 0.0001f)
    {
        v3ContactPoint = pBall->m_v3Position;
        v3ContactVelocity = v3Zero;
        fTargetHeight = v3ContactPoint.z;
        return fMinTime;
    }

    bool freeball = GetPredictedBallPosition(
        fMinTime, v3ContactPoint, v3ContactVelocity);
    fTargetHeight = v3ContactPoint.z;
    if (v3ContactPoint.z <= fHeight
        && (!bDownOnly || v3ContactVelocity.z <= 0.0f))
    {
        return fMinTime;
    }
    if (!freeball)
    {
        return -2.0f;
    }

    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    float fSimulationTime = fn_8011166C()->mSimulationTime;
    float fTestTime = fSimulationTime + fMinTime;
    float fLastZVel = 0.0f;
    nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
    while (iter.hasNext())
    {
        BallCacheInfo* pCur = *iter;
        if (pCur->mfTime >= fTestTime
            && ((pCur->mv3Position.z <= fHeight
                    && (!bDownOnly
                        || pCur->mv3LinearVelocity.z <= 0.0f))
                || (fLastZVel < 0.0f
                    && pCur->mv3LinearVelocity.z > 0.0f)))
        {
            v3ContactPoint = pCur->mv3Position;
            v3ContactVelocity = pCur->mv3LinearVelocity;
            fTargetHeight = v3ContactPoint.z;
            return pCur->mfTime - fSimulationTime;
        }
        fLastZVel = pCur->mv3LinearVelocity.z;
        iter.next();
    }

    fTestTime = fSimulationTime + 6.0f;
    while (mfLastCacheTime < fTestTime)
    {
        PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
        mfLastCacheTime += fPhysicsTick;
        BallCacheInfo* pNewInfo = AddCacheEntry(
            mfLastCacheTime, mpPredictWorld->mpPhysicsBall);
        if ((pNewInfo->mv3Position.z <= fHeight
                && (!bDownOnly
                    || pNewInfo->mv3LinearVelocity.z <= 0.0f))
            || (fLastZVel < 0.0f
                && pNewInfo->mv3LinearVelocity.z > 0.0f))
        {
            v3ContactPoint = pNewInfo->mv3Position;
            v3ContactVelocity = pNewInfo->mv3LinearVelocity;
            fTargetHeight = v3ContactPoint.z;
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
    float speedSq = nlGetLengthSquared3D(pBall->m_v3Velocity.x,
        pBall->m_v3Velocity.y,
        pBall->m_v3Velocity.z);
    if (speedSq < 0.0001f || fDistance < 0.0001f)
    {
        v3Position = pBall->m_v3Position;
        v3Velocity = v3Zero;
        return -1.0f;
    }
    if (bFreeBallOnly && pBall->m_pOwner != 0)
    {
        return -1.5f;
    }
    if (!GetPredictedBallPosition(0.0f, v3Position, v3Velocity))
    {
        return -1.5f;
    }

    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    float fSimulationTime = fn_8011166C()->mSimulationTime;
    float fDistanceTargetSq = fDistance * fDistance;
    nlDLListIterator<BallCacheInfo*> iter = mBallCacheList.Begin();
    BallCacheInfo* pNext = *iter;
    float fDistanceNextSq = nlGetLengthSquared3D(
        pNext->mv3Position.x - pBall->m_v3Position.x,
        pNext->mv3Position.y - pBall->m_v3Position.y,
        pNext->mv3Position.z - pBall->m_v3Position.z);

    while (!iter.IsEnd())
    {
        BallCacheInfo* pPrev = pNext;
        float fDistancePrevSq = fDistanceNextSq;
        iter.next();
        pNext = *iter;
        fDistanceNextSq = nlGetLengthSquared3D(
            pNext->mv3Position.x - pBall->m_v3Position.x,
            pNext->mv3Position.y - pBall->m_v3Position.y,
            pNext->mv3Position.z - pBall->m_v3Position.z);
        if (fDistanceNextSq > fDistanceTargetSq)
        {
            float sqrtPrev = nlSqrt(fDistancePrevSq, true);
            float sqrtNext = nlSqrt(fDistanceNextSq, true);
            float fPercent
                = (fDistance - sqrtPrev) / (sqrtNext - sqrtPrev);
            nlVecLerp(v3Position,
                pPrev->mv3Position,
                pNext->mv3Position,
                fPercent);
            nlVecLerp(v3Velocity, pPrev->mv3LinearVelocity, pNext->mv3LinearVelocity, fPercent);
            return Interpolate(
                       pPrev->mfTime, pNext->mfTime, fPercent)
                 - fSimulationTime;
        }
        if (fDistanceNextSq <= fDistancePrevSq)
        {
            v3Position = pPrev->mv3Position;
            v3Velocity = pPrev->mv3LinearVelocity;
            return -2.0f;
        }
    }

    BallCacheInfo* pCurCache
        = nlDLRingGetEnd(mBallCacheList.m_Head)->entry;
    float fDistanceCurSq = nlGetLengthSquared3D(
        pCurCache->mv3Position.x - pBall->m_v3Position.x,
        pCurCache->mv3Position.y - pBall->m_v3Position.y,
        pCurCache->mv3Position.z - pBall->m_v3Position.z);
    float fMaxTime = fSimulationTime + 6.0f;
    while (mfLastCacheTime < fMaxTime)
    {
        BallCacheInfo* pLastCache = pCurCache;
        float fDistanceLastSq = fDistanceCurSq;
        PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
        mfLastCacheTime += fPhysicsTick;
        pCurCache = AddCacheEntry(
            mfLastCacheTime, mpPredictWorld->mpPhysicsBall);
        fDistanceCurSq = nlGetLengthSquared3D(
            pCurCache->mv3Position.x - pBall->m_v3Position.x,
            pCurCache->mv3Position.y - pBall->m_v3Position.y,
            pCurCache->mv3Position.z - pBall->m_v3Position.z);
        if (fDistanceCurSq > fDistanceTargetSq)
        {
            float sqrtLast = nlSqrt(fDistanceLastSq, true);
            float sqrtCur = nlSqrt(fDistanceCurSq, true);
            float fPercent
                = (fDistance - sqrtLast) / (sqrtCur - sqrtLast);
            nlVecLerp(v3Position, pLastCache->mv3Position, pCurCache->mv3Position, fPercent);
            nlVecLerp(v3Velocity,
                pLastCache->mv3LinearVelocity,
                pCurCache->mv3LinearVelocity,
                fPercent);
            return Interpolate(
                       pLastCache->mfTime, pCurCache->mfTime, fPercent)
                 - fSimulationTime;
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

void FakeBallWorld::ResetBallIterator()
{
    nlVector3 v3Position;
    nlVector3 v3Velocity;
    GetPredictedBallPosition(0.0f, v3Position, v3Velocity);
    ResetBallIteratorState();
}

void FakeBallWorld::GetNextBallPosition(nlVector3& v3BallPos)
{
    if (mpCacheIterator->m_Curr != 0)
    {
        v3BallPos = (**mpCacheIterator)->mv3Position;
        mpCacheIterator->next();
        return;
    }

    float fPhysicsTick = FixedUpdateTask::GetPhysicsUpdateTick();
    PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, fPhysicsTick);
    mfLastCacheTime += fPhysicsTick;
    BallCacheInfo* newInfo
        = AddCacheEntry(mfLastCacheTime, mpPredictWorld->mpPhysicsBall);

    float fUnidentified0 = (float)fabs(newInfo->mv3Position.x);
    float fUnidentified1 = cField::GetGoalLineX(1U)
                         - mpPredictWorld->mpPhysicsBall->GetRadius();
    if (fUnidentified0 >= fUnidentified1)
    {
        mpPredictWorld->mUnidentified1D = true;
    }
    v3BallPos = newInfo->mv3Position;
}

void FakeBallWorld::GetNextBallPosVel(
    nlVector3& v3BallPos, nlVector3& v3BallVel)
{
    if (mpCacheIterator->m_Curr != 0)
    {
        BallCacheInfo* info = **mpCacheIterator;
        v3BallPos = info->mv3Position;
        v3BallVel = info->mv3LinearVelocity;
        mpCacheIterator->next();
        return;
    }

    float tick = FixedUpdateTask::GetPhysicsUpdateTick();
    PhysicsUpdate(mpPredictWorld->mpPhysicsWorld, tick);
    mfLastCacheTime += tick;
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
    nlVector3 v3CurrentVelocity;
    nlVector3 v3CurrentPosition;
    GetPredictedBallPosition(
        0.0f, v3CurrentPosition, v3CurrentVelocity);
    ResetBallIteratorState();

    float fPlayerDistanceFromStartingPoint = fPlayerReach;
    while (!bDone)
    {
        GetNextBallPosVel(v3NewBallPos, v3NewBallVel);
        fPlayerDistanceFromStartingPoint += fPlayerDistPerTick;
        float dx = v3NewBallPos.x - v3PlayerPos.x;
        float dy = v3NewBallPos.y - v3PlayerPos.y;
        float dist = nlSqrt(dx * dx + dy * dy, true);
        float adjustedDist
            = (float)fabs(dist - fPlayerDistanceFromStartingPoint);
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
            FakeBallWorld::ClearBallCache();
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
            FakeBallWorld::ClearBallCache();
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

PhysicsGoaliePlane::PhysicsGoaliePlane(float a, float b, float c,
    FakeBallWorld& fakeBallWorld)
    : PhysicsWall(fakeBallWorld.mpCollisionSpace, a, b, c)
    , mWorld(fakeBallWorld)
{
}
