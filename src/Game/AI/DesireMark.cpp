#include "Game/AI/Desire.h"
#include "Game/AI/ScriptMachine.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/Fielder.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Net.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/nlString.h"

#include "Game/UnidentifiedStaticStorage.h"

extern "C" DesireUpdate fn_800B9020(void*, cFielder*, const char*);
extern "C" DesireUpdate fn_80041B6C(void*, const unsigned int&, cFielder*);

static float sMarkLookAheadTime = 0.1f;
float gMarkUrgency = 0.8f;
float gMarkImmediateThreatUrgency = 1.5f;
float gMarkSlideAttackPossessionTime = 5.0f;
float gMarkFollowTimeDelayRangeScale = 0.8f;

static nlVector2 g_vMarkingNetPassBalance = { 0.0f, 0.25f };
static nlVector2 g_vMarkDistance = { 7.0f, 4.0f };
static nlVector2 g_vMarkFormationBalance = { 0.5f, 1.0f };
static nlVector2 g_vMarkBallOwner = { 0.0f, 0.5f };
static nlVector2 g_vMarkImmediateThreatCoeff = { 1.0f, 0.5f };
static nlVector2 g_vMarkFollowTimeDelay = { 0.3f, 0.1f };

float gMarkDistanceMinMultiplier = 0.5f;
static unsigned short sDesireMarkType = 0xFFFF;
int gMarkSlideAttackState = 16;
static unsigned short sDesireDefendPosType = 0xFFFF;
// The default transition result resides in initialized small data.
#pragma explicit_zero_data on
int gTransDesireDefendPosContinue = DESIRE_CONTINUE;
#pragma explicit_zero_data off

/**
 * Offset/Address/Size: 0x0 | 0x800B6DC0 | size: 0x48
 */
bool DesireMark::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    mThinkTimer.m_uWasRunning = mThinkTimer.m_uPackedTime != 0;
    mThinkTimer.m_uPackedTime = 0;
    return result;
}

/**
 * Offset/Address/Size: 0x48 | 0x800B6E08 | size: 0xD14
 */
