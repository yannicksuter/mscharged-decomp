#include "NL/nlDLListContainer.inl"
#include "Game/AI/DesireUserControlled.h"
#include "Game/Player.h"
#include "Game/CharacterTweaks.h"
#include "Game/DetInput.h"

#include "Game/AI/DesireSteering.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/Fielder.h"
#include "Game/AI/ShotMeter.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/Game.h"
#include <stddef.h>
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Task/FixedUpdateTask.h"



static unsigned short sDesireUserControlledType = 0xFFFF;
static unsigned int sMegaStrikeDesireState = 0x20;

/**
 * Offset/Address/Size: 0x0 | 0x800D448C | size: 0x100
 */
bool DesireUserControlled::Initialize(void* context)
{
    bool result = Desire::Initialize(context);

    DesireSteering* desire = (DesireSteering*)GetFielderDesire(
        m_pFielder, 34);
    ResetSteeringTargets(desire);
    ResetSteeringHistory(desire);
    ResetSteeringAvoidance(desire);

    mMaxDuration = -1.0f;

    DesireUpdate update;
    Update(
        &update, g_fSimulationTick);

    return result;
}

/**
 * Offset/Address/Size: 0x100 | 0x800D458C | size: 0x6BC
 */
void DesireUserControlled::Update(
    DesireUpdate* update, float fDeltaT)
{
    bool bHasPad = (bool)m_pFielder->GetGlobalPad();
    if (!bHasPad)
    {
        *update = 1;
        return;
    }

    if (g_pGame->m_eGameState == 1)
    {
        return;
    }
    else if (m_pFielder->GetActionState() == ACTION_SHOOT_TO_SCORE
        || m_pFielder->GetActionState() == ACTION_SHOT)
    {
        return;
    }
    else
    {
        fn_80098098(m_pFielder);
        if (m_pFielder->m_eActionState == ACTION_NEED_ACTION
            || m_pFielder->m_eActionState == ACTION_WAIT)
        {
            m_pFielder->StartRunning();
        }

        SetDesiredFacingDirection(m_pFielder);
        if (m_pFielder->m_eActionState == ACTION_RUNNING)
        {
            float fMaxSpeed = fn_8002C254(
                m_pFielder->GetTweaks());
            float fMinSpeed = GetJogSpeed(
                m_pFielder->GetTweaks());
            m_pFielder->SetDesiredSpeed(fMinSpeed, fMaxSpeed);
            m_pFielder->TestLooseBallControls(false);

            if (g_pBall->m_pOwner == NULL
                && (!IsWaluigiSuperPowerActive(m_pFielder)
                    || !m_pFielder->m_bSuperPowerTankOn))
            {
                m_pFielder->UpdateFacingToLooseBall();
            }
            return;
        }

        if (m_pFielder->m_eActionState == ACTION_UNKNOWN_30)
        {
            m_pFielder->TestButtonsWindup();
            if (m_pFielder->m_eActionState != ACTION_UNKNOWN_30)
            {
                return;
            }
            if (m_pFielder->m_pBall == NULL)
            {
                return;
            }

            m_pFielder->bIsModified
                = m_pFielder->IsActionModifierPressed();
            u8 bIsShotActive = true;
            eShotMeterState state;
            ShotMeter* pShotMeter = m_pFielder->m_pShotMeter;
            state = pShotMeter->m_eShotMeterState;
            if (state != SHOT_METER_ACTIVE
                && state != SHOT_METER_STS_ACTIVE)
            {
                bIsShotActive = false;
            }
            if (bIsShotActive)
            {
                if (!m_pFielder->GetGlobalPad()->IsPressed(0x1C, true))
                {
                    m_pFielder->InitActionShot(
                        m_pFielder->bIsModified,
                        false);
                }
                return;
            }

            if (pShotMeter->m_eShotMeterState == SHOT_METER_RELEASED
                || pShotMeter->m_eShotMeterState
                    == SHOT_METER_STS_RELEASED)
            {
                m_pFielder->InitActionShot(
                    m_pFielder->bIsModified,
                    false);
                return;
            }

            if (pShotMeter->m_eShotMeterState
                == SHOT_METER_STS_TRANSITION)
            {
                *update = 3;
                update->SetParameter(8, FuzzyVariant(FT_INT, sMegaStrikeDesireState));
            }
            return;
        }

        if (m_pFielder->m_eActionState == ACTION_RUNNING_WB)
        {
            float fMaxSpeed = m_pFielder->GetTweaks()->GetRunningSpeed();
            float fMinSpeed = GetJogSpeed(
                m_pFielder->GetTweaks());
            m_pFielder->SetDesiredSpeed(fMinSpeed, fMaxSpeed);
            m_pFielder->TestButtonsRunningWB(fDeltaT);
        }
    }
}

/**
 * Offset/Address/Size: 0x7BC | 0x800D4C48 | size: 0x4
 */
void DesireUserControlled::Cleanup()
{
}

/**
 * Offset/Address/Size: 0x7C0 | 0x800D4C4C | size: 0xC8
 */
inline void DesireUserControlled::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = cache->BeginType("DesireUserControlled");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x888 | 0x800D4D14 | size: 0x9C
 */
inline void DesireUserControlled::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireUserControlledType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireUserControlledType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireUserControlledType, data, context);
    cache->WriteData(sDesireUserControlledType, data,
        sizeof(DesireUserControlled) - offset);
}

/**
 * Offset/Address/Size: 0x924 | 0x800D4DB0 | size: 0x5C
 */
inline DesireUserControlled::~DesireUserControlled()
{
}
