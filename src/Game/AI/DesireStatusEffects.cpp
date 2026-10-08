#include "NL/nlDLListContainer.inl"
#include "Game/AI/Desire.h"
#include "Game/AI/AIContext.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/AI/ScriptMachine.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/ShotMeter.h"
#include "Game/CharacterTweaks.h"
#include "Game/CharacterTriggers.h"
#include "Game/DebugWriteCache.h"
#include "Game/Game.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Render/BulletBill.h"
#include "Game/Render/PeachPhoto.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

float gSlipperySlideFactor;
extern const nlVector3 gStatusEffectZeroVector = { 0.0f, 0.0f, 0.0f };

static unsigned short sDesireStarType = 0xFFFF;
static unsigned short sDesireMushroomType = 0xFFFF;
static unsigned short sDesireSlipperyType = 0xFFFF;
static unsigned short sDesireGooeyType = 0xFFFF;
static unsigned short sDesireShrinkType = 0xFFFF;
static unsigned short sDesireFrozenType = 0xFFFF;
static unsigned short sDesireConfusedType = 0xFFFF;

static float sSlipperyDuration = 2.0f;
static float sSlipperySlideFactor = 1.0f;
static float sShrinkDuration = 20.0f;
static float sShrinkSpeedScale = 0.9f;
static float sShrinkMovementScale = 0.75f;
static float sShrinkPlayerScale = 0.6f;
static float sShrinkScaleDuration = 0.15f;
static float sMushroomPlayerScale = 1.25f;
static float sStarShotAgeScale = 2.5f;
static float sConfusionRampDuration = 0.33f;
static nlVector2 sConfusionReapplyIncrement = { 0.1f, 0.0f };

/**
 * Offset/Address/Size: 0x0 | 0x800BC0C4 | size: 0x68
 */
bool DesireStar::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    mMaxDuration = GetStarEffectTime(m_pFielder->GetTweaks());
    m_pFielder->muInvincibleStatus |= 0x1F;
    EmitStar(m_pFielder, false);
    return result;
}

/**
 * Offset/Address/Size: 0x68 | 0x800BC12C | size: 0x74
 */
bool DesireStar::Reinitialize(void* context)
{
    mAgeTimer.m_uWasRunning = mAgeTimer.m_uPackedTime != 0;
    mAgeTimer.m_uPackedTime = 0;
    bool result = Desire::Initialize(context);
    mMaxDuration = GetStarEffectTime(m_pFielder->GetTweaks());
    EmitStar(m_pFielder, true);
    return result;
}

static inline bool IsSidekick(const cFielder* fielder)
{
    return !fielder->IsCaptain();
}

/**
 * Offset/Address/Size: 0xDC | 0x800BC1A0 | size: 0x58C
 */
void DesireStar::Update(DesireUpdate* update, float fDeltaT)
{
    eFielderActionState action = m_pFielder->m_eActionState;
    if (action == ACTION_SHOOT_TO_SCORE
        || (action == ACTION_UNKNOWN_30
            && m_pFielder->m_pShotMeter->m_eShotMeterState
                == SHOT_METER_STS_ACTIVE
            && IsSidekick(m_pFielder)))
    {
        float scaledDeltaT = fDeltaT * sStarShotAgeScale;
        mAgeTimer.Countup(scaledDeltaT - fDeltaT, 10.0f);
    }

    switch (m_pFielder->m_eActionState)
    {
    case ACTION_SHOT:
    case ACTION_UNKNOWN_32:
    case (eFielderActionState)0x21:
        *update = 1;
        break;
    default:
        if (!m_pFielder->IsInvincible())
        {
            m_pFielder->muInvincibleStatus |= 0x1F;
        }

        if (!g_pGame->IsGameplayOrOvertime())
        {
            *update = 1;
        }
        break;
    }
}

/**
 * Offset/Address/Size: 0x668 | 0x800BC72C | size: 0x3C
 */
void DesireStar::Cleanup()
{
    KillStar(m_pFielder);
    m_pFielder->ClearInvincibility(true);
}

/**
 * Offset/Address/Size: 0x6A4 | 0x800BC768 | size: 0x84
 */
