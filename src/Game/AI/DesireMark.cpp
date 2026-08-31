#include "Game/AI/Desire.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Net.h"
#include "Game/Player.h"
#include "Game/Team.h"

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" SkillTweaks* fn_800A636C(cTeam*);
extern "C" void fn_800401C0(cFielder*, const nlVector3&, float, float);
extern "C" nlVector3* fn_80040234(cFielder*);
extern "C" bool fn_800381B4(cFielder*, nlVector3*);
extern "C" float fn_800DA6E8(cFielder*);
extern "C" float fn_800D9070(cFielder*);
extern "C" float fn_800DEFD4(cFielder*);
extern "C" float fn_800DEAB4(cFielder*);
extern "C" cPlayer* fn_800DF790(cTeam*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);

extern cTeam* lbl_806E0E00;

static float lbl_806DC0B8 = 0.1f;

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
 * Offset/Address/Size: 0x1C9C | 0x800B8A5C | size: 0x40
 */
bool DesireDefendPos::UnidentifiedInitialize(void*)
{
    mvDesiredPosition = mUnidentifiedFielder->m_v3Position;
    mThinkTimer.m_unk0 = mThinkTimer.m_uPackedTime != 0;
    mThinkTimer.m_uPackedTime = 0;
    return true;
}

/**
 * Offset/Address/Size: 0x1CDC | 0x800B8A9C | size: 0x580
 */
