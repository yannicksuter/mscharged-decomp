#include "Game/AI/AvoidController.h"
#include "Game/CharacterTweaks.h"

#include "Game/AI/AvoidableObject.h"
#include "Game/AI/Fielder.h"
#include "Game/CharacterTemplate.h"
#include "Game/DebugWriteCache.h"
#include "NL/nlMemory.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/AI/Powerups.h"
#include "Game/MathHelpers.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/Field.h"
#include "Game/UnidentifiedStaticStorage.h"


static const nlVector2 v2Zero = { 0.0f, 0.0f };

#pragma explicit_zero_data on
static float sAvoidanceMemoryInitialSeconds = 0.0f;
#pragma explicit_zero_data off
static float sSidelineMaxDistance = 0.5f;
static float sSidelineMaxDistanceWhileAvoiding = 1.0f;
static float sInitialSidelineMaxDistance = 0.5f;
static float sInitialSidelineMaxDistanceWhileAvoiding = 1.0f;
static float sSidelineUnavoidableDot = -0.99f;
static float sAvoidanceMemoryRefreshSeconds = 0.01f;
static unsigned short sAvoidControllerType = 0xFFFF;

class AvoidanceRemovalCollector
{
public:
    AvoidanceRemovalCollector(AvoidableObject* pObject, nlList<ObstacleAvoidance>& list)
        : mpRemovedObject(pObject), mRemovals(list)
    {
    }
    void Collect(const u32&, ObstacleAvoidance*);

    AvoidableObject* mpRemovedObject;
    nlList<ObstacleAvoidance>& mRemovals;
};

inline float ObstacleAvoidance::GetWeight() const
{
    float fWeight = 1.0f;
    if (mFadeOutTimer.m_uPackedTime != 0)
        fWeight = mFadeOutTimer.GetSeconds() / 0.3f;
    return mWeight * fWeight;
}

class RepulsionAccumulator
{
public:
    RepulsionAccumulator(float fDeltaT,
        nlVector3& accumulated, float& totalWeight,
        nlVector3* vectors, float* weights, int* counts,
        nlList<ObstacleAvoidance>& list)
        : mfDeltaT(fDeltaT), mAccumulated(accumulated),
          mTotalWeight(totalWeight), mpCategoryVectors(vectors),
          mpCategoryWeights(weights), mpCategoryCounts(counts),
          mRemovals(list)
    {
    }
    void Accumulate(const u32&, ObstacleAvoidance*);
    float mfDeltaT;
    nlVector3& mAccumulated;
    float& mTotalWeight;
    nlVector3* mpCategoryVectors;
    float* mpCategoryWeights;
    int* mpCategoryCounts;
    nlList<ObstacleAvoidance>& mRemovals;
};

bool lbl_806E0BB8;


bool lbl_806E0BB9;




inline AvoidanceMemory::AvoidanceMemory()
    : mTimer()
{
    mTimer.SetSeconds(sAvoidanceMemoryInitialSeconds);
}

inline void AvoidController::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("AvoidController");
    cache->AddField(15, gDebugFieldTypes[15].size, 0,
        "m_pFielder");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&m_ThingsToAvoid - (u8*)this,
        "m_ThingsToAvoid");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&m_CurrentlyAvoiding - (u8*)this,
        "m_CurrentlyAvoiding");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fRepulsionMult - (u8*)this,
        "m_fRepulsionMult");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&m_VeryCloseToSideline - (u8*)this,
        "m_VeryCloseToSideline");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&m_SidelineUnavoidable - (u8*)this,
        "m_SidelineUnavoidable");
    cache->AddField(21, gDebugFieldTypes[21].size,
        (u8*)&m_SidelineNormal - (u8*)this,
        "m_SidelineNormal");
    cache->AddField(21, gDebugFieldTypes[21].size,
        (u8*)&m_SidelineDirection - (u8*)this,
        "m_SidelineDirection");
    cache->AddArrayField(22, gDebugFieldTypes[22].size,
        NUM_AVOIDABLES,
        (u8*)&m_LastRepulVec - (u8*)this,
        "m_LastRepulVec[]");
    cache->EndType();
}

inline bool AvoidController::IsAvoiding(int things)
{
    bool bCanAvoid = (m_ThingsToAvoid & things) && !Incapacitated(m_pFielder);
    bool result = bCanAvoid;
    switch (things)
    {
    case AVOID_FIELDERS:
    {
        result = false;
        bool bCanAvoidFielder = bCanAvoid && !m_pFielder->IsSuperGrowActive();
        if (bCanAvoidFielder && !m_pFielder->IsInvincibleChars())
            result = true;
        break;
    }
    case AVOID_POWERUPS:
        result = false;
        if (bCanAvoid && !m_pFielder->IsInvinciblePowerups())
            result = true;
        break;
    }
    return result;
}

inline void ObstacleAvoidance::Initialize(
    AvoidableObject* pObject, AvoidableObject* pOther)
{
    mpAvoider = pObject;
    mpObstacle = pOther;
    mActiveTimer.Clear();
    mFadeOutTimer.Clear();
    mRepulsion = v3Zero;
    mWeight = 0.0f;
    mRepulsionHistory.UnidentifiedReset();
}

inline void AvoidController::SetLastRepulsionVector(
    eAvoidableThings things, const nlVector3& v3Repulsion, float fWeight)
{
    int index = GetAvoidableIndex(things);
    m_LastRepulVec[index] = v3Repulsion;
    m_LastRepulWeight[index] = fWeight;
}

inline void AvoidanceContext::NormalizeRepulsion()
{
    mRepulsionMag = nlVec3Length(mRepulsionDir);
    float lengthSquared = nlVec3LengthSquared(mRepulsionDir);
    if (lengthSquared != 0.0f)
        nlVec3Scale(mRepulsionDir, nlRecipSqrt(lengthSquared, true));
}

