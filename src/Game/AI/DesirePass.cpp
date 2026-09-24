#include "Game/AI/DesirePass.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Fuzzy.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/AI/SpaceSearch.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/DebugWriteCache.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/nlMemory.h"

#include "Game/UnidentifiedStaticStorage.h"

static bool sDebugPassSpaceSearch;
const nlVector3 sStationaryTargetVelocity = { 0.0f, 0.0f, 0.0f };

static float sPassSearchRadius = 4.5f;
static float sPreparePassDuration = 3.0f;
static float sPassAbortThreshold = 0.85f;
static float sPassAbortThresholdVariation = 0.3f;
static float sPassArrivalDistance = 1.0f;
static unsigned short sDesirePreparePassType = 0xFFFF;
static unsigned short sDesirePassType = 0xFFFF;
static int sPassDesireState = FIELDERDESIRE_PASS;
// The default transition result resides in initialized small data.
#pragma explicit_zero_data on
static int sContinueDesire = DESIRE_CONTINUE;
#pragma explicit_zero_data off

/**
 * Offset/Address/Size: 0x0 | 0x800BA57C | size: 0x188
 */
bool DesirePreparePass::UnidentifiedInitialize(void* context)
{
    UnidentifiedVariantCollection* params =
        (UnidentifiedVariantCollection*)context;
    mpPassTarget = params->Get(14)->mData.pPlayer;
    mbVolleyPass = params->Get(16)->mData.b;
    if (mpPassTarget == 0)
    {
        return false;
    }

    float fDuration = sPreparePassDuration;
    mUnidentified078 = fDuration + 0.2f;
    if (mbVolleyPass)
    {
        mThinkTimer.m_unk0 = mThinkTimer.m_uPackedTime != 0;
        mThinkTimer.m_uPackedTime = 0;
    }
    else
    {
        mThinkTimer.SetSeconds(fDuration);
    }

    SkillTweaks* pSkillTweaks = fn_800A636C(g_pCurrentlyUpdatingTeam);
    float fReactionTime = 1.0f - pSkillTweaks->GetReaction(0);
    float fAbortThreshold = sPassAbortThreshold;
    float fReactionTimeRange =
        fAbortThreshold * (sPassAbortThresholdVariation * fReactionTime);
    mfAbortThreshold = fAbortThreshold
        + (nlRandomf(fReactionTimeRange)
            - (0.5f * fReactionTimeRange));

    m_pSpaceSearch = new (nlMalloc(sizeof(SSearchOpenLane), 8, false))
        SSearchOpenLane(m_pFielder, mpPassTarget);
    m_pFielder->SetSpaceSearch(m_pSpaceSearch);
    m_pFielder->m_pSpaceSearch->m_bDebugOn = sDebugPassSpaceSearch;
    m_pFielder->m_pSpaceSearch->FindBestPosition(
        mvDesiredPosition, m_pFielder->mUnidentified024.m_v3Position,
        DIR_UPFIELD, &mpPassTarget->mUnidentified024.m_v3Position,
        sPassSearchRadius, 0x8000);
    return true;
}

/**
 * Offset/Address/Size: 0x188 | 0x800BA704 | size: 0x6A0
 */
void DesirePreparePass::Update(DesireUpdate* update, float fDeltaT)
{
    if (Incapacitated(mpPassTarget))
    {
        *update = DESIRE_FINISHED;
        return;
    }

    mThinkTimer.Countdown(fDeltaT, 0.0f);
    m_pFielder->AddDesiredPosition(mvDesiredPosition, 1.5f, 1.0f);

    bool bSwitchToPassDesire = false;
    if (mThinkTimer.m_uPackedTime != 0)
    {
        float fInDanger = fn_80041B0C(GetFuzzyRuntime(),
            m_pFielder, "InDangerDelayed").mData.f;
        float fNotFarToTheirGoalie =
            FLESS(FarToTheirGoalie(g_pScriptCurrentFielder), 0.3f);
        float fDistanceToDesiredPos =
            m_pFielder->GetDistanceToDesiredPos();
        float fClosingSpeedToDesiredPos = GetClosingSpeed2D(
            mvDesiredPosition, sStationaryTargetVelocity,
            m_pFielder->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Velocity);
        if (fClosingSpeedToDesiredPos < 0.0f
            || fDistanceToDesiredPos <= sPassArrivalDistance
            || ((fInDanger >= fNotFarToTheirGoalie
                    ? fInDanger : fNotFarToTheirGoalie)
                >= mfAbortThreshold))
        {
            bSwitchToPassDesire = true;
        }
    }
    else
    {
        bSwitchToPassDesire = true;
    }

    if (bSwitchToPassDesire)
    {
        *update = DESIRE_CHANGE;
        update->SetParameter(8, FuzzyVariant(FT_INT, sPassDesireState));
        update->SetParameter(14, FuzzyVariant(mpPassTarget));
        update->SetParameter(16, FuzzyVariant(FT_BOOL, mbVolleyPass));
    }
}

/**
 * Offset/Address/Size: 0x828 | 0x800BADA4 | size: 0x50
 */
void DesirePreparePass::UnidentifiedCleanup()
{
    if (m_pSpaceSearch == m_pFielder->m_pSpaceSearch)
    {
        m_pFielder->SetSpaceSearch(0);
    }
    m_pSpaceSearch = 0;
}

/**
 * Offset/Address/Size: 0x878 | 0x800BADF4 | size: 0x164
 */