bool DesireMushroom::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    mMaxDuration = GetMushroomEffectTime(m_pFielder->GetTweaks());
    m_pFielder->EndShrink();
    if (!m_pFielder->IsSuperGrowActive())
    {
        m_pFielder->fn_8001EE74(sMushroomPlayerScale, 0.2f, -1.0f);
    }
    EmitMushroom(m_pFielder, false);
    return result;
}

/**
 * Offset/Address/Size: 0x728 | 0x800BC7EC | size: 0x64
 */
bool DesireMushroom::Reinitialize(void* context)
{
    mAgeTimer.m_uWasRunning = mAgeTimer.m_uPackedTime != 0;
    mAgeTimer.m_uPackedTime = 0;
    bool result = Desire::Initialize(context);
    EmitMushroom(m_pFielder, true);
    return result;
}

/**
 * Offset/Address/Size: 0x78C | 0x800BC850 | size: 0x4DC
 */
void DesireMushroom::Update(DesireUpdate* update, float)
{
    switch (m_pFielder->m_eActionState)
    {
    case (eFielderActionState)0:
    case ACTION_ELECTROCUTION:
    case (eFielderActionState)3:
    case (eFielderActionState)5:
    case ACTION_HIT_REACT:
    case ACTION_SHOT:
    case ACTION_SLIDE_ATTACK_REACT:
    case (eFielderActionState)24:
    case ACTION_BOMB_REACT:
    case ACTION_SHELL_REACT:
    case ACTION_BANANA_REACT:
    case ACTION_UNKNOWN_31:
        *update = 1;
        break;
    default:
        if (!g_pGame->IsGameplayOrOvertime())
        {
            *update = 1;
        }
        break;
    }
}

/**
 * Offset/Address/Size: 0xC68 | 0x800BCD2C | size: 0x64
 */
void DesireMushroom::Cleanup()
{
    KillMushroom(m_pFielder);
    if (!m_pFielder->IsSuperGrowActive()
        && !m_pFielder->IsShrunk())
    {
        m_pFielder->fn_8001EE74(1.0f, 0.2f, -1.0f);
    }
}

/**
 * Offset/Address/Size: 0xCCC | 0x800BCD90 | size: 0x3C
 */
bool DesireSlippery::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    mMaxDuration = sSlipperyDuration;
    gSlipperySlideFactor = sSlipperySlideFactor;
    return result;
}

/**
 * Offset/Address/Size: 0xD08 | 0x800BCDCC | size: 0x20
 */
bool DesireSlippery::Reinitialize(void* context)
{
    mAgeTimer.m_uWasRunning = mAgeTimer.m_uPackedTime != 0;
    mAgeTimer.m_uPackedTime = 0;
    return Desire::Initialize(context);
}

/**
 * Offset/Address/Size: 0xD28 | 0x800BCDEC | size: 0x288
 */
void DesireSlippery::Update(DesireUpdate* update, float)
{
    if (!g_pGame->IsGameplayOrOvertime())
    {
        *update = 1;
    }
}

/**
 * Offset/Address/Size: 0xFB0 | 0x800BD074 | size: 0x4
 */
void DesireSlippery::Cleanup()
{
}

/**
 * Offset/Address/Size: 0xFB4 | 0x800BD078 | size: 0x78
 */
DesireGooey::DesireGooey()
    : Desire(27, UnsetTransitionFunc(g_UnsetTransitionFunc))
    , mfGooPercentage(1.0f)
    , mfMaxGooEffect(1.0f)
    , mfAdditionalGooEffect(-1.0f)
    , mfGooTime(0.0f)
    , mf_NotRunning_SpeedScale(1.0f)
    , mf_NotRunning_MovementScale(1.0f)
{
}

/**
 * Offset/Address/Size: 0x102C | 0x800BD0F0 | size: 0xD4
 */
bool DesireGooey::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    UnidentifiedVariantCollection* params = (UnidentifiedVariantCollection*)context;
    float fGooEffect = params->Get(0)->mData.f;
    if (fGooEffect < mfMaxGooEffect)
    {
        mfAdditionalGooEffect = -1.0f;
        mfMaxGooEffect = params->Get(0)->mData.f;
        mfGooTime = params->Get(1)->mData.f;
        mf_NotRunning_SpeedScale = params->Get(2)->mData.f;
        mf_NotRunning_MovementScale = params->Get(3)->mData.f;
        mMaxDuration = mfGooTime;
    }
    else
    {
        mfAdditionalGooEffect = fGooEffect;
    }
    mfGooPercentage = 1.0f;
    return result;
}