static inline f32 ClampRunningWBSpeed(f32 speed, f32 maxSpeed)
{
    if (speed <= maxSpeed)
        return speed;
    else
        return maxSpeed;
}

AvoidController::AvoidController(cFielder* fielder)
    : m_Avoidances(16, 16)
{
    sSidelineMaxDistance = sInitialSidelineMaxDistance;
    sSidelineMaxDistanceWhileAvoiding = sInitialSidelineMaxDistanceWhileAvoiding;
    fn_8000F178(this);
    m_pFielder = fielder;
}

AvoidController::~AvoidController()
{
    m_Avoidances.Clear();
}

extern "C" void fn_8000F178(AvoidController* controller)
{
    controller->m_ThingsToAvoid = AVOID_NOTHING;
    controller->m_CurrentlyAvoiding = AVOID_NOTHING;
    controller->m_VeryCloseToSideline = false;
    controller->m_fRepulsionMult = 1.0f;
    controller->m_SidelineUnavoidable = false;
    controller->m_SidelineNormal = v2Zero;
    controller->m_SidelineDirection = v2Zero;

    for (int i = 0; i < sizeof(controller->m_LastRepulVec) / sizeof(controller->m_LastRepulVec[0]); ++i)
    {
        controller->m_LastRepulWeight[i] = 0.0f;
        controller->m_LastRepulVec[i] = v3Zero;
        controller->m_AvoidanceMemory[i].mRepulsion = v3Zero;
        controller->m_AvoidanceMemory[i].mTimer.Clear();
    }

    controller->m_Avoidances.Clear();
    controller->m_NumAvoidances = 0;
}

extern "C" void fn_8000F324(AvoidController* controller,
    void* context, DebugWriteCache* cache)
{
    if (sAvoidControllerType == 0xFFFF)
    {
        controller->RegisterDebugFields(&sAvoidControllerType, cache);
    }

    AvoidController* copy = (AvoidController*)cache->WriteData(sAvoidControllerType, controller, sizeof(AvoidController));
    if (copy != 0)
    {
        *(int*)&copy->m_pFielder = controller->m_pFielder == 0
            ? -1
            : controller->m_pFielder->mUnidentified120;
        cache->ChecksumData(sAvoidControllerType, copy, context);
    }
}

void AvoidController::SetThingsToAvoid(int thingsToAvoid)
{
    m_ThingsToAvoid = thingsToAvoid;
    if (thingsToAvoid == AVOID_NOTHING)
    {
        fn_8000F178(this);
    }
}

nlVector3& AvoidController::GetLastRepulsionVector(eAvoidableThings things)
{
    return m_LastRepulVec[GetAvoidableIndex(things)];
}

extern "C" float fn_8000F558(
    AvoidController* controller, eAvoidableThings things)
{
    return controller->m_LastRepulWeight[GetAvoidableIndex(things)];
}

extern "C" void RemoveFromAvoidControllers(AvoidableObject* pObject)
{
    if (pObject->mType == AVOID_FIELDERS || pObject->mType == AVOID_GOALIES)
    {
        return;
    }

    nlList<ObstacleAvoidance> list(0, 0);
    AvoidanceRemovalCollector callback(pObject, list);

    for (int i = 0; i < 10; ++i)
    {
        cCharacter* pCharacter = g_pCharacters[i];
        if (pCharacter != 0 && pCharacter->m_eClassType == FIELDER)
        {
            list.m_pEnd = 0;
            list.m_pStart = 0;
            AvoidController* controller = ((cFielder*)pCharacter)->GetAvoidController();
            ObstacleAvoidanceTree& tree = controller->m_Avoidances;
            tree.Walk(
                &callback, &AvoidanceRemovalCollector::Collect);

            ObstacleAvoidance* value = list.m_pStart;
            while (value != 0)
            {
                --controller->m_NumAvoidances;
                AvoidableObject* object = value->mpObstacle;
                value = value->next;
                tree.Remove(object->mId);
            }
        }
    }
}

void AvoidanceRemovalCollector::Collect(
    const u32&, ObstacleAvoidance* value)
{
    if (value->mpAvoider == mpRemovedObject || value->mpObstacle == mpRemovedObject)
    {
        nlListAddEnd(&mRemovals.m_pStart, &mRemovals.m_pEnd, value);
    }
}

static inline bool CanAddAvoidance(AvoidController& controller,
    AvoidableObject* pSelf, AvoidableObject* pObject, bool alreadyTracked)
{
    bool bCanAvoid = !alreadyTracked;
    if (bCanAvoid)
        bCanAvoid = controller.IsAvoiding(pObject->mType);
    if (bCanAvoid)
        bCanAvoid = pSelf != pObject;
    if (bCanAvoid)
        bCanAvoid = pSelf->GetAvoidanceWeight(pObject) > 0.0f;
    if (bCanAvoid)
        bCanAvoid = pSelf->IsWithinRange(pObject, -1.0f);
    return bCanAvoid;
}