void DesireMark::Update(DesireUpdate* update, float fDeltaT)
{
    bool bBestBallInterceptor = m_pFielder->m_pTeam->GetBestBallInterceptor() == m_pFielder;
    cFielder* pMark = m_pFielder->GetMark();
    if (pMark == 0 || pMark->fn_800344B0()
        || m_pFielder == g_pBall->m_pOwner
        || (bBestBallInterceptor
            && m_pFielder->m_pTeam->mpCurrentSituation == SITUATION_LOOSE))
    {
        *update = 1;
        return;
    }
    if (m_pFielder->IsOnSameTeam(g_pBall->m_pOwner)
        || bBestBallInterceptor)
    {
        if (update->mData.i == 0)
        {
            *update = 4;
        }
    }
    if (pMark->m_pBall != 0)
    {
        if (fn_800A636C(g_pCurrentlyUpdatingTeam)->Def_SlideAttackChance->GetValue() > 0.0f
            && Difficult(m_pFielder->m_pTeam) > 0.7f
            && pMark->mUnidentified1E4.m_tBallPossessionTimer.GetSeconds() > gMarkSlideAttackPossessionTime)
        {
            *update = 3;
            update->SetParameter(8, FuzzyVariant(FT_INT, gMarkSlideAttackState));
            update->SetParameter(14, FuzzyVariant((cPlayer*)pMark));
            return;
        }
    }

    mThinkTimer.Countdown(fDeltaT, 0.0f);
    if (mThinkTimer.m_uPackedTime == 0)
    {
        float fTimeDelay = Interpolate(g_vMarkFollowTimeDelay.x,
            g_vMarkFollowTimeDelay.y,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->Def_Marking->GetValue());
        float fTimeDelayRange = fTimeDelay * gMarkFollowTimeDelayRangeScale;
        mThinkTimer.SetSeconds(fTimeDelay
                               + (nlRandomf(fTimeDelayRange) - (0.5f * fTimeDelayRange)));

        nlVector3 v3MarkPosition;
        nlVector3 v3NetPosition;
        v3NetPosition = m_pFielder->m_pTeam->m_pNet->m_v3NetLocation;
        nlVec3ScaleAdd(v3MarkPosition, sMarkLookAheadTime, pMark->mUnidentified024.m_v3Velocity, pMark->mUnidentified024.m_v3Position);
        v3MarkPosition.z = 0.0f;
        nlVector3 v3Dir;
        nlVec3Sub(v3Dir, v3NetPosition, v3MarkPosition);
        nlVec3Normalize(v3Dir, v3Dir);

        float fMarkingNetPassBalance = Interpolate(g_vMarkingNetPassBalance.x,
            g_vMarkingNetPassBalance.y,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->Def_Marking->GetValue());
        float fMarkingDistance = Interpolate(g_vMarkDistance.x, g_vMarkDistance.y, fn_800A636C(g_pCurrentlyUpdatingTeam)->Def_Marking->GetValue());
        float fMarkFormationBalance = Interpolate(g_vMarkFormationBalance.x,
            g_vMarkFormationBalance.y,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->Def_Marking->GetValue());
        float fMarkBallOwnerBalance = Interpolate(g_vMarkBallOwner.x, g_vMarkBallOwner.y, fn_800A636C(g_pCurrentlyUpdatingTeam)->Def_Marking->GetValue());
        float fMarkThreatCoeff = Interpolate(g_vMarkImmediateThreatCoeff.x,
            g_vMarkImmediateThreatCoeff.y,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->Def_Marking->GetValue());
        fMarkingDistance *= Interpolate(gMarkDistanceMinMultiplier, 1.0f, FarToTheirNet(pMark));
        if ((bool)ReceivingPass(pMark) || (bool)fn_800DEAB4(pMark))
        {
            fMarkingDistance *= fMarkThreatCoeff;
        }
        if (pMark->m_pBall == 0)
        {
            cPlayer* pSBC = fn_800DF790(m_pFielder->m_pTeam->GetOtherTeam());
            if (pSBC != 0 && pSBC != pMark)
            {
                nlVector3 v3SBCDir;
                nlVector3 v3SBCPosition;
                nlVec3Set(v3SBCPosition,
                    (sMarkLookAheadTime * pSBC->mUnidentified024.m_v3Velocity.x) + pSBC->mUnidentified024.m_v3Position.x,
                    (sMarkLookAheadTime * pSBC->mUnidentified024.m_v3Velocity.y) + pSBC->mUnidentified024.m_v3Position.y,
                    (sMarkLookAheadTime * pSBC->mUnidentified024.m_v3Velocity.z) + pSBC->mUnidentified024.m_v3Position.z);
                nlVec3Sub(v3SBCDir, v3SBCPosition, v3MarkPosition);
                nlVec3Normalize(v3SBCDir, v3SBCDir);
                if (nlVec3DotProduct(v3SBCDir, v3Dir) >= 0.0f)
                {
                    float fToMarkNetPassBalance = 1.0f - fMarkingNetPassBalance;
                    nlVec3WeightedSum(v3Dir, fToMarkNetPassBalance, v3Dir, fMarkingNetPassBalance, v3SBCDir);
                }
                nlVector3 vThreatTarget;
                nlVec3Sub(vThreatTarget, v3NetPosition, v3SBCPosition);
                nlVec3Normalize(vThreatTarget, vThreatTarget);
                nlVec3Set(vThreatTarget, fMarkingDistance * vThreatTarget.x + v3SBCPosition.x, fMarkingDistance * vThreatTarget.y + v3SBCPosition.y, fMarkingDistance * vThreatTarget.z + v3SBCPosition.z);
                float fMarkBallOwner = fn_800B9020(GetFuzzyRuntime(),
                    m_pFielder,
                    "MarkBallOwner")
                                           .mData.f;
                if (fMarkBallOwner > 0.0f)
                {
                    m_pFielder->AddDesiredPosition(vThreatTarget, gMarkUrgency, fMarkBallOwner * fMarkBallOwnerBalance);
                }
            }
        }
        nlVector3 v3MarkTarget;
        nlVec3ScaleAdd(v3MarkTarget, fMarkingDistance, v3Dir, v3MarkPosition);
        m_pFielder->AddDesiredPosition(v3MarkTarget, gMarkUrgency, fMarkFormationBalance);
        nlVector3 v3FormationPosition;
        if (m_pFielder->CalculateFormationPosition(v3FormationPosition))
        {
            v3FormationPosition = m_pFielder->mUnidentified024.m_v3Position;
        }
        m_pFielder->AddDesiredPosition(v3FormationPosition, gMarkUrgency, 1.0f - fMarkFormationBalance);
    }
}

/**
 * Offset/Address/Size: 0xD5C | 0x800B7B1C | size: 0xF40
 */
