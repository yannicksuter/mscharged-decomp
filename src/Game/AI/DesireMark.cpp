#include "Game/AI/Desire.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderInput.h"
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

extern "C" void fn_800401C0(cFielder*, const nlVector3&, float, float);
extern "C" nlVector3* fn_80040234(cFielder*);
extern "C" bool fn_800381B4(cFielder*, nlVector3*);
float ReceivingPass(cFielder*);
extern "C" float fn_800DEAB4(cFielder*);
extern "C" cPlayer* fn_800DF790(cTeam*);
extern cTeam* g_pCurrentlyUpdatingTeam;
extern "C" void* fn_80311734(void*);
extern "C" void fn_800B6A1C(UnidentifiedDesireUpdate*, int, FuzzyVariant);
extern "C" UnidentifiedDesireUpdate fn_800B9020(void*, cFielder*, const char*);
extern "C" UnidentifiedDesireUpdate fn_80041B6C(void*, const unsigned int&, cFielder*);
extern "C" float fn_800DA050(cFielder*);
extern "C" bool fn_8031A04C();
extern "C" cTeam* fn_800D6670(cFielder*);

static float lbl_806DC0B8 = 0.1f;
float lbl_806DC0BC = 0.8f;
float lbl_806DC0C0 = 1.5f;
float lbl_806DC0C4 = 5.0f;
float lbl_806DC0C8 = 0.8f;
float lbl_806DC100 = 0.5f;
int lbl_806DC108 = 16;
extern int lbl_806DC110;

static nlVector2 g_vMarkingNetPassBalance = { 0.0f, 0.25f };
static nlVector2 g_vMarkDistance = { 7.0f, 4.0f };
static nlVector2 g_vMarkFormationBalance = { 0.5f, 1.0f };
static nlVector2 g_vMarkBallOwner = { 0.0f, 0.5f };
static nlVector2 g_vMarkImmediateThreatCoeff = { 1.0f, 0.5f };
static nlVector2 g_vMarkFollowTimeDelay = { 0.3f, 0.1f };

static unsigned short sDesireMarkType = 0xFFFF;
static unsigned short sDesireDefendPosType = 0xFFFF;

/**
 * Offset/Address/Size: 0x0 | 0x800B6DC0 | size: 0x48
 */
bool DesireMark::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    mThinkTimer.m_unk0 = mThinkTimer.m_uPackedTime != 0;
    mThinkTimer.m_uPackedTime = 0;
    return result;
}

/**
 * Offset/Address/Size: 0x48 | 0x800B6E08 | size: 0xD14
 */