void AvoidController::Update(float fDeltaT)
{
    float fTotalWeight_v3 = 0.0f;
    nlVector3 vAccumulated_v3 = v3Zero;
    nlVector3 v3Repulsion = v3Zero;
    float fWeights[NUM_AVOIDABLES];
    int nCounts[NUM_AVOIDABLES];
    nlVector3 v3Vectors[NUM_AVOIDABLES];
    for (int i = 0; i < NUM_AVOIDABLES; ++i)
    {
        nCounts[i] = 0;
        fWeights[i] = 0.0f;
        v3Vectors[i] = v3Zero;
    }

    bool bCanAvoid;
    m_VeryCloseToSideline = m_SidelineUnavoidable = false;
    bCanAvoid = IsAvoiding(AVOID_SIDELINES);
    if (bCanAvoid)
    {
        bool bHasGlobalPad = m_pFielder->GetGlobalPad() != 0;
        if (bHasGlobalPad)
            AvoidSidelines(v3Repulsion);
    }

    nlList<ObstacleAvoidance> list(0, 0);
    RepulsionAccumulator callback(fDeltaT,
        vAccumulated_v3, fTotalWeight_v3, v3Vectors, fWeights, nCounts, list);
    m_Avoidances.Walk(&callback,
        &RepulsionAccumulator::Accumulate);

    AvoidableObject* pSelf = m_pFielder->mUnidentified320;
    ObstacleAvoidance* value;
    for (AvoidableObject* pObject = gAvoidableObjects.m_pStart;
         pObject != 0 && m_NumAvoidances < 99; pObject = pObject->next)
    {
        if (CanAddAvoidance(*this, pSelf, pObject,
                m_Avoidances.FindGet((u32)pObject->mId, &value)))
        {
            if (!IsAvoiding(AVOID_SIDELINES)
                && pObject->mType == AVOID_POLYGONS
                && ((AvoidablePolygon*)pObject)->mPolygonType == 1)
                continue;
            ++m_NumAvoidances;
            value = m_Avoidances.UnidentifiedAddOrGet((u32)pObject->mId);
            value->Initialize(pSelf, pObject);
            value->Update(fDeltaT);
            float fWeight = value->GetWeight();
            if (fWeight)
            {
                const nlVector3& v3Repulsion = value->mRepulsion;
                nlVec3ScaleAdd(vAccumulated_v3, fWeight,
                    v3Repulsion, vAccumulated_v3);
                fTotalWeight_v3 += fWeight;
                int index = GetAvoidableIndex((eAvoidableThings)pObject->mType);
                nlVec3ScaleAdd(v3Vectors[index], fWeight,
                    v3Repulsion, v3Vectors[index]);
                fWeights[index] += fWeight;
                ++nCounts[index];
            }
        }
    }

    ObstacleAvoidance* entry = list.m_pStart;
    while (entry != 0)
    {
        --m_NumAvoidances;
        AvoidableObject* pObject = entry->mpObstacle;
        entry = entry->next;
        m_Avoidances.Remove((u32)pObject->mId);
    }
    list.m_pEnd = 0;
    list.m_pStart = 0;

    bool bAverageWithLastRepulsion = m_CurrentlyAvoiding != 0;
    m_CurrentlyAvoiding = 0;
    int nCount = 0;
    for (int i = 0; i < 7; ++i)
    {
        if (fWeights[i] > 0.0f)
        {
            nCount += nCounts[i];
            nlVec3Scale(v3Repulsion, v3Vectors[i], 1.0f / fWeights[i]);
            float fWeight = (fWeights[i] / nCounts[i]) / gAvoidableTweaks[i][0];
            eAvoidableThings things = (eAvoidableThings)GetAvoidableMask(i);
            m_CurrentlyAvoiding |= things;
            SetLastRepulsionVector(things, v3Repulsion, fWeight);
            m_AvoidanceMemory[i].mRepulsion = v3Repulsion;
            m_AvoidanceMemory[i].mTimer.SetSeconds(sAvoidanceMemoryRefreshSeconds);
        }
        else
        {
            eAvoidableThings things = (eAvoidableThings)GetAvoidableMask(i);
            m_CurrentlyAvoiding &= ~things;
            SetLastRepulsionVector(things, v3Zero, 0.0f);
        }
    }

    nlVector3 v3FinalRepulsion = v3Zero;
    float fWeight = 0.0f;
    if (fTotalWeight_v3 && m_fRepulsionMult)
    {
        nlVec3Scale(v3FinalRepulsion, vAccumulated_v3,
            m_fRepulsionMult / fTotalWeight_v3);
        fWeight = fTotalWeight_v3 / nCount;
        nlVector3 v3SmoothedRepulsion = v3FinalRepulsion;
        if (bAverageWithLastRepulsion)
        {
            const float fLastRepulsionWeight = 0.8f;
            nlVecLerp(v3SmoothedRepulsion,
                m_LastRepulVec[GetAvoidableIndex(AVOID_EVERYTHING)],
                v3FinalRepulsion, fLastRepulsionWeight);
        }
        ApplyRepulsionVector(v3SmoothedRepulsion);
    }
    SetLastRepulsionVector(AVOID_EVERYTHING, v3FinalRepulsion, fWeight);
    m_fRepulsionMult = 1.0f;
}

void RepulsionAccumulator::Accumulate(
    const u32&, ObstacleAvoidance* value)
{
    AvoidableObject* pObject = value->mpObstacle;
    AvoidController* controller = (((AvoidableFielder*)value->mpAvoider)->m_pFielder)->GetAvoidController();
    float fWeight = 0.0f;
    if (controller->IsAvoiding(pObject->mType))
    {
        value->Update(mfDeltaT);
        fWeight = value->GetWeight();
    }
    if (fWeight)
    {
        nlVec3ScaleAdd(mAccumulated, fWeight,
            value->mRepulsion, mAccumulated);
        mTotalWeight += fWeight;
        int index = GetAvoidableIndex((eAvoidableThings)pObject->mType);
        nlVec3ScaleAdd(mpCategoryVectors[index], fWeight,
            value->mRepulsion, mpCategoryVectors[index]);
        mpCategoryWeights[index] += fWeight;
        ++mpCategoryCounts[index];
    }
    else
    {
        nlListAddEnd(&mRemovals.m_pStart, &mRemovals.m_pEnd, value);
    }
}

