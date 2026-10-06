#include "NL/nlDLListContainer.inl"
#include "Game/AI/DesireUsePowerup.h"
#include "Game/AI/ScriptMachine.h"
#include "Game/AI/Powerups.h"
#include "Game/Physics/PhysicsEventQueue.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/DebugWriteCache.h"
#include "Game/Game.h"
#include "Game/EventDataTypes.h"
#include "Game/DB/StatsTracker.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/ChainChomp.h"
#include "Game/Team.h"
#include <stddef.h>
#include "Game/UnidentifiedStaticStorage.h"

extern "C" bool fn_8002EDC8(cFielder*, int);
extern "C" DesireUpdate fn_80041B6C(
    void*, const unsigned int&, cFielder*);
static unsigned short sDesireUsePowerupType = 0xFFFF;
#pragma explicit_zero_data on
static int sTransDesireUsePowerupContinue = 0;
#pragma explicit_zero_data off
static int sUsePowerupDesireState = 17;
static bool lbl_806DC3A4 = true;

/**
 * Offset/Address/Size: 0x0 | 0x800D2074 | size: 0xF48
 */
DesireUpdate TransDesireUsePowerup(
    AIContext* input)
{
    DesireUpdate result(FT_INT, sTransDesireUsePowerupContinue);
    cFielder* pFielder = (cFielder*)input->mData.pPlayer;
    unsigned long key = input->GetTimerKey(
        (unsigned long)TransDesireUsePowerup, 1);

    if (UserControlledT(fn_800D6670(pFielder))
        || (1.0f - fn_800D85F8(pFielder)))
    {
        result = 1;
    }
    else if (CheckScriptTimeBudget() && !input->IsTimerRunning(key)
        && !fn_800E0034())
    {
        input->SetTimer(key, 0.4f);
        unsigned int hash = nlStringHash("TransDesireUsePowerup");
        result = fn_80041B6C(input->mRuntime, hash, pFielder);
    }

    return DesireUpdate(result, -1.0f, -1.0f);
}

static int sPowerupThrowAnims[4] = { 0x59, 0x5C, 0x5B, 0x5A };

/**
 * Offset/Address/Size: 0xF48 | 0x800D2FBC | size: 0x128
 */
bool DesireUsePowerup::Initialize(void* context)
{
    bool result;
    ePowerUpType ePowerup;
    UnidentifiedVariantCollection* params;

    result = Desire::Initialize(context);
    params = (UnidentifiedVariantCollection*)context;

    mbThrowingPowerup = false;
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_uWasRunning
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    m_pFielder->m_nPowerupAnimID = -1;
    mMaxDuration = -1.0f;

    if (params->IsSet(15))
    {
        ePowerup = (ePowerUpType)params->Get(15)->mData.i;
        fn_800D3968(
            (cFielder*)params->Get(14)->mData.pPlayer,
            ePowerup,
            false);
    }

    return result;
}

/**
 * Offset/Address/Size: 0x1070 | 0x800D30E4 | size: 0x7B0
 */
void DesireUsePowerup::Update(
    DesireUpdate* update, float fDeltaT)
{
    if (update->mData.i == 3)
    {
        update->SetParameter(8, FuzzyVariant(sUsePowerupDesireState));
        update->SetParameter(9, FuzzyVariant(lbl_806DC3A4));
        return;
    }

    if (mtPowerupEffectTime.m_uPackedTime != 0
        && mtPowerupEffectTime.Countdown(fDeltaT, 0.0f))
    {
        *update = 1;
    }

    if (!g_pGame->IsGameplayOrOvertime())
    {
        *update = 1;
        return;
    }

    if (mbThrowingPowerup && mePowerup == POWER_UP_NONE)
    {
        *update = 1;
    }
}

/**
 * Offset/Address/Size: 0x1820 | 0x800D3894 | size: 0x3C
 */