/**
 * Offset/Address/Size: 0x1100 | 0x800BD1C4 | size: 0x2C
 */
bool DesireGooey::Reinitialize(void* context)
{
    mAgeTimer.m_uWasRunning = mAgeTimer.m_uPackedTime != 0;
    mAgeTimer.m_uPackedTime = 0;
    return Initialize(context);
}

/**
 * Offset/Address/Size: 0x112C | 0x800BD1F0 | size: 0x18
 */
float DesireGooey::GetSpeedScale()
{
    return InterpolateRangeClamped(
        1.0f, mfMaxGooEffect, 0.0f, 1.0f, mfGooPercentage);
}

/**
 * Offset/Address/Size: 0x1144 | 0x800BD208 | size: 0x344
 */
void DesireGooey::Update(
    DesireUpdate* update, float fDeltaT)
{
    if (mfAdditionalGooEffect != -1.0f)
    {
        mfMaxGooEffect += mfAdditionalGooEffect * (mfGooTime * fDeltaT);
    }

    mfGooPercentage = 1.0f
                    - (mAgeTimer.GetSeconds() / mMaxDuration);
    if (!m_pFielder->IsRunning())
    {
        m_pFielder->m_pCurrentAnimController
            ->m_fPlaybackSpeedScale = InterpolateRangeClamped(
            1.0f, mf_NotRunning_SpeedScale, 0.02f, 2.0f, mfGooPercentage);
        float movementScale = InterpolateRangeClamped(
            1.0f, mf_NotRunning_MovementScale, 0.02f, 2.0f, mfGooPercentage);
        m_pFielder->fn_8001EF6C(movementScale);
    }
    else
    {
        float movementScale = InterpolateRangeClamped(
            1.0f, mfMaxGooEffect, 0.0f, 1.0f, mfGooPercentage);
        m_pFielder->fn_8001EF6C(movementScale);
    }

    if (!g_pGame->IsGameplayOrOvertime())
    {
        *update = 1;
    }
}

/**
 * Offset/Address/Size: 0x1488 | 0x800BD54C | size: 0x48
 */
void DesireGooey::Cleanup()
{
    mfMaxGooEffect = 1.0f;
    m_pFielder->fn_8001EF6C(1.0f);
    m_pFielder->m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
}

/**
 * Offset/Address/Size: 0x14D0 | 0x800BD594 | size: 0x1C8
 */
bool DesireShrink::Initialize(void* context)
{
    cFielder* source;
    bool result = Desire::Initialize(context);
    mMaxDuration = sShrinkDuration;
    mfSlowPercentage = 1.0f;

    m_pFielder->EndMushroom();
    m_pFielder->EndMarioSuperPower();
    m_pFielder->EndWarioSuperPower(false);
    m_pFielder->EndPeteySuperPower(false);
    m_pFielder->EndBowserSuperPower(false);
    m_pFielder->EndWaluigiSuperPower();
    m_pFielder->EndLuigiSuperPower();
    m_pFielder->EndFrozenOrDazed();
    m_pFielder->fn_8001EE74(1.0f, 0.0f, -1.0f);
    m_pFielder->fn_8001EE74(
        sShrinkPlayerScale, sShrinkScaleDuration, sShrinkMovementScale);

    UnidentifiedVariantCollection* params
        = (UnidentifiedVariantCollection*)context;
    source = (cFielder*)params->Get(14)->mData.pointer;
    m_pFielder->SetTweaks(source->m_pSuperPowerTweaks);
    m_pFielder->m_pTweaks->mUnidentified004
        = m_pFielder->m_pNormalTweaks->mUnidentified004.GetValue();
    m_pFielder->m_pTweaks->fWidth
        = fn_8002BFA8(m_pFielder->m_pNormalTweaks, 1.0f);
    CreateMushroomEffect(m_pFielder);

    if (m_pFielder->m_pBall != 0)
    {
        if (m_pFielder->GetDesireState()
            == (eFielderDesireState)32)
        {
            m_pFielder->ReleaseBall(0);
            m_pFielder->EndDesire();
            m_pFielder->InitActionRunning();
        }
        else
        {
            m_pFielder->ReleaseBall(0);
            m_pFielder->ShootBallDueToContact(
                m_pFielder->mUnidentified024
                    .m_aActualFacingDirection);
        }
    }

    if (g_pGame->IsGameplayOrOvertime()
        && g_pGame->m_eGameState != 4)
    {
        PlaySound(source->m_uSoundSlotId, 0xE6E31092, 0, 0);
    }
    return result;
}

