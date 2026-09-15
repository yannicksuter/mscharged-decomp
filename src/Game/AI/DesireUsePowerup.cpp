#include "Game/AI/DesireUsePowerup.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/DesireUpdate.h"
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
extern "C" shdStateMachine* fn_80319E84(
    UnidentifiedScriptMachine*, int, UnidentifiedVariantCollection*, bool);
extern "C" cTeam* fn_800D6670(cFielder*);
extern "C" float fn_800D85F8(cFielder*);
extern "C" float fn_800E0034();
extern "C" bool fn_8031A04C();
extern "C" UnidentifiedDesireUpdate fn_80041B6C(
    void*, const unsigned int&, cFielder*);
extern "C" UnidentifiedStateTransition* fn_80315A14(
    UnidentifiedStateTransition*, void*);
extern "C" void fn_8009A5D8(
    cFielder*, ePowerUpType, int, unk_8009A5D8*);
extern "C" void fn_80148074(PowerupUsedEventData*);
extern "C" bool fn_80099C80(ePowerUpType);
extern "C" bool fn_80099C94(ePowerUpType);
extern "C" bool fn_80099CE8(ePowerUpType);
extern "C" bool fn_80099CC4(ePowerUpType);
extern "C" void fn_800B6A1C(UnidentifiedDesireUpdate*, int, const Variant&);
static int lbl_806DC3A0 = 17;
static bool lbl_806DC3A4 = true;
static int lbl_806DC39C = 0;
static int lbl_80502B10[4] = { 0x59, 0x5C, 0x5B, 0x5A };
static unsigned short sDesireUsePowerupType = 0xFFFF;

/**
 * Offset/Address/Size: 0x0 | 0x800D2074 | size: 0xF48
 */
extern "C" UnidentifiedDesireUpdate fn_800D2074(
    UnidentifiedFielderInput* input)
{
    UnidentifiedDesireUpdate result(FT_INT, lbl_806DC39C);
    cFielder* pFielder = (cFielder*)input->mData.pPlayer;
    unsigned long key = input->fn_8030F9B4(
        (unsigned long)fn_800D2074, 1);

    if (UserControlledT(fn_800D6670(pFielder))
        || (1.0f - fn_800D85F8(pFielder)))
    {
        result = 1;
    }
    else if (fn_8031A04C() && !input->fn_8030FB7C(key)
        && !fn_800E0034())
    {
        input->fn_8030FA10(key, 0.4f);
        unsigned int hash = nlStringHash("TransDesireUsePowerup");
        result = fn_80041B6C(input->mUnidentified14, hash, pFielder);
    }

    return UnidentifiedDesireUpdate(result, -1.0f, -1.0f);
}

/**
 * Offset/Address/Size: 0xF48 | 0x800D2FBC | size: 0x128
 */
bool DesireUsePowerup::UnidentifiedInitialize(void* context)
{
    cFielder* pTarget;
    cTeam* pTeam;
    bool result;
    ePowerUpType ePowerup;
    UnidentifiedVariantCollection* params;

    result = Desire::UnidentifiedInitialize(context);
    params = (UnidentifiedVariantCollection*)context;

    mbThrowingPowerup = false;
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_unk0
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    mUnidentifiedFielder->m_nPowerupAnimID = -1;
    mUnidentified078 = -1.0f;

    if (params->IsSet(15))
    {
        ePowerup = (ePowerUpType)params->Get(15)->mData.i;
        pTarget = (cFielder*)params->Get(14)->mData.pPlayer;

        if (fn_8002EDC8(mUnidentifiedFielder, -1))
        {
            pTeam = mUnidentifiedFielder->m_pTeam;
            if (ePowerup != POWER_UP_NONE
                && ePowerup != pTeam->GetCurrentPowerUp().eType)
            {
                pTeam->TogglePowerup(false);
            }

            fn_800D3A50(
                pTeam->GetCurrentPowerUp().eType,
                pTeam->GetCurrentPowerUp().nnumOfPowerups,
                pTarget);
        }
    }

    return result;
}

/**
 * Offset/Address/Size: 0x1070 | 0x800D30E4 | size: 0x7B0
 */