bool AvoidController::CalcDesiredVelocityToAvoidSideline(
    nlVector2& vNewDesiredVelDir,
    const nlVector2& vCurrentDesiredVelDir,
    const nlVector2& vCurrentVelDir,
    const nlVector2& vSidelinePos,
    const nlVector2& vSidelineNormal)
{
    nlVector2 vParallelVelDir;
    nlColour colour;
    bool bHitSideline = false;

    float fDotNormalVel = vSidelineNormal.x * vCurrentDesiredVelDir.x
        + vSidelineNormal.y * vCurrentDesiredVelDir.y;

    nlVector3 pPos = m_pFielder->m_pBall != NULL
        ? m_pFielder->m_pBall->m_v3Position : m_pFielder->m_DetChar.m_v3Position;
    pPos.z = 0.0f;
    nlVector3 v3SidelinePos = { 0.0f, 0.0f, 0.0f };
    v3SidelinePos.x = vSidelinePos.x;
    v3SidelinePos.y = vSidelinePos.y;
    nlVector3 v3SidelineNormal = { 0.0f, 0.0f, 0.0f };
    v3SidelineNormal.x = vSidelineNormal.x;
    v3SidelineNormal.y = vSidelineNormal.y;
    nlVector2 vToSideline;
    vToSideline.x = v3SidelinePos.x - pPos.x;
    vToSideline.y = v3SidelinePos.y - pPos.y;
    float fDistanceSquared = nlVec2LengthSquared(vToSideline);
    float fDistance = nlSqrt(fDistanceSquared, true);

    fDistance -= m_pFielder->mUnidentified320->GetRadius();
    float fMaxDistance = sSidelineMaxDistance;
    if (m_CurrentlyAvoiding & AVOID_SIDELINES)
    {
        fMaxDistance = sSidelineMaxDistanceWhileAvoiding;
    }

    m_SidelineUnavoidable = false;
    m_VeryCloseToSideline = false;
    float fMinDistance = 2.0f * sSidelineMaxDistanceWhileAvoiding;

    if (fDistance <= fMinDistance)
    {
        m_VeryCloseToSideline = true;
        m_SidelineNormal = vSidelineNormal;
        m_SidelineDirection = vNewDesiredVelDir;
    }

    if (fDistance <= fMaxDistance)
    {
        if (fDotNormalVel <= 0.0f)
        {
            if (fDotNormalVel > sSidelineUnavoidableDot)
            {
                float fCos, fSin;
                nlSinCos(&fSin, &fCos, 0x4000);

                vParallelVelDir.x = vSidelineNormal.x * fCos - vSidelineNormal.y * fSin;
                vParallelVelDir.y = vSidelineNormal.y * fCos + vSidelineNormal.x * fSin;

                if (vParallelVelDir.x * vCurrentDesiredVelDir.x + vParallelVelDir.y * vCurrentDesiredVelDir.y < 0.0f)
                {
                    nlVec2Sub(vParallelVelDir,
                        *(const nlVector2*)&v3Zero, vParallelVelDir);
                }

                vNewDesiredVelDir = vParallelVelDir;
            }
            else
            {
                vNewDesiredVelDir = v2Zero;
                m_SidelineUnavoidable = true;
            }
            bHitSideline = true;
            if (lbl_806E0BB9)
            {
                nlVector3 v3DebugPos;
                nlVector3 v3DebugNormalEnd;
                nlVec3Set(v3DebugPos,
                    vSidelinePos.x, vSidelinePos.y, 0.0f);
                nlVec3Set(v3DebugNormalEnd,
                    vSidelinePos.x + vSidelineNormal.x,
                    vSidelinePos.y + vSidelineNormal.y, 0.0f);
                nlColourSet(colour, 255, 0, 0, 255);
                g_ShapeRenderer.DrawEllipse2D(v3DebugPos,
                    0.2f, 1.0f, 1.0f, colour, true);
                nlColourSet(colour, 0, 0, 255, 255);
                g_ShapeRenderer.DrawLine3D(v3DebugPos, v3DebugNormalEnd, colour, true);
            }
        }
    }

    return bHitSideline;
}

bool AvoidController::CalcDesiredVelocityToAvoidCorner(
    nlVector2& vNewDesiredVelDir,
    const sCornerSegment& corner,
    const nlVector2& vCurrentDesiredVelDir,
    const nlVector2& vCurrentVelDir)
{
    bool bHitSideline = false;
    nlVector2 vSidelinePos;
    nlVector2 vSidelineNormal;
    nlVector2 vPosition = *(nlVector2*)&m_pFielder->m_DetChar.m_v3Position;
    nlVector2 vBallPosition;

    if (m_pFielder->m_pBall != NULL)
    {
        vBallPosition = *(nlVector2*)&m_pFielder->m_pBall->m_v3Position;
    }
    else
    {
        vBallPosition = vPosition;
    }

    nlVector2 vCornerToBall;
    nlVector2 vCornerToFielder;
    nlVector2 vBallToCorner;
    nlVec2Sub(vBallToCorner, corner.vCenter, vBallPosition);
    if (nlVec2Length(vBallToCorner) <= corner.fRadius)
    {
        nlVec2Sub(vCornerToBall, vBallPosition, corner.vCenter);

        f32 fAngle = 10430.378f * nlATan2f(vCornerToBall.y, vCornerToBall.x);
        u32 aCornerToPos = (u16)(s32)fAngle;

        u16 absEnd = (u16)abs_s16((s16)(aCornerToPos - corner.thetaEnd));

        u16 absStart = (u16)abs_s16((s16)(aCornerToPos - corner.thetaStart));

        if (absStart >= absEnd)
            absEnd = absStart;
        if ((s16)absEnd <= 0x4000)
        {
            nlVec2Sub(vCornerToFielder, vPosition, corner.vCenter);

            f32 fAngle2 = 10430.378f * nlATan2f(vCornerToFielder.y, vCornerToFielder.x);
            u32 aCornerToFielder = (u16)(s32)fAngle2;

            u16 absEnd2 = (u16)abs_s16((s16)(aCornerToFielder - corner.thetaEnd));

            u16 absStart2 = (u16)abs_s16((s16)(aCornerToFielder - corner.thetaStart));

            if (absStart2 >= absEnd2)
                absEnd2 = absStart2;
            if ((s16)absEnd2 <= 0x4000)
            {
                float fInvDistance = nlRecipSqrt(nlVec2DotProduct(vCornerToFielder, vCornerToFielder), true);
                nlVec2Set(vCornerToFielder, fInvDistance * vCornerToFielder.x, fInvDistance * vCornerToFielder.y);
                nlVec2Sub(vSidelineNormal, v2Zero, vCornerToFielder);
                float fRadius = corner.fRadius;
                nlVec2Set(vSidelinePos, fRadius * vCornerToFielder.x + corner.vCenter.x, fRadius * vCornerToFielder.y + corner.vCenter.y);
                bHitSideline = CalcDesiredVelocityToAvoidSideline(vNewDesiredVelDir, vCurrentDesiredVelDir, vCurrentVelDir, vSidelinePos, vSidelineNormal);
            }

        }
    }
    return bHitSideline;
}