/**
 * Offset/Address/Size: 0x1698 | 0x800BD75C | size: 0x8
 */
float DesireShrink::GetSpeedScale()
{
    return sShrinkSpeedScale;
}

/**
 * Offset/Address/Size: 0x16A0 | 0x800BD764 | size: 0x2AC
 */
void DesireShrink::Update(DesireUpdate* update, float)
{
    if (!(m_pFielder->mUnidentified024.m_fPlayerScale < 0.99f))
    {
        m_pFielder->fn_8001EE74(
            sShrinkPlayerScale, 0.0f, sShrinkMovementScale);
    }

    if (!g_pGame->IsGameplayOrOvertime())
    {
        *update = 1;
    }
}

/**
 * Offset/Address/Size: 0x194C | 0x800BDA10 | size: 0xA8
 */
void DesireShrink::Cleanup()
{
    CreateMushroomEffect(m_pFielder);
    m_pFielder->m_pTweaks = m_pFielder->m_pNormalTweaks;
    m_pFielder->fn_8001EE74(1.0f, sShrinkScaleDuration, 1.0f);
    if (g_pGame->IsGameplayOrOvertime() && g_pGame->m_eGameState != 4)
    {
        cFielder* captain = m_pFielder->m_pTeam->GetOtherTeam()->GetCaptain();
        PlaySound(captain->m_uSoundSlotId, 0x8011C562, 0, 0);
    }
}

static inline void SetAnimationUpdatePaused(cPlayer* player, bool paused)
{
    player->mUnidentified1E4.m_bSkipAnimUpdate = paused;
    player->mUnidentified1E4.m_fSkipTimer = 0.0f;
}

static inline void SetActionUpdatePaused(cPlayer* player, bool paused)
{
    player->mUnidentified1E4.m_bSkipActionUpdate = paused;
    player->mUnidentified1E4.m_fSkipTimer = 0.0f;
}

/**
 * Offset/Address/Size: 0x19F4 | 0x800BDAB8 | size: 0x24C
 */
bool DesireFrozen::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    switch (m_pFielder->m_eActionState)
    {
    case ACTION_SHOT:
    case ACTION_SHOOT_TO_SCORE:
    case (eFielderActionState)19:
    case ACTION_RUNNING_WB:
        if (!m_pFielder->IsYoshiSuperPowerActive())
        {
            m_pFielder->InitActionRunning();
        }
        break;
    case ACTION_UNKNOWN_32:
        if (m_pFielder->GetCharacterClass() == SHYGUY
            && m_pFielder->m_eActionState == ACTION_UNKNOWN_32
            && m_pFielder->m_pBulletBill->active)
        {
            m_pFielder->m_pBulletBill->Hide(false);
        }
        break;
    case (eFielderActionState)1:
    case (eFielderActionState)33:
        if (m_pFielder->GetCharacterClass() == BOO)
        {
            m_pFielder->EndAction();
            m_pFielder->RestoreTangibility(false);
        }
        break;
    }

    if (m_pFielder->IsSlideAttacking())
    {
        m_pFielder->fn_8004D238();
    }

    mePrevActionState = m_pFielder->m_eActionState;
    mfPrevFrozenTime = -1.0f;
    mePrevFrozenState = FROZEN_NONE;
    m_pFielder->SetVelocity(gStatusEffectZeroVector);
    m_pFielder->mUnidentified024.m_fDesiredSpeed = 0.0f;
    m_pFielder->mUnidentified024.m_fActualSpeed = 0.0f;
    if (KillDaze(m_pFielder))
    {
        mbWasDazed = true;
    }
    else
    {
        mbWasDazed = false;
    }
    KillConfused(m_pFielder);
    KillWindups();
    KillSlideTackleTrail(m_pFielder, 1);
    KillHitTrail(m_pFielder, 1);
    EndElectrocution(m_pFielder);
    KillDeke(m_pFielder);
    if (m_pFielder->mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
    {
        KillSkillshotPlayerOnFire(m_pFielder);
    }
    if (IsBowserSuperPowerActive(m_pFielder))
    {
        EndBowserSmoke(m_pFielder);
    }
    if (m_pFielder->m_bSuperPowerTankOn)
    {
        m_pFielder->TurnOffSuperPowerTank(true);
    }
    if (m_pFielder->GetDesireState() != FIELDERDESIRE_FINISH_ACTION)
    {
        m_pFielder->EndDesire();
    }

    UnidentifiedVariantCollection* params = (UnidentifiedVariantCollection*)context;
    SetFrozenState(params->Get(0)->mData.i);
    SetAnimationUpdatePaused(m_pFielder, true);
    SetActionUpdatePaused(m_pFielder, true);
    return result;
}

