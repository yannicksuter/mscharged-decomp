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

extern "C" int GetAvoidableIndex(eAvoidableThings);
extern "C" AvoidController* fn_8002E144(cFielder*);

static const nlVector2 v2Zero = { 0.0f, 0.0f };

#pragma explicit_zero_data on
static float sUnidentifiedMemorySeconds = 0.0f;
#pragma explicit_zero_data off
static float sUnidentifiedRepulsionValue0 = 0.5f;
static float sUnidentifiedRepulsionValue1 = 1.0f;
static float sUnidentifiedInitialValue0 = 0.5f;
static float sUnidentifiedInitialValue1 = 1.0f;
static float sUnidentifiedValue_806DB4F4 = -0.99f;
static float sUnidentifiedValue_806DB4F8 = 0.01f;
static unsigned short sAvoidControllerType = 0xFFFF;

class UnidentifiedAvoidanceCallback_8000F7FC
{
public:
    UnidentifiedAvoidanceCallback_8000F7FC(AvoidableObject* pObject, nlList<UnidentifiedAvoidanceValue>& list)
        : mUnidentified000(pObject), mUnidentified004(list)
    {
    }
    void UnidentifiedCallback(const u32&, UnidentifiedAvoidanceValue*);

    AvoidableObject* mUnidentified000;
    nlList<UnidentifiedAvoidanceValue>& mUnidentified004;
};

inline float UnidentifiedAvoidanceValue::UnidentifiedGetWeight() const
{
    float fWeight = 1.0f;
    if (mUnidentified024.m_uPackedTime != 0)
        fWeight = mUnidentified024.GetSeconds() / 0.3f;
    return mUnidentified018 * fWeight;
}

class UnidentifiedAvoidanceCallback_800102A8
{
public:
    UnidentifiedAvoidanceCallback_800102A8(float fDeltaT,
        nlVector3& accumulated, float& totalWeight,
        nlVector3* vectors, float* weights, int* counts,
        nlList<UnidentifiedAvoidanceValue>& list)
        : mUnidentified000(fDeltaT), mUnidentified004(accumulated),
          mUnidentified008(totalWeight), mUnidentified00C(vectors),
          mUnidentified010(weights), mUnidentified014(counts),
          mUnidentified018(list)
    {
    }
    void UnidentifiedCallback(const u32&, UnidentifiedAvoidanceValue*);
    float mUnidentified000;
    nlVector3& mUnidentified004;
    float& mUnidentified008;
    nlVector3* mUnidentified00C;
    float* mUnidentified010;
    int* mUnidentified014;
    nlList<UnidentifiedAvoidanceValue>& mUnidentified018;
};

extern "C" const nlVector3& fn_80040318(cFielder*);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" float fn_8002CE14(const PlayerTweaks*);
bool lbl_806E0BB8;

extern "C" const nlVector3& fn_80040234(cFielder*);

bool lbl_806E0BB9;

extern "C" void fn_802BCE50(
    const ShapeRender*, const nlVector3&, float, float, float,
    const nlColour&, bool);



extern "C" void fn_8000F178(AvoidController* controller);
inline UnidentifiedAvoidanceMemory::UnidentifiedAvoidanceMemory()
    : mTimer()
{
    mTimer.SetSeconds(sUnidentifiedMemorySeconds);
}

inline UnidentifiedAvoidanceValue::UnidentifiedAvoidanceValue()
{
    UnidentifiedInitialize(0, 0);
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

inline bool AvoidController::UnidentifiedCanAvoid(int things)
{
    bool bCanAvoid = false;
    if ((m_ThingsToAvoid & things) && Incapacitated(m_pFielder) == 0.0f)
        bCanAvoid = true;
    bool result = bCanAvoid;
    switch (things)
    {
    case AVOID_FIELDERS:
    {
        result = false;
        bool bCanAvoidFielder = bCanAvoid && !m_pFielder->fn_8003E74C();
        if (bCanAvoidFielder && !m_pFielder->IsInvincibleChars())
            result = true;
        break;
    }
    case AVOID_POWERUPS:
        result = false;
        if (bCanAvoid && !m_pFielder->UnidentifiedInvinciblePowerups())
            result = true;
        break;
    }
    return result;
}

inline void UnidentifiedAvoidanceValue::UnidentifiedInitialize(
    AvoidableObject* pObject, AvoidableObject* pOther)
{
    mUnidentified004 = pObject;
    mUnidentified008 = pOther;
    mUnidentified01C.UnidentifiedClear();
    mUnidentified024.UnidentifiedClear();
    mUnidentified00C = v3Zero;
    mUnidentified018 = 0.0f;
    mUnidentified02C.UnidentifiedReset();
}

inline void AvoidController::UnidentifiedSetLast(
    eAvoidableThings things, const nlVector3& v3Repulsion, float fWeight)
{
    int index = GetAvoidableIndex(things);
    m_LastRepulVec[index] = v3Repulsion;
    mUnidentified094[index] = fWeight;
}

inline void UnidentifiedAvoidanceContext::UnidentifiedNormalize()
{
    mUnidentified00C = nlVec3Length(mUnidentified000);
    float lengthSquared = nlVec3LengthSquared(mUnidentified000);
    if (lengthSquared != 0.0f)
        nlVec3Scale(mUnidentified000, nlRecipSqrt(lengthSquared, true));
}

static inline f32 ClampRunningWBSpeed(f32 speed, f32 maxSpeed)
{
    if (speed <= maxSpeed)
        return speed;
    else
        return maxSpeed;
}

AvoidController::AvoidController(cFielder* fielder)
    : mUnidentified174(16, 16)
{
    sUnidentifiedRepulsionValue0 = sUnidentifiedInitialValue0;
    sUnidentifiedRepulsionValue1 = sUnidentifiedInitialValue1;
    fn_8000F178(this);
    m_pFielder = fielder;
}

AvoidController::~AvoidController()
{
    mUnidentified174.Clear();
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
        controller->mUnidentified094[i] = 0.0f;
        controller->m_LastRepulVec[i] = v3Zero;
        controller->mUnidentified0B4[i].mRepulsion = v3Zero;
        controller->mUnidentified0B4[i].mTimer.UnidentifiedClear();
    }

    controller->mUnidentified174.Clear();
    controller->mUnidentified198 = 0;
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
    return controller->mUnidentified094[GetAvoidableIndex(things)];
}