bool DesirePass::UnidentifiedInitialize(void* context)
{
    bool result = true;
    UnidentifiedVariantCollection* params =
        (UnidentifiedVariantCollection*)context;
    Variant* pParam;
    if (params->IsSet(14))
    {
        pParam = params->Get(14);
    }
    else
    {
        pParam = params->Get(0);
    }
    mpPassTarget = pParam->mData.pPlayer;
    if (params->IsSet(16))
    {
        pParam = params->Get(16);
    }
    else
    {
        pParam = params->Get(1);
    }
    mbVolleyPass = pParam->mData.b;
    if (mpPassTarget == 0)
    {
        return false;
    }

    PlayerTweaks* pTweaks = m_pFielder->GetTweaks();
    float fMaxSpeed = pTweaks->GetRunningSpeed();
    m_pFielder->mUnidentified024.m_fDesiredSpeed =
        FMIN(m_pFielder->mUnidentified024.m_fDesiredSpeed, fMaxSpeed);

    if (m_pFielder->m_pBall != 0)
    {
        m_pFielder->InitActionPass(
            mpPassTarget, mbVolleyPass, 0, false);
    }
    else if (m_pFielder->CanContactLooseBall(false))
    {
        m_pFielder->InitActionLooseBallPass(
            (cFielder*)mpPassTarget, mbVolleyPass);
        result = m_pFielder->m_eActionState
                == ACTION_LOOSE_BALL_PASS
            || m_pFielder->m_eActionState
                == ACTION_LOOSE_BALL_SHOT;
    }
    else
    {
        result = false;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x9DC | 0x800BAF58 | size: 0x4
 */
void DesirePass::Update(DesireUpdate*, float)
{
}

/**
 * Offset/Address/Size: 0x9E0 | 0x800BAF5C | size: 0x4
 */
void DesirePass::UnidentifiedCleanup()
{
}

/**
 * Offset/Address/Size: 0x9E4 | 0x800BAF60 | size: 0xCC8
 */
DesireUpdate TransDesireLooseBallContact(
    UnidentifiedFuzzyRuntimeValue* fielderValue,
    UnidentifiedFuzzyRuntimeValue* action)
{
    DesireUpdate result(FT_INT, sContinueDesire);
    if (g_pBall->m_pOwner != 0)
    {
        result = DESIRE_FINISHED;
    }
    else if (action->mType == (eVariantType)13)
    {
        cFielder* pFielder =
            (cFielder*)fielderValue->mData.pPlayer;
        if (pFielder->CanContactLooseBall(false))
        {
            cFielder* pPassTarget;
            bool bVolleyPass = pFielder->IsActionModifierPressed();
            bool bActionInitialized;
            if (action->ExtraData.Get(1)->mData.b)
            {
                pFielder->InitActionLooseBallShot(bVolleyPass);
                bActionInitialized =
                    pFielder->m_eActionState == ACTION_LOOSE_BALL_SHOT;
            }
            else
            {
                pPassTarget = (cFielder*)
                    action->ExtraData.Get(0)->mData.pPlayer;
                if (pPassTarget != 0 && pPassTarget->CanReceivePass())
                {
                    pFielder->InitActionLooseBallPass(
                        pPassTarget, bVolleyPass);
                    bActionInitialized =
                        pFielder->m_eActionState
                        == ACTION_LOOSE_BALL_PASS;
                }
                else
                {
                    pFielder->InitActionLooseBallShot(
                        pFielder->IsActionModifierPressed());
                    bActionInitialized =
                        pFielder->m_eActionState
                        == ACTION_LOOSE_BALL_SHOT;
                }
            }
            if (bActionInitialized)
            {
                result = DESIRE_FINISHED;
            }
        }
    }
    else
    {
        result = DESIRE_FINISHED;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x187C | 0x800BBDF8 | size: 0x134
 */
inline void DesirePreparePass::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field =
        cache->BeginType("DesirePreparePass");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpPassTarget - (u8*)&mvDesiredPosition,
        "mpPassTarget");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbVolleyPass - (u8*)&mvDesiredPosition,
        "mbVolleyPass");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mfAbortThreshold - (u8*)&mvDesiredPosition,
        "mfAbortThreshold");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x19B0 | 0x800BBF2C | size: 0xC0
 */
inline void DesirePreparePass::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesirePreparePassType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesirePreparePassType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesirePreparePassType,
        (u8*)this + offset, sizeof(DesirePreparePass) - offset);
    if (data != 0)
    {
        DesirePreparePass* desire =
            (DesirePreparePass*)((u8*)data - offset);
        desire->mpPassTarget = (cPlayer*)(mpPassTarget == 0
                ? -1 : mpPassTarget->mUnidentified120);
        cache->ChecksumData(sDesirePreparePassType, data, context);
    }
}

/**
 * Offset/Address/Size: 0x16AC | 0x800BBC28 | size: 0x110
 */
inline void DesirePass::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesirePass");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpPassTarget - (u8*)&mvDesiredPosition,
        "mpPassTarget");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbVolleyPass - (u8*)&mvDesiredPosition,
        "mbVolleyPass");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x17BC | 0x800BBD38 | size: 0xC0
 */
inline void DesirePass::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesirePassType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesirePassType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesirePassType,
        (u8*)this + offset, sizeof(DesirePass) - offset);
    if (data != 0)
    {
        DesirePass* desire =
            (DesirePass*)((u8*)data - offset);
        desire->mpPassTarget = (cPlayer*)(mpPassTarget == 0
                ? -1 : mpPassTarget->mUnidentified120);
        cache->ChecksumData(sDesirePassType, data, context);
    }
}