void DesireMark::Update(UnidentifiedDesireUpdate* update, float fDeltaT)
{
    bool bBestBallInterceptor = mUnidentifiedFielder->m_pTeam->mpBestBallInterceptor == mUnidentifiedFielder;
    cFielder* pMark = mUnidentifiedFielder->GetMark();
    if (pMark == 0 || pMark->fn_800344B0()
        || mUnidentifiedFielder == g_pBall->m_pOwner
        || (bBestBallInterceptor
            && mUnidentifiedFielder->m_pTeam->mpCurrentSituation == SITUATION_LOOSE))
    {
        *update = 1;
        return;
    }
    if (mUnidentifiedFielder->IsOnSameTeam(g_pBall->m_pOwner)
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
            && Difficult(mUnidentifiedFielder->m_pTeam) > 0.7f
            && pMark->mUnidentified1E4.m_tBallPossessionTimer.GetSeconds() > lbl_806DC0C4)
        {
            *update = 3;
            fn_800B6A1C(update, 8, FuzzyVariant(FT_INT, lbl_806DC108));
            fn_800B6A1C(update, 14, FuzzyVariant((cPlayer*)pMark));
            return;
        }
    }

    mThinkTimer.Countdown(fDeltaT, 0.0f);
    if (mThinkTimer.m_uPackedTime == 0)
    {
        float fTimeDelay = Interpolate(g_vMarkFollowTimeDelay.x,
            g_vMarkFollowTimeDelay.y,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->Def_Marking->GetValue());
        float fTimeDelayRange = fTimeDelay * lbl_806DC0C8;
        mThinkTimer.SetSeconds(fTimeDelay
                               + (nlRandomf(fTimeDelayRange) - (0.5f * fTimeDelayRange)));

        nlVector3 v3MarkPosition;
        nlVector3 v3NetPosition;
        v3NetPosition = mUnidentifiedFielder->m_pTeam->m_pNet->m_v3NetLocation;
        nlVec3ScaleAdd(v3MarkPosition, lbl_806DC0B8, pMark->mUnidentified024.m_v3Velocity, pMark->mUnidentified024.m_v3Position);
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
        fMarkingDistance *= Interpolate(lbl_806DC100, 1.0f, FarToTheirNet(pMark));
        if ((bool)ReceivingPass(pMark) || (bool)fn_800DEAB4(pMark))
        {
            fMarkingDistance *= fMarkThreatCoeff;
        }
        if (pMark->m_pBall == 0)
        {
            cPlayer* pSBC = fn_800DF790(mUnidentifiedFielder->m_pTeam->GetOtherTeam());
            if (pSBC != 0 && pSBC != pMark)
            {
                nlVector3 v3SBCDir;
                nlVector3 v3SBCPosition;
                nlVec3ScaleAdd(v3SBCPosition, lbl_806DC0B8, pSBC->mUnidentified024.m_v3Velocity, pSBC->mUnidentified024.m_v3Position);
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
                float fMarkBallOwner = fn_800B9020(fn_80311734(this),
                    mUnidentifiedFielder,
                    "MarkBallOwner")
                                           .mData.f;
                if (fMarkBallOwner > 0.0f)
                {
                    fn_800401C0(mUnidentifiedFielder, vThreatTarget, lbl_806DC0BC, fMarkBallOwner * fMarkBallOwnerBalance);
                }
            }
        }
        nlVector3 v3MarkTarget;
        nlVec3ScaleAdd(v3MarkTarget, fMarkingDistance, v3Dir, v3MarkPosition);
        fn_800401C0(mUnidentifiedFielder, v3MarkTarget, lbl_806DC0BC, fMarkFormationBalance);
        nlVector3 v3FormationPosition;
        if (fn_800381B4(mUnidentifiedFielder, &v3FormationPosition))
        {
            v3FormationPosition = mUnidentifiedFielder->mUnidentified024.m_v3Position;
        }
        fn_800401C0(mUnidentifiedFielder, v3FormationPosition, lbl_806DC0BC, 1.0f - fMarkFormationBalance);
    }
}

/**
 * Offset/Address/Size: 0xD5C | 0x800B7B1C | size: 0xF40
 */
extern "C" UnidentifiedDesireUpdate fn_800B7B1C(UnidentifiedFielderInput* input)
{
    UnidentifiedDesireUpdate result(FT_INT, lbl_806DC110);
    cFielder* pFielder = (cFielder*)input->mData.pPlayer;
    unsigned long key = input->fn_8030F9B4((unsigned long)fn_800B7B1C, 1);
    if (pFielder->m_pBall != 0 || (bool)fn_800DA050(pFielder))
    {
        result = 1;
    }
    else if (fn_8031A04C() && !input->fn_8030FB7C(key))
    {
        input->fn_8030FA10(key, Interpolate(0.2f, 0.5f, 1.0f - Difficult(fn_800D6670(pFielder))));
        unsigned int hash = nlStringHash("TransDesireDefendPosHelper");
        result = fn_80041B6C(input->mUnidentified14, hash, pFielder);
    }
    return UnidentifiedDesireUpdate(result, -1.0f, -1.0f);
}

/**
 * Offset/Address/Size: 0x1C9C | 0x800B8A5C | size: 0x40
 */
bool DesireDefendPos::UnidentifiedInitialize(void*)
{
    mvDesiredPosition = mUnidentifiedFielder->mUnidentified024.m_v3Position;
    mThinkTimer.m_unk0 = mThinkTimer.m_uPackedTime != 0;
    mThinkTimer.m_uPackedTime = 0;
    return true;
}

/**
 * Offset/Address/Size: 0x1CDC | 0x800B8A9C | size: 0x580
 */