DesireUpdate TransDesireDefendPos(AIContext* input)
{
    DesireUpdate result(FT_INT, gTransDesireDefendPosContinue);
    cFielder* pFielder = (cFielder*)input->mData.pPlayer;
    unsigned long key = input->GetTimerKey((unsigned long)TransDesireDefendPos, 1);
    if (pFielder->m_pBall != 0 || (bool)fn_800DA050(pFielder))
    {
        result = 1;
    }
    else if (CheckScriptTimeBudget() && !input->IsTimerRunning(key))
    {
        input->SetTimer(key, Interpolate(0.2f, 0.5f, 1.0f - Difficult(fn_800D6670(pFielder))));
        unsigned int hash = nlStringHash("TransDesireDefendPosHelper");
        result = fn_80041B6C(input->mRuntime, hash, pFielder);
    }
    return DesireUpdate(result, -1.0f, -1.0f);
}

/**
 * Offset/Address/Size: 0x1C9C | 0x800B8A5C | size: 0x40
 */
bool DesireDefendPos::Initialize(void*)
{
    mvDesiredPosition = m_pFielder->mUnidentified024.m_v3Position;
    mThinkTimer.m_uWasRunning = mThinkTimer.m_uPackedTime != 0;
    mThinkTimer.m_uPackedTime = 0;
    return true;
}

/**
 * Offset/Address/Size: 0x1CDC | 0x800B8A9C | size: 0x580
 */