/**
 * Offset/Address/Size: 0x1C40 | 0x800BDD04 | size: 0xAC
 */
bool DesireFrozen::Reinitialize(void* context)
{
    if (meFrozenState == FROZEN_MEGA_STRIKE)
    {
        return false;
    }

    mePrevFrozenState = meFrozenState;
    mfPrevFrozenTime = mMaxDuration - mAgeTimer.GetSeconds();
    mAgeTimer.m_uWasRunning = mAgeTimer.m_uPackedTime != 0;
    mAgeTimer.m_uPackedTime = 0;

    UnidentifiedVariantCollection* params = (UnidentifiedVariantCollection*)context;
    SetFrozenState(params->Get(0)->mData.i);
    KillFreeze(m_pFielder);
    return Desire::Initialize(context);
}

static inline float GetCharacterOpacity(const cCharacter* character)
{
    return character->mUnidentified178;
}

/**
 * Offset/Address/Size: 0x1CEC | 0x800BDDB0 | size: 0xB4
 */
void DesireFrozen::Update(DesireUpdate*, float)
{
    if (meFrozenState == FROZEN_PHOTO && gPeachPhotoState.textureReady)
    {
        if (GetCharacterOpacity(m_pFielder) != 0.0f)
        {
            m_pFielder->mUnidentified178 = 0.0f;
        }
    }
    if (!g_pGame->IsGameplayOrOvertime())
    {
        if (m_pFielder->IsMarioSuperPowerActive() || m_pFielder->IsLuigiSuperPowerActive())
        {
            m_pFielder->EndSuperPower(0);
        }
    }
}

/**
 * Offset/Address/Size: 0x1DA0 | 0x800BDE64 | size: 0x258
 */