inline bool AvoidController::CalcDesiredVelocityToAvoidSideline(
    nlVector2& vNewDesiredVelDir,
    const sSideLinePlane& sideline,
    const nlVector2& vCurrentDesiredVelDir,
    const nlVector2& vCurrentVelDir)
{
    nlVector2 vSidelinePos = *(nlVector2*)&m_pFielder->m_DetChar.m_v3Position;
    nlVector2 vSidelineNormal;
    nlVec2Sub(vSidelineNormal, v2Zero, sideline.vNormal);
    if (vSidelineNormal.x == 0.0f)
        vSidelinePos.y = sideline.fDistance * sideline.vNormal.y;
    else
        vSidelinePos.x = sideline.fDistance * sideline.vNormal.x;
    bool bHitSideline = CalcDesiredVelocityToAvoidSideline(vNewDesiredVelDir,
        vCurrentDesiredVelDir, vCurrentVelDir, vSidelinePos, vSidelineNormal);
    return bHitSideline;
}

bool AvoidController::AvoidSidelines(nlVector3& v3OutRepulsion)
{
    u16 aDesiredMovementDir;
    bool bHitSideline;
    bool bTurboAllowed;
    nlVector2 vCurrentVelDir;
    nlVector2 vCurrentDesiredVelDir;
    nlVector2 vNewDesiredVelDir;

    m_SidelineRepulsion = v3Zero;
    if (m_pFielder->GetDistanceToDesiredPos() <= 0.25f)
        return false;
    bTurboAllowed = true;
    nlSinCos(&vCurrentVelDir.y, &vCurrentVelDir.x, m_pFielder->m_DetChar.m_aActualMovementDirection);
    vCurrentDesiredVelDir = *(const nlVector2*)&m_pFielder->GetDesiredVelocity();
    float fLengthSquared = vCurrentDesiredVelDir.x * vCurrentDesiredVelDir.x + vCurrentDesiredVelDir.y * vCurrentDesiredVelDir.y;
    if (fLengthSquared > 0.0f)
    {
        float fInvLength = nlRecipSqrt(fLengthSquared, true);
        nlVec2Set(vCurrentDesiredVelDir, fInvLength * vCurrentDesiredVelDir.x, fInvLength * vCurrentDesiredVelDir.y);
    }
    else
        nlPolarToCartesian(vCurrentDesiredVelDir.x, vCurrentDesiredVelDir.y, m_pFielder->m_DetChar.m_aDesiredMovementDirection, 1.0f);
    vNewDesiredVelDir = vCurrentDesiredVelDir;
    {
        for (int i = 0; i < 4; i++)
        {
            sCornerSegment corner = cField::GetCorner(i);
            bHitSideline = CalcDesiredVelocityToAvoidCorner(vNewDesiredVelDir, corner, vCurrentDesiredVelDir, vCurrentVelDir);
            if (bHitSideline)
                break;
        }
    }
    if (!bHitSideline)
    {
        for (int i = 0; i < 4; i++)
        {
            bHitSideline = CalcDesiredVelocityToAvoidSideline(vNewDesiredVelDir,
                cField::GetSideline(i), vCurrentDesiredVelDir, vCurrentVelDir);
            if (bHitSideline)
                break;
        }
    }
    if (bHitSideline)
    {
        bool isZero = nlNear(v2Zero, vNewDesiredVelDir);
        if (isZero)
            bTurboAllowed = false;
        else
        {
            float fDot = nlVec2DotProduct(vNewDesiredVelDir, vCurrentVelDir);
            if (fDot < 0.99f)
                bTurboAllowed = false;
            aDesiredMovementDir = nlVector3ToAngle(
                *(const nlVector3*)&vNewDesiredVelDir);
            m_pFielder->fn_8001DCE0(aDesiredMovementDir);
            m_pFielder->SetDesiredFacingDirection(aDesiredMovementDir, false);
        }
    }
    if (!bTurboAllowed && m_pFielder->IsRunning() && m_pFielder->m_pBall != NULL)
    {
        f32 fDesiredSpeed = ClampRunningWBSpeed(m_pFielder->m_DetChar.m_fDesiredSpeed, m_pFielder->GetTweaks()->GetRunningSpeed());
        aDesiredMovementDir = m_pFielder->m_DetChar.m_aDesiredMovementDirection;
        m_pFielder->m_DetChar.m_fDesiredSpeed = fDesiredSpeed;
        m_pFielder->fn_8001DCE0(aDesiredMovementDir);
        m_pFielder->SetDesiredFacingDirection(aDesiredMovementDir, false);
    }
    v3OutRepulsion = m_SidelineRepulsion;
    return bHitSideline;
}