extern "C" void fn_8000F594(AvoidableObject* pObject)
{
    if (pObject->mType == AVOID_FIELDERS || pObject->mType == AVOID_GOALIES)
    {
        return;
    }

    nlList<UnidentifiedAvoidanceValue> list(0, 0);
    UnidentifiedAvoidanceCallback_8000F7FC callback(pObject, list);

    for (int i = 0; i < 10; ++i)
    {
        cCharacter* pCharacter = g_pCharacters[i];
        if (pCharacter != 0 && pCharacter->m_eClassType == FIELDER)
        {
            list.m_pEnd = 0;
            list.m_pStart = 0;
            AvoidController* controller = fn_8002E144((cFielder*)pCharacter);
            UnidentifiedAvoidanceTree& tree = controller->mUnidentified174;
            tree.Walk(
                &callback, &UnidentifiedAvoidanceCallback_8000F7FC::UnidentifiedCallback);

            UnidentifiedAvoidanceValue* value = list.m_pStart;
            while (value != 0)
            {
                --controller->mUnidentified198;
                AvoidableObject* object = value->mUnidentified008;
                value = value->next;
                tree.Remove(object->mUnidentified008);
            }
        }
    }
}

void UnidentifiedAvoidanceCallback_8000F7FC::UnidentifiedCallback(
    const u32&, UnidentifiedAvoidanceValue* value)
{
    if (value->mUnidentified004 == mUnidentified000 || value->mUnidentified008 == mUnidentified000)
    {
        nlListAddEnd(&mUnidentified004.m_pStart, &mUnidentified004.m_pEnd, value);
    }
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
    bCanAvoid = UnidentifiedCanAvoid(AVOID_SIDELINES);
    if (bCanAvoid)
    {
        bool bHasGlobalPad = m_pFielder->GetGlobalPad() != 0;
        if (bHasGlobalPad)
            AvoidSidelines(v3Repulsion);
    }

    nlList<UnidentifiedAvoidanceValue> list(0, 0);
    UnidentifiedAvoidanceCallback_800102A8 callback(fDeltaT,
        vAccumulated_v3, fTotalWeight_v3, v3Vectors, fWeights, nCounts, list);
    mUnidentified174.Walk(&callback,
        &UnidentifiedAvoidanceCallback_800102A8::UnidentifiedCallback);

    AvoidableObject* pSelf = m_pFielder->mUnidentified320;
    UnidentifiedAvoidanceValue* value;
    for (AvoidableObject* pObject = gAvoidableObjects.m_pStart;
         pObject != 0 && mUnidentified198 < 99; pObject = pObject->next)
    {
        bool bCanAvoid = !mUnidentified174.FindGet((u32)pObject->mUnidentified008, &value);
        if (bCanAvoid)
            bCanAvoid = UnidentifiedCanAvoid(pObject->mType);
        if (bCanAvoid)
            bCanAvoid = pSelf != pObject;
        if (bCanAvoid)
            bCanAvoid = pSelf->UnidentifiedVirtual24(pObject) > 0.0f;
        if (bCanAvoid)
            bCanAvoid = pSelf->UnidentifiedVirtual20(pObject, -1.0f);
        if (bCanAvoid)
        {
            if (!UnidentifiedCanAvoid(AVOID_SIDELINES)
                && pObject->mType == AVOID_UNIDENTIFIED_08
                && ((AvoidablePolygon*)pObject)->mUnidentified014 == 1)
                continue;
            ++mUnidentified198;
            value = mUnidentified174.UnidentifiedAddOrGet((u32)pObject->mUnidentified008);
            value->UnidentifiedInitialize(pSelf, pObject);
            value->Update(fDeltaT);
            float fWeight = value->UnidentifiedGetWeight();
            if (fWeight)
            {
                const nlVector3& v3Repulsion = value->mUnidentified00C;
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

    UnidentifiedAvoidanceValue* entry = list.m_pStart;
    while (entry != 0)
    {
        --mUnidentified198;
        AvoidableObject* pObject = entry->mUnidentified008;
        entry = entry->next;
        mUnidentified174.Remove((u32)pObject->mUnidentified008);
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
            nlVec3Scale(v3Repulsion, v3Vectors[i], 1.0f / fWeights[i]);
            float fWeight = (fWeights[i] / nCounts[i]) / sUnidentifiedTweaks[i][0];
            nCount += nCounts[i];
            eAvoidableThings things = (eAvoidableThings)GetAvoidableMask(i);
            m_CurrentlyAvoiding |= things;
            UnidentifiedSetLast(things, v3Repulsion, fWeight);
            mUnidentified0B4[i].mRepulsion = v3Repulsion;
            mUnidentified0B4[i].mTimer.SetSeconds(sUnidentifiedValue_806DB4F8);
        }
        else
        {
            eAvoidableThings things = (eAvoidableThings)GetAvoidableMask(i);
            m_CurrentlyAvoiding &= ~things;
            UnidentifiedSetLast(things, v3Zero, 0.0f);
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
            nlVec3WeightedSum(v3SmoothedRepulsion,
                1.0f - fLastRepulsionWeight,
                m_LastRepulVec[GetAvoidableIndex(AVOID_EVERYTHING)],
                fLastRepulsionWeight, v3FinalRepulsion);
        }
        ApplyRepulsionVector(v3SmoothedRepulsion);
    }
    UnidentifiedSetLast(AVOID_EVERYTHING, v3FinalRepulsion, fWeight);
    m_fRepulsionMult = 1.0f;
}

void UnidentifiedAvoidanceCallback_800102A8::UnidentifiedCallback(
    const u32&, UnidentifiedAvoidanceValue* value)
{
    AvoidableObject* pObject = value->mUnidentified008;
    AvoidController* controller = fn_8002E144(
        ((AvoidableFielder*)value->mUnidentified004)->m_pFielder);
    float fWeight = 0.0f;
    if (controller->UnidentifiedCanAvoid(pObject->mType))
    {
        value->Update(mUnidentified000);
        fWeight = value->UnidentifiedGetWeight();
    }
    if (fWeight)
    {
        nlVec3ScaleAdd(mUnidentified004, fWeight,
            value->mUnidentified00C, mUnidentified004);
        mUnidentified008 += fWeight;
        int index = GetAvoidableIndex((eAvoidableThings)pObject->mType);
        nlVec3ScaleAdd(mUnidentified00C[index], fWeight,
            value->mUnidentified00C, mUnidentified00C[index]);
        mUnidentified010[index] += fWeight;
        ++mUnidentified014[index];
    }
    else
    {
        nlListAddEnd(&mUnidentified018.m_pStart, &mUnidentified018.m_pEnd, value);
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
        ? m_pFielder->m_pBall->m_v3Position : m_pFielder->mUnidentified024.m_v3Position;
    pPos.z = 0.0f;
    nlVector3 v3SidelinePos = { 0.0f, 0.0f, 0.0f };
    v3SidelinePos.x = vSidelinePos.x;
    v3SidelinePos.y = vSidelinePos.y;
    nlVector3 v3SidelineNormal = { 0.0f, 0.0f, 0.0f };
    v3SidelineNormal.x = vSidelineNormal.x;
    v3SidelineNormal.y = vSidelineNormal.y;
    nlVector2 vUnidentified018;
    vUnidentified018.x = v3SidelinePos.x - pPos.x;
    vUnidentified018.y = v3SidelinePos.y - pPos.y;
    float fDistanceSquared = nlVec2LengthSquared(vUnidentified018);
    float fDistance = nlSqrt(fDistanceSquared, true);

    fDistance -= m_pFielder->mUnidentified320->GetRadius();
    float fMaxDistance = sUnidentifiedRepulsionValue0;
    if (m_CurrentlyAvoiding & AVOID_SIDELINES)
    {
        fMaxDistance = sUnidentifiedRepulsionValue1;
    }

    m_SidelineUnavoidable = false;
    m_VeryCloseToSideline = false;
    float fMinDistance = 2.0f * sUnidentifiedRepulsionValue1;

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
            if (fDotNormalVel > sUnidentifiedValue_806DB4F4)
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
                nlVector3 vUnidentified034;
                nlVector3 vUnidentified028;
                nlVec3Set(vUnidentified034,
                    vSidelinePos.x, vSidelinePos.y, 0.0f);
                nlVec3Set(vUnidentified028,
                    vSidelinePos.x + vSidelineNormal.x,
                    vSidelinePos.y + vSidelineNormal.y, 0.0f);
                nlColourSet(colour, 255, 0, 0, 255);
                fn_802BCE50(&g_ShapeRenderer, vUnidentified034,
                    0.2f, 1.0f, 1.0f, colour, true);
                nlColourSet(colour, 0, 0, 255, 255);
                g_ShapeRenderer.DrawLine3D(vUnidentified034, vUnidentified028, colour, true);
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
    nlVector2 vPosition = *(nlVector2*)&m_pFielder->mUnidentified024.m_v3Position;
    nlVector2 vBallPosition;

    if (m_pFielder->m_pBall != NULL)
    {
        vBallPosition = *(nlVector2*)&m_pFielder->m_pBall->m_v3Position;
    }
    else
    {
        vBallPosition = vPosition;
    }

    nlVector2 vUnidentified018;
    nlVector2 vUnidentified010;
    nlVector2 vUnidentified008;
    nlVec2Sub(vUnidentified008, corner.vCenter, vBallPosition);
    if (nlVec2Length(vUnidentified008) <= corner.fRadius)
    {
        nlVec2Sub(vUnidentified018, vBallPosition, corner.vCenter);

        f32 fAngle = 10430.378f * nlATan2f(vUnidentified018.y, vUnidentified018.x);
        u32 aCornerToPos = (u16)(s32)fAngle;

        u16 absEnd = (u16)abs_s16((s16)(aCornerToPos - corner.thetaEnd));

        u16 absStart = (u16)abs_s16((s16)(aCornerToPos - corner.thetaStart));

        if (absStart >= absEnd)
            absEnd = absStart;
        if ((s16)absEnd <= 0x4000)
        {
            nlVec2Sub(vUnidentified010, vPosition, corner.vCenter);

            f32 fAngle2 = 10430.378f * nlATan2f(vUnidentified010.y, vUnidentified010.x);
            u32 aCornerToFielder = (u16)(s32)fAngle2;

            u16 absEnd2 = (u16)abs_s16((s16)(aCornerToFielder - corner.thetaEnd));

            u16 absStart2 = (u16)abs_s16((s16)(aCornerToFielder - corner.thetaStart));

            if (absStart2 >= absEnd2)
                absEnd2 = absStart2;
            if ((s16)absEnd2 <= 0x4000)
            {
                float fInvDistance = nlRecipSqrt(nlVec2DotProduct(vUnidentified010, vUnidentified010), true);
                nlVec2Set(vUnidentified010, fInvDistance * vUnidentified010.x, fInvDistance * vUnidentified010.y);
                nlVec2Sub(vSidelineNormal, v2Zero, vUnidentified010);
                float fRadius = corner.fRadius;
                nlVec2Set(vSidelinePos, fRadius * vUnidentified010.x + corner.vCenter.x, fRadius * vUnidentified010.y + corner.vCenter.y);
                bHitSideline = CalcDesiredVelocityToAvoidSideline(vNewDesiredVelDir, vCurrentDesiredVelDir, vCurrentVelDir, vSidelinePos, vSidelineNormal);
            }

        }
    }
    return bHitSideline;
}

bool AvoidController::AvoidSidelines(nlVector3& v3OutRepulsion)
{
    bool bHitSideline;
    bool bTurboAllowed;
    nlVector2 vCurrentVelDir;
    nlVector2 vCurrentDesiredVelDir;
    nlVector2 vNewDesiredVelDir;
    sCornerSegment corner;

    mUnidentified028 = v3Zero;
    if (m_pFielder->GetDistanceToDesiredPos() <= 0.25f)
        return false;
    bTurboAllowed = true;
    nlSinCos(&vCurrentVelDir.y, &vCurrentVelDir.x, m_pFielder->mUnidentified024.m_aActualMovementDirection);
    vCurrentDesiredVelDir = *(const nlVector2*)&fn_80040318(m_pFielder);
    float fLengthSquared = vCurrentDesiredVelDir.x * vCurrentDesiredVelDir.x + vCurrentDesiredVelDir.y * vCurrentDesiredVelDir.y;
    if (fLengthSquared > 0.0f)
    {
        float fInvLength = nlRecipSqrt(fLengthSquared, true);
        nlVec2Set(vCurrentDesiredVelDir, fInvLength * vCurrentDesiredVelDir.x, fInvLength * vCurrentDesiredVelDir.y);
    }
    else
        nlPolarToCartesian(vCurrentDesiredVelDir.x, vCurrentDesiredVelDir.y, m_pFielder->mUnidentified024.m_aDesiredMovementDirection, 1.0f);
    vNewDesiredVelDir = vCurrentDesiredVelDir;
    {
        u8* pBase = (u8*)cField::mCorners;
        int i = 0;
        for (; i < 4; i++)
        {
            u32* pSrc = (u32*)(pBase + i * sizeof(sCornerSegment));
            ((u32*)&corner)[0] = pSrc[0];
            ((u32*)&corner)[1] = pSrc[1];
            ((u32*)&corner)[2] = pSrc[2];
            ((u32*)&corner)[3] = pSrc[3];
            bHitSideline = CalcDesiredVelocityToAvoidCorner(vNewDesiredVelDir, corner, vCurrentDesiredVelDir, vCurrentVelDir);
            if (bHitSideline)
                break;
        }
    }
    if (!bHitSideline)
    {
        u8* pBase = (u8*)cField::mSidelines;
        int i = 0;
        for (; i < 4; i++)
        {
            sSideLinePlane* pSide = (sSideLinePlane*)(pBase + i * sizeof(sSideLinePlane));
            nlVector2 vSidelineNormal;
            nlVector2 vSidelinePos = *(nlVector2*)&m_pFielder->mUnidentified024.m_v3Position;
            float normY = v2Zero.y - pSide->vNormal.y;
            vSidelineNormal.x = v2Zero.x - pSide->vNormal.x;
            vSidelineNormal.y = normY;
            if (vSidelineNormal.x == 0.0f)
                vSidelinePos.y = pSide->fDistance * pSide->vNormal.y;
            else
                vSidelinePos.x = pSide->fDistance * pSide->vNormal.x;
            bHitSideline = CalcDesiredVelocityToAvoidSideline(vNewDesiredVelDir, vCurrentDesiredVelDir, vCurrentVelDir, vSidelinePos, vSidelineNormal);
            if (bHitSideline)
                break;
        }
    }
    if (bHitSideline)
    {
        bool isZero = nlNear(v2Zero.x, vNewDesiredVelDir.x) && nlNear(v2Zero.y, vNewDesiredVelDir.y);
        if (isZero)
            bTurboAllowed = false;
        else
        {
            float fDot = nlVec2DotProduct(vNewDesiredVelDir, vCurrentVelDir);
            if (fDot < 0.99f)
                bTurboAllowed = false;
            u16 aDesiredMovementDir = nlVector3ToAngle(
                *(const nlVector3*)&vNewDesiredVelDir);
            m_pFielder->fn_8001DCE0(aDesiredMovementDir);
            m_pFielder->Unknown8(aDesiredMovementDir, false);
        }
    }
    if (!bTurboAllowed && m_pFielder->IsRunning() && m_pFielder->m_pBall != NULL)
    {
        f32 fDesiredSpeed = ClampRunningWBSpeed(m_pFielder->mUnidentified024.m_fDesiredSpeed, m_pFielder->GetTweaks()->GetRunningSpeed());
        u16 aDesiredMovementDir = m_pFielder->mUnidentified024.m_aDesiredMovementDirection;
        m_pFielder->mUnidentified024.m_fDesiredSpeed = fDesiredSpeed;
        m_pFielder->fn_8001DCE0(aDesiredMovementDir);
        m_pFielder->Unknown8(aDesiredMovementDir, false);
    }
    v3OutRepulsion = mUnidentified028;
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
                nlVector3 vUnidentified030;
                nlVector3 vUnidentified024;
                nlVector3 vUnidentified018;
                nlVec3Set(vUnidentified018,
                    m_SidelineDirection.x, m_SidelineDirection.y, 0.0f);
                nlVec3Project(vUnidentified030,
                    rRepulsionDir, vUnidentified018);
                nlVec3Sub(vUnidentified024, rRepulsionDir, vUnidentified030);
                nlVec3ScaleAdd(rRepulsionDir, -1.0f, vUnidentified024, vUnidentified030);
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
        nlVector3 vUnidentified00C;
        nlVec3ScaleAdd(vUnidentified00C, 0.5f, v3Repulsion, m_pFielder->mUnidentified024.m_v3Position);
        g_ShapeRenderer.DrawLine3D(m_pFielder->mUnidentified024.m_v3Position, vUnidentified00C, colour, true);
    }

    nlVec3Add(v3Repulsion, v3Repulsion, fn_80040318(m_pFielder));
    float fDesiredSpeed = fn_8002E1B0(m_pFielder);
    float fResultantMag = nlVec3Length(v3Repulsion);
    fDesiredSpeed = fResultantMag <= fDesiredSpeed ? fResultantMag : fDesiredSpeed;
    float fUnidentifiedSpeed = m_pFielder->GetSpeedPowerupAdjusted(fn_8002CE14(m_pFielder->GetTweaks()));
    if (fDesiredSpeed >= 0.35f * fUnidentifiedSpeed)
    {
        fDesiredSpeed = fDesiredSpeed >= fUnidentifiedSpeed ? fDesiredSpeed : fUnidentifiedSpeed;
        m_pFielder->mUnidentified024.m_fDesiredSpeed = fDesiredSpeed;
        m_pFielder->fn_8001DCE0(nlVector3ToAngle(v3Repulsion));
        m_pFielder->Unknown8(nlVector3ToAngle(v3Repulsion), false);
    }
    else
        m_pFielder->mUnidentified024.m_fDesiredSpeed = 0.0f;
}

void UnidentifiedAvoidanceValue::Update(float fDeltaT)
{
    float fUnidentifiedPrevious = mUnidentified018 > 0.0f;
    nlVector3 v3Repulsion = v3Zero;
    float fWeight = mUnidentified004->UnidentifiedVirtual24(mUnidentified008);
    if (fWeight == 0.0f)
        mUnidentified018 = 0.0f;
    else
    {
        UnidentifiedAvoidanceContext context;
        bool bUnidentifiedResult = false;
        UnidentifiedPrepareContext(context, fDeltaT);
        if (context.mUnidentified018 <= mUnidentified008->mTweaks[2])
        {
            switch (mUnidentified008->mType)
            {
            case AVOID_FIELDERS:
            case AVOID_GOALIES:
                bUnidentifiedResult = UnidentifiedMovingResponse(context, fDeltaT);
                break;
            case AVOID_POWERUPS:
            {
                AvoidablePowerup* pPowerup = (AvoidablePowerup*)mUnidentified008;
                if (pPowerup->m_pChainChomp != 0
                    || pPowerup->m_pPowerup->m_eType == POWER_UP_RED_SHELL)
                {
                    bUnidentifiedResult = UnidentifiedMovingResponse(context, fDeltaT);
                    break;
                }
            }
            case AVOID_UNIDENTIFIED_08:
            case AVOID_BOWSER:
            case AVOID_UNIDENTIFIED_20:
                bUnidentifiedResult = mUnidentified008->UnidentifiedVirtual2C()
                    ? UnidentifiedResponse_800121D0(context, fDeltaT)
                    : UnidentifiedResponse_800123D8(context, fDeltaT);
                break;
            }
            context.mUnidentified00C *= mUnidentified004->UnidentifiedVirtual28(mUnidentified008);
        }
        if (bUnidentifiedResult && context.mUnidentified00C >= 0.1f)
        {
            mUnidentified024.UnidentifiedClear();
            mUnidentified018 = fWeight * context.mUnidentified010;
            context.mUnidentified00C *= context.mUnidentified010;
            nlVec3Scale(v3Repulsion, context.mUnidentified000, context.mUnidentified00C);
            mUnidentified02C.Update(mUnidentified00C, v3Repulsion, fDeltaT);
        }
        else
            fWeight = 0.0f;
    }
    if (mUnidentified018 && !fWeight)
    {
        if (mUnidentified024.m_uPackedTime == 0)
            mUnidentified024.SetSeconds(0.3f);
        if (mUnidentified024.Countdown(fDeltaT, 0.0f))
        {
            mUnidentified02C.UnidentifiedReset();
            mUnidentified018 = 0.0f;
        }
        else
        {
            nlVector3 value = mUnidentified02C.UnidentifiedLast();
            nlVec3Scale(value, mUnidentified024.GetSeconds() / 0.3f);
            mUnidentified02C.Update(mUnidentified00C, value, fDeltaT);
        }
    }
    if (!fUnidentifiedPrevious && mUnidentified018)
        mUnidentified01C.UnidentifiedClear();
    mUnidentified01C.Countup(fDeltaT, 10.0f);
}

void UnidentifiedAvoidanceValue::UnidentifiedPrepareContext(
    UnidentifiedAvoidanceContext& context, float fDeltaT)
{
    bool bUnidentifiedOther = mUnidentified008->UnidentifiedVirtual1C(
        mUnidentified004->GetPosition(), context.mUnidentified02C, context.mUnidentified044);
    context.mUnidentified014 = mUnidentified004->UnidentifiedVirtual1C(
        context.mUnidentified02C, context.mUnidentified020, context.mUnidentified038) || bUnidentifiedOther;
    context.mUnidentified018 = nlSqrt(nlVec3DistanceSquared2D(context.mUnidentified020, context.mUnidentified02C), true);
    if (context.mUnidentified014)
        context.mUnidentified018 *= -1.0f;

    cFielder* pFielder = ((AvoidableFielder*)mUnidentified004)->m_pFielder;
    if (mUnidentified008->mType == AVOID_FIELDERS
        && !((AvoidableFielder*)mUnidentified008)->m_pFielder->IsOnSameTeam(pFielder))
        context.mUnidentified018 -= mUnidentified008->UnidentifiedVirtual18();
    context.mUnidentified010 = NormalizeVal(context.mUnidentified018,
        mUnidentified008->mTweaks[2], mUnidentified008->mTweaks[1]);
    context.mUnidentified01C = GetClosingSpeed2D(
        context.mUnidentified020, fn_80040318(pFielder),
        context.mUnidentified02C, mUnidentified008->GetVelocity());
    float fClosingSpeed = context.mUnidentified01C;
    context.mUnidentified01C = nlMaxEquals(0.0f, fClosingSpeed);
    context.mUnidentified050 = nlVec2Length(*(const nlVector2*)&fn_80040318(pFielder));
    if (context.mUnidentified050 > 0.1f)
        nlVec3Scale(context.mUnidentified054, fn_80040318(pFielder), 1.0f / context.mUnidentified050);
    else
        context.mUnidentified054 = v3Zero;

    if (mUnidentified008->mType == AVOID_UNIDENTIFIED_08)
    {
        AvoidablePolygon* pPolygon = (AvoidablePolygon*)mUnidentified008;
        if (pPolygon->mUnidentified014 == 2 && pPolygon->mUnidentified064 != 0
            && !pPolygon->mUnidentified064->IsOnSameTeam(((AvoidableFielder*)mUnidentified004)->m_pFielder))
            nlVec3Scale(context.mUnidentified044, -1.0f);
    }
}

bool UnidentifiedAvoidanceValue::UnidentifiedMovingResponse(
    UnidentifiedAvoidanceContext& context, float fDeltaT)
{
    cFielder* pFielder = ((AvoidableFielder*)mUnidentified004)->m_pFielder;
    float fDistanceSquared = nlVec3DistanceSquared2D(
        fn_80040234(pFielder), mUnidentified008->GetPosition())
        - (mUnidentified008->mTweaks[1] * mUnidentified008->mTweaks[1]);
    float fRadius = mUnidentified008->GetRadius();
    bool bUnidentifiedCollision = fDistanceSquared < fRadius * fRadius;
    if (context.mUnidentified014)
        return UnidentifiedResponse_800127E0(!bUnidentifiedCollision, context, fDeltaT);
    if (bUnidentifiedCollision)
    {
        const nlVector3& v3Normal = context.mUnidentified044;
        float fDot = -context.UnidentifiedGetAlignment();
        context.mUnidentified014 = 2;
        nlVec3Scale(context.mUnidentified000, v3Normal, context.mUnidentified01C);
        if (!(fDot >= 0.7f))
        {
            nlVector3 v3Repulsion;
            UnidentifiedTurn(v3Repulsion, context.mUnidentified054, v3Normal, false);
            nlVec3ScaleAdd(context.mUnidentified000, context.mUnidentified050, v3Repulsion, context.mUnidentified000);
        }
        context.UnidentifiedNormalize();
        return true;
    }

    float fWeight = InterpolateRangeClamped(1.0f, 0.0f, 0.7f, 1.0f,
        -nlVec3DotProduct(context.mUnidentified044, context.mUnidentified054));
    context.mUnidentified000 = context.mUnidentified044;
    if (fWeight < 1.0f)
    {
        UnidentifiedTurn(context.mUnidentified000, context.mUnidentified054, context.mUnidentified044, true);
        nlVec3WeightedSum(context.mUnidentified000, 1.0f - fWeight, context.mUnidentified000, fWeight, context.mUnidentified044);
        nlVec3Scale(context.mUnidentified000,
            nlRecipSqrt(nlVec3LengthSquared(context.mUnidentified000), true));
    }
    context.mUnidentified00C = fn_8002E1B0(((AvoidableFielder*)mUnidentified004)->m_pFielder);
    nlVec3Scale(context.mUnidentified000, context.mUnidentified00C);
    nlVec3ScaleAdd(context.mUnidentified000, 0.9f * context.mUnidentified01C, context.mUnidentified044, context.mUnidentified000);
    context.UnidentifiedNormalize();
    return true;
}

bool UnidentifiedAvoidanceValue::UnidentifiedResponse_800121D0(
    UnidentifiedAvoidanceContext& context, float fDeltaT)
{
    cFielder* pFielder = ((AvoidableFielder*)mUnidentified004)->m_pFielder;
    float fDistanceSquared = nlVec3DistanceSquared2D(
        fn_80040234(pFielder), mUnidentified008->GetPosition())
        - (mUnidentified008->mTweaks[1] * mUnidentified008->mTweaks[1]);
    float fRadius = mUnidentified008->GetRadius();
    bool bUnidentifiedCollision = fDistanceSquared < fRadius * fRadius;
    if (context.mUnidentified014)
        return UnidentifiedResponse_800127E0(bUnidentifiedCollision ? 0 : 2, context, fDeltaT);

    nlVector3 v3Velocity = mUnidentified008->GetVelocity();
    v3Velocity.z = 0.0f;
    float fLengthSquared = nlVec3LengthSquared(v3Velocity);
    if (fLengthSquared > 0.1f)
    {
        nlVec3Scale(v3Velocity, 1.0f / nlSqrt(fLengthSquared, true));
        UnidentifiedTurn(context.mUnidentified000, context.mUnidentified044, v3Velocity, false);
    }
    else
        context.mUnidentified000 = context.mUnidentified044;
    context.mUnidentified00C = nlMaxEquals(context.mUnidentified01C,
        fn_8002E1B0(((AvoidableFielder*)mUnidentified004)->m_pFielder));
    return true;
}

bool UnidentifiedAvoidanceValue::UnidentifiedResponse_800123D8(
    UnidentifiedAvoidanceContext& context, float fDeltaT)
{
    cFielder* pFielder = ((AvoidableFielder*)mUnidentified004)->m_pFielder;
    float fDistanceSquared = nlVec3DistanceSquared2D(
        fn_80040234(pFielder), mUnidentified008->GetPosition())
        - (mUnidentified008->mTweaks[1] * mUnidentified008->mTweaks[1]);
    float fRadius = mUnidentified008->GetRadius();
    bool bUnidentifiedCollision = fDistanceSquared < fRadius * fRadius;
    if (context.mUnidentified014)
        return UnidentifiedResponse_800127E0(1, context, fDeltaT);
    if (bUnidentifiedCollision)
    {
        const nlVector3& v3Normal = context.mUnidentified044;
        float fDot = -context.UnidentifiedGetAlignment();
        context.mUnidentified014 = 2;
        nlVec3Scale(context.mUnidentified000, v3Normal, context.mUnidentified01C);
        if (fDot >= 0.0f)
        {
            nlVector3 v3Repulsion;
            UnidentifiedTurn(v3Repulsion, context.mUnidentified054, v3Normal, false);
            nlVec3ScaleAdd(context.mUnidentified000, context.mUnidentified050, v3Repulsion, context.mUnidentified000);
        }
        context.UnidentifiedNormalize();
        return true;
    }

    float fWeight = InterpolateRangeClamped(1.0f, 0.0f, 0.7f, 1.0f,
        -nlVec3DotProduct(context.mUnidentified044, context.mUnidentified054));
    context.mUnidentified000 = context.mUnidentified044;
    if (fWeight < 1.0f)
    {
        UnidentifiedTurn(context.mUnidentified000, context.mUnidentified054, context.mUnidentified044, false);
        nlVec3WeightedSum(context.mUnidentified000, 1.0f - fWeight, context.mUnidentified000, fWeight, context.mUnidentified044);
        nlVec3Scale(context.mUnidentified000,
            nlRecipSqrt(nlVec3LengthSquared(context.mUnidentified000), true));
    }
    context.mUnidentified00C = fn_8002E1B0(((AvoidableFielder*)mUnidentified004)->m_pFielder);
    nlVec3Scale(context.mUnidentified000, context.mUnidentified00C);
    nlVec3ScaleAdd(context.mUnidentified000, 0.9f * context.mUnidentified01C, context.mUnidentified044, context.mUnidentified000);
    context.UnidentifiedNormalize();
    return true;
}

bool UnidentifiedAvoidanceValue::UnidentifiedResponse_800127E0(
    int mode, UnidentifiedAvoidanceContext& context, float fDeltaT)
{
    context.mUnidentified000 = context.mUnidentified044;
    if (mode != 0 && context.mUnidentified050 > 0.5f)
    {
        if (mode == 1)
        {
            float fWeight = InterpolateRangeClamped(1.0f, 0.0f, 0.0f, 1.0f,
                -nlVec3DotProduct(context.mUnidentified044, context.mUnidentified054));
            if (fWeight < 1.0f)
            {
                UnidentifiedTurn(context.mUnidentified000, context.mUnidentified054, context.mUnidentified044, true);
                nlVec3WeightedSum(context.mUnidentified000, 1.0f - fWeight, context.mUnidentified000, fWeight, context.mUnidentified044);
                nlVec3Scale(context.mUnidentified000, nlRecipSqrt(nlVec3LengthSquared(context.mUnidentified000), true));
            }
        }
        else if (mode == 2)
        {
            nlVector3 v3Velocity = mUnidentified008->GetVelocity();
            float fLengthSquared = nlVec3LengthSquared(v3Velocity);
            if (fLengthSquared > 0.1f)
            {
                nlVec3Scale(v3Velocity, 1.0f / nlSqrt(fLengthSquared, true));
                UnidentifiedTurn(context.mUnidentified000, context.mUnidentified054, v3Velocity, false);
            }
        }
    }
    context.mUnidentified014 = 1;
    context.mUnidentified00C = fn_8002E1B0(((AvoidableFielder*)mUnidentified004)->m_pFielder);
    nlVec3Scale(context.mUnidentified000, context.mUnidentified00C);
    nlVec3ScaleAdd(context.mUnidentified000, -context.mUnidentified050, context.mUnidentified054, context.mUnidentified000);
    context.UnidentifiedNormalize();
    return true;
}

void UnidentifiedAvoidanceValue::UnidentifiedTurn(
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