void DesireFrozen::Cleanup()
{
    switch (meFrozenState)
    {
    case FROZEN_NONE:
        break;
    case FROZEN_ICE:
        EmitUnFreeze(m_pFielder);
        PowerupBase::PlayPowerupSound(POWER_UP_FREEZE_SHELL,
            PowerupBase::PWRUP_SOUND_END, m_pFielder->m_pPhysicsCharacter, 0.0f, 0);
        break;
    case FROZEN_PHOTO:
        m_pFielder->mUnidentified17D = false;
        m_pFielder->SetTangible(true, false);
        m_pFielder->mUnidentified178 = 1.0f;
        m_pFielder->SetModelType(0);
        break;
    case FROZEN_MEGA_STRIKE:
        SetAnimationUpdatePaused(m_pFielder, false);
        SetActionUpdatePaused(m_pFielder, false);
        switch (mePrevFrozenState)
        {
        case FROZEN_NONE:
            break;
        case FROZEN_ICE:
            EmitUnFreeze(m_pFielder);
            break;
        case FROZEN_PHOTO:
            m_pFielder->mUnidentified17D = false;
            m_pFielder->SetTangible(true, false);
            m_pFielder->mUnidentified178 = 1.0f;
            m_pFielder->SetModelType(0);
            break;
        case FROZEN_MEGA_STRIKE:
        case FROZEN_SHATTERED:
        default:
            break;
        }
        m_pFielder->EndConfusion();
        if (m_pFielder->mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
        {
            m_pFielder->fn_8009750C();
            m_pFielder->EndAction();
        }
        m_pFielder->EndMarioSuperPower();
        m_pFielder->EndLuigiSuperPower();
        m_pFielder->EndStar();
        m_pFielder->EndMushroom();
        break;
    case FROZEN_SHATTERED:
        m_pFielder->SetTangible(true, false);
        m_pFielder->mUnidentified178 = 1.0f;
        break;
    }

    if (m_pFielder->mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
    {
        EmitSkillshotPlayerOnFire(m_pFielder);
    }
    if (IsBowserSuperPowerActive(m_pFielder))
    {
        EmitBowserSmoke(m_pFielder);
    }
    if (m_pFielder->IsConfused())
    {
        EmitConfused(m_pFielder);
    }
    if (mbWasDazed)
    {
        EmitDaze(m_pFielder);
    }
    if (m_pFielder->m_eActionState == ACTION_ELECTROCUTION)
    {
        EmitElectrocution(m_pFielder);
    }
    if (m_pFielder->m_eActionState == (eFielderActionState)1)
    {
        EmitDeke(m_pFielder);
    }
    SetAnimationUpdatePaused(m_pFielder, false);
    SetActionUpdatePaused(m_pFielder, false);
    if (m_pFielder->m_eActionState == ACTION_NEED_ACTION)
    {
        m_pFielder->EndDesire();
        m_pFielder->StartRunning();
    }
}

/**
 * Offset/Address/Size: 0x1FF8 | 0x800BE0BC | size: 0xF0
 */
void DesireFrozen::Activate(float duration, int state)
{
    UnidentifiedVariantCollection params;
    params.Set(7, FuzzyVariant(duration));
    params.Set(0, FuzzyVariant(state));
    ActivateConcurrentState(mScriptMachine, 29, &params, mActive);
}

/**
 * Offset/Address/Size: 0x20E8 | 0x800BE1AC | size: 0x14C
 */
void DesireFrozen::SetFrozenState(int state)
{
    switch (state)
    {
    case FROZEN_NONE:
        break;
    case FROZEN_ICE:
        m_pFielder->fn_8009750C();
        EmitFreeze(m_pFielder);
        m_pFielder->fn_8001F1D8();
        if (mePrevFrozenState != FROZEN_ICE)
        {
            PlaySound(16, 0x1DFB6861, 0, 0);
        }
        break;
    case FROZEN_PHOTO:
        m_pFielder->mUnidentified17D = true;
        m_pFielder->SetTangible(false, false);
        break;
    case FROZEN_MEGA_STRIKE:
        m_pFielder->EndStar();
        if (mePrevFrozenState == FROZEN_ICE)
        {
            EmitUnFreeze(m_pFielder);
        }
        break;
    case FROZEN_SHATTERED:
        m_pFielder->SetTangible(false, false);
        m_pFielder->mUnidentified178 = 0.0f;
        m_pFielder->ResetEffects();
        m_pFielder->ClearInvincibility(false);
        PowerupBase::StopPowerupInEffectSound(POWER_UP_STAR,
            PowerupBase::PWRUP_SOUND_IN_EFFECT, m_pFielder);
        if (m_pFielder->GetCharacterClass() == SHYGUY)
        {
            m_pFielder->m_pBulletBill->Hide(true);
        }
        break;
    }
    meFrozenState = state;
}

static inline int GetFacingDirection(cFielder* fielder)
{
    return fielder->GetActualFacing();
}

/**
 * Offset/Address/Size: 0x2234 | 0x800BE2F8 | size: 0x178
 */
bool DesireConfused::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    mfConfusedDirection = 16384.0f + nlRandomf(16384.0f);
    if (nlRandomf(1.0f) < 0.5f)
    {
        mfConfusedDirection = -mfConfusedDirection;
    }
    mfConfusedPercentage = 0.0f;
    EmitConfused(m_pFielder);

    if (m_pFielder->m_pBall != 0)
    {
        if (m_pFielder->GetDesireState()
            == (eFielderDesireState)32)
        {
            m_pFielder->ReleaseBall(0);
            m_pFielder->EndDesire();
            m_pFielder->InitActionRunning();
        }
        else if (m_pFielder->mUnidentified024.m_eCharacterClass
                     == SHYGUY
                 && m_pFielder->m_eActionState
                        == ACTION_UNKNOWN_32)
        {
            int direction = GetFacingDirection(m_pFielder);
            bool hasGlobalPad = m_pFielder->GetGlobalPad() != 0;
            if (hasGlobalPad)
            {
                direction = (unsigned short)(direction + (int)(mfConfusedDirection * mfConfusedPercentage));
            }
            m_pFielder->SetFacingDirection(direction, true);
        }
        else
        {
            m_pFielder->ReleaseBall(0);
            m_pFielder->ShootBallDueToContact(
                m_pFielder->mUnidentified024.m_aActualFacingDirection);
        }
    }

    mvDesiredPosition = gStatusEffectZeroVector;
    mvDesiredPosition.x = 1.0f;
    GetStateMachineAIContext(this)->SetTimer(0xFF, 0.0f);
    return result;
}

/**
 * Offset/Address/Size: 0x23AC | 0x800BE470 | size: 0xB8
 */
bool DesireConfused::Reinitialize(void* context)
{
    mAgeTimer.m_uWasRunning = mAgeTimer.m_uPackedTime != 0;
    mAgeTimer.m_uPackedTime = 0;
    bool result = Desire::Initialize(context);
    mfConfusedPercentage += sConfusionReapplyIncrement.x;
    if (mfConfusedPercentage >= 1.0f)
    {
        mfConfusedPercentage = 1.0f;
    }

    if (m_pFielder->m_pBall != 0
        && (m_pFielder->mUnidentified024.m_eCharacterClass
                != SHYGUY
            || m_pFielder->m_eActionState
                   != ACTION_UNKNOWN_32))
    {
        m_pFielder->ReleaseBall(0);
        m_pFielder->ShootBallDueToContact(
            m_pFielder->mUnidentified024.m_aActualFacingDirection);
    }
    return result;
}

/**
 * Offset/Address/Size: 0x2464 | 0x800BE528 | size: 0x7FC
 */
void DesireConfused::Update(
    DesireUpdate* update, float)
{
    mfConfusedPercentage
        += mAgeTimer.GetSeconds() / sConfusionRampDuration;
    if (mfConfusedPercentage >= 1.0f)
    {
        mfConfusedPercentage = 1.0f;
    }

    if (!g_pGame->IsGameplayOrOvertime())
    {
        *update = 1;
    }
    if (m_pFielder->IsStarActive())
    {
        *update = 1;
    }
    if (m_pFielder->mUnidentified1E4.m_tFireTimer.m_uPackedTime
        != 0)
    {
        *update = 1;
    }

    bool hasGlobalPad = m_pFielder->GetGlobalPad() != 0;
    if (!hasGlobalPad)
    {
        if (!GetStateMachineAIContext(this)->IsTimerRunning(0xFF))
        {
            GetStateMachineAIContext(this)->SetTimer(0xFF, 0.5f);
            nlPolar polar;
            polar.r = 1.0f;
            polar.a = nlRandom(0xFFFF);
            nlPolarToCartesian(mvDesiredPosition, polar);
        }

        if (!ReceivingPass(m_pFielder))
        {
            nlVector3 position;
            nlVec3ScaleAdd(position, 5.0f, mvDesiredPosition,
                m_pFielder->GetPosition());
            m_pFielder->AddDesiredPosition(position, 1.0f, 4.0f);
        }
    }
}

/**
 * Offset/Address/Size: 0x2C60 | 0x800BED24 | size: 0x70
 */
void DesireConfused::AdjustInputDirection(unsigned short* direction)
{
    bool hasGlobalPad = m_pFielder->GetGlobalPad() != 0;
    if (hasGlobalPad)
    {
        *direction += (int)(mfConfusedDirection * mfConfusedPercentage);
    }
}

/**
 * Offset/Address/Size: 0x2CD0 | 0x800BED94 | size: 0x8
 */
void DesireConfused::Cleanup()
{
    KillConfused(m_pFielder);
}

/**
 * Offset/Address/Size: 0x3704 | 0x800BF7C8 | size: 0xC8
 */
inline void DesireStar::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireStar");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x37CC | 0x800BF890 | size: 0x9C
 */
inline void DesireStar::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireStarType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireStarType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireStarType, data, context);
    cache->WriteData(sDesireStarType, data, sizeof(DesireStar) - offset);
}