void DesireUsePowerup::Update(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    if (update->mData.i == 3)
    {
        fn_800B6A1C(update, 8, FuzzyVariant(lbl_806DC3A0));
        fn_800B6A1C(update, 9, FuzzyVariant(lbl_806DC3A4));
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
void DesireUsePowerup::UnidentifiedCleanup()
{
    mbThrowingPowerup = false;
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_unk0
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    mUnidentifiedFielder->m_nPowerupAnimID = -1;
}

/**
 * Offset/Address/Size: 0x185C | 0x800D38D0 | size: 0x98
 */
extern "C" void fn_800D38D0(DesireUsePowerup* pDesire)
{
    if (!fn_8002EDC8(pDesire->mUnidentifiedFielder, -1))
    {
        return;
    }

    if (!pDesire->mUnidentifiedActive)
    {
        fn_80319E84(pDesire->mUnidentified018, 17, NULL, false);
    }

    cTeam* pTeam = pDesire->mUnidentifiedFielder->m_pTeam;
    pDesire->fn_800D3A50(pTeam->GetCurrentPowerUp().eType,
        pTeam->GetCurrentPowerUp().nnumOfPowerups, NULL);
}

/**
 * Offset/Address/Size: 0x18F4 | 0x800D3968 | size: 0xE8
 */
void DesireUsePowerup::fn_800D3968(
    cFielder* pTarget, ePowerUpType ePowerup, bool bActivate)
{
    if (!fn_8002EDC8(mUnidentifiedFielder, -1))
    {
        return;
    }

    if (!mUnidentifiedActive && bActivate)
    {
        fn_80319E84(mUnidentified018, 17, NULL, false);
    }

    cTeam* pTeam = mUnidentifiedFielder->m_pTeam;
    if (ePowerup != POWER_UP_NONE
        && ePowerup != pTeam->GetCurrentPowerUp().eType)
    {
        pTeam->TogglePowerup(false);
    }

    fn_800D3A50(pTeam->GetCurrentPowerUp().eType,
        pTeam->GetCurrentPowerUp().nnumOfPowerups, pTarget);
}

/**
 * Offset/Address/Size: 0x19DC | 0x800D3A50 | size: 0x26C
 */
void DesireUsePowerup::fn_800D3A50(
    ePowerUpType ePowerup, int nNumPowerups, cFielder* pTarget)
{
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_unk0
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    mUnidentifiedFielder->m_nPowerupAnimID = -1;
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
            pTarget = FindPowerupTarget(mUnidentifiedFielder, ePowerup);
        }
        break;
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
        fn_800D3CBC(this);
        break;
    default:
        int nDirection = 0;
        if (mpTarget != NULL)
        {
            nDirection = (mUnidentifiedFielder->GetFacingDeltaToPosition(
                mpTarget->mUnidentified024.m_v3Position) >> 14) & 3;
        }

        switch (mUnidentifiedFielder->mUnidentified024.m_eCharacterClass)
        {
        case (eCharacterClass)3:
            if (mUnidentifiedFielder->m_eAnimID == 0x52
                || mUnidentifiedFielder->m_eAnimID == 0x54)
            {
                if (nDirection == 0) nDirection = 3;
                else if (nDirection == 2) nDirection = 1;
            }
            else if (mUnidentifiedFielder->m_eAnimID == 0x53
                || mUnidentifiedFielder->m_eAnimID == 0x55)
            {
                if (nDirection == 1) nDirection = 2;
                else if (nDirection == 3) nDirection = 0;
            }
            break;
        case (eCharacterClass)7:
        case (eCharacterClass)11:
        case (eCharacterClass)13:
            if (mUnidentifiedFielder->m_eAnimID == 0x52
                || mUnidentifiedFielder->m_eAnimID == 0x54)
            {
                if (nDirection == 1) nDirection = 2;
                else if (nDirection == 3) nDirection = 0;
            }
            else if (mUnidentifiedFielder->m_eAnimID == 0x53
                || mUnidentifiedFielder->m_eAnimID == 0x55)
            {
                if (nDirection == 0) nDirection = 3;
                else if (nDirection == 2) nDirection = 1;
            }
            break;
        default:
            break;
        }

        mUnidentifiedFielder->SetPowerupAnimState(lbl_80502B10[nDirection]);
        mUnidentifiedFielder->m_nPowerupAnimID = lbl_80502B10[nDirection];
        mUnidentifiedFielder->mtPowerupThrowTime.SetSeconds(0.2f);
        break;
    }

    if (!bRetainPowerup)
    {
        mUnidentifiedFielder->m_pTeam->ClearCurrentPowerUp();
    }
}

inline void DesireUsePowerup::UnidentifiedResetPowerupState()
{
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_unk0
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    mUnidentifiedFielder->m_nPowerupAnimID = -1;
}

/**
 * Offset/Address/Size: 0x1C48 | 0x800D3CBC | size: 0x518
 */