void AvoidController::ApplyRepulsionVector(nlVector3 v3Repulsion)
{
    float fRepulsionMag = nlVec3Length(v3Repulsion);
    if (fRepulsionMag <= 0.1f)
        return;

    nlVector3 v3RepulsionDir;
    nlVector3& rRepulsionDir = v3RepulsionDir;
    nlVec3Normalize(rRepulsionDir, v3Repulsion);

    if (m_VeryCloseToSideline)
    {
        float fDotNormalVel = rRepulsionDir.x * m_SidelineNormal.x
            + rRepulsionDir.y * m_SidelineNormal.y;
        if (fDotNormalVel < -0.1f)
        {
            if (nlVec2DotProduct(m_SidelineDirection, m_SidelineDirection) > 0.0f)
            {
                nlVector3 v3Parallel;
                nlVector3 v3Perpendicular;
                nlVector3 v3SidelineDir;
                nlVec3Set(v3SidelineDir,
                    m_SidelineDirection.x, m_SidelineDirection.y, 0.0f);
                nlVec3Project(v3Parallel,
                    rRepulsionDir, v3SidelineDir);
                nlVec3Sub(v3Perpendicular, rRepulsionDir, v3Parallel);
                nlVec3ScaleAdd(rRepulsionDir, -1.0f, v3Perpendicular, v3Parallel);
                rRepulsionDir.z = 0.0f;
                nlVec3Scale(v3Repulsion, rRepulsionDir, fRepulsionMag);
            }
            else
            {
                float fScaledNormalY = fRepulsionMag * m_SidelineNormal.y;
                float fScaledNormalX = fRepulsionMag * m_SidelineNormal.x;
                v3Repulsion.y = fScaledNormalY;
                v3Repulsion.x = fScaledNormalX;
            }
        }
    }

    if (lbl_806E0BB8)
    {
        nlColour colour;
        nlColourSet(colour, 0, 0, 255, 255);
        nlVector3 v3DebugLineEnd;
        nlVec3ScaleAdd(v3DebugLineEnd, 0.5f, v3Repulsion, m_pFielder->m_DetChar.m_v3Position);
        g_ShapeRenderer.DrawLine3D(m_pFielder->m_DetChar.m_v3Position, v3DebugLineEnd, colour, true);
    }

    nlVec3Add(v3Repulsion, v3Repulsion, m_pFielder->GetDesiredVelocity());
    float fDesiredSpeed = m_pFielder->GetRunningSpeed();
    float fResultantMag = nlVec3Length(v3Repulsion);
    fDesiredSpeed = fResultantMag <= fDesiredSpeed ? fResultantMag : fDesiredSpeed;
    float fJogSpeed = m_pFielder->GetSpeedPowerupAdjusted(GetJogSpeed(m_pFielder->GetTweaks()));
    if (fDesiredSpeed >= 0.35f * fJogSpeed)
    {
        fDesiredSpeed = fDesiredSpeed >= fJogSpeed ? fDesiredSpeed : fJogSpeed;
        m_pFielder->m_DetChar.m_fDesiredSpeed = fDesiredSpeed;
        m_pFielder->fn_8001DCE0(nlVector3ToAngle(v3Repulsion));
        m_pFielder->SetDesiredFacingDirection(nlVector3ToAngle(v3Repulsion), false);
    }
    else
        m_pFielder->m_DetChar.m_fDesiredSpeed = 0.0f;
}

ObstacleAvoidance::ObstacleAvoidance()
{
    Initialize(0, 0);
}

void ObstacleAvoidance::Update(float fDeltaT)
{
    float fWasActive = mWeight > 0.0f;
    nlVector3 v3Repulsion = v3Zero;
    float fWeight = mpAvoider->GetAvoidanceWeight(mpObstacle);
    if (fWeight == 0.0f)
        mWeight = 0.0f;
    else
    {
        AvoidanceContext context;
        bool bResponded = false;
        CalcContext(context, fDeltaT);
        if (context.mGap <= mpObstacle->mTweaks[2])
        {
            switch (mpObstacle->mType)
            {
            case AVOID_FIELDERS:
            case AVOID_GOALIES:
                bResponded = AgentResponse(context, fDeltaT);
                break;
            case AVOID_POWERUPS:
            {
                AvoidablePowerup* pPowerup = (AvoidablePowerup*)mpObstacle;
                if (pPowerup->m_pChainChomp != 0
                    || pPowerup->m_pPowerup->m_eType == POWER_UP_RED_SHELL)
                {
                    bResponded = AgentResponse(context, fDeltaT);
                    break;
                }
            }
            case AVOID_POLYGONS:
            case AVOID_BOWSER:
            case AVOID_PATCHES:
                bResponded = mpObstacle->IsMobile()
                    ? MobileObstacleResponse(context, fDeltaT)
                    : StaticObstacleResponse(context, fDeltaT);
                break;
            }
            context.mRepulsionMag *= mpAvoider->GetAvoidanceStrength(mpObstacle);
        }
        if (bResponded && context.mRepulsionMag >= 0.1f)
        {
            mFadeOutTimer.Clear();
            mWeight = fWeight * context.mProximity;
            context.mRepulsionMag *= context.mProximity;
            nlVec3Scale(v3Repulsion, context.mRepulsionDir, context.mRepulsionMag);
            mRepulsionHistory.Update(mRepulsion, v3Repulsion, fDeltaT);
        }
        else
            fWeight = 0.0f;
    }
    if (mWeight && !fWeight)
    {
        if (mFadeOutTimer.m_uPackedTime == 0)
            mFadeOutTimer.SetSeconds(0.3f);
        if (mFadeOutTimer.Countdown(fDeltaT, 0.0f))
        {
            mRepulsionHistory.UnidentifiedReset();
            mWeight = 0.0f;
        }
        else
        {
            mRepulsionHistory.Update(mRepulsion,
                mRepulsionHistory.GetLastSample(), fDeltaT, 0.3f, &mFadeOutTimer);
        }
    }
    if (!fWasActive && mWeight)
        mActiveTimer.Clear();
    mActiveTimer.Countup(fDeltaT, 10.0f);
}