void DesireDefendPos::Update(
    DesireUpdate*, float fDeltaT)
{
    mThinkTimer.Countdown(fDeltaT, 0.0f);
    if (mThinkTimer.m_uPackedTime != 0)
    {
        m_pFielder->AddDesiredPosition(mvDesiredPosition, 1.0f, 1.0f);
        return;
    }

    SkillTweaks* pSkillTweaks = fn_800A636C(g_pCurrentlyUpdatingTeam);
    float fMarkingSkill = pSkillTweaks->Def_Marking->GetValue();
    float fTimeDelay = Interpolate(g_vMarkFollowTimeDelay.x,
        g_vMarkFollowTimeDelay.y,
        fMarkingSkill);
    float fTimeDelayRange = fTimeDelay * gMarkFollowTimeDelayRangeScale;
    mThinkTimer.SetSeconds(fTimeDelay
                           + (nlRandomf(fTimeDelayRange) - (0.5f * fTimeDelayRange)));

    pSkillTweaks = fn_800A636C(g_pCurrentlyUpdatingTeam);
    fMarkingSkill = pSkillTweaks->Def_Marking->GetValue();

    float fMarkingNetPassBalance = Interpolate(
        g_vMarkingNetPassBalance.x,
        g_vMarkingNetPassBalance.y,
        fMarkingSkill);
    float fMarkingDistance = Interpolate(
        g_vMarkDistance.x, g_vMarkDistance.y, fMarkingSkill);
    float fMarkFormationBalance = Interpolate(
        g_vMarkFormationBalance.x,
        g_vMarkFormationBalance.y,
        fMarkingSkill);
    float fMarkBallOwnerBalance = Interpolate(
        g_vMarkBallOwner.x, g_vMarkBallOwner.y, fMarkingSkill);
    float fMarkThreatCoeff = Interpolate(
        g_vMarkImmediateThreatCoeff.x,
        g_vMarkImmediateThreatCoeff.y,
        fMarkingSkill);
    float fUrgency = gMarkUrgency;

    float fFormationBalanceScale = InterpolateRangeClamped(
        1.5f, 1.0f, 0.0f, 0.5f, NearToFormationPosition(m_pFielder));
    fMarkFormationBalance /= fFormationBalanceScale;

    nlVector3 v3NetPosition = m_pFielder->m_pTeam->m_pNet->m_v3NetLocation;
    int nMarks = 0;
    int i;
    for (i = 0; i < 4; ++i)
    {
        cFielder* pMark = m_pFielder->GetMark(i);
        if (pMark == 0)
        {
            break;
        }

        nlVector3 v3MarkPosition;
        int difficulty = GameInfoManager::Instance()->mCurrentDifficulty[(short)m_pFielder->m_pTeam->m_nSide];
        if ((unsigned int)(difficulty - 5) <= 2
            && (pMark->m_pBall != 0
                || (bool)ReceivingPass(pMark)
                || (bool)fn_800DEAB4(pMark)))
        {
            v3MarkPosition = pMark->mUnidentified024.m_v3Position;
            fMarkingDistance *= fMarkThreatCoeff;
            fUrgency = gMarkImmediateThreatUrgency;
        }
        else
        {
            nlVec3ScaleAdd(v3MarkPosition, sMarkLookAheadTime, pMark->mUnidentified024.m_v3Velocity, pMark->mUnidentified024.m_v3Position);
        }
        v3MarkPosition.z = 0.0f;

        nlVector3 v3Dir;
        nlVec3Sub(v3Dir, v3NetPosition, v3MarkPosition);
        nlVec3Normalize(v3Dir, v3Dir);

        fMarkingDistance *= Interpolate(
            gMarkDistanceMinMultiplier, 1.0f, FarToTheirNet(pMark));

        if (pMark->m_pBall == 0)
        {
            cPlayer* pSBC = fn_800DF790(m_pFielder->m_pTeam->GetOtherTeam());
            if (pSBC != 0 && pSBC != pMark)
            {
                nlVector3 v3SBCDir;
                nlVector3 v3SBCPosition;
                nlVec3Set(v3SBCPosition,
                    (sMarkLookAheadTime * pSBC->mUnidentified024.m_v3Velocity.x) + pSBC->mUnidentified024.m_v3Position.x,
                    (sMarkLookAheadTime * pSBC->mUnidentified024.m_v3Velocity.y) + pSBC->mUnidentified024.m_v3Position.y,
                    (sMarkLookAheadTime * pSBC->mUnidentified024.m_v3Velocity.z) + pSBC->mUnidentified024.m_v3Position.z);

                nlVec3Sub(v3SBCDir, v3SBCPosition, v3MarkPosition);
                nlVec3Normalize(v3SBCDir, v3SBCDir);

                if (nlVec3DotProduct(v3SBCDir, v3Dir) >= 0.0f)
                {
                    float fToMarkNetPassBalance = 1.0f - fMarkingNetPassBalance;
                    nlVec3WeightedSum(v3Dir, fToMarkNetPassBalance, v3Dir, fMarkingNetPassBalance, v3SBCDir);
                }
            }
        }

        nlVector3 v3MarkTarget;
        nlVec3Set(v3MarkTarget,
            (fMarkingDistance * v3Dir.x) + v3MarkPosition.x,
            (fMarkingDistance * v3Dir.y) + v3MarkPosition.y,
            (fMarkingDistance * v3Dir.z) + v3MarkPosition.z);
        m_pFielder->AddDesiredPosition(v3MarkTarget, fUrgency, fMarkFormationBalance);
        ++nMarks;
    }

    float fFormationWeight;
    if (nMarks != 0)
    {
        fFormationWeight = (1.0f - fMarkFormationBalance) * (float)nMarks;
    }
    else
    {
        fFormationWeight = 1.0f;
    }

    if (fFormationWeight > 0.0f)
    {
        nlVector3 v3FormationPosition;
        bool bInPosition = m_pFielder->CalculateFormationPosition(v3FormationPosition);
        if (bInPosition)
        {
            v3FormationPosition = m_pFielder->mUnidentified024.m_v3Position;
        }
        m_pFielder->AddDesiredPosition(v3FormationPosition, gMarkUrgency, fFormationWeight);
    }

    mvDesiredPosition = m_pFielder->GetDesiredPosition();
}

/**
 * Offset/Address/Size: 0x225C | 0x800B901C | size: 0x4
 */
void DesireDefendPos::Cleanup()
{
}

/**
 * Offset/Address/Size: 0x2260 | 0x800B9020 | size: 0x4
 */
extern "C" DesireUpdate fn_800B9020(
    void* runtime, cFielder* fielder, const char* name)
{
    return fn_80041B0C(runtime, fielder, name);
}

/**
 * Offset/Address/Size: 0x23C8 | 0x800B9188 | size: 0xC8
 */
inline void DesireMark::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireMark");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x2490 | 0x800B9250 | size: 0x9C
 */
inline void DesireMark::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireMarkType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireMarkType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireMarkType, data, context);
    cache->WriteData(sDesireMarkType, data,
        sizeof(DesireMark) - offset);
}

/**
 * Offset/Address/Size: 0x2264 | 0x800B9024 | size: 0xC8
 */
inline void DesireDefendPos::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireDefendPos");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x232C | 0x800B90EC | size: 0x9C
 */
inline void DesireDefendPos::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireDefendPosType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireDefendPosType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireDefendPosType, data, context);
    cache->WriteData(sDesireDefendPosType, data,
        sizeof(DesireDefendPos) - offset);
}