void DesireDefendPos::Update(
    UnidentifiedDesireUpdate*, float fDeltaT)
{
    mThinkTimer.Countdown(fDeltaT, 0.0f);
    if (mThinkTimer.m_uPackedTime != 0)
    {
        fn_800401C0(
            mUnidentifiedFielder, mvDesiredPosition, 1.0f, 1.0f);
        return;
    }

    SkillTweaks* pSkillTweaks = fn_800A636C(g_pCurrentlyUpdatingTeam);
    float fMarkingSkill = pSkillTweaks->Def_Marking->GetValue();
    float fTimeDelay = Interpolate(g_vMarkFollowTimeDelay.x,
        g_vMarkFollowTimeDelay.y,
        fMarkingSkill);
    float fTimeDelayRange = fTimeDelay * lbl_806DC0C8;
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
    float fSpeed = lbl_806DC0BC;

    float fFormationBalanceScale = InterpolateRangeClamped(
        1.5f, 1.0f, 0.0f, 0.5f, NearToFormationPosition(mUnidentifiedFielder));
    fMarkFormationBalance /= fFormationBalanceScale;

    nlVector3 v3NetPosition = mUnidentifiedFielder->m_pTeam->m_pNet->m_v3NetLocation;
    int nMarks = 0;
    int i;
    for (i = 0; i < 4; ++i)
    {
        cFielder* pMark = mUnidentifiedFielder->GetMark(i);
        if (pMark == 0)
        {
            break;
        }

        nlVector3 v3MarkPosition;
        int difficulty = GameInfoManager::Instance()->mCurrentDifficulty[(short)mUnidentifiedFielder->m_pTeam->m_nSide];
        if ((unsigned int)(difficulty - 5) <= 2
            && (pMark->m_pBall != 0
                || (bool)ReceivingPass(pMark)
                || (bool)fn_800DEAB4(pMark)))
        {
            v3MarkPosition = pMark->mUnidentified024.m_v3Position;
            fMarkingDistance *= fMarkThreatCoeff;
            fSpeed = lbl_806DC0C0;
        }
        else
        {
            nlVec3ScaleAdd(v3MarkPosition, lbl_806DC0B8, pMark->mUnidentified024.m_v3Velocity, pMark->mUnidentified024.m_v3Position);
        }
        v3MarkPosition.z = 0.0f;

        nlVector3 v3Dir;
        nlVec3Sub(v3Dir, v3NetPosition, v3MarkPosition);
        nlVec3Normalize(v3Dir, v3Dir);

        fMarkingDistance *= Interpolate(
            lbl_806DC100, 1.0f, FarToTheirNet(pMark));

        if (pMark->m_pBall == 0)
        {
            cPlayer* pSBC = fn_800DF790(mUnidentifiedFielder->m_pTeam->GetOtherTeam());
            if (pSBC != 0 && pSBC != pMark)
            {
                nlVector3 v3SBCDir;
                nlVector3 v3SBCPosition;
                nlVec3ScaleAdd(v3SBCPosition, lbl_806DC0B8, pSBC->mUnidentified024.m_v3Velocity, pSBC->mUnidentified024.m_v3Position);

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
        fn_800401C0(mUnidentifiedFielder, v3MarkTarget, fSpeed, fMarkFormationBalance);
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
        bool bInPosition = fn_800381B4(mUnidentifiedFielder, &v3FormationPosition);
        if (bInPosition)
        {
            v3FormationPosition = mUnidentifiedFielder->mUnidentified024.m_v3Position;
        }
        fn_800401C0(mUnidentifiedFielder, v3FormationPosition, lbl_806DC0BC, fFormationWeight);
    }

    mvDesiredPosition = *fn_80040234(mUnidentifiedFielder);
}

/**
 * Offset/Address/Size: 0x225C | 0x800B901C | size: 0x4
 */
void DesireDefendPos::UnidentifiedCleanup()
{
}

/**
 * Offset/Address/Size: 0x2260 | 0x800B9020 | size: 0x4
 */
extern "C" UnidentifiedDesireUpdate fn_800B9020(
    void* runtime, cFielder* fielder, const char* name)
{
    return fn_80041B0C(runtime, fielder, name);
}

/**
 * Offset/Address/Size: 0x2264 | 0x800B9024 | size: 0xC8
 */
void DesireDefendPos::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireDefendPos");
    cache->AddField(22, gDebugFieldTypes[22].size,
        0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x232C | 0x800B90EC | size: 0x9C
 */
void DesireDefendPos::UnidentifiedVirtual7(
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

/**
 * Offset/Address/Size: 0x23C8 | 0x800B9188 | size: 0xC8
 */
void DesireMark::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireMark");
    cache->AddField(22, gDebugFieldTypes[22].size,
        0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x2490 | 0x800B9250 | size: 0x9C
 */
void DesireMark::UnidentifiedVirtual7(
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
 * Offset/Address/Size: 0x252C | 0x800B92EC | size: 0x5C
 */
DesireMark::~DesireMark()
{
}

/**
 * Offset/Address/Size: 0x2588 | 0x800B9348 | size: 0x5C
 */
DesireDefendPos::~DesireDefendPos()
{
}