void ObstacleAvoidance::CalcContext(
    AvoidanceContext& context, float fDeltaT)
{
    bool bAvoiderInsideObstacle = mpObstacle->GetClosestBoundaryPoint(
        mpAvoider->GetPosition(), context.mObstaclePoint, context.mObstacleNormal);
    context.mContactState = mpAvoider->GetClosestBoundaryPoint(
        context.mObstaclePoint, context.mAvoiderPoint, context.mAvoiderNormal) || bAvoiderInsideObstacle;
    context.mGap = nlSqrt(nlVec3DistanceSquared2D(context.mAvoiderPoint, context.mObstaclePoint), true);
    if (context.mContactState)
        context.mGap *= -1.0f;

    cFielder* pFielder = ((AvoidableFielder*)mpAvoider)->m_pFielder;
    if (mpObstacle->mType == AVOID_FIELDERS
        && !((AvoidableFielder*)mpObstacle)->m_pFielder->IsOnSameTeam(pFielder))
        context.mGap -= mpObstacle->GetAttackReach();
    context.mProximity = NormalizeVal(context.mGap,
        mpObstacle->mTweaks[2], mpObstacle->mTweaks[1]);
    context.mClosingSpeed = GetClosingSpeed2D(
        context.mAvoiderPoint, pFielder->GetDesiredVelocity(),
        context.mObstaclePoint, mpObstacle->GetVelocity());
    float fClosingSpeed = context.mClosingSpeed;
    context.mClosingSpeed = nlMaxEquals(0.0f, fClosingSpeed);
    context.mDesiredSpeed = nlVec2Length(*(const nlVector2*)&pFielder->GetDesiredVelocity());
    if (context.mDesiredSpeed > 0.1f)
        nlVec3Scale(context.mDesiredDir, pFielder->GetDesiredVelocity(), 1.0f / context.mDesiredSpeed);
    else
        context.mDesiredDir = v3Zero;

    if (mpObstacle->mType == AVOID_POLYGONS)
    {
        AvoidablePolygon* pPolygon = (AvoidablePolygon*)mpObstacle;
        if (pPolygon->mPolygonType == 2 && pPolygon->mOwner != 0
            && !pPolygon->mOwner->IsOnSameTeam(((AvoidableFielder*)mpAvoider)->m_pFielder))
            nlVec3Scale(context.mObstacleNormal, -1.0f);
    }
}

bool ObstacleAvoidance::AgentResponse(
    AvoidanceContext& context, float fDeltaT)
{
    cFielder* pFielder = ((AvoidableFielder*)mpAvoider)->m_pFielder;
    float fDistanceSquared = nlVec3DistanceSquared2D(
        pFielder->GetDesiredPosition(), mpObstacle->GetPosition())
        - (mpObstacle->mTweaks[1] * mpObstacle->mTweaks[1]);
    float fRadius = mpObstacle->GetRadius();
    bool bDesiredPosBlocked = fDistanceSquared < fRadius * fRadius;
    if (context.mContactState)
        return OverlapResponse(!bDesiredPosBlocked, context, fDeltaT);
    if (bDesiredPosBlocked)
    {
        const nlVector3& v3Normal = context.mObstacleNormal;
        context.mContactState = 2;
        nlVec3Scale(context.mRepulsionDir, v3Normal, context.mClosingSpeed);
        if (!(-nlVec3DotProduct(context.mObstacleNormal, context.mDesiredDir) >= 0.7f))
        {
            nlVector3 v3Repulsion;
            CalcPerpendicularToward(v3Repulsion, context.mDesiredDir, v3Normal, false);
            nlVec3ScaleAdd(context.mRepulsionDir, context.mDesiredSpeed, v3Repulsion, context.mRepulsionDir);
        }
        context.NormalizeRepulsion();
        return true;
    }

    float fWeight = InterpolateRangeClamped(1.0f, 0.0f, 0.7f, 1.0f,
        -nlVec3DotProduct(context.mObstacleNormal, context.mDesiredDir));
    context.mRepulsionDir = context.mObstacleNormal;
    if (fWeight < 1.0f)
    {
        CalcPerpendicularToward(context.mRepulsionDir, context.mDesiredDir, context.mObstacleNormal, true);
        nlVec3WeightedSum(context.mRepulsionDir, 1.0f - fWeight, context.mRepulsionDir, fWeight, context.mObstacleNormal);
        nlVec3Scale(context.mRepulsionDir,
            nlRecipSqrt(nlVec3LengthSquared(context.mRepulsionDir), true));
    }
    context.mRepulsionMag = ((AvoidableFielder*)mpAvoider)->m_pFielder->GetRunningSpeed();
    nlVec3Scale(context.mRepulsionDir, context.mRepulsionMag);
    nlVec3ScaleAdd(context.mRepulsionDir, 0.9f * context.mClosingSpeed, context.mObstacleNormal, context.mRepulsionDir);
    context.NormalizeRepulsion();
    return true;
}

bool ObstacleAvoidance::MobileObstacleResponse(
    AvoidanceContext& context, float fDeltaT)
{
    cFielder* pFielder = ((AvoidableFielder*)mpAvoider)->m_pFielder;
    float fDistanceSquared = nlVec3DistanceSquared2D(
        pFielder->GetDesiredPosition(), mpObstacle->GetPosition())
        - (mpObstacle->mTweaks[1] * mpObstacle->mTweaks[1]);
    float fRadius = mpObstacle->GetRadius();
    bool bDesiredPosBlocked = fDistanceSquared < fRadius * fRadius;
    if (context.mContactState)
        return OverlapResponse(bDesiredPosBlocked ? 0 : 2, context, fDeltaT);

    nlVector3 v3Velocity = mpObstacle->GetVelocity();
    v3Velocity.z = 0.0f;
    float fLengthSquared = nlVec3LengthSquared(v3Velocity);
    if (fLengthSquared > 0.1f)
    {
        nlVec3Scale(v3Velocity, 1.0f / nlSqrt(fLengthSquared, true));
        CalcPerpendicularToward(context.mRepulsionDir, context.mObstacleNormal, v3Velocity, false);
    }
    else
        context.mRepulsionDir = context.mObstacleNormal;
    context.mRepulsionMag = nlMaxEquals(context.mClosingSpeed,
        ((AvoidableFielder*)mpAvoider)->m_pFielder->GetRunningSpeed());
    return true;
}