extern "C" void fn_800D3CBC(DesireUsePowerup* pDesire)
{
    if (g_pGame->mbCaptainShotToScoreOn)
    {
        pDesire->UnidentifiedResetPowerupState();
        return;
    }

    ePowerUpType ePowerup = pDesire->mUnidentifiedFielder->GetPowerupType();
    switch (ePowerup)
    {
    case POWER_UP_NONE:
        return;
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
        if (!pDesire->mUnidentifiedFielder->fn_8003E6EC())
        {
            UnidentifiedVariantCollection params;
            UnidentifiedStateTransition* pTransition;
            if (pDesire->mUnidentified070.UnidentifiedIsUnset())
            {
                pTransition = &pDesire->mUnidentified068;
            }
            else
            {
                pTransition = &pDesire->mUnidentified070;
            }
            params.Set(10, FuzzyVariant(FT_U32,
                pTransition->mUnidentifiedHash));
            fn_80319E84(pDesire->mUnidentified018, 23, &params, false);
            UnidentifiedStateTransition transition;
            fn_80315A14(&transition, (void*)fn_800D2074);
            pDesire->mUnidentified070 = transition;
            pDesire->UnidentifiedResetPowerupState();
        }
        break;
    }
    case POWER_UP_STAR:
    {
        fn_80319E84(pDesire->mUnidentified018, 24, NULL, true);
        pDesire->UnidentifiedResetPowerupState();
        break;
    }
    case POWER_UP_MUSHROOM:
    {
        fn_80319E84(pDesire->mUnidentified018, 25, NULL, true);
        pDesire->UnidentifiedResetPowerupState();
        break;
    }
    case POWER_UP_GREEN_SHELL:
    case POWER_UP_RED_SHELL:
    case POWER_UP_SPINY_SHELL:
    case POWER_UP_FREEZE_SHELL:
    case POWER_UP_BANANA:
    case POWER_UP_BOBOMB:
    {
        bool bHasPad = pDesire->mUnidentifiedFielder->GetGlobalPad() != NULL;
        if (bHasPad)
        {
            pDesire->mpTarget = FindPowerupTarget(
                pDesire->mUnidentifiedFielder,
                pDesire->mUnidentifiedFielder->GetPowerupType());
        }

        unk_8009A5D8 params;
        fn_8009A5D8(pDesire->mUnidentifiedFielder, pDesire->mePowerup,
            pDesire->mnNumPowerups, &params);
        unk_8009A5D8 throwParams = params;
        if (PowerupCreateAndThrow(pDesire->mUnidentifiedFielder,
                pDesire->mpTarget, &throwParams))
        {
            PowerupUsedEventData* event
                = (PowerupUsedEventData*)g_PowerupUsedEventDataPool.Allocate();
            event->Type = params.eType;
            event->Thrower = pDesire->mUnidentifiedFielder;
            event->Target = pDesire->mpTarget;
            fn_80148074(event);
        }
        pDesire->UnidentifiedResetPowerupState();
        break;
    }
    case POWER_UP_CHAIN_CHOMP:
    {
        gNPCManager->GetChainChomp()->Spawn(pDesire->mUnidentifiedFielder);
        pDesire->UnidentifiedResetPowerupState();
        break;
    }
    default:
        break;
    }

    if (g_pGame->IsGameplayOrOvertime())
    {
        StatsTracker::s_pInstance->TrackStat(
            STATS_19, pDesire->mUnidentifiedFielder->m_pTeam->m_nSide,
            pDesire->mUnidentifiedFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        if (fn_80099C80(ePowerup))
        {
            StatsTracker::s_pInstance->TrackStat(
                STATS_1A, pDesire->mUnidentifiedFielder->m_pTeam->m_nSide,
                pDesire->mUnidentifiedFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
        else if (fn_80099C94(ePowerup))
        {
            StatsTracker::s_pInstance->TrackStat(
                STATS_1C, pDesire->mUnidentifiedFielder->m_pTeam->m_nSide,
                pDesire->mUnidentifiedFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
        else if (fn_80099CE8(ePowerup))
        {
            StatsTracker::s_pInstance->TrackStat(
                STATS_1D, pDesire->mUnidentifiedFielder->m_pTeam->m_nSide,
                pDesire->mUnidentifiedFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
        else if (fn_80099CC4(ePowerup))
        {
            StatsTracker::s_pInstance->TrackStat(
                STATS_1B, pDesire->mUnidentifiedFielder->m_pTeam->m_nSide,
                pDesire->mUnidentifiedFielder->mUnidentified1E4.m_ID, 0, 0, 0, 0);
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
    cache->AddField(22, gDebugFieldTypes[22].size,
        0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
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