void DesireDefendPos::UnidentifiedUpdate(
    UnidentifiedDesireUpdate*, float fDeltaT)
{
    mThinkTimer.Countdown(fDeltaT, 0.0f);
    if (mThinkTimer.m_uPackedTime != 0)
    {
        fn_800401C0(
            mUnidentifiedFielder, mvDesiredPosition, 1.0f, 1.0f);
        return;
    }

    SkillTweaks* pSkillTweaks = fn_800A636C(lbl_806E0E00);
    float fMarkingSkill = pSkillTweaks->Def_Marking->fn_800A0D6C();
    float fTimeDelay = Interpolate(g_vMarkFollowTimeDelay.x,
        g_vMarkFollowTimeDelay.y, fMarkingSkill);
    float fTimeDelayRange = fTimeDelay * 0.8f;
    mThinkTimer.SetSeconds(fTimeDelay
        + (nlRandomf(fTimeDelayRange) - (0.5f * fTimeDelayRange)));

    pSkillTweaks = fn_800A636C(lbl_806E0E00);
    fMarkingSkill = pSkillTweaks->Def_Marking->fn_800A0D6C();

    float fMarkingNetPassBalance = Interpolate(
        g_vMarkingNetPassBalance.x,
        g_vMarkingNetPassBalance.y, fMarkingSkill);
    float fMarkingDistance = Interpolate(
        g_vMarkDistance.x, g_vMarkDistance.y, fMarkingSkill);
    float fMarkFormationBalance = Interpolate(
        g_vMarkFormationBalance.x,
        g_vMarkFormationBalance.y, fMarkingSkill);
    float fMarkBallOwnerBalance = Interpolate(
        g_vMarkBallOwner.x, g_vMarkBallOwner.y, fMarkingSkill);
    float fMarkThreatCoeff = Interpolate(
        g_vMarkImmediateThreatCoeff.x,
        g_vMarkImmediateThreatCoeff.y, fMarkingSkill);
    float fSpeed = 0.8f;

    float fFormationBalanceScale = InterpolateRangeClamped(
        1.5f, 1.0f, 0.0f, 0.5f,
        fn_800DA6E8(mUnidentifiedFielder));
    fMarkFormationBalance /= fFormationBalanceScale;

    nlVector3 v3NetPosition =
        mUnidentifiedFielder->m_pTeam->m_pNet->m_v3NetLocation;
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
        int difficulty = GameInfoManager::Instance()->mCurrentDifficulty[
            (short)mUnidentifiedFielder->m_pTeam->m_nSide];
        if ((unsigned int)(difficulty - 5) <= 2
            && (pMark->m_pBall != 0
                || fn_800DEFD4(pMark) != 0.0f
                || fn_800DEAB4(pMark) != 0.0f))
        {
            v3MarkPosition = pMark->m_v3Position;
            fMarkingDistance *= fMarkThreatCoeff;
            fSpeed = 1.5f;
        }
        else
        {
            nlVec3ScaleAdd(v3MarkPosition, lbl_806DC0B8,
                pMark->m_v3Velocity, pMark->m_v3Position);
        }
        v3MarkPosition.z = 0.0f;

        nlVector3 v3Dir;
        nlVec3Sub(v3Dir, v3NetPosition, v3MarkPosition);
        nlVec3Scale(
            v3Dir, nlRecipSqrt(nlVec3LengthSquared(v3Dir), true));

        fMarkingDistance *= Interpolate(
            0.5f, 1.0f, fn_800D9070(pMark));

        if (pMark->m_pBall == 0)
        {
            cPlayer* pSBC =
                fn_800DF790(mUnidentifiedFielder->m_pTeam->GetOtherTeam());
            if (pSBC != 0 && pSBC != pMark)
            {
                nlVector3 v3SBCPosition;
                nlVec3ScaleAdd(v3SBCPosition, lbl_806DC0B8,
                    pSBC->m_v3Velocity, pSBC->m_v3Position);

                nlVector3 v3SBCDir;
                nlVec3Sub(v3SBCDir, v3SBCPosition, v3MarkPosition);
                nlVec3Scale(v3SBCDir,
                    nlRecipSqrt(nlVec3LengthSquared(v3SBCDir), true));

                if (nlVec3DotProduct(v3SBCDir, v3Dir) >= 0.0f)
                {
                    float fToMarkNetPassBalance =
                        1.0f - fMarkingNetPassBalance;
                    nlVec3Set(v3Dir,
                        (fToMarkNetPassBalance * v3Dir.x)
                            + (fMarkingNetPassBalance * v3SBCDir.x),
                        (fToMarkNetPassBalance * v3Dir.y)
                            + (fMarkingNetPassBalance * v3SBCDir.y),
                        (fToMarkNetPassBalance * v3Dir.z)
                            + (fMarkingNetPassBalance * v3SBCDir.z));
                }
            }
        }

        nlVector3 v3MarkTarget;
        nlVec3Set(v3MarkTarget,
            (fMarkingDistance * v3Dir.x) + v3MarkPosition.x,
            (fMarkingDistance * v3Dir.y) + v3MarkPosition.y,
            (fMarkingDistance * v3Dir.z) + v3MarkPosition.z);
        fn_800401C0(mUnidentifiedFielder, v3MarkTarget,
            fSpeed, fMarkFormationBalance);
        ++nMarks;
    }

    float fFormationWeight;
    if (nMarks != 0)
    {
        fFormationWeight =
            (1.0f - fMarkFormationBalance) * (float)nMarks;
    }
    else
    {
        fFormationWeight = 1.0f;
    }

    if (fFormationWeight > 0.0f)
    {
        nlVector3 v3FormationPosition;
        bool bInPosition =
            fn_800381B4(mUnidentifiedFielder, &v3FormationPosition);
        if (bInPosition)
        {
            v3FormationPosition = mUnidentifiedFielder->m_v3Position;
        }
        fn_800401C0(mUnidentifiedFielder, v3FormationPosition,
            0.8f, fFormationWeight);
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
 * Offset/Address/Size: 0x2264 | 0x800B9024 | size: 0xC8
 */
void DesireDefendPos::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireDefendPos");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
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
    fn_80339450(cache, sDesireDefendPosType, data, context);
    fn_8033930C(cache, sDesireDefendPosType, data,
        sizeof(DesireDefendPos) - offset);
}

/**
 * Offset/Address/Size: 0x23C8 | 0x800B9188 | size: 0xC8
 */
void DesireMark::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireMark");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
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
    fn_80339450(cache, sDesireMarkType, data, context);
    fn_8033930C(cache, sDesireMarkType, data,
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