/**
 * Offset/Address/Size: 0x35A0 | 0x800BF664 | size: 0xC8
 */
inline void DesireMushroom::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireMushroom");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x3668 | 0x800BF72C | size: 0x9C
 */
inline void DesireMushroom::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireMushroomType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireMushroomType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireMushroomType, data, context);
    cache->WriteData(sDesireMushroomType, data, sizeof(DesireMushroom) - offset);
}

/**
 * Offset/Address/Size: 0x343C | 0x800BF500 | size: 0xC8
 */
inline void DesireSlippery::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireSlippery");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x3504 | 0x800BF5C8 | size: 0x9C
 */
inline void DesireSlippery::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSlipperyType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireSlipperyType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireSlipperyType, data, context);
    cache->WriteData(sDesireSlipperyType, data, sizeof(DesireSlippery) - offset);
}

/**
 * Offset/Address/Size: 0x3224 | 0x800BF2E8 | size: 0x17C
 */
inline void DesireGooey::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireGooey");
    cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfGooPercentage - (u8*)&mvDesiredPosition, "mfGooPercentage");
    cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfMaxGooEffect - (u8*)&mvDesiredPosition, "mfMaxGooEffect");
    cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfGooTime - (u8*)&mvDesiredPosition, "mfGooTime");
    cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mf_NotRunning_SpeedScale - (u8*)&mvDesiredPosition, "mf_NotRunning_SpeedScale");
    cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mf_NotRunning_MovementScale - (u8*)&mvDesiredPosition, "mf_NotRunning_MovementScale");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x33A0 | 0x800BF464 | size: 0x9C
 */