void DesireUsePowerup::Cleanup()
{
    mbThrowingPowerup = false;
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_uWasRunning
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    m_pFielder->m_nPowerupAnimID = -1;
}

/**
 * Offset/Address/Size: 0x185C | 0x800D38D0 | size: 0x98
 */
extern "C" void fn_800D38D0(DesireUsePowerup* pDesire)
{
    if (!fn_8002EDC8(pDesire->m_pFielder, -1))
    {
        return;
    }

    if (!pDesire->mActive)
    {
        ActivateConcurrentState(pDesire->mScriptMachine, 17, NULL, false);
    }

    cTeam* pTeam = pDesire->m_pFielder->m_pTeam;
    pDesire->SetPowerup(pTeam->GetCurrentPowerUp().eType,
        pTeam->GetCurrentPowerUp().nnumOfPowerups, NULL);
}

/**
 * Offset/Address/Size: 0x18F4 | 0x800D3968 | size: 0xE8
 */
void DesireUsePowerup::fn_800D3968(
    cFielder* pTarget, ePowerUpType ePowerup, bool bActivate)
{
    if (!fn_8002EDC8(m_pFielder, -1))
    {
        return;
    }

    if (!mActive && bActivate)
    {
        ActivateConcurrentState(mScriptMachine, 17, NULL, false);
    }

    cTeam* pTeam = m_pFielder->m_pTeam;
    if (ePowerup != POWER_UP_NONE
        && ePowerup != pTeam->GetCurrentPowerUp().eType)
    {
        pTeam->TogglePowerup(false);
    }

    SetPowerup(pTeam->GetCurrentPowerUp().eType,
        pTeam->GetCurrentPowerUp().nnumOfPowerups, pTarget);
}

/**
 * Offset/Address/Size: 0x19DC | 0x800D3A50 | size: 0x26C
 */
void DesireUsePowerup::SetPowerup(
    ePowerUpType ePowerup, int nNumPowerups, cFielder* pTarget)
{
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_uWasRunning
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    m_pFielder->m_nPowerupAnimID = -1;
    mbThrowingPowerup = true;

    switch (ePowerup)
    {
    case POWER_UP_GREEN_SHELL:
    case POWER_UP_RED_SHELL:
    case POWER_UP_SPINY_SHELL:
    case POWER_UP_FREEZE_SHELL:
    case POWER_UP_BANANA:
    case POWER_UP_BOBOMB:
        if (pTarget == NULL)
        {
            pTarget = FindPowerupTarget(m_pFielder, ePowerup);
        }
        break;
    case POWER_UP_NONE:
    case POWER_UP_CHAIN_CHOMP:
    case POWER_UP_MUSHROOM:
    case POWER_UP_STAR:
    default:
        pTarget = NULL;
        break;
    }

    mePowerup = ePowerup;
    mnNumPowerups = nNumPowerups;
    mpTarget = pTarget;

    if (ePowerup == POWER_UP_NONE)
    {
        return;
    }

    bool bRetainPowerup = false;
    switch (ePowerup)
    {
    case (ePowerUpType)12:
    case (ePowerUpType)15:
    case (ePowerUpType)16:
    case (ePowerUpType)20:
        bRetainPowerup = true;
    case (ePowerUpType)6:
    case (ePowerUpType)7:
    case (ePowerUpType)8:
    case (ePowerUpType)9:
    case (ePowerUpType)10:
    case (ePowerUpType)11:
    case (ePowerUpType)13:
    case (ePowerUpType)14:
    case (ePowerUpType)17:
    case (ePowerUpType)18:
    case (ePowerUpType)19:
        ThrowPowerup(this);
        break;
    default:
        int nDirection = 0;
        if (mpTarget != NULL)
        {
            nDirection = (m_pFielder->GetFacingDeltaToPosition(
                mpTarget->mUnidentified024.m_v3Position) >> 14) & 3;
        }

        switch (m_pFielder->mUnidentified024.m_eCharacterClass)
        {
        case (eCharacterClass)3:
            if (m_pFielder->m_eAnimID == 0x52
                || m_pFielder->m_eAnimID == 0x54)
            {
                if (nDirection == 0) nDirection = 3;
                else if (nDirection == 2) nDirection = 1;
            }
            else if (m_pFielder->m_eAnimID == 0x53
                || m_pFielder->m_eAnimID == 0x55)
            {
                if (nDirection == 1) nDirection = 2;
                else if (nDirection == 3) nDirection = 0;
            }
            break;
        case (eCharacterClass)7:
        case (eCharacterClass)11:
        case (eCharacterClass)13:
            if (m_pFielder->m_eAnimID == 0x52
                || m_pFielder->m_eAnimID == 0x54)
            {
                if (nDirection == 1) nDirection = 2;
                else if (nDirection == 3) nDirection = 0;
            }
            else if (m_pFielder->m_eAnimID == 0x53
                || m_pFielder->m_eAnimID == 0x55)
            {
                if (nDirection == 0) nDirection = 3;
                else if (nDirection == 2) nDirection = 1;
            }
            break;
        default:
            break;
        }

        m_pFielder->SetPowerupAnimState(sPowerupThrowAnims[nDirection]);
        m_pFielder->m_nPowerupAnimID = sPowerupThrowAnims[nDirection];
        m_pFielder->mtPowerupThrowTime.SetSeconds(0.2f);
        break;
    }

    if (!bRetainPowerup)
    {
        m_pFielder->m_pTeam->ClearCurrentPowerUp();
    }
}