bool ObstacleAvoidance::StaticObstacleResponse(
    AvoidanceContext& context, float fDeltaT)
{
    cFielder* pFielder = ((AvoidableFielder*)mpAvoider)->m_pFielder;
    float fDistanceSquared = nlVec3DistanceSquared2D(
        pFielder->GetDesiredPosition(), mpObstacle->GetPosition())
        - (mpObstacle->mTweaks[1] * mpObstacle->mTweaks[1]);
    float fRadius = mpObstacle->GetRadius();
    bool bDesiredPosBlocked = fDistanceSquared < fRadius * fRadius;
    if (context.mContactState)
        return OverlapResponse(1, context, fDeltaT);
    if (bDesiredPosBlocked)
    {
        const nlVector3& v3Normal = context.mObstacleNormal;
        context.mContactState = 2;
        nlVec3Scale(context.mRepulsionDir, v3Normal, context.mClosingSpeed);
        if (-nlVec3DotProduct(context.mObstacleNormal, context.mDesiredDir) >= 0.0f)
        {
            nlVector3 v3Repulsion;
            CalcPerpendicularToward(v3Repulsion, context.mDesiredDir, v3Normal, false);
            nlVec3ScaleAdd(context.mRepulsionDir, context.mDesiredSpeed, v3Repulsion, context.mRepulsionDir);
        }
        context.NormalizeRepulsion();
        return true;
    }

    float fWeight = InterpolateRangeClamped(1.0f, 0.0f, 0.7f, 1.0f,
        -nlVec3DotProduct(context.mObstacleNormal, context.mDesiredDir));
    context.mRepulsionDir = context.mObstacleNormal;
    if (fWeight < 1.0f)
    {
        CalcPerpendicularToward(context.mRepulsionDir, context.mDesiredDir, context.mObstacleNormal, false);
        nlVec3WeightedSum(context.mRepulsionDir, 1.0f - fWeight, context.mRepulsionDir, fWeight, context.mObstacleNormal);
        nlVec3Scale(context.mRepulsionDir,
            nlRecipSqrt(nlVec3LengthSquared(context.mRepulsionDir), true));
    }
    context.mRepulsionMag = ((AvoidableFielder*)mpAvoider)->m_pFielder->GetRunningSpeed();
    nlVec3Scale(context.mRepulsionDir, context.mRepulsionMag);
    nlVec3ScaleAdd(context.mRepulsionDir, 0.9f * context.mClosingSpeed, context.mObstacleNormal, context.mRepulsionDir);
    context.NormalizeRepulsion();
    return true;
}

bool ObstacleAvoidance::OverlapResponse(
    int mode, AvoidanceContext& context, float fDeltaT)
{
    context.mRepulsionDir = context.mObstacleNormal;
    if (mode != 0 && context.mDesiredSpeed > 0.5f)
    {
        if (mode == 1)
        {
            float fWeight = InterpolateRangeClamped(1.0f, 0.0f, 0.0f, 1.0f,
                -nlVec3DotProduct(context.mObstacleNormal, context.mDesiredDir));
            if (fWeight < 1.0f)
            {
                CalcPerpendicularToward(context.mRepulsionDir, context.mDesiredDir, context.mObstacleNormal, true);
                nlVec3WeightedSum(context.mRepulsionDir, 1.0f - fWeight, context.mRepulsionDir, fWeight, context.mObstacleNormal);
                nlVec3Scale(context.mRepulsionDir, nlRecipSqrt(nlVec3LengthSquared(context.mRepulsionDir), true));
            }
        }
        else if (mode == 2)
        {
            nlVector3 v3Velocity = mpObstacle->GetVelocity();
            float fLengthSquared = nlVec3LengthSquared(v3Velocity);
            if (fLengthSquared > 0.1f)
            {
                nlVec3Scale(v3Velocity, 1.0f / nlSqrt(fLengthSquared, true));
                CalcPerpendicularToward(context.mRepulsionDir, context.mDesiredDir, v3Velocity, false);
            }
        }
    }
    context.mContactState = 1;
    context.mRepulsionMag = ((AvoidableFielder*)mpAvoider)->m_pFielder->GetRunningSpeed();
    nlVec3Scale(context.mRepulsionDir, context.mRepulsionMag);
    nlVec3ScaleAdd(context.mRepulsionDir, -context.mDesiredSpeed, context.mDesiredDir, context.mRepulsionDir);
    context.NormalizeRepulsion();
    return true;
}

void ObstacleAvoidance::CalcPerpendicularToward(
    nlVector3& output, const nlVector3& first, const nlVector3& second, bool rotateFirst)
{
    int angle = 0x4000;
    if (nlAbs(nlVec3DotProduct(second, first)) < 0.998f)
    {
        unsigned short secondAngle = nlVector3ToAngle(second);
        unsigned short firstAngle = nlVector3ToAngle(first);
        float difference = (short)(secondAngle - firstAngle);
        angle = difference > 0.0f ? 0x4000 : 0xC000;
    }
    if (rotateFirst)
    {
        float fCos, fSin;
        nlSinCos(&fSin, &fCos, angle);
        float x = first.x;
        float y = first.y;
        output.x = x * fCos - y * fSin;
        output.y = y * fCos + x * fSin;
    }
    else
    {
        float fCos, fSin;
        nlSinCos(&fSin, &fCos, -angle);
        float x = second.x;
        float y = second.y;
        output.x = x * fCos - y * fSin;
        output.y = y * fCos + x * fSin;
    }
    output.z = 0.0f;
}