inline void DesireGooey::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireGooeyType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireGooeyType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireGooeyType, data, context);
    cache->WriteData(sDesireGooeyType, data, sizeof(DesireGooey) - offset);
}

/**
 * Offset/Address/Size: 0x309C | 0x800BF160 | size: 0xEC
 */
inline void DesireShrink::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireShrink");
    cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfSlowPercentage - (u8*)&mvDesiredPosition, "mfSlowPercentage");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x3188 | 0x800BF24C | size: 0x9C
 */
inline void DesireShrink::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireShrinkType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireShrinkType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireShrinkType, data, context);
    cache->WriteData(sDesireShrinkType, data, sizeof(DesireShrink) - offset);
}

/**
 * Offset/Address/Size: 0x2E84 | 0x800BEF48 | size: 0x17C
 */
inline void DesireFrozen::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireFrozen");
    cache->AddField(DEBUG_FIELD_ENUM, gDebugFieldTypes[DEBUG_FIELD_ENUM].size, (u8*)&meFrozenState - (u8*)&mvDesiredPosition, "meFrozenState");
    cache->AddField(DEBUG_FIELD_ENUM, gDebugFieldTypes[DEBUG_FIELD_ENUM].size, (u8*)&mePrevFrozenState - (u8*)&mvDesiredPosition, "mePrevFrozenState");
    cache->AddField(DEBUG_FIELD_ENUM, gDebugFieldTypes[DEBUG_FIELD_ENUM].size, (u8*)&mePrevActionState - (u8*)&mvDesiredPosition, "mePrevActionState");
    cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfPrevFrozenTime - (u8*)&mvDesiredPosition, "mfPrevFrozenTime");
    cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mbWasDazed - (u8*)&mvDesiredPosition, "mbWasDazed");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x3000 | 0x800BF0C4 | size: 0x9C
 */
inline void DesireFrozen::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireFrozenType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireFrozenType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireFrozenType, data, context);
    cache->WriteData(sDesireFrozenType, data, sizeof(DesireFrozen) - offset);
}

/**
 * Offset/Address/Size: 0x2CD8 | 0x800BED9C | size: 0x110
 */
inline void DesireConfused::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireConfused");
    cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfConfusedPercentage - (u8*)&mvDesiredPosition, "mfConfusedPercentage");
    cache->AddField(DEBUG_FIELD_INT, gDebugFieldTypes[DEBUG_FIELD_INT].size, (u8*)&mfConfusedDirection - (u8*)&mvDesiredPosition, "mfConfusedDirection");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x2DE8 | 0x800BEEAC | size: 0x9C
 */
inline void DesireConfused::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireConfusedType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireConfusedType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireConfusedType, data, context);
    cache->WriteData(sDesireConfusedType, data, sizeof(DesireConfused) - offset);
}