inline void DesireUsePowerup::ResetPowerupState()
{
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_uWasRunning
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    m_pFielder->m_nPowerupAnimID = -1;
}

/**
 * Offset/Address/Size: 0x1C48 | 0x800D3CBC | size: 0x518
 */
void ThrowPowerup(DesireUsePowerup* pDesire)
{
    if (g_pGame->mbCaptainShotToScoreOn)
    {
        pDesire->ResetPowerupState();
        return;
    }

    ePowerUpType ePowerup = pDesire->m_pFielder->GetPowerupType();
    switch (ePowerup)
    {
    case (ePowerUpType)9:
    case (ePowerUpType)10:
    case (ePowerUpType)11:
    case (ePowerUpType)12:
    case (ePowerUpType)13:
    case (ePowerUpType)14:
    case (ePowerUpType)15:
    case (ePowerUpType)16:
    case (ePowerUpType)17:
    case (ePowerUpType)18:
    case (ePowerUpType)19:
    case (ePowerUpType)20:
    {
        if (!pDesire->m_pFielder->fn_8003E6EC())
        {
            UnidentifiedVariantCollection params;
            TransitionFunc* pTransition
                = !pDesire->mOverrideTransition.IsUnset()
                ? &pDesire->mOverrideTransition.mValue
                : &pDesire->mDefaultTransition.mValue;
            params.Set(10, FuzzyVariant(FT_U32,
                pTransition->mFuncHash));
            ActivateConcurrentState(pDesire->mScriptMachine, 23, &params, false);
            NativeTransitionFunc transition((void*)TransDesireUsePowerup);
            pDesire->mOverrideTransition.mValue.mFuncHash
                = transition.mValue.mFuncHash;
            pDesire->mOverrideTransition.mValue.mNativeFunc
                = transition.mValue.mNativeFunc;
            pDesire->ResetPowerupState();
        }
        break;
    }
    case POWER_UP_STAR:
    {
        ActivateConcurrentState(pDesire->mScriptMachine, 24, NULL, true);
        pDesire->ResetPowerupState();
        break;
    }
    case POWER_UP_MUSHROOM:
    {
        ActivateConcurrentState(pDesire->mScriptMachine, 25, NULL, true);
        pDesire->ResetPowerupState();
        break;
    }
    case POWER_UP_GREEN_SHELL:
    case POWER_UP_RED_SHELL:
    case POWER_UP_SPINY_SHELL:
    case POWER_UP_FREEZE_SHELL:
    case POWER_UP_BANANA:
    case POWER_UP_BOBOMB:
    {
        bool bHasPad = pDesire->m_pFielder->GetGlobalPad() != NULL;
        if (bHasPad)
        {
            pDesire->mpTarget = FindPowerupTarget(
                pDesire->m_pFielder,
                pDesire->m_pFielder->GetPowerupType());
        }

        PowerupThrowParameters params;
        BuildPowerupThrowParameters(pDesire->m_pFielder, pDesire->mePowerup,
            pDesire->mnNumPowerups, &params);
        if (PowerupCreateAndThrow(pDesire->m_pFielder,
                pDesire->mpTarget, params))
        {
            PowerupUsedEventData* event
                = (PowerupUsedEventData*)g_PowerupUsedEventDataPool.Allocate();
            event->Type = params.eType;
            event->Thrower = pDesire->m_pFielder;
            event->Target = pDesire->mpTarget;
            QueuePowerupUsed(event);
        }
        pDesire->ResetPowerupState();
        break;
    }
    case POWER_UP_CHAIN_CHOMP:
    {
        gNPCManager->GetChainChomp()->Spawn(
            pDesire->m_pFielder, NULL);
        pDesire->ResetPowerupState();
        break;
    }
    case POWER_UP_NONE:
        return;
    }

    if (g_pGame->IsGameplayOrOvertime())
    {
        StatsTracker::s_pInstance->TrackStat(
            STATS_19, pDesire->m_pFielder->m_pTeam->m_nSide,
            pDesire->m_pFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        if (IsMushroomPowerup(ePowerup))
        {
            StatsTracker::s_pInstance->TrackStat(
                STATS_1A, pDesire->m_pFielder->m_pTeam->m_nSide,
                pDesire->m_pFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
        else if (IsStarOrChainChompPowerup(ePowerup))
        {
            StatsTracker::s_pInstance->TrackStat(
                STATS_1C, pDesire->m_pFielder->m_pTeam->m_nSide,
                pDesire->m_pFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
        else if (IsCaptainPowerup(ePowerup))
        {
            StatsTracker::s_pInstance->TrackStat(
                STATS_1D, pDesire->m_pFielder->m_pTeam->m_nSide,
                pDesire->m_pFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
        else if (IsDrawablePowerup(ePowerup))
        {
            StatsTracker::s_pInstance->TrackStat(
                STATS_1B, pDesire->m_pFielder->m_pTeam->m_nSide,
                pDesire->m_pFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
    }
}

/**
 * Offset/Address/Size: 0x2160 | 0x800D41D4 | size: 0x17C
 */
void DesireUsePowerup::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = cache->BeginType("DesireUsePowerup");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpTarget - (u8*)&mvDesiredPosition, "mpTarget");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbThrowingPowerup - (u8*)&mvDesiredPosition,
        "mbThrowingPowerup");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mePowerup - (u8*)&mvDesiredPosition, "mePowerup");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&mnNumPowerups - (u8*)&mvDesiredPosition,
        "mnNumPowerups");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mtPowerupEffectTime - (u8*)&mvDesiredPosition,
        "mtPowerupEffectTime");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x22DC | 0x800D4350 | size: 0xC0
 */
void DesireUsePowerup::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireUsePowerupType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireUsePowerupType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireUsePowerupType,
        (u8*)this + offset, sizeof(DesireUsePowerup) - offset);
    if (data != NULL)
    {
        DesireUsePowerup* copy
            = (DesireUsePowerup*)((u8*)data - offset);
        *(int*)&copy->mpTarget
            = mpTarget == NULL ? -1 : mpTarget->mUnidentified120;
        cache->ChecksumData(sDesireUsePowerupType, data, context);
    }
}

/**
 * Offset/Address/Size: 0x239C | 0x800D4410 | size: 0x5C
 */
DesireUsePowerup::~DesireUsePowerup()
{
}
