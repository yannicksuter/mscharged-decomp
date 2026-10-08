#include "Game/AI/DesireUpdate.h"
#include "NL/nlDLListContainer.inl"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "NL/nlFunction.inl"
#include "Game/AI/Fielder.inl"
#include "Game/PoseAccumulator.h"
#include "Game/AI/ScriptMachine.h"
#include "Game/DetInput.h"
#include "Game/Audio/GameStreams.h"
#include "Game/RumbleActions.h"
#include "Game/AI/FielderDesireMachine.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/FielderActions.h"
#include "Game/AI/DesireSteering.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/DesirePass.h"
#include "Game/AI/DesireUsePowerup.h"
#include "Game/AI/DesireSuperPower.h"
#include "Game/AI/HeadTrack.h"
#include "Game/AI/AvoidableObject.h"
#include "NL/nlMain.h"
#include "NL/nlString.h"
#include "NL/nlSlotPool.h"

#include "Game/AI/ShotMeter.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/Ball.h"
#include "Game/Render/BulletBill.h"
#include "Game/CharacterTweaks.h"
#include "Game/DebugWriteCache.h"
#include "Game/DB/StatsTracker.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "NL/nlMath.inl"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsColumn.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Sys/audio.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/YoshiEggObject.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/KoopaShellObject.h"
#include "Game/Render/ChainChomp.h"
#include "Game/Render/WindDebris.h"
#include "Game/Render/ThwompObject.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Team.h"
#include "Game/Weather.h"
#include "Game/Terrain.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/Task/FixedUpdateTask.h"
#include "math.h"
#include <stddef.h>
#include "Game/DB/StadiumInfo.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include "Game/CharacterTriggers.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

float gHeadTrackingSmoothTime = 0.22f;
float gImpactRumbleX = 0.035f;
float gImpactRumbleY = 0.02f;
float gImpactRumbleSpring = 2500.0f;
float gImpactRumbleDamping = 5.0f;
float gSuperImpactRumbleX = 0.2f;
float gSuperImpactRumbleY = 0.225f;
float gSuperImpactRumbleSpring = 4300.0f;
float gSuperImpactRumbleDamping = 5.75f;
float gHeavyImpactRumbleX = 0.065f;
float gHeavyImpactRumbleY = 0.05f;
float gHeavyImpactRumbleSpring = 3900.0f;
float gHeavyImpactRumbleDamping = 6.3f;
float gBulletImpactRumbleX = 0.25f;
float gBulletImpactRumbleY = 0.175f;
float gBulletImpactRumbleSpring = 4450.0f;
float gBulletImpactRumbleDamping = 5.8f;
float gStopAnimPlaybackSpeed = 1.33f;
float gBackRunningAnimPlaybackSpeed = 1.33f;
float gBackRunningTerrainSpeedBoost = 0.33f;
float gHardStopAnimPlaybackSpeed = 2.0f;
float gHardStopTerrainSpeedBoost = 0.58f;
float gStartAnimPlaybackSpeed = 1.2f;
float gStartTerrainSpeedBoost = 0.33f;
float gSlideInterceptTimeScale = 0.33f;
float gSlideBallContactRadiusBuffer = 0.2f;
float gGasConfusionDuration = 5.0f;
float gKoopaBirdoSkillshotHeightFraction = 0.6f;
float gWindDebrisKnockbackScale = 2.5f;
float gWindDebrisKnockbackZ = 20.0f;
float gYoshiYolkGooDuration = 0.2f;
float gYoshiYolkAnimSpeedScale = 0.6f;
float gYoshiYolkMovementScale = 0.1f;
float gMuckHoleGooDuration = 1.0f;
float gMuckHoleAnimSpeedScale = 0.6f;
float gMuckHoleMovementScale = 0.1f;
float gSandGooDuration = 0.2f;
float gSandRunningSpeedScale = 0.6f;
float gSandAnimSpeedScale = 1.0f;
float gSandMovementScale = 0.6f;
float gFirePatchBurnDuration = 4.5f;
float gThwompBallReleaseSpeed = 10.0f;
float gShotChargeMin = 1.1f;
float gShotChargeMax = 1.1f;
float gSuperPowerHeadTrackingHeight = 1.0f;
bool gUseMovementStickForDeke = true;
float gBaseDekeDistance = 2.5f;
float gDekeChargeDistance = 8.5f;
float gDekeDirectionSpread = 1000.0f;
float gBooDekeAlpha = 0.4f;
float gBooDekeFadeTime = 2.5f;
float gMontyReappearRadius = 2.25f;
float gKoopaSkillshotSpeed = 27.5f;
float gBirdoSkillshotSpeed = 35.0f;
float gShotSpinMinSpeed = 25.0f;
float gShotSpinMaxSpeed = 45.0f;
float gShotSpinMinScale = 0.2f;
float gShotSpinMaxScale = 1.2f;
float gShotYSpinBase = 25.0f;
float gShotYSpinRange = 5.0f;
float gShotYSpinMinDistance = 13.0f;
float gShotYSpinMaxDistance = 18.0f;
float gClearMinFlightTime = 1.33f;
float gClearMaxFlightTime = 1.75f;
float gClearMinDistance = 12.5f;
float gClearMaxDistance = 23.5f;
float gClearPositionWeight = 0.2f;
float gClearShotMeterWeight = 0.33f;
float gPenaltyPossessionGraceTime = 0.66f;
float gHitFrameLockMinIntensity = 0.2f;
float gLightHitReactionThreshold = -0.425f;
float gHeavyHitReactionThreshold = 0.425f;
bool gHitFrameLockRequiresHeavyReaction = true;
float gModifiedShotFarTargetOffset = 5.0f;
float gModifiedShotMinTargetOffsetFraction = 0.9f;
float gChipShotMinVerticalSpeed = 22.5f;
float gChipShotMaxVerticalSpeed = 22.5f;
float gModifiedShotNearSpeed = 8.3f;
float gModifiedShotFarSpeed = 12.075f;
float gModifiedShotNearDistance = 8.875f;
float gModifiedShotFarDistance = 20.0f;
float gChipShotAirResistance = 0.15f;
bool gbUseDumpCharging = true;
float gDumpChargeMin = 1.5f;
float gDumpChargeMax = 1.5f;
float gLooseBallContactBufferTime = 0.45f;
static TweakBoolBinding sUseDumpChargingTweak(
    "gbUseDumpCharging", "Game/Gameplay/Charging/Dump", &gbUseDumpCharging, true);

bool gForceUserControl;
bool gForceHomeUserControl;
bool gForceAwayUserControl;
bool gDisableClearAngleLimits;
float gWindDebrisKnockbackZRange;
bool gUseBowserPeteyHeadTracking;
bool gShrinkerIgnoreTeammates;
float gIntangibleAlpha;
bool gDisableElectrocution;
unsigned char gEnableStandingWallElectrocution;
unsigned char gForceHeavyHitReaction;

static inline cFielder* GetAIOrderedFielder(cTeam* pTeam, s32 i)
{
    return pTeam->m_pAIOrderedFielders[i];
}

static inline s16 GetAngleDifference(u32 a, u32 b)
{
    return (s16)(a - b);
}

static inline bool IsPowerupBlockedByAction(eFielderActionState eActionState)
{
    switch (eActionState)
    {
    case 3:
    case 0x18:
        return true;
    default:
        return false;
    }
}

static inline bool IsSidekick(const cFielder* pFielder)
{
    return !pFielder->IsCaptain();
}

static inline bool CanShootFromPosition(cFielder* pFielder, bool requireBall)
{
    float radius = 0.0f;
    pFielder->m_pPhysicsCharacter->GetRadius(&radius);

    float offset = 0.1f + radius;
    float maxX = offset + pFielder->GetPosition().x;
    float minX = pFielder->GetPosition().x - offset;
    bool maxXInHalf = maxX * pFielder->m_pTeam->GetOtherNet()->m_fDirection >= 0.0f;
    bool inAttackingHalf = true;
    if (!maxXInHalf)
    {
        bool minXInHalf = minX * pFielder->m_pTeam->GetOtherNet()->m_fDirection >= 0.0f;
        if (!minXInHalf)
        {
            inAttackingHalf = false;
        }
    }

    bool canShoot = !requireBall && inAttackingHalf;
    if (pFielder->HasBall())
    {
        canShoot = inAttackingHalf;
    }
    return canShoot;
}

static inline void GetCharacterSpecialActive(
    const cFielder* fielder, eCharacterClass character, bool& active)
{
    active = false;
    if (fielder->GetCharacterClass() != character)
    {
        return;
    }
    if (!fielder->IsSuperPowerActive())
    {
        return;
    }
    active = true;
}

inline bool cFielder::CheckReceivePassState()
{
    bool bCanReceivePass = false;
    bool bPassesPeachCheck = false;
    bool bPassesBowserJrCheck = false;
    bool bPassesDaisyCheck = false;
    bool bPassesYoshiCheck = false;
    bool bPassesDKCheck = false;
    bool bPassesSkillshotCheck = false;
    bool bPassesActionCheck = false;

    if (!IsFallenDown())
    {
        bool bAllowedAction = true;
        unsigned int nActionIndex
            = (unsigned int)(m_eActionState - 1);
        if (nActionIndex <= 0x1F
            && ((1U << nActionIndex) & 0x90000001U) != 0)
        {
            bAllowedAction = false;
        }

        if (bAllowedAction)
        {
            bPassesActionCheck = true;
        }
    }

    if (bPassesActionCheck
        && m_eActionState != (eFielderActionState)0x21)
    {
        bPassesSkillshotCheck = true;
    }

    if (bPassesSkillshotCheck)
    {
        bool bExcluded = IsDKSuperPowerActive();
        if (!bExcluded)
        {
            bPassesDKCheck = true;
        }
    }

    if (bPassesDKCheck)
    {
        bool active;
        GetCharacterSpecialActive(this, YOSHI, active);
        if (!active)
        {
            bPassesYoshiCheck = true;
        }
    }

    if (bPassesYoshiCheck)
    {
        bool active;
        GetCharacterSpecialActive(this, DAISY, active);
        if (!active)
        {
            bPassesDaisyCheck = true;
        }
    }

    if (bPassesDaisyCheck)
    {
        bool active;
        GetCharacterSpecialActive(this, BOWSERJR, active);
        if (!active)
        {
            bPassesBowserJrCheck = true;
        }
    }

    if (bPassesBowserJrCheck)
    {
        bool active;
        GetCharacterSpecialActive(this, PEACH, active);
        if (!active)
        {
            bPassesPeachCheck = true;
        }
    }

    if (bPassesPeachCheck)
    {
        DesireFrozen* pAction = (DesireFrozen*)
            GetConcurrentState(m_pAIContext->mScriptMachine, 0x1D);
        bool bFrozenStateActive = false;
        if (pAction != 0 && pAction->mActive
            && pAction->meFrozenState != DesireFrozen::FROZEN_NONE)
        {
            bFrozenStateActive = true;
        }
        if (!bFrozenStateActive)
        {
            bCanReceivePass = true;
        }
    }

    return bCanReceivePass;
}

inline bool cFielder::IsAvailableToReceivePass()
{
    return CheckReceivePassState();
}

static inline u8 IsHittingForCollision(const cFielder* fielder)
{
    return fielder->IsHitting();
}

static inline float GetCollisionHitIntensity(cFielder* hittee,
    const nlVector3& contactPosition, const nlVector3& velocity,
    const cFielder* hitter)
{
    float closingSpeed = GetClosingSpeed(contactPosition, velocity,
        hitter->mUnidentified024.m_v3Position, v3Zero);
    float runningSpeed = fn_8002C254(hittee->m_pTweaks);
    float minimumSpeed = -runningSpeed;
    return NormalizeVal(closingSpeed, minimumSpeed, runningSpeed);
}

static inline int HasBallForCollision(const cFielder* fielder)
{
    return fielder->HasBall();
}

static inline void ResolveSlideAttack(cFielder* pWinner, cFielder* pLoser)
{
    int bHadBall = HasBallForCollision(pLoser);
    bool bBallTooHigh = g_pBall->m_v3Position.z > 0.66f;
    pLoser->InitActionSlideAttackReact(pWinner, false);
    pWinner->bAttackSucceeded = true;
    if (bHadBall && !bBallTooHigh)
    {
        pWinner->PickupBall(g_pBall);
        pWinner->DoPenaltyCardBooking(pLoser, PEN_TYPE_SLIDE_WITH_BALL);
    }
    else
    {
        pWinner->DoPenaltyCardBooking(pLoser, PEN_TYPE_SLIDE_NO_BALL);
    }
}

static inline void ClearPhysicsPatchesOfType(int type)
{
    for (int i = 0; i < 60; ++i)
    {
        PhysicsPatch* pPatch = lbl_806E12C8->fn_801745B8(i);
        if (pPatch != 0 && pPatch->GetType() == type)
        {
            pPatch->Unknown0();
        }
    }
}

static inline void AimClearBallAtTeammate(cFielder* player, int index, float fClearDistance,
    u16& aClearingAngle, nlVector3& v3Direction, nlPolar& pDirection)
{
    cFielder* const pFielder = GetAIOrderedFielder(player->m_pTeam, index);
    bool bCanReceivePass;
    bool bCondition6;
    bool bCondition5;
    bool bCondition4;
    bool bCondition3;
    bool bCondition2;
    bool bCondition1;
    bool bCondition0;
    if (pFielder != player)
    {
        bCanReceivePass = false;
        bCondition6 = false;
        bCondition5 = false;
        bCondition4 = false;
        bCondition3 = false;
        bCondition2 = false;
        bCondition1 = false;
        bCondition0 = false;

        if (!pFielder->IsFallenDown())
        {
            bool bAllowedAction = true;
            unsigned int nActionIndex
                = (unsigned int)(pFielder->m_eActionState - 1);
            if (nActionIndex <= 0x1F
                && ((1U << nActionIndex) & 0x90000001U) != 0)
            {
                bAllowedAction = false;
            }

            if (bAllowedAction)
            {
                bCondition0 = true;
            }
        }

        if (bCondition0
            && pFielder->m_eActionState != (eFielderActionState)0x21)
        {
            bCondition1 = true;
        }

        if (bCondition1)
        {
            bool bExcluded = pFielder->IsDKSuperPowerActive();
            if (!bExcluded)
            {
                bCondition2 = true;
            }
        }

        if (bCondition2)
        {
            bool bExcluded = pFielder->IsYoshiSuperPowerActive();
            if (!bExcluded)
            {
                bCondition3 = true;
            }
        }

        if (bCondition3)
        {
            bool bExcluded
                = pFielder->mUnidentified024.m_eCharacterClass == DAISY
               && IsConcurrentStateActive(pFielder->m_pAIContext->mScriptMachine, 0x17);
            if (!bExcluded)
            {
                bCondition4 = true;
            }
        }

        if (bCondition4)
        {
            bool bExcluded
                = pFielder->mUnidentified024.m_eCharacterClass == BOWSERJR
               && IsConcurrentStateActive(pFielder->m_pAIContext->mScriptMachine, 0x17);
            if (!bExcluded)
            {
                bCondition5 = true;
            }
        }

        if (bCondition5)
        {
            bool bExcluded = pFielder->IsPeachSuperPowerActive();
            if (!bExcluded)
            {
                bCondition6 = true;
            }
        }

        if (bCondition6)
        {
            bool bActionActive = pFielder->IsFrozenStateActive();
            if (!bActionActive)
            {
                bCanReceivePass = true;
            }
        }

        if (bCanReceivePass
            && AIsgn(pFielder->mUnidentified024.m_v3Position.x) != AIsgn(player->mUnidentified024.m_v3Position.x))
        {
            if (nlSqrt(nlVec3DistanceSquared2D(pFielder->mUnidentified024.m_v3Position,
                    player->mUnidentified024.m_v3Position), true) > 0.5f * fClearDistance)
            {
                nlVec3Sub(v3Direction, pFielder->mUnidentified024.m_v3Position, player->mUnidentified024.m_v3Position);
                nlCartesianToPolar(pDirection, v3Direction);
                aClearingAngle = pDirection.a;
            }
        }
    }
}

static inline void UpdateClearingAngleForTeammate(cFielder* player, int index, float distance,
    u16& angle, nlVector3& direction, nlPolar& polar)
{
    AimClearBallAtTeammate(player, index, distance, angle, direction, polar);
}

static inline bool CanStartHit(const cFielder* fielder)
{
    return fielder->GetCharacterClass() != YOSHI && !fielder->IsFrozenStateActive();
}

static inline bool IsInHitAction(const cFielder* fielder)
{
    return CanStartHit(fielder) && fielder->m_eActionState == ACTION_HIT;
}

static inline bool HasHitWindowStarted(const cFielder* fielder, float fAnimTime)
{
    return IsInHitAction(fielder) && fAnimTime >= fn_8002D020(fielder->m_pTweaks);
}

static void FindHeadTrackingHitTarget(cFielder* fielder, cPlayer*& target)
{
    {
        UnidentifiedVariant_80054AB8 bestTarget = CallFielderFuzzyFunction(
            FuzzyAIGetFielderRuntime(fielder), "BestHitTarget", fielder);
        if (bestTarget.IsPointerType())
        {
            target = bestTarget.GetPlayer();
            return;
        }
    }
    target = 0;
}

extern u16 gFielderDesireStateDebugType;

struct FielderDesireShdState
{
    void RegisterDebugFields(unsigned short* type, DebugWriteCache* cache);
    u32 m_nTransitionFuncHash;
    u32 m_nLastActiveTime;
    float m_fMaxDuration;
    float m_fMinDuration;
    float m_fAge;
};

#define REGISTER_FIELDER_FIELD(type, base, field, name) \
    cache->AddField(type, gDebugFieldTypes[type].size, \
        (u8*)&(field) - (u8*)&(base), name)

static inline void EndFrozenState(cFielder* fielder, int state)
{
    if (((DesireFrozen*)GetConcurrentState(
            fielder->m_pAIContext->mScriptMachine, 29))->IsActiveFrozenState(state))
    {
        RequestStateMachineDeactivation(GetConcurrentState(
            fielder->m_pAIContext->mScriptMachine, 29));
    }
}

static inline void SetStoppingAnimState(cFielder* fielder, int animation)
{
    fielder->SetAnimState(animation, true, 0.2f, false, false);
    fielder->InitMovementFromAnim(0, v3Zero, 1.0f, false);
    fielder->mUnidentified024.m_fDesiredSpeed = 0.0f;
}

static inline bool IsCharacterSuperPowerActive(const cFielder* fielder, eCharacterClass character)
{
    return fielder->GetCharacterClass() == character && fielder->IsSuperPowerActive();
}

inline void cFielder::SetRunLeanSAB(const int* anims, int count, int primary)
{
    cPN_SingleAxisBlender* blender = CreateSingleAxisBlender(anims, count, primary, RunningSABcallback, 0.1f, 0, 0.5f);
    cPN_SAnimController* synchronized = (cPN_SAnimController*)blender->GetChild(primary);
    synchronized->m_fSynchronizedWeight = 0.0f;
    for (int i = 0; i < count; ++i)
    {
        if (i != primary)
        {
            cPN_SAnimController* next = (cPN_SAnimController*)blender->GetChild(i);
            next->m_bIsSynchronized = true;
            synchronized->m_pSynchronizedController = next;
            synchronized = next;
        }
    }
    *m_pAILayer = new cPN_Blender(*m_pAILayer, blender, 0.1f);
}

static inline bool IsShotValueBelowThreshold(float value, const TweakFloatBinding& threshold)
{
    return value < threshold.GetValue();
}

inline float cFielder::CalculateShotProbability(float fValue)
{
    if (IsShotValueBelowThreshold(fValue, fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue1))
    {
        float upperValue = fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue1.GetValue();
        float upperChance = fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotChance1.GetValue();
        return InterpolateRangeClamped(fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotChance0,
            upperChance, 0.0f, upperValue, fValue);
    }
    else if (IsShotValueBelowThreshold(fValue, fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue2))
    {
        return InterpolateRangeClamped(
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotChance1,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotChance2,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue1,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue2,
            fValue);
    }
    else if (IsShotValueBelowThreshold(fValue, fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue3))
    {
        return InterpolateRangeClamped(
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotChance2,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotChance3,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue2,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue3,
            fValue);
    }
    else
    {
        return InterpolateRangeClamped(
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotChance3,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotChance4,
            fn_800A636C(g_pCurrentlyUpdatingTeam)->fShotValue3,
            1.0f,
            fValue);
    }
}

inline float cFielder::EvaluateShotProbability(float fValue)
{
    return CalculateShotProbability(fValue);
}

static inline float GetShotTargetDistance(const nlVector3& ballPosition, const nlVector3& target)
{
    nlVector3 delta;
    nlVec3Sub(delta, ballPosition, target);
    return nlSqrt(nlVec3LengthSquared(delta), true);
}

#include "NL/nlBind_impl.h"

inline bool cFielder::ShouldSkipHardStopAnim()
{
    bool specialMovement = ((IsCharacterSuperPowerActive(this, (eCharacterClass)6)
            || IsCharacterSuperPowerActive(this, (eCharacterClass)1))
            || (IsCharacterSuperPowerActive(this, (eCharacterClass)11) && m_bSuperPowerTankOn))
        || IsConcurrentStateActive(m_pAIContext->mScriptMachine, 27);
    bool skip = specialMovement || (ReceivingPass(this) && g_pBall->m_tPassTargetTimer.GetSeconds() < 0.5f);
    if (!skip && GetDesireState() == (eFielderDesireState)20)
    {
        Desire* desire = GetFielderDesire(this, 20);
        if (desire->mAgeTimer.GetSeconds() < 0.05f)
            skip = true;
    }
    return skip;
}

static inline bool CanShootWithoutPossession(cFielder* fielder)
{
    return CanShootFromPosition(fielder, false);
}

static inline float GetSlideInterceptTimeLimit(PlayerTweaks* tweaks)
{
    return gSlideInterceptTimeScale * (GetSlideTime(tweaks) + GetSlideDecelTime(tweaks));
}

inline bool cFielder::IsDaisySuperPowerActive() const
{
    return IsCharacterSuperPowerActive(this, (eCharacterClass)2);
}

inline bool cFielder::CanBeCaughtInPhoto() const
{
    return mbTangible || IsCharacterSuperPowerActive(this, (eCharacterClass)8);
}

inline bool cFielder::CanBeFrozen() const
{
    bool frozen = IsFrozen();
    bool canFreeze = false;
    if (!frozen && !IsInvincible())
        canFreeze = true;
    return canFreeze;
}

inline bool cFielder::CanBeAffectedByPhoto() const
{
    bool canFreeze = CanBeFrozen();
    bool susceptible = false;
    if (canFreeze && GetCharacterClass() != (eCharacterClass)5)
        susceptible = true;
    return susceptible && CanBeCaughtInPhoto();
}

cFielder::cFielder(int nPlayerID, int nTeamID, eCharacterClass cc,
    const int* nModelID, cSHierarchy* pHierarchy,
    cAnimInventory* pAnimInventory,
    const CharacterPhysicsData* pCharacterPhysicsData, PlayerTweaks* pCharTweaks,
    PlayerTweaks* pUnidentifiedTweaks,
    AnimRetargetList* pAnimRetargetList, int nIndex)
    : cPlayer(nPlayerID, cc, nModelID, pHierarchy, pAnimInventory,
          pCharacterPhysicsData, pCharTweaks->mUnidentified004,
          fn_8002BFA8(pCharTweaks, 1.0f), pAnimRetargetList, nIndex, FIELDER)
    , mActionCrowdVars(false, -1.0f)
    , m_aDekeDirection(0)
    , m_bDekeReset(false)
    , m_nDPadDownCounter(2)
    , m_fElectrocutionTime(0.0f)
    , m_fElectrocutionLiftTime(0.0f)
    , m_bGroundElectrocution(false)
    , m_fFallingTime(0.0f)
    , m_v3SuckToSpot(v3Zero)
    , m_fHitDistance(0.0f)
    , m_bHitReactFrameLock(false)
    , bYoshiInWindup(false)
    , bIsModified(false)
    , mActionLooseBallPassVars()
    , m_fOneTimerAnimTime(0.0f)
    , m_pPassTarget(0)
    , m_bAllowLeadPass(false)
    , m_bOneTouchPass(false)
    , mActionRunPassVars()
    , mActionRunningVars()
    , mActionRunningWBVars()
    , m_eSlideAttackState(0)
    , bAttackSucceeded(false)
    , m_bSlideAttackReset(true)
    , m_fMegaStrikeNumBalls(0.0f)
    , m_fMegaStrikeAccuracy(0.0f)
    , m_fMegaStrikeReceivedTimestamp(-1.0f)
    , m_fMegaStrikeFirstPressPosition(-1.0f)
    , m_fMegaStrikeSecondPressPosition(-1.0f)
    , m_fMegaStrikeGreenRegionWidth(-1.0f)
    , m_fMegaStrikeMeterPosition(-1.0f)
    , m_fMegaStrikePhaseTimeRemaining(0.0f)
    , m_fMegaStrikeSecondPhaseDuration(0.0f)
    , m_fMegaStrikeFirstPhaseDuration(0.0f)
    , m_bMegaStrikeSecondPhase(false)
    , m_fLocalMegaStrikeNumBalls(0.0f)
    , m_fLocalMegaStrikeAccuracy(0.0f)
    , m_fMegaStrikeSegment1Position(0.0f)
    , m_fMegaStrikeSegment2Position(0.0f)
    , m_fMegaStrikeSegment3Position(0.0f)
    , m_fMegaStrikeSegment4Position(0.0f)
    , mUnidentified3D4(0.0f)
    , m_nStunnedAngularAcceleration(0)
    , m_nStunnedAngularVelocity(0)
    , m_bSuperPowerTankOn(false)
    , m_bSuperPowerTankShutdownPending(false)
    , m_fSuperPowerTankLevel(0.0f)
    , m_fSuperPowerTankCapacity(0.0f)
    , mActionBowserSuper()
    , m_fNextGasTime(0.0f)
    , mWaluigiWallState()
    , m_fPeteyLastMuckBallTime(0.0f)
    , m_fPeteyMuckBallSpeed(0.0f)
    , m_fPeteySuperPowerTime(0.0f)
    , mActionBooSkillshot()
    , m_bMontyDekeFinished(false)
    , m_tMoveToTurboTimer(0.0f)
    , mtPostDekeTimer(0.0f)
    , mtPowerupThrowTime(0.0f)
{
    mfAirInterceptHeight[0] = -1.0f;
    mfAirInterceptHeight[1] = -1.0f;
    m_bHasBeenUpdated = false;
    m_eActionState = ACTION_NEED_ACTION;
    m_bInPosition = false;
    m_nPowerupAnimID = -1;
    m_eRole = (eRole)0;
    mbWasHitByPowerupThisFrame = false;
    mbTangible = true;
    mbIgnorePadSwitchRelease = false;
    for (int i = 0; i < 4; i++)
    {
        m_pMark[i] = 0;
    }
    m_tMoveToTurboTimer.UnidentifiedClear();
    mtPostDekeTimer.UnidentifiedClear();
    mtPowerupThrowTime.UnidentifiedClear();
    muInvincibleStatus = 0;
    m_nMegaStrikeResultState = 0;
    m_pNormalTweaks = pCharTweaks;
    m_pTweaks = pCharTweaks;
    m_pSuperPowerTweaks = pUnidentifiedTweaks;

    m_pShotMeter = new (8, false) ShotMeter();
    m_pAIContext = new (8, false) AIContext(this,
        new (8, false) FielderDesireMachine(),
        new (8, false) FuzzyAIRuntime());
    m_pAIContext->mScriptMachine->Initialize();

    bIsModified = false;
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)6)
    {
        mWaluigiWallState.mUnidentified08
            = new (8, false) WaluigiWallManager();
    }
    else
    {
        mWaluigiWallState.mUnidentified08 = 0;
    }

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)19)
    {
        m_pBulletBill = gNPCManager->fn_801A9D20();
    }
    else
    {
        m_pBulletBill = 0;
    }
}

cFielder::~cFielder()
{
    CleanUpAction(ACTION_NEED_ACTION);
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)6)
    {
        delete mWaluigiWallState.mUnidentified08;
    }
    if (m_pBulletBill != 0)
    {
        m_pBulletBill->Hide(true);
    }
    delete m_pShotMeter;
    m_pAIContext->Cleanup(true, true);
    delete m_pAIContext;
}

float cFielder::GetMegaStrikeMeterPosition()
{
    return m_fMegaStrikeMeterPosition;
}

eFielderDesireState cFielder::GetDesireState()
{
    ScriptMachine* machine = m_pAIContext->mScriptMachine;
    if (machine != 0 && machine->mActiveState != 0)
    {
        return (eFielderDesireState)machine->mActiveState->mState;
    }
    return (eFielderDesireState)-1;
}

Desire* GetFielderDesire(cFielder* pFielder, int nAction)
{
    shdStateMachine* pAction
        = GetScriptMachineState(pFielder->m_pAIContext->mScriptMachine, nAction);
    if (pAction == 0)
    {
        pAction = GetConcurrentState(
            pFielder->m_pAIContext->mScriptMachine, nAction);
    }
    return (Desire*)pAction;
}

void cFielder::EndDesire()
{
    ScriptMachine* machine = m_pAIContext->mScriptMachine;
    if (machine != 0)
    {
        DeactivateScriptMachine(machine);
    }
}

void cFielder::EndAllDesires()
{
    ScriptMachine* machine = m_pAIContext->mScriptMachine;
    if (machine != 0)
    {
        DeactivateScriptMachine(machine);
        DeactivateConcurrentStates(m_pAIContext->mScriptMachine);
    }
}

AvoidController* cFielder::GetAvoidController()
{
    return ((DesireSteering*)GetFielderDesire(this, 34))->m_pAvoidance;
}

FuzzyRuntimeBase* cFielder::GetFuzzyRuntime() const
{
    return m_pAIContext->mRuntime;
}

ScriptMachine* GetFielderScriptMachine(cFielder* pFielder)
{
    return pFielder->m_pAIContext->mScriptMachine;
}

float cFielder::GetRunningSpeed()
{
    float speed;
    if (g_pBall->GetOwnerFielder() == this)
    {
        speed = m_pTweaks->GetRunningSpeed();
    }
    else
    {
        speed = fn_8002C254(m_pTweaks);
    }
    return GetSpeedPowerupAdjusted(speed);
}

bool cFielder::EndMushroom()
{
    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 25))
    {
        DeactivateConcurrentState(m_pAIContext->mScriptMachine, 25);
        return true;
    }
    return false;
}

bool cFielder::EndShrink()
{
    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 28))
    {
        DeactivateConcurrentState(m_pAIContext->mScriptMachine, 28);
        return true;
    }
    return false;
}

bool cFielder::EndStar()
{
    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 24))
    {
        DeactivateConcurrentState(m_pAIContext->mScriptMachine, 24);
        return true;
    }
    return false;
}

void cFielder::EndFrozenOrDazed()
{
    if (IsFrozen())
    {
        EndFrozenState(this, 2);
    }
    else if (IsFielderDazed(this))
    {
        EndDaze();
    }
}

void cFielder::EndConfusion()
{
    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 30))
    {
        RequestStateMachineDeactivation(GetConcurrentState(
            m_pAIContext->mScriptMachine, 30));
    }
}

void cFielder::EndDaze()
{
    EndFrozenState(this, 1);
}

bool cFielder::EndSuperPower(int)
{
    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 23))
    {
        RequestStateMachineDeactivation(GetFielderDesire(this, 23));
        return true;
    }
    return false;
}

bool cFielder::EndBowserSuperPower(bool value)
{
    bool active = false;
    if (mUnidentified024.m_eCharacterClass == BOWSER
        && IsConcurrentStateActive(m_pAIContext->mScriptMachine, 23))
    {
        active = true;
    }
    if (active)
    {
        fn_800C9D74((DesireSuperPower*)GetFielderDesire(this, 23), value);
        return true;
    }
    return false;
}

bool cFielder::EndMarioSuperPower()
{
    bool active = false;
    if (mUnidentified024.m_eCharacterClass == MARIO
        && IsConcurrentStateActive(m_pAIContext->mScriptMachine, 23))
    {
        active = true;
    }
    if (active)
    {
        DeactivateConcurrentState(m_pAIContext->mScriptMachine, 23);
        return true;
    }
    return false;
}

bool cFielder::EndLuigiSuperPower()
{
    bool active = false;
    if (mUnidentified024.m_eCharacterClass == LUIGI
        && IsConcurrentStateActive(m_pAIContext->mScriptMachine, 23))
    {
        active = true;
    }
    if (active)
    {
        DeactivateConcurrentState(m_pAIContext->mScriptMachine, 23);
        return true;
    }
    return false;
}

bool cFielder::EndWaluigiSuperPower()
{
    bool active = false;
    if (mUnidentified024.m_eCharacterClass == WALUIGI
        && IsConcurrentStateActive(m_pAIContext->mScriptMachine, 23))
    {
        active = true;
    }
    if (active)
    {
        DeactivateConcurrentState(m_pAIContext->mScriptMachine, 23);
        return true;
    }
    return false;
}

bool cFielder::EndWarioSuperPower(bool)
{
    bool active = false;
    if (mUnidentified024.m_eCharacterClass == WARIO
        && IsConcurrentStateActive(m_pAIContext->mScriptMachine, 23))
    {
        active = true;
    }
    if (active)
    {
        RequestStateMachineDeactivation(GetFielderDesire(this, 23));
        return true;
    }
    return false;
}

bool cFielder::EndPeteySuperPower(bool)
{
    bool active = false;
    if (mUnidentified024.m_eCharacterClass == PETEY
        && IsConcurrentStateActive(m_pAIContext->mScriptMachine, 23))
    {
        active = true;
    }
    if (active)
    {
        RequestStateMachineDeactivation(GetFielderDesire(this, 23));
        return true;
    }
    return false;
}

int cFielder::GetPreviousDesireState() const
{
    ScriptMachine* machine = m_pAIContext->mScriptMachine;
    if (machine != 0 && machine->mPreviousState != 0)
    {
        return machine->mPreviousState->mState;
    }
    return -1;
}

int GetFielderHitReaction(cFielder* pReceiver,
    cFielder* pAttacker, float attackIntensity)
{
    float fReceiverDefenseSize = pReceiver->m_pTweaks->GetDefenseSize();
    float fAttackerDefenseSize = pAttacker->m_pTweaks->GetDefenseSize();
    int nReaction = 1;
    if (gForceHeavyHitReaction || GameInfoManager::Instance()->IsRule0x8Equal1())
        return 2;

    if (fAttackerDefenseSize < 0.0f && fReceiverDefenseSize >= 0.0f)
        return 0;
    if (fReceiverDefenseSize < 0.0f && fAttackerDefenseSize >= 0.0f)
        return 2;

    attackIntensity -= 0.5f;
    float fRelativeStrength = fAttackerDefenseSize - fReceiverDefenseSize;
    fRelativeStrength += attackIntensity;
    if (fRelativeStrength <= gLightHitReactionThreshold)
        nReaction = 0;
    else if (fRelativeStrength >= gHeavyHitReactionThreshold)
        nReaction = 2;

    if (pAttacker->IsMushroomActive()
        || pAttacker->IsInvincible()
        || pAttacker->IsInvincibleChars()
        || IsFielderFrontInvincible(pAttacker, &pReceiver->GetPosition())
        || pAttacker->IsSuperGrowActive())
    {
        nReaction = 2;
    }
    return nReaction;
}

bool CanUsePowerup(cFielder* pFielder, int nPowerupType)
{
    if (nPowerupType == -1)
    {
        nPowerupType = pFielder->m_pTeam->GetCurrentPowerUp().eType;
    }

    eFielderActionState eActionState = pFielder->m_eActionState;
    if (IsPowerupBlockedByAction(eActionState))
    {
        return false;
    }

    if (eActionState == (eFielderActionState)0x23)
    {
        return false;
    }

    bool bFrozen = IsFielderDazed(pFielder) || pFielder->IsFrozen();
    if (bFrozen)
    {
        return false;
    }

    if (pFielder->IsYoshiSuperPowerActive())
    {
        return false;
    }

    if (pFielder->GetDesireState() == (eFielderDesireState)0x20)
    {
        if (pFielder->m_eActionState == ACTION_SHOT)
        {
            return false;
        }
        if (nPowerupType >= 9 && nPowerupType <= 0x14)
        {
            return false;
        }
    }

    if (pFielder->m_nPowerupAnimID >= 0)
    {
        return false;
    }

    if (pFielder->mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
    {
        return false;
    }

    if (pFielder->m_eActionState == ACTION_ELECTROCUTION && pFielder->m_eAnimID != 0x78
        && pFielder->m_eAnimID != 0x7B)
    {
        return false;
    }

    if (pFielder->IsFallenDown()
        && (nPowerupType == POWER_UP_MUSHROOM || nPowerupType == POWER_UP_STAR))
    {
        return false;
    }

    if (pFielder->m_eActionState == ACTION_UNKNOWN_32
        && (nPowerupType == POWER_UP_MUSHROOM || nPowerupType == POWER_UP_STAR))
    {
        return false;
    }

    if (IsWaluigiSuperPowerActive(pFielder) || IsBowserSuperPowerActive(pFielder) || pFielder->IsPeteySuperPowerActive())
    {
        if (pFielder->IsFallenDown())
        {
            return false;
        }

        switch (pFielder->m_eActionState)
        {
        case 1:
        case ACTION_HIT:
        case ACTION_LATE_ONETIMER_FROM_VOLLEY:
        case ACTION_LOOSE_BALL_PASS:
        case ACTION_LOOSE_BALL_SHOT:
        case ACTION_SHOOT_TO_SCORE:
        case ACTION_ONETIMER:
        case ACTION_ONETOUCH_PASS_FROM_VOLLEY:
        case ACTION_PASS:
        case ACTION_RECEIVE_PASS:
        case (eFielderActionState)0x13:
        case ACTION_UNKNOWN_15:
        case ACTION_SLIDE_ATTACK:
            return false;
        case ACTION_UNKNOWN_30:
            if (IsWaluigiSuperPowerActive(pFielder))
            {
                return false;
            }
            break;
        }
    }

    cFielder* pCaptain = pFielder->m_pTeam->GetCaptain();
    if (pFielder->m_pTeam->fn_800A6764())
    {
        eCharacterClass eCaptainClass = pCaptain->mUnidentified024.m_eCharacterClass;
        if (eCaptainClass == BOWSER || eCaptainClass == PETEY || eCaptainClass == WALUIGI)
        {
            switch (pFielder->m_eActionState)
            {
            case ACTION_HIT:
                return false;
            case 1:
                return false;
            case (eFielderActionState)0x1C:
                return false;
            }

            if ((eCaptainClass == WALUIGI || eCaptainClass == BOWSER || eCaptainClass == PETEY)
                && pFielder->IsFallenDown())
            {
                return false;
            }
        }
    }
    return true;
}

bool cFielder::CanBeHitBySkillshot()
{
    bool result = true;
    if (IsInvincible())
        result = false;
    else
    {
        switch (m_eActionState)
        {
        case 3:
        case 24:
            result = false;
            break;
        case ACTION_ELECTROCUTION:
            if (m_eAnimID == 0x76)
                result = false;
            break;
        }
    }
    return result;
}

bool cFielder::CanGetElectrocuted() const
{
    if (gDisableElectrocution || GameInfoManager::Instance()->IsRule0x4Equal2())
        return false;
    if (IsInFallAction())
        return false;
    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 24))
        return false;
    if (IsYoshiSuperPowerActive())
        return false;
    if (GameInfoManager::Instance()->GetStadium() == 11 && m_eActionState == 35)
        return false;
    if (GameInfoManager::Instance()->GetStadium() == 11)
    {
        float x = (float)fabs(mUnidentified024.m_v3Position.x);
        if (x < cField::GetGoalLineX(1U) - 1.0f)
            return false;
    }
    return true;
}

bool cFielder::CanGetElectrocuted(
    const CollisionPlayerWallData* eventData)
{
    if (!CanGetElectrocuted())
    {
        return false;
    }

    switch (m_eActionState)
    {
    case (eFielderActionState)0:
    case (eFielderActionState)5:
    case (eFielderActionState)6:
    case (eFielderActionState)23:
    case (eFielderActionState)25:
    case (eFielderActionState)26:
    case (eFielderActionState)27:
    case (eFielderActionState)31:
    case (eFielderActionState)34:
    case (eFielderActionState)35:
    {
        if (IsFallenDown())
        {
            float netPostRadius = cNet::GetPostRadius();
            float netWidth = m_pTeam->m_pNet->GetNetWidth();
            float minYElectrocutionPosition
                = netWidth / 2.0f + netPostRadius;
            float netHeight = m_pTeam->m_pNet->GetNetHeight();
            nlVector3 jointPos
                = GetJointPosition(m_nBip01JointIndex_0xA4);
            if ((float)fabs(eventData->contactPoint.y)
                    > minYElectrocutionPosition
                || (float)fabs(jointPos.z) > netHeight)
            {
                return true;
            }
        }
        break;
    }
    default:
    {
        float netPostRadius = cNet::GetPostRadius();
        float netWidth = m_pTeam->m_pNet->GetNetWidth();
        float minYElectrocutionPosition
            = netWidth / 2.0f + netPostRadius;
        float netHeight = m_pTeam->m_pNet->GetNetHeight();
        nlVector3 jointPos
            = GetJointPosition(m_nBip01JointIndex_0xA4);
        if ((float)fabs(eventData->contactPoint.y)
                > minYElectrocutionPosition
            || (float)fabs(jointPos.z) > netHeight)
        {
            bool bUnidentified = false;
            if (mUnidentified024.m_eCharacterClass == WALUIGI
                && IsConcurrentStateActive(
                    m_pAIContext->mScriptMachine, 0x17))
            {
                bUnidentified = true;
            }

            if (bUnidentified && m_bSuperPowerTankOn)
            {
                DeliverWaluigiWallEndEvent(g_pGame, this);
                TurnOffSuperPowerTank(true);
                return false;
            }

            if ((mUnidentified024.m_eCharacterClass == WALUIGI
                    || mUnidentified024.m_eCharacterClass == DAISY
                    || mUnidentified024.m_eCharacterClass == (eCharacterClass)0x11)
                && m_eActionState == (eFielderActionState)1)
            {
                if (m_pCurrentAnimController->m_fTime > 0.7f
                    || mUnidentified024.m_v3Position.z > 1.0f)
                {
                    return true;
                }
            }

            if (gEnableStandingWallElectrocution != 0
                || GameInfoManager::Instance()->IsRule0x4Equal3())
            {
                if (m_eActionState != (eFielderActionState)2)
                {
                    return true;
                }
            }
        }
        else
        {
            return false;
        }
        break;
    }
    }

    return false;
}

bool cFielder::CanDoCaptainShootToScore()
{
    if (GameInfoManager::Instance()->IsRule0x8Equal4())
    {
        return false;
    }

    if (g_pBall->GetOwnerFielder() != 0)
    {
        bool homeMegastrikeEnabled = false;
        if (GameInfoManager::Instance()
                ->GetCurrentSettings()
                ->mHomeMegastrikeEnabled
            && m_pTeam->m_nSide == HOME)
        {
            homeMegastrikeEnabled = true;
        }

        bool megastrikeEnabled = false;
        if (homeMegastrikeEnabled
            || (GameInfoManager::Instance()
                    ->GetCurrentSettings()
                    ->mAwayMegastrikeEnabled
                && m_pTeam->m_nSide == AWAY))
        {
            megastrikeEnabled = true;
        }

        bool isCaptain = IsCaptain();
        if (megastrikeEnabled && isCaptain)
        {
            if (CanShootFromPosition(this, false))
            {
                return true;
            }
        }
    }

    return false;
}

bool cFielder::CanContactLooseBall(bool requireBestInterceptor)
{
    nlVector3 v3BallPos;
    nlVector3 v3ContactPos;
    if (mfAirInterceptHeight[0] < 0.0f)
    {
        const LooseBallContactAnimInfo* pLeadAnimInfo = GetOneTimerLeadGroundContactAnims();
        float fAnimContactFrame = pLeadAnimInfo->fAnimContactFrame;
        const cSAnim* pLeadAnim = m_pAnimInventory->GetAnim(pLeadAnimInfo->nAnimID);
        GetJointPositionFuture(&v3ContactPos, pLeadAnimInfo->nAnimID, m_nBallJointIndex,
            GetNormalizedContactTime(pLeadAnim, fAnimContactFrame), true, true, false, true);
        mfAirInterceptHeight[0] = v3ContactPos.z;
    }

    float fInterceptHeight = mfAirInterceptHeight[0] * GetPlayerScale();
    if (g_pBall->m_v3Position.z > 3.0f * fInterceptHeight)
    {
        return false;
    }

    if (g_pBall->m_pOwner == 0 && g_pBall->m_tNoPickupTimer.m_uPackedTime == 0)
    {
        bool bPassInFlight = false;
        if ((g_pBall->meBallState == 5 || g_pBall->meBallState == 3) && g_pBall->m_pPassTarget != 0)
        {
            bPassInFlight = true;
        }

        if (!(bPassInFlight && requireBestInterceptor) && !fn_80014E20(g_pBall))
        {
            bool bHasGlobalPad = GetGlobalPad() != 0;
            if (bHasGlobalPad && GetDesireState() >= 0x14)
            {
                return true;
            }

            float fGroundContactFrame = GetOneTimerIdleGroundContactAnims()[0].fAnimContactFrame;
            const cSAnim* pGroundAnim = GetAnimInventory()->GetAnim(GetOneTimerIdleGroundContactAnims()[0].nAnimID);
            float fGroundContactTime = GetNormalizedContactTime(pGroundAnim, fGroundContactFrame);
            const cSAnim* pVolleyAnim = GetAnimInventory()->GetAnim(GetOneTimerIdleVolleyContactAnims()[0].nAnimID);
            float fVolleyContactTime = GetNormalizedContactTime(pVolleyAnim, GetOneTimerIdleVolleyContactAnims()[0].fAnimContactFrame);

            for (float fTime = 0.0f; fTime < fGroundContactTime; fTime += FixedUpdateTask::GetPhysicsUpdateTick())
            {
                fn_800180F4(g_pBall, &v3BallPos, fTime);
                u16 aMoveDirection = mUnidentified024.m_aActualMovementDirection;
                float fDeltaX = v3BallPos.x - mUnidentified024.m_v3Position.x;
                float fDeltaY = v3BallPos.y - mUnidentified024.m_v3Position.y;
                u16 aBallDirection = RadToAng16(nlATan2f(fDeltaY, fDeltaX));
                u16 aDelta = abs_s16((s16)(aBallDirection - aMoveDirection));
                float fReachScale = InterpolateRangeClamped(1.25f, 3.0f, 32768.0f, 0.0f, aDelta);
                nlVector2 v2Delta;
                v2Delta.x = v3BallPos.x - mUnidentified024.m_v3Position.x;
                v2Delta.y = v3BallPos.y - mUnidentified024.m_v3Position.y;
                if (nlVec2Length(v2Delta) <= fReachScale * mUnidentified024.m_fPlayerScale)
                {
                    if (requireBestInterceptor)
                    {
                        if (m_pTeam->GetGoalie()->fn_8009670C(&v3BallPos, true) == this)
                        {
                            return true;
                        }
                    }
                    else
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool cFielder::CanReceivePass()
{
    return IsAvailableToReceivePass();
}

void cFielder::SetDesiredFacingDirection(unsigned short aParam, bool bParam)
{
    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1E))
    {
        DesireConfused* pAction = (DesireConfused*)
            GetScriptMachineState(m_pAIContext->mScriptMachine, 0x1E);
        if (pAction == 0)
        {
            pAction = (DesireConfused*)GetConcurrentState(
                m_pAIContext->mScriptMachine, 0x1E);
        }
        pAction->AdjustInputDirection(&aParam);
    }

    cCharacter::SetDesiredFacingDirection(aParam, bParam);
}

void cFielder::SetNormalTweaks()
{
    m_pTweaks = m_pNormalTweaks;
    if (m_eActionState == ACTION_RUNNING)
        SetRunningAnimState(0.1f);
    else if (m_eActionState == ACTION_RUNNING_WB)
        SetRunningWBAnimState(0.1f);
}

void cFielder::SetSuperPowerTweaks()
{
    m_pTweaks = m_pSuperPowerTweaks;
    if (m_eActionState == ACTION_RUNNING)
        SetRunningAnimState(0.1f);
    else if (m_eActionState == ACTION_RUNNING_WB)
        SetRunningWBAnimState(0.1f);
}

void cFielder::SetThingsToAvoid(int nParam)
{
    DesireSteering* pAction = (DesireSteering*)
        GetScriptMachineState(m_pAIContext->mScriptMachine, 0x22);
    if (pAction == 0)
    {
        pAction = (DesireSteering*)GetConcurrentState(
            m_pAIContext->mScriptMachine, 0x22);
    }
    pAction->m_ThingsToAvoid = nParam;
}

void cFielder::SetAvoidanceMultiplier(float fParam)
{
    DesireSteering* pAction = (DesireSteering*)
        GetScriptMachineState(m_pAIContext->mScriptMachine, 0x22);
    if (pAction == 0)
    {
        pAction = (DesireSteering*)GetConcurrentState(
            m_pAIContext->mScriptMachine, 0x22);
    }
    pAction->m_fAvoidanceMult = fParam;
}

void cFielder::SetTweaks(PlayerTweaks* pParam)
{
    if (pParam != 0)
    {
        m_pTweaks = pParam;
    }
    else
    {
        if (m_pTweaks != 0)
        {
            delete m_pTweaks;
        }
        m_pTweaks = m_pNormalTweaks;
    }
}

void cFielder::AddMark(cFielder* pParam)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_pMark[i] == 0)
        {
            m_pMark[i] = pParam;
            return;
        }
    }
}

void cFielder::ClearMarks()
{
    for (int i = 0; i < 4; i++)
    {
        m_pMark[i] = 0;
    }
}

bool cFielder::IsMarking(cFielder* pParam)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_pMark[i] == pParam)
        {
            return true;
        }
    }
    return false;
}

float GetFielderHitReach(cFielder* pFielder)
{
    if (pFielder->m_fHitDistance < 0.0001f)
    {
        cSAnim* pAnim = pFielder->m_pAnimInventory->GetAnim(0x67);
        nlVector3 v3Start;
        nlVector3 v3End;
        if (pFielder->mUnidentified024.m_eCharacterClass != YOSHI)
        {
            pAnim->GetRootTrans(GetNormalizedContactTime(pAnim, fn_8002D020(pFielder->m_pTweaks)), &v3Start);
            pAnim->GetRootTrans(GetNormalizedContactTime(pAnim, fn_8002D050(pFielder->m_pTweaks)), &v3End);
        }
        else
        {
            int nJointIndex = pFielder->m_pPoseAccumulator->GetBaseHierarchy()->GetNodeIndexByID(
                nlStringLowerHash("bip01 Ponytail12"));
            pFielder->GetJointPositionFuture(&v3Start, 0x67, nJointIndex, 2.0f / pAnim->GetNumFrames(),
                true, true, true, true);
            pFielder->GetJointPositionFuture(&v3End, 0x67, nJointIndex, 6.0f / pAnim->GetNumFrames(),
                true, true, true, true);
        }
        nlVector3 v3Delta;
        nlVec3Set(v3Delta, v3End.x - v3Start.x, v3End.y - v3Start.y, v3End.z - v3Start.z);
        pFielder->m_fHitDistance = nlVec3Length(v3Delta);
    }
    float fScale = pFielder->mUnidentified024.m_fMovementScale;
    float fReach = pFielder->m_fHitDistance * fScale;
    fReach += fn_8002BFA8(pFielder->m_pTweaks, pFielder->GetPlayerScale());
    return fReach;
}

float cFielder::GetHeadTrackingFinalPhaseTime()
{
    return 0.75f;
}

void cFielder::CollideWithCharacterCallback(CollisionPlayerPlayerData* pData)
{
    cPlayer* pPlayerCollidedWith = pData->player2;
    if (pPlayerCollidedWith->m_eClassType != FIELDER)
        return;

    cFielder* pFielderCollidedWith = (cFielder*)pPlayerCollidedWith;
    TestCollisionForInvincibility(pFielderCollidedWith);

    if (!IsOnSameTeam(pFielderCollidedWith))
    {
        if (IsFallenDown())
            return;

        u8 gotHit = IsHittingForCollision(pFielderCollidedWith);
        if (gotHit)
        {
            u8 hitteeIsHitter = 1;
            u8 bAlsoHitting = IsHittingForCollision(this);
            if (bAlsoHitting)
            {
                if (pFielderCollidedWith->GetTweaks()->GetDefenseSize()
                    < GetTweaks()->GetDefenseSize())
                {
                    hitteeIsHitter = 0;
                }
                else if (pFielderCollidedWith->GetTweaks()->GetDefenseSize()
                    > GetTweaks()->GetDefenseSize())
                {
                    hitteeIsHitter = 1;
                }
                else
                {
                    float fNumKeys = pFielderCollidedWith->m_pCurrentAnimController
                        ->m_pSAnim->m_nNumKeys;
                    float fHitTime = fn_8002D038(pFielderCollidedWith->GetTweaks())
                        / fNumKeys;
                    float fMyHitTime = fabsf(m_pCurrentAnimController->get_fTime() - fHitTime);
                    float fOtherHitTime = fabsf(pFielderCollidedWith->m_pCurrentAnimController->get_fTime() - fHitTime);
                    if (fMyHitTime <= fOtherHitTime)
                        hitteeIsHitter = 0;
                }
            }

            if (!hitteeIsHitter)
                return;

            float thisRadius, otherRadius;
            m_pPhysicsCharacter->m_pPlayerPlayerColumn->GetRadius(&thisRadius);
            pFielderCollidedWith->m_pPhysicsCharacter->m_pPlayerPlayerColumn->GetRadius(&otherRadius);
            float combinedRadius = thisRadius + otherRadius;
            if (fabsf(mUnidentified024.m_v3Position.x) > thisRadius + cField::GetGoalLineX(1U))
                return;

            float sinVal, cosVal;
            nlSinCos(&sinVal, &cosVal, pFielderCollidedWith->mUnidentified024.m_aActualFacingDirection);
            nlVector3 adjustedPosition;
            adjustedPosition = pFielderCollidedWith->mUnidentified024.m_v3Position;
            adjustedPosition.x += cosVal * combinedRadius;
            adjustedPosition.y += sinVal * combinedRadius;

            float attackIntensity = GetCollisionHitIntensity(this, adjustedPosition,
                pData->velocity1, pFielderCollidedWith);
            int nUnidentified = GetFielderHitReaction(this, pFielderCollidedWith, attackIntensity);
            bool doFrameLock = false;
            if (m_pBall != 0 && (attackIntensity >= gHitFrameLockMinIntensity || IsSuperGrowActive()))
            {
                if (gHitFrameLockRequiresHeavyReaction)
                {
                    if (nUnidentified == 2 || IsSuperGrowActive())
                        doFrameLock = true;
                }
                else
                {
                    doFrameLock = true;
                }
            }

            if (doFrameLock)
            {
                PlaySound(0, 0xE89BA529, 0, 0);
            }
            else
            {
                switch (nUnidentified)
                {
                case 0:
                    PlaySound(0, 0x057208DA, 0, 0);
                    break;
                case 1:
                    PlaySound(0, 0xECE94BBB, 0, 0);
                    break;
                case 2:
                    PlaySound(0, 0xE8120AC5, 0, 0);
                    break;
                }
            }

            fn_80047240(pFielderCollidedWith,
                pFielderCollidedWith->mUnidentified024.m_aActualFacingDirection,
                nUnidentified, doFrameLock, true);
            PlayerAttackData* pAttackData = g_PlayerAttackDataPool.Allocate();
            pAttackData->pAttacker = pFielderCollidedWith;
            u8 bHasGlobalPad = pFielderCollidedWith->GetGlobalPad() != 0;
            pAttackData->nAttackerPadID = bHasGlobalPad
                ? pFielderCollidedWith->GetGlobalPad()->GetPadID() : -1;
            pAttackData->pTarget = this;
            pAttackData->mUnidentified0C = nUnidentified;
            pAttackData->mUnidentified10 = false;
            QueueAttackSuccessEvent(g_pGame, pAttackData);
            PlayRumbleAction(2, pFielderCollidedWith->GetGlobalPad());
        }
        else if (pFielderCollidedWith->IsSlideAttacking() && m_eActionState != ACTION_HIT)
        {
            s16 nHitteeToHitterFacingDelta = pFielderCollidedWith->GetFacingDeltaToPosition(mUnidentified024.m_v3Position);
            s16 nHitterToHitteeFacingDelta = GetFacingDeltaToPosition(pFielderCollidedWith->mUnidentified024.m_v3Position);
            u8 isThisSlideAttacking = IsSlideAttacking();
            if (isThisSlideAttacking)
            {
                if (GetTweaks()->GetDefenseSize()
                    < pFielderCollidedWith->GetTweaks()->GetDefenseSize())
                {
                    ResolveSlideAttack(pFielderCollidedWith, this);
                }
                else if (GetTweaks()->GetDefenseSize()
                    > pFielderCollidedWith->GetTweaks()->GetDefenseSize())
                {
                    ResolveSlideAttack(this, pFielderCollidedWith);
                }
                else if (GetActualSpeed() < pFielderCollidedWith->GetActualSpeed())
                {
                    ResolveSlideAttack(pFielderCollidedWith, this);
                }
                else
                {
                    ResolveSlideAttack(this, pFielderCollidedWith);
                }
            }
            else
            {
                ResolveSlideAttack(pFielderCollidedWith, this);
            }
        }
        else if (GetActionState() == ACTION_LOOSE_BALL_PASS
            || GetActionState() == ACTION_LOOSE_BALL_SHOT)
        {
            nlVector3 v3Position = mUnidentified024.m_v3Position;
            float otherRadius, thisRadius;
            pFielderCollidedWith->m_pPhysicsCharacter->GetRadius(&otherRadius);
            m_pPhysicsCharacter->GetRadius(&thisRadius);
            nlVector2 v2Delta = {
                pFielderCollidedWith->mUnidentified024.m_v3Position.x - v3Position.x,
                pFielderCollidedWith->mUnidentified024.m_v3Position.y - v3Position.y,
            };
            float fOverlap = thisRadius + otherRadius - nlVec2Length(v2Delta);
            if (!(fOverlap > 0.0f))
                return;

            nlVector3 v3BallDelta;
            nlVec3Sub(v3BallDelta, g_pBall->m_v3Position, mUnidentified024.m_v3Position);
            v3BallDelta.z = 0.0f;
            nlVector3 v3PlayerDelta;
            nlVec3Sub(v3PlayerDelta, pFielderCollidedWith->mUnidentified024.m_v3Position, mUnidentified024.m_v3Position);
            v3PlayerDelta.z = 0.0f;
            nlVector3 v3Projection;
            nlVec3Project(v3Projection, v3PlayerDelta, v3BallDelta);
            nlVector3 v3Direction;
            nlVec3Sub(v3Direction, v3PlayerDelta, v3Projection);
            if (nlVec3LengthSquared(v3Direction) < 0.001f)
            {
                v3Direction.x = v3BallDelta.y;
                v3Direction.y = -v3BallDelta.x;
            }
            nlVec3Normalize(v3Direction, v3Direction);
            nlVec3ScaleAdd(v3Direction, fOverlap, v3Direction,
                pFielderCollidedWith->mUnidentified024.m_v3Position);
            pFielderCollidedWith->SetPosition(v3Direction);
        }
    }
    else if (pFielderCollidedWith->IsSlideAttacking()
        && !pFielderCollidedWith->IsFallenDown()
        && !pFielderCollidedWith->IsSuperGrowActive())
    {
        if (!pFielderCollidedWith->IsInvincibleChars())
            pFielderCollidedWith->fn_8004D238();
    }
}

bool cFielder::CollideWithShellCallback(ePowerupSize size, bool largeSound, const nlVector3& position, const nlVector3& velocity)
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0
        || (!IsFallenDown() && m_eActionState != ACTION_POST_WHISTLE && !IsInvinciblePowerups()))
    {
        if (!IsCharacterInAir(0.5f) && size == POWERUPSIZE_LARGE && !IsSuperGrowActive())
            InitActionKnockdownReact(velocity);
        else
        {
            InitActionShellReact(position, velocity);
            ePowerupSize soundSize = size;
            if (largeSound)
                soundSize = POWERUPSIZE_LARGE;
            switch (soundSize)
            {
            case POWERUPSIZE_SMALL:
                PlayAttackReactionSounds(gGameTweaks.m_pGameTweaks->fSmallShellHitReactionVolume);
                break;
            case POWERUPSIZE_MEDIUM:
                PlayAttackReactionSounds(gGameTweaks.m_pGameTweaks->fMediumShellHitReactionVolume);
                break;
            case POWERUPSIZE_LARGE:
                PlayAttackReactionSounds(gGameTweaks.m_pGameTweaks->fBombHitReactionVolume);
                break;
            }
        }
        return true;
    }
    return false;
}

#include "Game/AI/FuzzyRuntimeCall.h"
#include "Game/AI/FuzzyVariantCollection.inl"

void TryFielderQueuedPass(cFielder* pFielder)
{
    if (pFielder->m_pBall != 0 && pFielder->mUnidentified1E4.m_eLastPadAction == 0x1B)
    {
        cPlayer* pPassTarget;
        if (pFielder->GetGlobalPad() == 0)
        {
            UnidentifiedVariant_80054AB8 vBestTarget = CallFielderFuzzyFunction(
                FuzzyAIGetFielderRuntime(pFielder), "BestPassTarget", pFielder);
            pPassTarget = vBestTarget.GetPlayer();
        }
        else
        {
            pPassTarget = fn_80096F54(pFielder,
                pFielder->GetGlobalPad() != 0 ? pFielder->GetGlobalPad()->IsPressed(0x17, true) : false);
        }

        if (pPassTarget != 0)
        {
            pFielder->DoRegularPassing(pPassTarget, pFielder->bIsModified, true, false, false,
                GetSlowestVolleyPassSpeed(pFielder->GetTweaks()),
                GetFastestVolleyPassSpeed(pFielder->GetTweaks()));
        }
        pFielder->mUnidentified1E4.m_eLastPadAction = 0x32;
    }
}

void SetFielderFrozenState(cFielder* pFielder, int nFrozenState, float fFrozenTime)
{
    bool bHasEgg = false;
    if (pFielder->mUnidentified024.m_eCharacterClass == YOSHI
        && IsConcurrentStateActive(pFielder->m_pAIContext->mScriptMachine, 0x17))
    {
        bHasEgg = true;
    }

    if (bHasEgg)
    {
        gNPCManager->mpYoshiEgg->Suspend(false, fFrozenTime);
    }
    else if (pFielder->m_pBall != 0)
    {
        pFielder->ReleaseBall(0);
        if ((pFielder->mUnidentified024.m_eCharacterClass == (eCharacterClass)0xE
                || pFielder->mUnidentified024.m_eCharacterClass == BIRDO)
            && pFielder->m_eActionState == (eFielderActionState)0x15)
        {
            if (gNPCManager->mpKoopaShell != 0 && gNPCManager->mpKoopaShell->mVisible)
            {
                gNPCManager->mpKoopaShell->Deactivate(false);
            }
            if (gNPCManager->mpBirdoEgg != 0 && gNPCManager->mpBirdoEgg->mVisible)
            {
                gNPCManager->mpBirdoEgg->Hide(false);
            }
        }

        if (pFielder->mUnidentified024.m_eCharacterClass == BOWSER
            && pFielder->m_eActionState == (eFielderActionState)1)
        {
            nlVector3 v3WarpPos = pFielder->mUnidentified024.m_v3Position;
            v3WarpPos.z = 0.18f;
            nlVector3 v3Offset;
            u16 aFacing = pFielder->mUnidentified024.m_aActualFacingDirection;
            nlPolarToCartesian(v3Offset.x, v3Offset.y, aFacing,
                fn_8002BFA8(pFielder->m_pTweaks, pFielder->GetPlayerScale()));
            v3Offset.z = 0.18f;
            nlVec3Add(v3WarpPos, v3WarpPos, v3Offset);
            g_pBall->WarpTo(v3WarpPos);
        }

        if (nFrozenState != 2)
        {
            nlVector3 v3ReleaseVelocity;
            nlPolarToCartesian(v3ReleaseVelocity.x, v3ReleaseVelocity.y,
                pFielder->mUnidentified024.m_aActualFacingDirection,
                2.0f + pFielder->GetActualSpeed());
            v3ReleaseVelocity.z = 0.5f;
            g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
        }
    }

    ((DesireFrozen*)GetConcurrentState(pFielder->m_pAIContext->mScriptMachine, 0x1D))
        ->Activate(fFrozenTime, nFrozenState);
}

bool cFielder::FreezeWithPeachPhoto(float duration)
{
    bool tangible = CanBeAffectedByPhoto();
    if (GetCharacterClass() == (eCharacterClass)18)
    {
        if (GetJointPosition(m_nHeadJointIndex).z < 0.0f || m_eActionState == 34)
            return false;
    }
    bool yoshiActive;
    GetCharacterSpecialActive(this, (eCharacterClass)8, yoshiActive);
    if (yoshiActive)
    {
        if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 23))
            RequestStateMachineDeactivation(GetFielderDesire(this, 23));
        return false;
    }
    if (IsMarioSuperPowerActive() || IsLuigiSuperPowerActive() || IsDKSuperPowerActive() || IsDaisySuperPowerActive())
        return false;
    if (tangible)
    {
        bool controlled = GetGlobalPad() != 0;
        if (controlled == true)
            SwapController(false);
        SetFielderFrozenState(this, 2, duration);
        return true;
    }
    return false;
}

bool cFielder::CollideWithFreezeCallback()
{
    if (m_eActionState != ACTION_POST_WHISTLE && !IsInvinciblePowerups()
        && mbTangible && !IsFielderDazed(this) && !IsFrozen() && CanReactToGroundEffects())
    {
        SetFielderFrozenState(this, 1, gGameTweaks.m_pGameTweaks->fFreezeShellFrozenTime);
        return true;
    }
    return false;
}

bool cFielder::CollideWithBananaCallback(const nlVector3& position)
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0
        || (!IsStuck() && !IsFallenDown() && m_eActionState != ACTION_POST_WHISTLE && !IsInvinciblePowerups()))
    {
        InitActionBananaReact(position);
        return true;
    }
    return false;
}

bool cFielder::CollideWithBobombCallback(const nlVector3& position, float radius)
{
    if (!IsInvinciblePowerups() && mbTangible && CanReactToGroundEffects())
    {
        if (g_pGame->IsGameplayOrOvertime())
        {
            AddRandomDirt();
            fn_8001F1C0(2);
        }
        InitActionBombReact(position, radius);
        return true;
    }
    return false;
}

void cFielder::CollideWithShockwaveCallback(const nlVector3& position)
{
    if (m_eActionState != ACTION_POST_WHISTLE && !IsInvincible() && !IsInvincibleHammers()
        && mbTangible && CanReactToGroundEffects())
    {
        AddRandomDirt();
        if (g_pBall->m_pOwner == this)
        {
            ReleaseBall(0);
            nlVector3 velocity;
            nlPolarToCartesian(velocity.x, velocity.y, mUnidentified024.m_aActualFacingDirection, 2.0f + GetActualSpeed());
            velocity.z = 0.5f;
            g_pBall->ShootRelease(velocity, SPINTYPE_NONE);
        }
        InitActionBombHitReact(position);
        PlayRumbleAction(3, GetGlobalPad());
    }
}

void cFielder::CollideWithChainCallback(ChainChomp* chain)
{
    if (!IsFallenDown() || mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
    {
        unsigned short direction = nlATan2Angle(mUnidentified024.m_v3Position.y - chain->mv3Position.y,
            mUnidentified024.m_v3Position.x - chain->mv3Position.x);
        if (m_pBall != 0)
        {
            ReleaseBall(0);
            nlVector3 velocity;
            nlPolarToCartesian(velocity.x, velocity.y, direction, 2.0f + GetActualSpeed());
            velocity.z = 0.5f;
            g_pBall->ShootRelease(velocity, SPINTYPE_NONE);
        }
        if (chain->IsFrozen())
            InitActionShellReact(chain->mv3Position, chain->mv3Velocity);
        else if (chain->meChainChompState != CHAIN_STATE_RECOVER)
            fn_80047240(chain->mpThrower, direction, 2, false, false);
        else
            InitActionKnockdownReact(chain->mv3Velocity);
        if (chain->mpThrower != 0 && g_pGame->IsGameplayOrOvertime() && !IsOnSameTeam(chain->mpThrower))
            StatsTracker::Instance()->TrackStat((ePlayerStats)0x1E, m_pTeam->m_nSide,
                mUnidentified1E4.m_ID, chain->mnThrowerPadID, 0, 0, 0);
    }
}

void cFielder::CollideWithWindDebrisCallback(WindDebris* debris)
{
    if (!IsInvincible() && !IsShattered() && mbTangible && m_eActionState != 0 && m_eActionState != 35)
    {
        EndFrozenOrDazed();
        nlVector3 debrisVelocity = debris->mv3Velocity;
        nlVector3 velocity;
        velocity.x = debrisVelocity.x * gWindDebrisKnockbackScale;
        velocity.y = debrisVelocity.y * gWindDebrisKnockbackScale;
        velocity.z = gWindDebrisKnockbackZ + nlRandomf(gWindDebrisKnockbackZRange);
        float goalLine = cField::GetGoalLineX(1U);
        if (mUnidentified024.m_v3Position.x > goalLine || mUnidentified024.m_v3Position.x < -1.0f * goalLine)
        {
            velocity.z = 0.0f;
            velocity.x = 2.0f * nlSqrt(nlVec3LengthSquared(velocity), true);
            if (mUnidentified024.m_v3Position.x > goalLine)
                velocity.x *= -1.0f;
            velocity.y = 0.0f;
            velocity.z = 8.0f;
        }
        fn_80044148(velocity);
        PlaySound(11, debris->mUnidentified08C, 0, 0);
        PlayRumbleAction(3, GetGlobalPad());
        EmitTackleImpact(this);
    }
}

void cFielder::CollideWithThwompCallback(CollisionThwompPlayerData* event)
{
    if (event == 0 || event->thwomp == 0)
        return;
    if (!IsInvincible() && mbTangible)
    {
        if (event->state == THWOMP_STATE_FALLING || (IsCharacterSuperPowerActive(this, WALUIGI) && m_bSuperPowerTankOn))
        {
            if (g_pBall->m_pOwner == this)
            {
                ReleaseBall(0);
                nlVector3 velocity;
                nlPolarToCartesian(velocity.x, velocity.y, mUnidentified024.m_aActualFacingDirection, gThwompBallReleaseSpeed);
                velocity.z = gThwompBallReleaseSpeed;
                g_pBall->SetVelocity(velocity, SPINTYPE_NONE, 0);
            }
            const nlVector3* thwompPosition = event->thwomp->GetPosition();
            nlVector3 direction;
            nlVec3Set(direction, thwompPosition->x - mUnidentified024.m_v3Position.x,
                thwompPosition->y - mUnidentified024.m_v3Position.y, 0.0f);
            nlVec3Scale(direction, direction, nlRecipSqrt(nlVec3LengthSquared(direction), false));
            if (IsCharacterSuperPowerActive(this, WALUIGI) && m_bSuperPowerTankOn)
            {
                if (!IsFallenDown())
                    InitActionShellReact(*event->thwomp->GetPosition(), v3Zero);
            }
            else
                InitActionKnockdownReact(direction);
            PlayRumbleAction(3, GetGlobalPad());
        }
    }
}

void cFielder::CollideWithWallCallback(
    const CollisionPlayerWallData* eventData)
{
    cPlayer::CollideWithWallCallback(eventData);

    DesireFrozen* pAction = (DesireFrozen*)
        GetConcurrentState(m_pAIContext->mScriptMachine, 0x1D);
    bool bActionActive = false;
    if (pAction != 0 && pAction->mActive
        && pAction->meFrozenState != DesireFrozen::FROZEN_NONE)
    {
        bActionActive = true;
    }

    bool bShellReact;
    if (!bActionActive
        && m_eActionState == (eFielderActionState)0x16)
    {
        bShellReact = true;
    }
    else
    {
        bShellReact = false;
    }
    if (bShellReact)
    {
        s16 facingDelta
            = (s16)GetFacingDeltaToPosition(eventData->contactPoint);
        int absFacingDelta;
        if (facingDelta < 0)
        {
            absFacingDelta = -facingDelta;
        }
        else
        {
            absFacingDelta = facingDelta;
        }

        if ((u16)absFacingDelta < 0x2000)
        {
            fn_8004D238();
        }
    }

    if (CanGetElectrocuted(eventData))
    {
        InitActionElectrocution(
            eventData->contactPoint, eventData->wallNormal, true);

        int stadium = GameInfoManager::Instance()->GetStadium();
        if (GetStadiumUnknown0x10(stadium))
        {
            unsigned long soundID = 0xCE269987;
            if (m_pTeam->m_nSide == 0)
            {
                soundID = 0x5089F33E;
            }
            PlayCrowdReaction(soundID);
        }
    }
    else if (m_eActionState != (eFielderActionState)3
             && GameInfoManager::Instance()->GetStadium() == 0x0B)
    {
        float distance = (float)fabs(mUnidentified024.m_v3Position.y);
        distance -= fn_8002BFA8(m_pTweaks, 1.0f);
        if (distance > cField::GetSidelineY(1) + 0.5f)
        {
            fn_80046244();
        }
    }
}

void cFielder::CollideWithPatchCallback(const UnidentifiedEventData24* eventData)
{
    int type = eventData->mUnidentified10->m_Type;
    if (type == 1)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && mUnidentified1E4.m_tFireTimer.m_uPackedTime == 0
            && m_eActionState != ACTION_ELECTROCUTION
            && !IsStuck() && !IsInvincible())
        {
            if (m_eActionState == ACTION_UNKNOWN_34)
            {
                fn_80097358(this, gFirePatchBurnDuration);
                return;
            }
            if (m_pBall != 0)
            {
                ReleaseBall(0);
                ShootBallDueToContact(eventData->mUnidentified10->m_Velocity);
            }
            fn_8004E11C(gFirePatchBurnDuration);
            PlayRumbleAction(2, GetGlobalPad());
        }
    }
    else if (type == 0)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !IsStuck() && !IsInvincible())
        {
            UnidentifiedVariantCollection params;
            params.Set(7, FuzzyVariant(gGasConfusionDuration));
            ActivateConcurrentState(m_pAIContext->mScriptMachine, 0x1E, &params,
                IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1E));
        }
    }
    else if (type == 2)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !IsInFallAction() && GetActionState() != ACTION_UNKNOWN_34
            && !IsInvincible())
        {
            fn_80045AEC(eventData->mUnidentified10);
        }
        else if (eventData->mUnidentified10->m_pTarget != 0
            && eventData->mUnidentified10->m_pTarget == this)
        {
            eventData->mUnidentified10->fn_80173AF4();
        }
    }
    else if (type == 5 || type == 4 || type == 11)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !IsInFallAction() && !IsInvincible())
        {
            PhysicsPatchInfo* info = GetPhysicsPatchInfo(type);
            UnidentifiedVariantCollection params;
            if (type == 5)
            {
                params.Set(0, FuzzyVariant(info->mFriction));
                params.Set(1, FuzzyVariant(gYoshiYolkGooDuration));
                params.Set(2, FuzzyVariant(gYoshiYolkAnimSpeedScale));
                params.Set(3, FuzzyVariant(gYoshiYolkMovementScale));
            }
            else if (type == 4)
            {
                params.Set(0, FuzzyVariant(info->mFriction));
                params.Set(1, FuzzyVariant(gMuckHoleGooDuration));
                params.Set(2, FuzzyVariant(gMuckHoleAnimSpeedScale));
                params.Set(3, FuzzyVariant(gMuckHoleMovementScale));
            }
            else if (type == 11)
            {
                params.Set(0, FuzzyVariant(gSandRunningSpeedScale));
                params.Set(1, FuzzyVariant(gSandGooDuration));
                params.Set(2, FuzzyVariant(gSandAnimSpeedScale));
                params.Set(3, FuzzyVariant(gSandMovementScale));
            }
            if (type == 4)
            {
                AddRandomDirt();
                fn_8001F1C0(1);
            }
            ActivateConcurrentState(m_pAIContext->mScriptMachine, 0x1B, &params,
                IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1B));
        }
    }
    else if (type == 3)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !IsInFallAction() && !IsInvincible())
        {
            AddRandomDirt();
            fn_8001F1C0(1);
            nlVector3 v3Unidentified = eventData->mUnidentified10->m_Velocity;
            v3Unidentified.z = 0.0f;
            if (nlVec3LengthSquared(v3Unidentified) == 0.0f)
            {
                nlVec3Set(v3Unidentified,
                    mUnidentified024.m_v3Position.x - eventData->mUnidentified10->m_pOwner->mUnidentified024.m_v3Position.x,
                    mUnidentified024.m_v3Position.y - eventData->mUnidentified10->m_pOwner->mUnidentified024.m_v3Position.y,
                    0.0f);
            }
            nlVec3Scale(v3Unidentified,
                nlRecipSqrt(nlVec3LengthSquared(v3Unidentified), false));
            nlPolar polar;
            nlCartesianToPolar(polar, v3Unidentified);
            fn_80047240(eventData->mUnidentified10->m_pOwner,
                polar.a, 1, false, false);
        }
    }
    else if (type == 7)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !IsInFallAction() && !IsInvincible())
        {
            cPlayer* pOwner = eventData->mUnidentified10->m_pOwner;
            if (gShrinkerIgnoreTeammates && IsOnSameTeam(pOwner))
            {
                return;
            }
            fn_800470B4(this, eventData->mUnidentified10->m_pOwner);
            if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1C))
            {
                return;
            }
            UnidentifiedVariantCollection params;
            params.Set(14, FuzzyVariant(pOwner));
            ActivateConcurrentState(m_pAIContext->mScriptMachine, 0x1C, &params, false);
        }
    }
    else if (type == 6)
    {
        cFielder* pOwner = (cFielder*)eventData->mUnidentified10->m_pOwner;
        if (pOwner != this && !IsFallenDown())
        {
            if (IsSuperGrowActive() || IsInvincibleChars())
            {
                if (!pOwner->IsInvincibleChars())
                {
                    pOwner->fn_80047240(this,
                        pOwner->mUnidentified024.m_aActualFacingDirection + 0x8000, 1, false, false);
                    PlaySound(0, 0xECE94BBB, 0, 0);
                }
                else
                {
                    fn_80047240(pOwner, pOwner->mUnidentified024.m_aActualFacingDirection, 0, false, false);
                    PlaySound(pOwner->m_uSoundSlotId, 0x9E87FEBC, 0, 0);
                    PlayRumbleAction(2, pOwner->GetGlobalPad());
                }
            }
            else if (!IsOnSameTeam(pOwner))
            {
                fn_80047240(pOwner, pOwner->mUnidentified024.m_aActualFacingDirection, 1, false, true);
                PlaySound(pOwner->m_uSoundSlotId, 0x9E87FEBC, 0, 0);
                PlayRumbleAction(2, pOwner->GetGlobalPad());
            }
        }
    }
    else if (type == 12)
    {
        ActivateConcurrentState(m_pAIContext->mScriptMachine, 0x19, 0, true);
    }
    else if (type == 8 || type == 9)
    {
        if (!IsInFallAction() && !IsInvincible()
            && m_eActionState != (eFielderActionState)0x18
            && m_eActionState != (eFielderActionState)0x23)
        {
            PlayRumbleAction(3, GetGlobalPad());
            nlVector3 v3Unidentified = mUnidentified024.m_v3Velocity;
            v3Unidentified.z = 25.0f;
            fn_80044148(v3Unidentified);
            fn_80097358(this, 5.0f);
        }
    }
    else if (type == 10)
    {
        if (!IsInFallAction() && !IsStarActive()
            && !IsYoshiSuperPowerActive() && !IsInvincible()
            && !IsCharacterInAir(eventData->mUnidentified10->GetRadius()))
        {
            PlayRumbleAction(3, GetGlobalPad());
            nlVector3 v3Start;
            nlVec3ScaleAdd(v3Start, -100.0f,
                eventData->mUnidentified10->fn_80173CCC(),
                eventData->mUnidentified10->GetPosition());
            nlVector3 v3End;
            nlVec3ScaleAdd(v3End, 100.0f,
                eventData->mUnidentified10->fn_80173CCC(),
                eventData->mUnidentified10->GetPosition());
            nlVector3 v3Unidentified = GetClosestPointOnLineABFromPointC(
                v3Start, v3End, mUnidentified024.m_v3Position);
            nlVec3Set(v3Unidentified,
                mUnidentified024.m_v3Position.x - v3Unidentified.x,
                mUnidentified024.m_v3Position.y - v3Unidentified.y, 0.0f);
            nlVec3Scale(v3Unidentified,
                nlRecipSqrt(nlVec3LengthSquared(v3Unidentified), false));
            InitActionElectrocution(mUnidentified024.m_v3Position, v3Unidentified, false);
        }
    }
}

void cFielder::ClearPassTargetIfAmThePassTarget()
{
    cBall* pBall = g_pBall;
    if (pBall->UnidentifiedHasPassTarget())
    {
        if (pBall->m_pPassTarget == this)
        {
            if (pBall->m_pOwner != 0)
            {
                fn_80015C38(pBall, 2);
            }
            else
            {
                fn_80015C38(pBall, 0);
            }
        }
    }
}

bool cFielder::IsInFallAction() const
{
    switch (GetActionState())
    {
    case 3:
    case 0x18:
        return true;
    default:
        return false;
    }
}

bool IsFielderFrontInvincible(cFielder* pFielder, const nlVector3* position)
{
    bool bInvincibleFront = pFielder->IsInvincibleFront();

    s16 facingDelta = pFielder->GetFacingDeltaToPosition(*position);
    bool result = false;
    if (bInvincibleFront && (u16)(facingDelta < 0 ? -facingDelta : facingDelta) < 0x4000)
        result = true;
    return result;
}

bool cFielder::IsAboveFielder(cFielder* pOtherFielder) const
{
    if (pOtherFielder->mUnidentified024.m_eCharacterClass == BOWSER
        && pOtherFielder->m_eActionState == 1)
    {
        return IsCharacterInAir(pOtherFielder->mUnidentified024.m_fPlayerScale);
    }
    if (mUnidentified024.m_eCharacterClass == BOWSER && m_eActionState == 1)
        return false;
    if (mUnidentified024.m_eCharacterClass == YOSHI && m_eActionState == 1)
        return false;
    if (mUnidentified024.m_eCharacterClass == DIDDYKONG && m_eActionState == 0x1E)
        return false;
    if (pOtherFielder->mUnidentified024.m_eCharacterClass == BOWSERJR
        && pOtherFielder->m_eActionState == ACTION_SLIDE_ATTACK)
    {
        float fPlayerScale = pOtherFielder->mUnidentified024.m_fPlayerScale;
        fPlayerScale = 0.5f * fPlayerScale;
        return IsCharacterInAir(fPlayerScale);
    }
    if (mUnidentified024.m_eCharacterClass == BOWSERJR && m_eActionState == ACTION_SLIDE_ATTACK)
        return false;
    if (pOtherFielder->mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10
        && pOtherFielder->m_eActionState == ACTION_SLIDE_ATTACK)
    {
        float fPlayerScale = pOtherFielder->mUnidentified024.m_fPlayerScale;
        fPlayerScale = 0.6f * fPlayerScale;
        return IsCharacterInAir(fPlayerScale);
    }
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10
        && m_eActionState == ACTION_SLIDE_ATTACK)
        return false;

    float leftFootZ = GetJointPosition(m_nLeftFootJointIndex).z;
    float rightFootZ = GetJointPosition(m_nRightFootJointIndex).z;
    bool bRunning = false;
    eFielderActionState eActionState = m_eActionState;
    if (eActionState == ACTION_RUNNING || eActionState == 0x13
        || IsRunningWithBall())
    {
        bRunning = true;
    }
    if (bRunning)
        leftFootZ = rightFootZ = 0.0f;

    nlVector3 v3Unidentified0, v3Unidentified1;
    m_pPhysicsCharacter->GetBonePositions(
        PHYSBONE_FIELDER_HEAD, v3Unidentified0, v3Unidentified1);
    float headZ = v3Unidentified0.z;
    leftFootZ = nlMinEquals(nlMinEquals(leftFootZ, rightFootZ), headZ) - 0.15f;
    if (leftFootZ < 0.0f)
        leftFootZ = 0.0f;

    float fUnidentified1 = pOtherFielder->GetJointPosition(
                                          pOtherFielder->m_nLeftFootJointIndex)
                              .z;
    float fUnidentified2 = pOtherFielder->GetJointPosition(
                                          pOtherFielder->m_nRightFootJointIndex)
                              .z;
    nlVector3 v3Unidentified2, v3Unidentified3;
    pOtherFielder->m_pPhysicsCharacter->GetBonePositions(
        PHYSBONE_FIELDER_HEAD, v3Unidentified2, v3Unidentified3);
    float fUnidentified3 = nlMaxEquals(
        nlMaxEquals(fUnidentified1, fUnidentified2), v3Unidentified2.z);
    fUnidentified3 += 0.15f;
    if (leftFootZ > fUnidentified3)
        return true;
    return false;
}

bool cFielder::HasLooseBallContactPriority(cFielder* pOtherFielder) const
{
    switch (m_eActionState)
    {
    case ACTION_LOOSE_BALL_PASS:
    case ACTION_LOOSE_BALL_SHOT:
    {
        float fUnidentified0 = m_pTweaks->GetDefenseSize();
        float fUnidentified1 = pOtherFielder->m_pTweaks->GetDefenseSize();
        if (fUnidentified1 > fUnidentified0)
            return false;
        if (fUnidentified0 > fUnidentified1)
            return true;

        switch (pOtherFielder->m_eActionState)
        {
        case ACTION_LOOSE_BALL_PASS:
        case ACTION_LOOSE_BALL_SHOT:
        case ACTION_ONETIMER:
        case ACTION_RECEIVE_PASS:
        {
            float fAnimTime = m_pCurrentAnimController->m_fTime;
            float fUnidentified2 = m_fOneTimerAnimTime - fAnimTime;
            if (fUnidentified2 > 0.0f
                && fUnidentified2 <= pOtherFielder->m_fOneTimerAnimTime
                                         - pOtherFielder->m_pCurrentAnimController->m_fTime)
            {
                return true;
            }
            break;
        }
        default:
            return true;
        }
        break;
    }
    default:
        break;
    }
    return false;
}

bool cFielder::IsRunning() const
{
    bool bRunning = false;
    if (m_eActionState == ACTION_RUNNING || m_eActionState == 0x13
        || IsRunningWithBall())
    {
        bRunning = true;
    }
    return bRunning;
}

bool cFielder::IsReceivingVolleyPass() const
{
    bool result = false;
    int state;
    if (m_pAIContext->mScriptMachine != 0
        && m_pAIContext->mScriptMachine->mActiveState != 0)
    {
        state = m_pAIContext->mScriptMachine->mActiveState
                    ->GetState();
    }
    else
    {
        state = -1;
    }
    if (state == 0x16)
    {
        result = ((DesireReceivePass*)
            m_pAIContext->mScriptMachine->mActiveState)->IsVolleyReceive();
    }
    return result;
}

void cFielder::CleanUpAction(eFielderActionState actionState)
{
    switch (m_eActionState)
    {
    case ACTION_HIT:
        if (m_pController != 0)
        {
            m_pController->ResetAccelerationHistory();
        }
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)8)
        {
            m_pHeadTrack->UnidentifiedReset();
            ClearPhysicsPatchesOfType(6);
        }
        break;

    case 0:
        m_ModelType = CharModel_Rigid;
        mUnidentified024.m_v3Position.z = 0.0f;
        mUnidentified024.m_v3Velocity.z = 0.0f;
        EndElectrocution(this);
        break;

    case 1:
        CleanActionDeke();
        break;

    case ACTION_ELECTROCUTION:
        m_ModelType = CharModel_Rigid;
        mUnidentified024.m_v3Position.z = 0.0f;
        mUnidentified024.m_v3Velocity.z = 0.0f;
        EndElectrocution(this);
        break;

    case 3:
        mUnidentified17C = true;
        mUnidentified024.m_v3Position.z = 0.0f;
        mUnidentified024.m_v3Velocity.z = 0.0f;
        if (GameInfoManager::Instance()->GetStadium() == 0x0B)
        {
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            m_pPhysicsCharacter->m_CanCollideWithWall = true;
        }
        break;

    case 0x18:
        mUnidentified024.m_v3Position.z = 0.0f;
        mUnidentified024.m_v3Velocity.z = 0.0f;
        break;

    case ACTION_LOOSE_BALL_PASS:
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
        SetNoPickUpTime(0.0f);
        bIsModified = false;
        break;

    case ACTION_LOOSE_BALL_SHOT:
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
        SetNoPickUpTime(0.0f);
        bIsModified = false;
        break;

    case ACTION_ONETIMER:
        EndBlur();
        bIsModified = false;
        break;

    case ACTION_PASS:
        m_pPassTarget = 0;
        bIsModified = false;
        break;

    case ACTION_RUNNING:
        mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
        m_tMoveToTurboTimer.UnidentifiedClear();
        if (IsYoshiSuperPowerActive())
        {
            if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x17))
            {
                RequestStateMachineDeactivation(GetFielderDesire(this, 0x17));
            }
        }
        break;

    case 0x13:
    {
        PlayerTweaks* pTweaks = m_pTweaks;
        InitMovementRunning(fn_8002C0AC(pTweaks),
            fn_8002CF10(pTweaks), fn_8002C180(pTweaks),
            fn_8002CF24(pTweaks));
        mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
        mActionRunPassVars.fSpeed = 0.0f;
        m_tMoveToTurboTimer.UnidentifiedClear();
        mActionRunPassVars = ActRunPassVars();
        break;
    }

    case ACTION_RUNNING_WB:
        mUnidentified1E4.m_eLastPadAction = 50;
        if (IsYoshiSuperPowerActive())
        {
            if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x17))
            {
                RequestStateMachineDeactivation(GetFielderDesire(this, 0x17));
            }
        }
        break;

    case ACTION_UNKNOWN_15:
        CleanActionShot(actionState);
        break;

    case ACTION_SLIDE_ATTACK:
        KillSlideTackleTrail(this, 0);
        StopSound(0x2AE03886, this);
        break;

    case 0x1C:
        KillDaze(this);
        if (IsBowserSuperPowerActive(this))
        {
            EmitBowserSmoke(this);
        }
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x12 && !mbTangible)
        {
            m_pPhysicsCharacter->m_CanCollideWithBall = true;
            m_pPhysicsCharacter->m_CanCollideWithCharacters = true;
            if (m_pBall != 0)
            {
                g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
                g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
            }
            mbTangible = true;
            if (m_pBall != 0)
            {
                m_pBall->m_bVisible = true;
            }
            mUnidentified17C = true;
            mUnidentified178 = 1.0f;
        }
        break;

    case ACTION_UNKNOWN_30:
        fn_8004BF58(actionState);
        break;

    case ACTION_UNKNOWN_32:
        fn_8004EC40();
        break;

    case 0x21:
    {
        Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
        pGoalie->m_pPhysicsCharacter->m_CanCollideWithBall = true;
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
        mActionBooSkillshot.bFollowingBall = false;
        nlVector3 v3Position = mUnidentified024.m_v3Position;
        if (v3Position.z != 0.0f)
        {
            v3Position.z = 0.0f;
            SetPosition(v3Position);
        }
        RestoreTangibility(false);
        break;
    }

    case ACTION_UNKNOWN_34:
        fn_8004F180();
        break;

    case 0x23:
        if (actionState != 3 && actionState != 0x18)
        {
            mUnidentified024.m_v3Position.z = 0.0f;
            mUnidentified024.m_v3Velocity.z = 0.0f;
        }
        if (GameInfoManager::Instance()->GetStadium() == 0x0B)
        {
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            m_pPhysicsCharacter->m_CanCollideWithWall = true;
        }
        break;

    default:
        break;
    }

    m_eActionState = ACTION_NEED_ACTION;
}

float cFielder::GetShotProbability(float fValue)
{
    return EvaluateShotProbability(fValue);
}

void cFielder::CalcRegularShot(nlVector3& velocity, nlVector3& target, int ballState)
{
    float heightVariance, widthVariance;
    float shotValue = m_pShotMeter->m_fScoreValue;
    Goalie* goalie = m_pTeam->GetOtherTeam()->GetGoalie();
    cBall* ball = g_pBall;
    float shotTime;
    DoFindBestShotTarget(target, shotTime, ballState);
    float inverseTime = 1.0f / shotTime;
    float distance = GetShotTargetDistance(ball->m_v3Position, target);
    float speed = distance * inverseTime;
    float accuracy;
    if (ballState == 7)
        accuracy = (1.1f - shotValue) * (1.5f * distance);
    else
        accuracy = distance * (1.0f - 0.3f * shotValue);
    GameTweaks* tweaks = gGameTweaks.m_pGameTweaks;
    widthVariance = accuracy * tweaks->fShotWidthVariance.GetValue();
    heightVariance = accuracy * tweaks->fShotHeightVariance.GetValue();
    target.y += 0.5f * widthVariance - nlRandomf(widthVariance);
    target.z += nlRandomf(heightVariance);
    if (ballState == 7)
        g_pBall->m_pPhysicsBall->mfBallAirResistance = gChipShotAirResistance;
    g_pBall->ShootAtFast(velocity, target, speed);
    if (ballState == 7)
    {
        float maxHeightVelocity = Interpolate(gChipShotMinVerticalSpeed, gChipShotMaxVerticalSpeed, m_pTweaks->fShooting);
        if (velocity.z > maxHeightVelocity)
            velocity.z = maxHeightVelocity;
    }
    float probability = CalculateShotProbability(shotValue);
    if (nlRandomf(100.0f) < probability)
        goalie->mbShouldMiss = true;
    else
        goalie->mbShouldMiss = false;
}

void cFielder::SetSlideAttackSuccessFlag()
{
    bAttackSucceeded = true;
}

void cFielder::ShootBallDueToContact(const nlVector3& v3IncomingVelocity)
{
    if (GetActionState() == ACTION_SHOOT_TO_SCORE || GetActionState() == ACTION_SHOT)
    {
        g_pBall->ShootRelease(v3Zero, SPINTYPE_NONE);
        return;
    }

    nlVector3 v3ReleaseVelocity;
    nlVec3Add(v3ReleaseVelocity, v3IncomingVelocity, mUnidentified024.m_v3Velocity);
    float fMinSpeedSquared = 0.001f * 0.001f;
    if (nlVec3LengthSquared(v3IncomingVelocity) < fMinSpeedSquared
        || nlVec3LengthSquared(mUnidentified024.m_v3Velocity) < fMinSpeedSquared
        || nlVec3LengthSquared(v3ReleaseVelocity) < fMinSpeedSquared)
    {
        nlVector3 v3ReleaseVelocity;
        nlPolarToCartesian(v3ReleaseVelocity.x, v3ReleaseVelocity.y,
            mUnidentified024.m_aActualFacingDirection, 2.0f);
        v3ReleaseVelocity.z = 0.5f;
        g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
        return;
    }

    nlVec3Normalize(v3ReleaseVelocity, v3ReleaseVelocity);
    float fSpeed = mUnidentified024.m_fActualSpeed;
    fSpeed = 2.0f + fSpeed;
    nlVec3Scale(v3ReleaseVelocity, v3ReleaseVelocity, fSpeed);
    v3ReleaseVelocity.z = 0.5f;
    g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
}

void cFielder::ShootBallDueToContact(unsigned short aShootDirection)
{
    nlVector3 v3ReleaseVelocity;
    float fRadius = mUnidentified024.m_fActualSpeed;
    fRadius = 2.0f + fRadius;
    nlPolarToCartesian(v3ReleaseVelocity.x, v3ReleaseVelocity.y,
        aShootDirection, fRadius);
    v3ReleaseVelocity.z = 0.5f;

    g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
}

void cFielder::DoClearBall()
{
    u16 aClearingAngle;
    int i;
    nlVector3 v3Target;
    nlVector3 v3ClearBallVelocity;
    float fAbsPosition = fabsf(mUnidentified024.m_v3Position.x);
    float fPositionValue = gClearPositionWeight * InterpolateRangeClamped(0.0f, 1.0f,
        cField::GetGoalLineX(1U), 0.0f, fAbsPosition);
    float fBallChargeValue = (1.0f - gClearPositionWeight) * fn_800156A8(g_pBall);
    float fDesiredTime = Interpolate(gClearMinFlightTime, gClearMaxFlightTime,
        fBallChargeValue + fPositionValue);
    ShotMeter* pShotMeter = m_pShotMeter;
    float fShotMeterSpeed = pShotMeter->m_fSpeedValue;
    float fShotMeterValue = gClearShotMeterWeight * fShotMeterSpeed;
    float fPlayerValue = (1.0f - gClearShotMeterWeight) * fn_8002BE38(m_pTweaks);
    float fClearDistance = Interpolate(gClearMinDistance, gClearMaxDistance,
        fShotMeterValue + fPlayerValue);

    aClearingAngle = mUnidentified024.m_aActualFacingDirection;
    if (m_pController != NULL)
    {
        if (m_pController->GetMovementStickMagnitude() > 0.01f)
        {
            aClearingAngle = m_pController->GetMovementStickDirection();
        }
    }
    else
    {
        aClearingAngle = (u16)nlRandom(0xFFFF);
        for (i = 0; i < 4; i++)
        {
            nlVector3 v3Direction;
            nlPolar pDirection;
            UpdateClearingAngleForTeammate(this, i, fClearDistance,
                aClearingAngle, v3Direction, pDirection);
        }
    }

    if (!gDisableClearAngleLimits)
    {
        nlVector3 v3Top;
        nlVector3 v3Bottom;
        nlVector3 v3Net;
        v3Top.x = v3Bottom.x = 5.0f * AIsgn(m_pTeam->GetOtherNet()->m_v3NetLocation.x);
        v3Top.y = cField::GetSidelineY(1);
        v3Bottom.y = cField::GetSidelineY(0);
        v3Top.z = v3Bottom.z = 0.0f;
        nlVec3Sub(v3Net, m_pTeam->GetOtherNet()->m_v3NetLocation, mUnidentified024.m_v3Position);
        nlVec3Sub(v3Top, v3Top, mUnidentified024.m_v3Position);
        nlVec3Sub(v3Bottom, v3Bottom, mUnidentified024.m_v3Position);

        nlPolar pClearingTopAngle;
        nlPolar pClearingBottomAngle;
        nlPolar pNet;
        nlCartesianToPolar(pClearingTopAngle, v3Top);
        nlCartesianToPolar(pClearingBottomAngle, v3Bottom);
        nlCartesianToPolar(pNet, v3Net);
        u32 aNet = pNet.a;
        u32 aTop = pClearingTopAngle.a;
        u32 aBottom = pClearingBottomAngle.a;
        s16 nTopDelta = GetAngleDifference(aNet, aTop);
        s16 nBottomDelta = GetAngleDifference(aNet, aBottom);
        s16 nDelta = GetAngleDifference(aNet, aClearingAngle);
        if (abs_ang16(nDelta) < abs_ang16(nTopDelta)
            && abs_ang16(nDelta) < abs_ang16(nBottomDelta))
        {
            nlVector3 v3TopPost;
            nlVector3 v3BottomPost;
            m_pTeam->GetOtherNet()->GetPostLocation(v3TopPost, 0, 0.0f);
            v3BottomPost = v3TopPost;
            v3BottomPost.y = -v3TopPost.y;
            nlVec3Sub(v3TopPost, v3TopPost, mUnidentified024.m_v3Position);
            nlVec3Sub(v3BottomPost, v3BottomPost, mUnidentified024.m_v3Position);
            nlPolar pTopPost;
            nlPolar pBottomPost;
            nlCartesianToPolar(pTopPost, v3TopPost);
            nlCartesianToPolar(pBottomPost, v3BottomPost);
            s16 nTopPostDelta = GetAngleDifference(pNet.a, pTopPost.a);
            s16 nBottomPostDelta = GetAngleDifference(pNet.a, pBottomPost.a);
            if (abs_ang16(nDelta) < abs_ang16(nTopPostDelta)
                && abs_ang16(nDelta) < abs_ang16(nBottomPostDelta))
            {
                if (abs_ang16(nTopPostDelta) < abs_ang16(nBottomPostDelta))
                {
                    aClearingAngle = pTopPost.a;
                }
                else
                {
                    aClearingAngle = pBottomPost.a;
                }
            }
        }
        else
        {
            nTopDelta = GetAngleDifference(aTop, aClearingAngle);
            nBottomDelta = GetAngleDifference(aBottom, aClearingAngle);
            aClearingAngle = abs_ang16(nTopDelta) < abs_ang16(nBottomDelta)
                ? pClearingTopAngle.a : pClearingBottomAngle.a;
        }
    }

    nlVector3 v3Direction;
    nlPolarToCartesian(v3Direction.x, v3Direction.y, aClearingAngle, fClearDistance);
    v3Direction.z = 0.0f;
    nlVec3Add(v3Target, mUnidentified024.m_v3Position, v3Direction);
    if (m_pBall != NULL)
    {
        ReleaseBall(1);
    }
    if (gbUseDumpCharging && m_eClassType == FIELDER)
    {
        float fCharge = Interpolate(gDumpChargeMin, gDumpChargeMax,
            InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE38(m_pTweaks)));
        fn_800154FC(g_pBall, fCharge + GetBallChargeValue(g_pBall, 0));
    }
    g_pBall->ShootAtFast(v3ClearBallVelocity, v3Target, fDesiredTime);
    g_pBall->ShootRelease(v3ClearBallVelocity, SPINTYPE_BACK);
    SetNoPickUpTime(0.2f);
}

float cFielder::GetModifiedShotTargetDistance(float shotDistance)
{
    return InterpolateRangeClamped(gModifiedShotFarTargetOffset, 0.36f,
        gModifiedShotFarDistance, gModifiedShotNearDistance, shotDistance);
}

bool cFielder::IsActionModifierPressed()
{
    if (GetGlobalPad() != 0)
    {
        return GetGlobalPad()->IsPressed(0x17, true);
    }
    return false;
}

bool cFielder::DoLooseBallContactFromIdle(nlVector3& animStart, float& animStartTime, nlVector3& ballContact, float& ballContactTime, unsigned short facing, const LooseBallContactAnimInfo* info)
{
    const cSAnim* anim = m_pAnimInventory->GetAnim(info->nAnimID);
    nlVector3 localOffset;
    GetJointPositionFuture(&localOffset, info->nAnimID, m_nBallJointIndex,
        anim->GetNormalizedTime(info->fAnimContactFrame), true, true, false, true);
    nlVector3 worldOffset;
    nlVec2Rotate(*(nlVector2*)&worldOffset, *(const nlVector2*)&localOffset, facing);
    worldOffset.z = localOffset.z;
    nlVector3 contactTarget;
    nlVec3Add(contactTarget, mUnidentified024.m_v3Position, worldOffset);
    FakeBallWorld::ResetBallIterator();
    float bestDistanceSquared = 0.0f;
    bool done = false;
    float simulatedTime = 0.0f;
    while (!done || simulatedTime > 5.0f)
    {
        nlVector3 ballPosition;
        FakeBallWorld::GetNextBallPosition(ballPosition);
        nlVector3 delta;
        nlVec3Sub(delta, contactTarget, ballPosition);
        float distanceSquared = nlVec3LengthSquared(delta);
        if (simulatedTime > 0.0f && distanceSquared > bestDistanceSquared)
            done = true;
        if (!done)
        {
            ballContact = ballPosition;
            bestDistanceSquared = distanceSquared;
            simulatedTime += FixedUpdateTask::GetPhysicsUpdateTick();
            if (simulatedTime > 5.0f)
                return false;
        }
    }
    nlVec3Sub(animStart, ballContact, worldOffset);
    animStartTime = simulatedTime - anim->GetNormalizedTime(info->fAnimContactFrame) * anim->GetDuration();
    ballContactTime = simulatedTime;
    return true;
}

bool cFielder::DoLooseBallContactFromRun(nlVector3& animStart, float& animStartTime, nlVector3& ballContact, float& ballContactTime, const LooseBallContactAnimInfo* info, const nlVector3& passIntercept, unsigned int facing)
{
    FakeBallWorld::ResetBallIterator();
    float simulatedTime = 0.0f;
    float bestDistanceSquared = 0.0f;
    float bestTime;
    nlVector3 bestIntercept;
    while (simulatedTime < 5.0f)
    {
        nlVector3 ballPosition;
        FakeBallWorld::GetNextBallPosition(ballPosition);
        simulatedTime += FixedUpdateTask::GetPhysicsUpdateTick();
        nlVector2 delta = { ballPosition.x - passIntercept.x, ballPosition.y - passIntercept.y };
        float distanceSquared = nlVec2LengthSquared(delta);
        if (!(distanceSquared < bestDistanceSquared) && simulatedTime != FixedUpdateTask::GetPhysicsUpdateTick())
            break;
        bestDistanceSquared = distanceSquared;
        bestTime = simulatedTime;
        bestIntercept = ballPosition;
    }
    if (simulatedTime >= 5.0f)
        return false;
    const cSAnim* anim = m_pAnimInventory->GetAnim(info->nAnimID);
    float timeToContact = GetNormalizedContactTime(anim, info->fAnimContactFrame) * anim->GetDuration();
    nlVector3 worldOffset;
    nlVector3 localOffset;
    GetJointPositionFuture(&localOffset, info->nAnimID, m_nBallJointIndex,
        GetNormalizedContactTime(anim, info->fAnimContactFrame), true, true, false, true);
    nlVec2Rotate(*(nlVector2*)&worldOffset, *(const nlVector2*)&localOffset, facing);
    worldOffset.z = localOffset.z;
    nlVec3Sub(animStart, bestIntercept, worldOffset);
    animStart.z = 0.0f;
    animStartTime = bestTime - timeToContact;
    ballContact = bestIntercept;
    ballContactTime = bestTime;
    return true;
}

float CalcPenaltyWorth(ePenaltyType type)
{
    float minAmount = 0.0f;
    float maxAmount = 0.0f;
    switch (type)
    {
    case PEN_TYPE_HIT_WITH_BALL:
    {
        GameTweaks* tweaks = gGameTweaks.m_pGameTweaks;
        minAmount = tweaks->fPowerupHitWithBallMinAmount;
        maxAmount = tweaks->fPowerupHitWithBallMaxAmount;
        break;
    }
    case PEN_TYPE_HIT_NO_BALL:
    {
        GameTweaks* tweaks = gGameTweaks.m_pGameTweaks;
        minAmount = tweaks->fPowerupHitNoBallMinAmount;
        maxAmount = tweaks->fPowerupHitNoBallMaxAmount;
        break;
    }
    case PEN_TYPE_SLIDE_WITH_BALL:
    {
        GameTweaks* tweaks = gGameTweaks.m_pGameTweaks;
        minAmount = tweaks->fPowerupSlideWithBallMinAmount;
        maxAmount = tweaks->fPowerupSlideWithBallMaxAmount;
        break;
    }
    case PEN_TYPE_SLIDE_NO_BALL:
    {
        GameTweaks* tweaks = gGameTweaks.m_pGameTweaks;
        minAmount = tweaks->fPowerupSlideNoBallMinAmount;
        maxAmount = tweaks->fPowerupSlideNoBallMaxAmount;
        break;
    }
    }
    return InterpolateRangeClamped(minAmount, maxAmount, 0.0f, 1.0f, nlRandomf(1.0f));
}

void cFielder::DoPenaltyCardBooking(cFielder* foulee, ePenaltyType type)
{
    if (foulee->mUnidentified1E4.m_tBallUnPossessionTimer.GetSeconds() < gPenaltyPossessionGraceTime)
    {
        if (type == PEN_TYPE_HIT_NO_BALL)
            type = PEN_TYPE_HIT_WITH_BALL;
        else if (type == PEN_TYPE_SLIDE_NO_BALL)
            type = PEN_TYPE_SLIDE_WITH_BALL;
    }
    float worth = CalcPenaltyWorth(type);
    if (worth > 0.0f && foulee->m_pTeam->IncrementPowerupMeter(worth, foulee, true))
    {
        PenaltyData* data = g_PenaltyDataPool.Allocate();
        data->fPenaltyWorth = worth;
        data->pFouler = this;
        data->pFoulee = foulee;
        g_pGame->mUnidentified49C.mPenaltyEvent.Queue(data, Function<PenaltyData*>(FreePenaltyData));
    }
}

void cFielder::PlayImpactCameraRumble()
{
    if (IsSuperGrowActive())
        FireCameraRumbleFilter(gSuperImpactRumbleX, gSuperImpactRumbleY, gSuperImpactRumbleSpring, gSuperImpactRumbleDamping);
    else if (GetCharacterClass() == (eCharacterClass)7 || GetCharacterClass() == (eCharacterClass)13 || GetCharacterClass() == (eCharacterClass)9)
        FireCameraRumbleFilter(gHeavyImpactRumbleX, gHeavyImpactRumbleY, gHeavyImpactRumbleSpring, gHeavyImpactRumbleDamping);
    else if (GetCharacterClass() == (eCharacterClass)19)
        FireCameraRumbleFilter(gBulletImpactRumbleX, gBulletImpactRumbleY, gBulletImpactRumbleSpring, gBulletImpactRumbleDamping);
    else
        FireCameraRumbleFilter(gImpactRumbleX, gImpactRumbleY, gImpactRumbleSpring, gImpactRumbleDamping);
}

void cFielder::UpdateFacingToLooseBall()
{
    const cBall* ball = g_pBall;
    const nlVector3& ballPosition = ball->GetPosition();
    nlVector2 delta;
    nlVec2Set(delta, ballPosition.x - mUnidentified024.m_v3Position.x,
        ballPosition.y - mUnidentified024.m_v3Position.y);
    if (nlVec2LengthSquared(delta) <= 4.0f)
    {
        int count;
        float times[2];
        CalcInterceptXY(mUnidentified024.m_v3Position, fn_8002C254(GetTweaks()), 0.0f,
            ballPosition, ball->m_v3Velocity, count, times);
        if (count != 0)
        {
            float time = count == 2 ? (times[0] < times[1] ? times[0] : times[1]) : times[0];
            nlVector2 future;
            nlVec2Set(future, time * g_pBall->m_v3Velocity.x + g_pBall->m_v3Position.x,
                time * g_pBall->m_v3Velocity.y + g_pBall->m_v3Position.y);
            nlVector2 interceptDelta;
            nlVec2Set(interceptDelta, future.x - mUnidentified024.m_v3Position.x,
                future.y - mUnidentified024.m_v3Position.y);
            unsigned short direction = nlATan2Angle(interceptDelta.y, interceptDelta.x);
            if (nlAbsAngle(direction - mUnidentified024.m_aDesiredFacingDirection) <= 0x4000)
                cCharacter::SetDesiredFacingDirection(direction, true);
        }
    }
}

void cFielder::IncrementPowerupMeter(int nParam, float fAmount)
{
    if (fAmount > 0.0f)
    {
        m_pTeam->IncrementPowerupMeter(fAmount, this, false);
    }
}

bool cFielder::IsReceivePassHitRequested(unsigned short* direction)
{
    if (IsWaluigiSuperPowerActive(this) && m_bSuperPowerTankOn)
        return false;
    if (mUnidentified1E4.m_tBallUnPossessionTimer.GetSeconds() > 0.0f
        && m_pController->DetectRightShake(direction))
    {
        *direction = m_pController->GetMovementStickDirection();
        return true;
    }
    return false;
}

bool cFielder::IsDekeRequested(unsigned short* direction)
{
    if (mtPostDekeTimer.m_uPackedTime != 0)
        return false;
    if (IsWaluigiSuperPowerActive(this) && m_bSuperPowerTankOn)
        return false;
    unsigned short padDirection = 0;
    if (GetDekePadDirection(&padDirection))
    {
        if (!m_bSlideAttackReset)
            return false;
        if (!m_bDekeReset)
            return false;
        if (--m_nDPadDownCounter == 0)
        {
            if (gUseMovementStickForDeke && GetGlobalPad() != 0
                && m_pController->GetMovementStickMagnitude() > 0.01f)
                *direction = m_pController->GetMovementStickDirection();
            else
                *direction = padDirection;
        }
        else if (m_nDPadDownCounter < 0)
            m_nDPadDownCounter = 0;
    }
    else
    {
        m_bSlideAttackReset = true;
        m_bDekeReset = true;
    }
    if (m_nDPadDownCounter == 0)
    {
        m_nDPadDownCounter = 2;
        return true;
    }
    return false;
}

bool cFielder::IsReceivePassDekeRequested(unsigned short* direction)
{
    unsigned short padDirection = 0;
    if (GetDekePadDirection(&padDirection))
    {
        if (!m_bSlideAttackReset)
            return false;
        if (!m_bDekeReset)
            return false;
        if (mUnidentified1E4.m_tBallUnPossessionTimer.GetSeconds() < 0.0f)
            return false;
        if (--m_nDPadDownCounter == 0)
            *direction = padDirection;
        else if (m_nDPadDownCounter < 0)
            m_nDPadDownCounter = 0;
        if (m_nDPadDownCounter == 0)
        {
            m_nDPadDownCounter = 2;
            return true;
        }
    }
    else
    {
        m_bSlideAttackReset = true;
        m_bDekeReset = true;
    }
    return false;
}

bool cFielder::GetDekePadDirection(unsigned short* direction)
{
    bool pressed = false;
    unsigned short angle = 0;
    if (m_pController->m_pGlobalPad->IsPressed(11, true))
    {
        angle = 0x8000;
        if (m_pController->m_pGlobalPad->IsPressed(13, true))
            angle -= DegreesToAngle(45.0f);
        else if (m_pController->m_pGlobalPad->IsPressed(14, true))
            angle += 0x2000;
        pressed = true;
    }
    if (m_pController->m_pGlobalPad->IsPressed(12, true))
    {
        angle = 0;
        if (m_pController->m_pGlobalPad->IsPressed(13, true))
            angle += 0x2000;
        else if (m_pController->m_pGlobalPad->IsPressed(14, true))
            angle -= 0x2000;
        pressed = true;
    }
    if (m_pController->m_pGlobalPad->IsPressed(13, true))
    {
        angle = 0x4000;
        if (m_pController->m_pGlobalPad->IsPressed(11, true))
            angle += 0x2000;
        else if (m_pController->m_pGlobalPad->IsPressed(12, true))
            angle -= 0x2000;
        pressed = true;
    }
    if (m_pController->m_pGlobalPad->IsPressed(14, true))
    {
        angle = 0xc000;
        if (m_pController->m_pGlobalPad->IsPressed(11, true))
            angle -= 0x2000;
        else if (m_pController->m_pGlobalPad->IsPressed(12, true))
            angle += 0x2000;
        pressed = true;
    }
    if (pressed)
        *direction = angle;
    return pressed;
}

bool cFielder::IsDekePadPressed()
{
    bool pressed = false;
    if (GetGlobalPad()->IsPressed(11, true))
        pressed = true;
    if (GetGlobalPad()->IsPressed(12, true))
        pressed = true;
    if (GetGlobalPad()->IsPressed(14, true))
        pressed = true;
    if (GetGlobalPad()->IsPressed(13, true))
        pressed = true;
    return pressed;
}

cFielder* cFielder::DoFindBestHitTarget()
{
    UnidentifiedVariant_80054AB8 vBestTarget = CallFielderFuzzyFunction(
        FuzzyAIGetFielderRuntime(this), "BestHitTarget", this);
    if (vBestTarget.IsPointerType())
    {
        return (cFielder*)vBestTarget.mData.pPlayer;
    }
    return 0;
}

void cFielder::DoFindBestShotTarget(nlVector3& v3PositionOut, float& fShotSpeed, int nParam)
{
    cBall* pBall = g_pBall;
    Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();

    float kBallAllowance = 0.18f + cNet::GetPostRadius();
    kBallAllowance += gGameTweaks.m_pGameTweaks->fShotPostOffset;
    float fDist2NetSide = 0.5f * cNet::GetNetWidth() - kBallAllowance;
    cNet* pNet = m_pTeam->GetOtherNet();
    nlVector3 v3Target;
    v3Target.x = pNet->m_v3NetLocation.x;
    v3Target.y = nlMinEquals(nlMaxEquals(pBall->m_v3Position.y, -fDist2NetSide), fDist2NetSide);
    v3Target.z = nlMinEquals(nlMaxEquals(pBall->m_v3Position.z, 0.18f),
        cNet::GetNetHeight() - kBallAllowance);
    float fShotDist = nlSqrt(nlVec3DistanceSquared2D(pBall->m_v3Position, v3Target), true);

    if (nParam == 8 && (mUnidentified024.m_eCharacterClass == 14 || mUnidentified024.m_eCharacterClass == 12))
    {
        if (mUnidentified024.m_eCharacterClass == 14)
        {
            fShotSpeed = gKoopaSkillshotSpeed;
        }
        else if (mUnidentified024.m_eCharacterClass == 12)
        {
            fShotSpeed = gBirdoSkillshotSpeed;
        }
    }
    else if (nParam == 7)
    {
        fShotSpeed = InterpolateRangeClamped(gModifiedShotNearSpeed, gModifiedShotFarSpeed,
            gModifiedShotNearDistance, gModifiedShotFarDistance, fShotDist);
    }
    else
    {
        float speedFactor = InterpolateRangeClamped(0.0f, 1.0f, 18.0f, 6.0f, fShotDist);
        float fShotMinSpeed;
        float fShotMaxSpeed;
        float fCharge = fn_800156A8(g_pBall);
        fShotMinSpeed = fn_8002C780(m_pTweaks);
        fShotMaxSpeed = fn_8002C758(m_pTweaks);
        fShotSpeed = Interpolate(fShotMinSpeed, fShotMaxSpeed, fCharge);
    }

    float fAbsBallX = fabsf(pBall->m_v3Position.x);
    float fAbsBallY = fabsf(pBall->m_v3Position.y);
    float fAimValue = m_pShotMeter->GetShotAimValue();
    float fAbsAimValue = fabsf(fAimValue);

    if (fAbsBallY < 1.5f + fDist2NetSide
        && (fAbsBallX > fabsf(pGoalie->mUnidentified024.m_v3Position.x)
            || fAbsBallX > cField::GetGoalLineX(1U) - 1.5f))
    {
        v3PositionOut.x = 1.005f * pNet->m_v3NetLocation.x;
        v3PositionOut.y = 0.9f * v3Target.y;
        v3PositionOut.z = v3Target.z + nlRandomf(0.2f);
        if (fShotDist < 2.0f)
        {
            fShotSpeed = 12.0f;
        }
        if (fAbsBallY < fDist2NetSide
            && pBall->m_v3Position.z < cNet::GetNetHeight() - kBallAllowance)
        {
            v3PositionOut.x = 1.1f * pNet->m_v3NetLocation.x;
        }
    }
    else
    {
        nlVector3 v3Post1Delta;
        nlVector3 v3Post2Delta;
        nlVector3 v3GoalieDelta;
        float fNetBaseY = pNet->m_v3NetLocation.y;
        nlVector3 v3Post1 = pNet->m_v3NetLocation;
        nlVector3 v3Post2 = pNet->m_v3NetLocation;
        v3Post1.y = fNetBaseY - fDist2NetSide;
        v3Post2.y = fNetBaseY + fDist2NetSide;

        nlVec3Sub(v3Post1Delta, v3Post1, pBall->m_v3Position);
        nlVec3Sub(v3Post2Delta, v3Post2, pBall->m_v3Position);
        nlVec3Sub(v3GoalieDelta, pGoalie->mUnidentified024.m_v3Position, pBall->m_v3Position);

        u32 aAngPost1 = nlVector3ToAngle(v3Post1Delta);
        u32 aAngPost2 = nlVector3ToAngle(v3Post2Delta);
        u32 aAngGoalie = nlVector3ToAngle(v3GoalieDelta);
        u16 uAbsP1G = (u16)abs_s16(GetAngleDifference(aAngPost1, aAngGoalie));
        u16 uAbsP2G = (u16)abs_s16(GetAngleDifference(aAngPost2, aAngGoalie));
        u16 uAbsP1P2 = (u16)abs_s16(GetAngleDifference(aAngPost1, aAngPost2));

        v3PositionOut.x = 1.005f * pNet->m_v3NetLocation.x;

        float fProbability;
        if (fAbsAimValue > 0.01f)
        {
            fProbability = 0.5f - 0.5f * fAimValue;
        }
        else if (uAbsP1G >= uAbsP1P2)
        {
            fProbability = 1.0f;
        }
        else if (uAbsP2G >= uAbsP1P2)
        {
            fProbability = 0.0f;
        }
        else
        {
            float fNetDX = pNet->m_v3NetLocation.x - pBall->m_v3Position.x;
            float fNetDY = pNet->m_v3NetLocation.y - pBall->m_v3Position.y;
            float fAngToNet = nlATan2f(fNetDY, fNetDX);
            u16 angle2Net = (u16)(s32)(10430.378f * fAngToNet);
            if (pNet->m_v3NetLocation.x < 0.0f)
            {
                angle2Net += 0x8000;
            }
            s16 sAng2Net = (s16)angle2Net;
            if ((u16)abs_s16(sAng2Net) > 0x2000)
            {
                float fGD1Sq = nlVec3DistanceSquared2D(pGoalie->mUnidentified024.m_v3Position, v3Post1);
                float fGD2Sq = nlVec3DistanceSquared2D(pGoalie->mUnidentified024.m_v3Position, v3Post2);
                fProbability = nlMinEquals(nlMaxEquals(fGD1Sq / (fGD1Sq + fGD2Sq), 0.03f), 0.97f);
            }
            else if (3 * uAbsP1G < uAbsP2G || 3 * uAbsP2G < uAbsP1G)
            {
                if (uAbsP1G < uAbsP2G)
                {
                    fProbability = nlMaxEquals(0.05f, 0.5f * (int)(3U * uAbsP1G - uAbsP2G) / (int)(uAbsP1G + uAbsP2G));
                }
                else
                {
                    fProbability = nlMinEquals(0.95f, 1.0f - 0.5f * (int)(3U * uAbsP2G - uAbsP1G) / (int)(uAbsP1G + uAbsP2G));
                }
            }
            else
            {
                float fAngleLimit = 8192.0f;
                fProbability = InterpolateRangeClamped(0.15f, 0.85f, -fAngleLimit, fAngleLimit, -(float)(s32)sAng2Net);
            }
        }

        if (nlRandomf(1.0f) < fProbability)
        {
            v3PositionOut.y = pBall->m_v3Position.y + v3Post1Delta.y;
            float fDistPost2Sq = nlVec3LengthSquared(v3Post2Delta);
            float fDistPost1Sq = nlVec3LengthSquared(v3Post1Delta);
            if (fDistPost1Sq < fDistPost2Sq)
            {
                v3PositionOut.x = 0.985f * pNet->m_v3NetLocation.x;
            }
        }
        else
        {
            v3PositionOut.y = pBall->m_v3Position.y + v3Post2Delta.y;
            float fDistPost1Sq = nlVec3LengthSquared(v3Post1Delta);
            float fDistPost2Sq = nlVec3LengthSquared(v3Post2Delta);
            if (fDistPost2Sq < fDistPost1Sq)
            {
                v3PositionOut.x = 0.985f * pNet->m_v3NetLocation.x;
            }
        }

        if (nParam == 8 && (mUnidentified024.m_eCharacterClass == 14 || mUnidentified024.m_eCharacterClass == 12))
        {
            v3PositionOut.z = cNet::GetNetHeight() * gKoopaBirdoSkillshotHeightFraction;
            v3PositionOut.y = 0.0f;
        }
        else if (bIsModified)
        {
            v3PositionOut.z = cNet::GetNetHeight() - kBallAllowance;
            nlVector3 v3Direction = v3Zero;
            nlVector3 v3BallPosition = pBall->m_v3Position;
            v3BallPosition.z = v3PositionOut.z;
            nlVec3Sub(v3Direction, v3BallPosition, v3PositionOut);
            if (nlVec3LengthSquared(v3Direction) > 0.01f)
            {
                nlVec3Scale(v3Direction, v3Direction,
                    nlRecipSqrt(nlVec3LengthSquared(v3Direction), true));
                float fDistance = GetModifiedShotTargetDistance(fShotDist);
                float fOffset = fDistance * gModifiedShotMinTargetOffsetFraction
                    + nlRandomf(fDistance * (1.0f - gModifiedShotMinTargetOffsetFraction));
                nlVec3Scale(v3Direction, v3Direction, fOffset);
                nlVec3Add(v3PositionOut, v3PositionOut, v3Direction);
            }
        }
        else
        {
            float fHeightVariance = InterpolateRangeClamped(1.0f, 0.2f, 1.0f, 0.0f, m_pTweaks->fShooting);
            float fHeightAllowance = 0.18f + gGameTweaks.m_pGameTweaks->fShotHeightOffsetFromPost.GetValue();
            float fAllowableHeight = cNet::GetNetHeight() - 2.0f * fHeightAllowance;
            float fMinimumHeight = (1.0f - fHeightVariance) * fAllowableHeight;
            float fHeightRange = fHeightVariance * fAllowableHeight;
            v3PositionOut.z = fMinimumHeight + fHeightAllowance + nlRandomf(fHeightRange);
        }
    }
}

void cFielder::DoRegularShooting(bool bParam)
{
    nlVector3 v3BallVelocity;
    nlVector3 v3Target;
    int nBallState = 6;
    bool bHideBall = false;

    if (m_pShotMeter->m_eShotMeterState == SHOT_METER_STS_RELEASED)
    {
        nBallState = 8;
        if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1C))
        {
            DeactivateConcurrentState(m_pAIContext->mScriptMachine, 0x1C);
        }
        if (mUnidentified024.m_eCharacterClass == 14 || mUnidentified024.m_eCharacterClass == 12)
        {
            bHideBall = true;
            g_pBall->m_pPhysicsBall->fn_8013FE00();
        }
    }
    else if (bIsModified)
    {
        nBallState = 7;
    }

    float fCharge = Interpolate(gShotChargeMin, gShotChargeMax,
        InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE84(m_pTweaks)));
    fn_800154FC(g_pBall, fCharge + GetBallChargeValue(g_pBall, 0));
    CalcRegularShot(v3BallVelocity, v3Target, nBallState);

    if (nBallState == 8)
    {
        g_pBall->m_uGoalType = 2;
    }
    else if (m_eActionState == ACTION_ONETIMER
        || m_eActionState == ACTION_LATE_ONETIMER_FROM_VOLLEY
        || (m_eActionState == ACTION_UNKNOWN_15
            && mUnidentified1E4.m_tBallPossessionTimer.GetSeconds() < 0.1f))
    {
        g_pBall->m_uGoalType = 1;
    }
    else
    {
        g_pBall->m_uGoalType = 0;
    }

    if (m_pBall != NULL)
    {
        ReleaseBall(nBallState);
    }
    if (bHideBall)
    {
        g_pBall->m_bVisible = false;
    }
    g_pBall->m_v3ShotTarget = v3Target;

    eSpinType spinType;
    nlVector3 v3AngVel;
    if (nBallState == 7)
    {
        spinType = SPINTYPE_BACK;
        v3AngVel = v3Zero;
    }
    else
    {
        float fSpinScale = InterpolateRangeClamped(gShotSpinMinScale, gShotSpinMaxScale,
            gShotSpinMinSpeed, gShotSpinMaxSpeed, nlVec3Length(v3BallVelocity));
        spinType = SPINTYPE_PARAMETER;
        float fXSpin = 8.0f * fSpinScale;
        float fZSpin = 15.0f * fSpinScale;
        float fHalfXSpin = 0.5f * fXSpin;
        v3AngVel.x = fHalfXSpin - nlRandomf(fXSpin);
        v3AngVel.y = 0.0f;

        if (!m_pTeam->GetOtherTeam()->GetGoalie()->mbShouldMiss
            && m_pTweaks->fShooting > 0.25f && m_pTweaks->fShooting < 0.75f)
        {
            float fMinDistanceSq = gShotYSpinMinDistance * gShotYSpinMinDistance;
            float fDistanceSq = nlVec3DistanceSquared2D(
                mUnidentified024.m_v3Position, m_pTeam->GetOtherNet()->m_v3NetLocation);
            if (fDistanceSq > fMinDistanceSq)
            {
                float fDistanceValue = InterpolateRangeClamped(0.2f, 1.0f,
                    gShotYSpinMinDistance, gShotYSpinMaxDistance, nlSqrt(fDistanceSq, true));
                v3AngVel.y = -fSpinScale * fDistanceValue
                    * (gShotYSpinBase + nlRandomf(gShotYSpinRange));
            }
        }
        v3AngVel.z = fZSpin + nlRandomf(fZSpin);

        nlVector3 v3Delta;
        nlVec3Sub(v3Delta, v3Target, g_pBall->m_v3Position);
        bool bNegZSpin = false;
        if (fabsf(v3Delta.x) < fabsf(v3Delta.y))
        {
            if (v3Delta.x * v3Delta.y > 0.0f)
            {
                bNegZSpin = true;
            }
        }
        else
        {
            if (v3Target.x * v3Target.y > 0.0f)
            {
                bNegZSpin = true;
            }
        }
        if (bNegZSpin)
        {
            v3AngVel.z *= -1.0f;
        }
        RotateVectorZAxis(v3AngVel, v3AngVel, mUnidentified024.m_aActualFacingDirection);
        if (m_eActionState == ACTION_ONETIMER)
        {
            nlVec3Scale(v3AngVel, 0.4f);
        }

        if ((g_pBall->m_uGoalType == 0 || g_pBall->m_uGoalType == 2)
            && m_pTweaks->fShooting > 0.25f)
        {
            if (m_eActionState == ACTION_LOOSE_BALL_SHOT)
            {
                switch (m_eAnimID)
                {
                case 0x34:
                case 0x35:
                case 0x36:
                case 0x37:
                case 0x38:
                case 0x39:
                case 0x3A:
                case 0x3B:
                {
                    float fCharge = fn_800156A8(g_pBall);
                    PhysicsBall* pPhysicsBall = g_pBall->m_pPhysicsBall;
                    pPhysicsBall->mbUseMagnusEffect = true;
                    pPhysicsBall->mfChargeBonus = fCharge;
                    break;
                }
                }
            }
            else
            {
                float fCharge = fn_800156A8(g_pBall);
                PhysicsBall* pPhysicsBall = g_pBall->m_pPhysicsBall;
                pPhysicsBall->mbUseMagnusEffect = true;
                pPhysicsBall->mfChargeBonus = fCharge;
            }
        }
    }

    g_pBall->Shoot(this, v3BallVelocity, v3AngVel, spinType, nBallState, bParam);
    SetNoPickUpTime(0.2f);
    if (nBallState == 8 && mUnidentified024.m_eCharacterClass == 16)
    {
        fn_8004ED64();
    }
    if (g_pGame->IsGameplayOrOvertime())
    {
        ShotAtGoalData* pShotData = g_ShotAtGoalDataPool.Allocate();
        pShotData->pShooter = this;
        QueueShotAtGoalEvent(g_pGame, pShotData);
        if (nBallState != 8)
        {
            ePlayerStats stat = STATS_00;
            float fCharge = fn_800156A8(g_pBall);
            if (fCharge >= 0.8f)
            {
                stat = STATS_02;
            }
            else if (fCharge >= 0.4f)
            {
                stat = STATS_01;
            }
            StatsTracker::Instance()->TrackStat(stat, m_pTeam->m_nSide, mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
    }
}

void cFielder::DoResetShotMeter(float fTime)
{
    m_pShotMeter->Reset(this);
    m_pShotMeter->m_fTime = fTime;
}

bool cFielder::IsActionDone() const
{
    return (u8)(m_eActionState == ACTION_NEED_ACTION);
}

void cFielder::SetAction(eFielderActionState actionState)
{
    CleanUpAction(actionState);
    m_eActionState = actionState;
}

void cFielder::ClearInvincibility(bool force)
{
    if (!IsConcurrentStateActive(m_pAIContext->mScriptMachine, 24) || force == true)
    {
        muInvincibleStatus = 0;
    }
}

bool cFielder::CalculateFormationPosition(nlVector3& position)
{
    m_bInPosition = m_pTeam->CalculateFormationPosition(position, this, m_bInPosition);
    return m_bInPosition;
}

static LooseBallContactAnimInfo gOneTimerIdleGroundContactAnims[4] = {
    { 0x38, 9.0f, 0xE000, 0x2000 },
    { 0x39, 9.0f, 0xA000, 0xE000 },
    { 0x3B, 9.0f, 0x6000, 0xA000 },
    { 0x3A, 9.0f, 0x2000, 0x6000 },
};

static LooseBallContactAnimInfo gOneTimerIdleVolleyContactAnims[4] = {
    { 0x44, 4.0f, 0xE000, 0x2000 },
    { 0x45, 4.0f, 0xA000, 0xE000 },
    { 0x47, 4.0f, 0x6000, 0xA000 },
    { 0x46, 4.0f, 0x2000, 0x6000 },
};

static LooseBallContactAnimInfo gOneTimerLeadGroundContactAnims[2] = {
    { 0x48, 6.0f, 0xC000, 0x4000 },
    { 0x49, 6.0f, 0x4000, 0xC000 },
};

const LooseBallContactAnimInfo* GetOneTimerIdleGroundContactAnims()
{
    return gOneTimerIdleGroundContactAnims;
}

int GetNumOneTimerIdleGroundContactAnims()
{
    return sizeof(gOneTimerIdleGroundContactAnims) / sizeof(gOneTimerIdleGroundContactAnims[0]);
}

const LooseBallContactAnimInfo* GetOneTimerIdleVolleyContactAnims()
{
    return gOneTimerIdleVolleyContactAnims;
}

int GetNumOneTimerIdleVolleyContactAnims()
{
    return sizeof(gOneTimerIdleVolleyContactAnims) / sizeof(gOneTimerIdleVolleyContactAnims[0]);
}

const LooseBallContactAnimInfo* GetOneTimerLeadGroundContactAnims()
{
    return gOneTimerLeadGroundContactAnims;
}

int GetNumOneTimerLeadGroundContactAnims()
{
    return sizeof(gOneTimerLeadGroundContactAnims) / sizeof(gOneTimerLeadGroundContactAnims[0]);
}

const LooseBallContactAnimInfo* cFielder::FindLooseBallContactAnim(
    const LooseBallContactAnimInfo* pBallContactAnimInfo,
    int nNumContactAnims, unsigned short aFutureFacingDirection,
    const nlVector3& v3FuturePosition, const nlVector3& v3OneTimerTarget,
    float fAngle)
{
    nlVector3 v3Unidentified;
    nlVec3Sub(v3Unidentified, v3OneTimerTarget, v3FuturePosition);
    u16 aNetAngle = nlVector3ToAngle(v3Unidentified) - aFutureFacingDirection;

    const LooseBallContactAnimInfo* pBestBallContactAnimInfo = NULL;
    for (int i = 0; i < nNumContactAnims; i++)
    {
        if (pBallContactAnimInfo[i].aIncomingAngleMin
            < pBallContactAnimInfo[i].aIncomingAngleMax)
        {
            if (aNetAngle >= pBallContactAnimInfo[i].aIncomingAngleMin
                && aNetAngle <= pBallContactAnimInfo[i].aIncomingAngleMax)
            {
                pBestBallContactAnimInfo = &pBallContactAnimInfo[i];
            }
        }
        else if (aNetAngle >= pBallContactAnimInfo[i].aIncomingAngleMin
            || aNetAngle <= pBallContactAnimInfo[i].aIncomingAngleMax)
        {
            pBestBallContactAnimInfo = &pBallContactAnimInfo[i];
        }
    }
    return pBestBallContactAnimInfo;
}

bool cFielder::IsFallenDown() const
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
    {
        return true;
    }

    if (IsInFallAction())
    {
        return true;
    }

    if (GetActionState() == (eFielderActionState)0x21)
    {
        if (m_eAnimID != 0x81 || m_pCurrentAnimController->m_fTime < 0.3f)
        {
            return true;
        }
        return false;
    }

    if (GetActionState() == (eFielderActionState)0x22
        || GetActionState() == (eFielderActionState)0x23)
    {
        return true;
    }

    float fGetUpFrame = -1.0f;
    switch (m_eAnimID)
    {
    case 0x7F:
        fGetUpFrame = 44.0f;
        break;
    case 0x65:
        fGetUpFrame = 67.0f;
        break;
    case 0x66:
        fGetUpFrame = 64.0f;
        break;
    case 0x6A:
        fGetUpFrame = 30.0f;
        break;
    case 0x6E:
        fGetUpFrame = 43.0f;
        break;
    case 0x72:
        fGetUpFrame = 56.0f;
        break;
    case 0x6B:
    case 0x6D:
        fGetUpFrame = 30.0f;
        break;
    case 0x6F:
    case 0x71:
        fGetUpFrame = 45.0f;
        break;
    case 0x73:
    case 0x75:
        fGetUpFrame = 60.0f;
        break;
    case 0x6C:
        fGetUpFrame = 30.0f;
        break;
    case 0x70:
        fGetUpFrame = 43.0f;
        break;
    case 0x74:
        fGetUpFrame = 56.0f;
        break;
    case 0x5F:
        fGetUpFrame = 42.0f;
        break;
    case 0x61:
        fGetUpFrame = 46.0f;
        break;
    case 0x60:
    case 0x62:
        fGetUpFrame = 43.0f;
        break;
    case 0x63:
        fGetUpFrame = 46.0f;
        break;
    case 0x64:
        fGetUpFrame = 42.0f;
        break;
    case 0x56:
        fGetUpFrame = 108.0f;
        break;
    case 0x76:
    case 0x77:
    case 0x79:
    case 0x7A:
        fGetUpFrame = (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
        break;
    case 0x7C:
        fGetUpFrame = (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
        break;
    case 0x7D:
        fGetUpFrame = 55.0f;
        break;
    case 0x78:
    case 0x7B:
        fGetUpFrame = 29.0f;
        break;
    case 0x68:
        if (mUnidentified024.m_eCharacterClass == BOWSER)
        {
            fGetUpFrame = (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
        }
        break;
    }

    return m_pCurrentAnimController->m_fTime
        < fGetUpFrame / m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
}

bool cFielder::IsHitting() const
{
    const float fAnimTime
        = m_pCurrentAnimController->m_fTime * m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
    return HasHitWindowStarted(this, fAnimTime) && fAnimTime <= fn_8002D050(m_pTweaks);
}

bool cFielder::IsSlideAttacking() const
{
    if (!IsFrozenStateActive() && m_eActionState == ACTION_SLIDE_ATTACK)
        return true;
    return false;
}

bool cFielder::IsStriker() const
{
    return m_eRole == ROLE_STRIKER;
}

bool cFielder::IsWinger() const
{
    return m_eRole == ROLE_WINGER;
}

bool cFielder::IsMidField() const
{
    return m_eRole == ROLE_MIDFIELD;
}

bool cFielder::IsDefense() const
{
    return m_eRole == ROLE_DEFENCE;
}

unsigned int cFielder::IsFrozen() const
{
    return ((DesireFrozen*)GetConcurrentState(m_pAIContext->mScriptMachine, 0x1D))->IsActiveFrozenState(DesireFrozen::FROZEN_PHOTO);
}

bool IsFielderDazed(const cFielder* pFielder)
{
    return ((DesireFrozen*)GetConcurrentState(pFielder->m_pAIContext->mScriptMachine, 0x1D))->IsActiveFrozenState(DesireFrozen::FROZEN_ICE);
}

unsigned int cFielder::IsShattered() const
{
    return ((DesireFrozen*)GetConcurrentState(
        m_pAIContext->mScriptMachine, 29))->IsActiveFrozenState(DesireFrozen::FROZEN_SHATTERED);
}

bool cFielder::IsMegaStrikeFrozen() const
{
    return ((DesireFrozen*)GetConcurrentState(
        m_pAIContext->mScriptMachine, 29))->IsActiveFrozenState(DesireFrozen::FROZEN_MEGA_STRIKE);
}

bool cFielder::IsStuck() const
{
    return ((DesireFrozen*)GetConcurrentState(m_pAIContext->mScriptMachine, 0x1D))
               ->IsActiveFrozenState(DesireFrozen::FROZEN_ICE)
        || ((DesireFrozen*)GetConcurrentState(m_pAIContext->mScriptMachine, 0x1D))
               ->IsActiveFrozenState(DesireFrozen::FROZEN_PHOTO);
}

bool cFielder::IsFrozenStateActive() const
{
    DesireFrozen* pAction = (DesireFrozen*)
        GetConcurrentState(m_pAIContext->mScriptMachine, 0x1D);
    return pAction != 0 && pAction->mActive && pAction->meFrozenState != DesireFrozen::FROZEN_NONE;
}

float cFielder::CalcSlideAttackBallIntercept(nlVector3& target, int direction)
{
    nlVector3 velocity = g_pBall->m_v3Velocity;
    nlVector3 position = g_pBall->GetPosition();
    int count;
    float maxTime = GetSlideInterceptTimeLimit(m_pTweaks);
    const cBall* ball = g_pBall;
    if (ball->GetOwner() == 0 && ball->meBallState != 5)
    {
        if (ball->HasActivePassTarget())
            maxTime = ball->m_tPassTargetTimer.GetSeconds();
        nlVector3 interceptVelocity;
        float interceptTime, closestDistance;
        float speed = GetSlideAttackSpeed(direction);
        if (FakeBallWorld::FindBallIntercept(GetPosition(), fn_8002BFA8(m_pTweaks, GetPlayerScale()), speed,
                target, interceptVelocity, interceptTime, closestDistance, maxTime)
            && target.z < 0.5f)
            return interceptTime;
        if (g_pBall->UnidentifiedHasPassTarget())
        {
            velocity = v3Zero;
            position = g_pBall->m_v3PassIntercept;
        }
    }
    else if (ball->meBallState == 5)
    {
        velocity = g_pBall->GetPassTargetFielder()->GetVelocity();
        position = g_pBall->GetPassTargetFielder()->GetPosition();
    }
    else if (g_pBall->GetOwnerFielder() != 0
        && (g_pBall->GetOwnerFielder()->m_eActionState == ACTION_SHOOT_TO_SCORE
            || g_pBall->GetOwnerFielder()->m_eActionState == ACTION_SHOT
            || g_pBall->GetOwnerFielder()->m_eActionState == (eFielderActionState)1))
    {
        velocity = v3Zero;
        position = g_pBall->GetOwnerFielder()->GetPosition();
    }
    else if (g_pBall->GetOwnerFielder() != 0
        && g_pBall->GetOwnerFielder()->m_eActionState == ACTION_SLIDE_ATTACK)
    {
        velocity = v3Zero;
        position = g_pBall->GetOwnerFielder()->GetPosition();
    }
    else if (g_pBall->GetOwnerFielder() != 0
        && g_pBall->GetOwnerFielder()->GetCharacterClass() == (eCharacterClass)12)
    {
        nlVector3 average;
        nlVecLerp(average, g_pBall->GetPosition(), g_pBall->GetOwnerFielder()->GetPosition(), 0.5f);
        position = average;
    }
    nlVector3 landingSpot;
    float solutions[2];
    float speed = GetSlideAttackSpeed(direction);
    CalcInterceptXY(GetPosition(), speed, fn_8002BFA8(m_pTweaks, GetPlayerScale()), position, velocity, count, solutions);
    float time;
    if (count != 0)
    {
        if (count == 2)
            time = solutions[0] < solutions[1] ? solutions[0] : solutions[1];
        else
            time = solutions[0];
    }
    else
        time = -1.0f;
    float landingTime = g_pBall->PredictLandingSpotAndTime(landingSpot, 0, 0, 0.0f);
    if (time >= 0.0f && time <= maxTime && time <= landingTime)
    {
        target.x = velocity.x * time + position.x;
        target.y = velocity.y * time + position.y;
    }
    else
    {
        target.x = velocity.x * maxTime + position.x;
        target.y = velocity.y * maxTime + position.y;
    }
    target.z = 0.0f;
    return time;
}

bool cFielder::CanPickupBall(cBall* pBall, bool bParam)
{
    if (IsStuck())
    {
        return false;
    }

    if (IsFallenDown())
    {
        return false;
    }

    bool bUnidentified = false;
    if (mUnidentified024.m_eCharacterClass == YOSHI
        && IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x17))
    {
        bUnidentified = true;
    }

    if (bUnidentified)
    {
        return false;
    }

    return cPlayer::CanPickupBall(pBall, bParam);
}

bool cFielder::CanDoSidekickShootToScore()
{
    bool canShoot = CanShootFromPosition(this, false);
    if (!canShoot || !HasBall() || !IsSidekick(this) || bIsModified)
    {
        return false;
    }

    if (GameInfoManager::Instance()->IsRule0x8Equal4())
    {
        return false;
    }

    bool homeEnabled = false;
    if (GameInfoManager::Instance()->GetCurrentSettings()->m_unk18
        && m_pTeam->m_nSide == HOME)
    {
        homeEnabled = true;
    }

    bool enabled = false;
    if (homeEnabled
        || (GameInfoManager::Instance()->GetCurrentSettings()->m_unk19
            && m_pTeam->m_nSide == AWAY))
    {
        enabled = true;
    }
    return enabled;
}

bool cFielder::CanReactToGroundEffects() const
{
    switch (m_eActionState)
    {
    case ACTION_ELECTROCUTION:
        if (m_bGroundElectrocution || m_eAnimID == 0x78)
        {
            return true;
        }
        return false;
    case 3:
    case 24:
        if (m_fFallingTime > 0.0f)
            return false;
        return true;
    default:
        return true;
    }
}

void cFielder::GetApproachPosition(nlVector3* position, const nlVector3* from, float predictionTime)
{
    float radius = 1.0f + mUnidentified320->GetRadius();
    nlVector3 center;
    if (predictionTime > 0.0f)
        nlVec3ScaleAdd(center, predictionTime, mUnidentified024.m_v3Velocity,
            mUnidentified024.m_v3Position);
    else
        center = mUnidentified024.m_v3Position;
    nlVec3Sub(*position, *from, center);
    nlVec3Normalize(*position, *position);
    nlVec3ScaleAdd(*position, radius, *position, center);
}

float cFielder::GetAirInterceptHeight(int type)
{
    if (mfAirInterceptHeight[type] < 0.0f)
    {
        const LooseBallContactAnimInfo* anim = gOneTimerIdleVolleyContactAnims;
        if (type == 0)
            anim = gOneTimerLeadGroundContactAnims;
        nlVector3 position;
        const cSAnim* contactAnim = m_pAnimInventory->GetAnim(anim->nAnimID);
        GetJointPositionFuture(&position, anim->nAnimID, m_nBallJointIndex,
            contactAnim->GetNormalizedTime(anim->fAnimContactFrame),
            true, true, false, true);
        mfAirInterceptHeight[type] = position.z;
    }
    return mfAirInterceptHeight[type] * GetPlayerScale();
}

float cFielder::GetDekeDistance()
{
    float charge = GetBallChargeValue(g_pBall, 0);
    float extraDistance = gDekeChargeDistance * (charge / 4.0f);
    if (extraDistance > gDekeChargeDistance)
        extraDistance = gDekeChargeDistance;
    return gBaseDekeDistance + extraDistance;
}

void cFielder::BeginDekeIntangibility()
{
    if (mbTangible)
    {
        if (GetCharacterClass() == (eCharacterClass)16)
        {
            SetTangible(false, false);
            mtPostDekeTimer.UnidentifiedClear();
            mUnidentified178 = gIntangibleAlpha;
            EmitBooDekePuffStart(this);
        }
        else if (GetCharacterClass() == (eCharacterClass)8)
        {
            SetTangible(false, false);
            mUnidentified178 = gIntangibleAlpha;
        }
        else if (GetCharacterClass() == (eCharacterClass)18)
        {
            SetTangible(false, false);
            mUnidentified178 = gIntangibleAlpha;
            if (m_eActionState == (eFielderActionState)1 || m_eActionState == (eFielderActionState)32)
            {
                EmitMontyDekeEnter(this);
                if (m_pBall != 0)
                    m_pBall->m_pPhysicsBall->mbCanGoThroughGround = true;
            }
            else if (m_eActionState == (eFielderActionState)28)
                EmitMontySquishEnter(this);
        }
        else if (m_pBall != 0 && m_eActionState == (eFielderActionState)1)
        {
            SetTangible(false, false);
            mUnidentified178 = gIntangibleAlpha;
            int spread = gDekeDirectionSpread;
            float distance = GetDekeDistance();
            ResetBallCharge(g_pBall, false);
            if (GetCharacterClass() == (eCharacterClass)6)
                EmitDekeEnter(this, "waluigi_deke_enter");
            else if (GetCharacterClass() == (eCharacterClass)2)
                EmitDekeEnter(this, "daisy_deke_enter");
            else if (GetCharacterClass() == (eCharacterClass)17)
                EmitDekeEnter(this, "drybones_deke_enter");
            unsigned short direction = GetActualFacing() + (nlRandomf(2 * spread) - spread);
            float safeY = cField::GetSidelineY(1U) - fn_8002BFA8(m_pTweaks, GetPlayerScale()) - 0.25f;
            bool nearSideline = fabsf(mUnidentified024.m_v3Position.y) >= safeY;
            nlVector3 destination;
            nlPolarToCartesian(destination.x, destination.y, direction, 1.0f);
            destination.z = 0.0f;
            nlVec3Scale(destination, destination, distance);
            nlVec3Add(destination, destination, mUnidentified024.m_v3Position);
            bool beyondSideline = fabsf(destination.y) > cField::GetSidelineY(1U);
            bool fixed = false;
            if (GameInfoManager::Instance()->GetStadium() != 11 && m_pPhysicsCharacter->m_CanCollideWithWall)
                fixed = cField::FixOutOfBoundsPosition(destination, 0.9f * fn_8002BFA8(m_pTweaks, GetPlayerScale()), false);
            else if (m_pPhysicsCharacter->m_CanCollideWithGoalLine)
                fixed = cField::FixOutOfBoundsX(destination, false, 0.9f * fn_8002BFA8(m_pTweaks, GetPlayerScale()));
            bool overGoal = false;
            if (fixed)
            {
                if (fabsf(destination.y) < 0.5f * cNet::GetNetWidth())
                {
                    destination.x = AIsgn(destination.x) * cField::GetGoalLineX(1U);
                    if (gDisableElectrocution || GameInfoManager::Instance()->IsRule0x4Equal2()
                        || IsConcurrentStateActive(m_pAIContext->mScriptMachine, 24))
                    {
                        if (destination.y < 0.0f)
                            destination.y = -(1.0f + (0.5f * cNet::GetNetWidth() + fn_8002BFA8(m_pTweaks, GetPlayerScale())));
                        else
                            destination.y = 1.0f + (0.5f * cNet::GetNetWidth() + fn_8002BFA8(m_pTweaks, GetPlayerScale()));
                        destination.z = 0.0f;
                    }
                    else
                    {
                        destination.z = 3.5f;
                        overGoal = true;
                    }
                }
                if (nearSideline && fabsf(destination.x) <= cField::GetGoalLineX(1U) - fn_8002BFA8(m_pTweaks, GetPlayerScale()) - 2.0f && !beyondSideline)
                    destination.y = safeY * AIsgn(destination.y);
            }
            SetPosition(destination);
            if (fixed && overGoal && m_pBall != 0)
            {
                ReleaseBall(0);
                destination.x = AIsgn(destination.x) * (cField::GetGoalLineX(1U) - 0.5f);
                g_pBall->SetPosition(destination);
                fn_800156F8(g_pBall, 0);
            }
        }
    }
}

void cFielder::EmitMegaStrikeWindup()
{
    KillWindups();
    switch (mUnidentified024.m_eCharacterClass)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
        EmitWindupAtBall("ball_sts_windup");
        break;
    }
}

void cFielder::RestoreTangibility(bool fadeIn)
{
    if (!mbTangible)
    {
        SetTangible(true, false);
        if (GetCharacterClass() == (eCharacterClass)16)
        {
            if (mUnidentified178 < 1.0f)
            {
                if (fadeIn && g_pGame->IsGameplayOrOvertime())
                    mtPostDekeTimer.SetSeconds(gBooDekeFadeTime);
                else
                    mUnidentified178 = 1.0f;
            }
            EmitBooDekePuffEnd(this);
        }
        else if (GetCharacterClass() == (eCharacterClass)8)
        {
            mUnidentified178 = 1.0f;
        }
        else if (GetCharacterClass() == (eCharacterClass)18)
        {
            mUnidentified178 = 1.0f;
            if (!IsFallenDown())
            {
                CharacterImpactEvent event;
                event.v3Position = mUnidentified024.m_v3Position;
                event.fRadius = gMontyReappearRadius;
                event.pCharacter = this;
                DeliverMontyReappearEvent(g_pGame, &event);
                EmitMontyDekeExit(this);
            }
            else
                EmitMontySquishExit(this);
            if (m_eActionState == (eFielderActionState)32 && m_pBall != 0)
                g_pBall->m_pPhysicsBall->mbCanCollideGoalie = false;
            g_pBall->m_pPhysicsBall->mbCanGoThroughGround = false;
        }
        else
        {
            mUnidentified178 = 1.0f;
            PlaySound(m_uSoundSlotId, 0x5bf8e132, 0, 0);
            if (GetCharacterClass() == (eCharacterClass)6)
                EmitDekeExit(this, "waluigi_deke_enter");
            else if (GetCharacterClass() == (eCharacterClass)2)
                EmitDekeExit(this, "daisy_deke_enter");
            else if (GetCharacterClass() == (eCharacterClass)17)
                EmitDekeExit(this, "drybones_deke_enter");
        }
    }
}

void cFielder::CleanActionDeke()
{
    mUnidentified1E4.m_eLastPadAction = 50;
    m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
    bIsModified = false;
    if (IsInvincibleChars())
        EndDeke(this);
    if (m_pController != 0)
        m_pController->ResetAccelerationHistory();
    if (GetCharacterClass() == (eCharacterClass)16)
        RestoreTangibility(true);
    else if (GetCharacterClass() == (eCharacterClass)18
        || GetCharacterClass() == (eCharacterClass)6
        || GetCharacterClass() == (eCharacterClass)2
        || GetCharacterClass() == (eCharacterClass)17)
        RestoreTangibility(false);
    if (IsCharacterSuperPowerActive(this, (eCharacterClass)1) && g_pGame->GetGameState() != 4)
        EmitBowserSmoke(this);
    if (m_pBall != 0 && !m_pBall->m_bVisible)
        m_pBall->m_bVisible = true;
}

void cFielder::CleanActionShootToScore()
{
    if (!IsConcurrentStateActive(m_pAIContext->mScriptMachine, 24))
        muInvincibleStatus = 0;
    m_nMegaStrikeResultState = 0;
    if (m_pTeam->GetOtherTeam()->GetGoalie()->mGoalieActionState != GOALIEACTION_MEGA_STRIKE)
        g_pGame->mpWeatherManager->Resume();
    StopSound(0x05C8E379, this);
    StopSound(0xBF541A4C, this);
}

void cFielder::CleanActionShot(eFielderActionState newAction)
{
    bIsModified = false;
    m_pShotMeter->Abort();
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)16 && newAction != 33)
        mUnidentified178 = 1.0f;
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)12 && g_pBall->meBallState != 8)
    {
        if (gNPCManager->mpBirdoEgg != 0 && gNPCManager->mpBirdoEgg->mVisible)
            gNPCManager->mpBirdoEgg->Hide(false);
    }
    else if (mUnidentified024.m_eCharacterClass == (eCharacterClass)14 && g_pBall->meBallState != 8)
    {
        if (gNPCManager->mpKoopaShell != 0 && gNPCManager->mpKoopaShell->mVisible)
            gNPCManager->mpKoopaShell->Deactivate(false);
    }
}

void cFielder::ResetAnimState()
{
    fn_80097648(-1.0f);
    SetAnimState(0, false, 0.0f, false, false);
    m_pCurrentAnimController->SetTime(0.0f);
    InitMovementNone(0.0f, 0.0f);
}

void cFielder::SetStartAnimState(int animState)
{
    static int runStartAnims[4] = { 2, 2, 3, 1 };
    if ((IsCharacterSuperPowerActive(this, (eCharacterClass)1)
            || IsCharacterSuperPowerActive(this, (eCharacterClass)6)
            || IsCharacterSuperPowerActive(this, (eCharacterClass)11)) && m_bSuperPowerTankOn)
    {
        SetRunningAnimState(0.1f);
    }
    else if (animState != -1)
    {
        SetAnimState(runStartAnims[animState], true, 0.2f, false, false);
        s16 turnAdjust = CalcAnimTurnAdjust(mUnidentified024.m_aActualFacingDirection,
            mUnidentified024.m_aDesiredFacingDirection, m_eAnimID, 1.0f);
        InitMovementFromAnim(turnAdjust, v3Zero, 1.0f, false);
        m_pCurrentAnimController->m_fPlaybackSpeedScale = gStartAnimPlaybackSpeed
            + InterpolateRangeClamped(0.0f, gStartTerrainSpeedBoost, 0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
    }
    else
    {
        int direction = ((mUnidentified024.m_aDesiredFacingDirection - mUnidentified024.m_aActualFacingDirection + 0x2000) >> 14) & 3;
        if (direction != 0)
        {
            SetAnimState(runStartAnims[direction], true, 0.2f, false, false);
            s16 turnAdjust = CalcAnimTurnAdjust(mUnidentified024.m_aActualFacingDirection,
                mUnidentified024.m_aDesiredFacingDirection, m_eAnimID, 1.0f);
            InitMovementFromAnim(turnAdjust, v3Zero, 1.0f, false);
            m_pCurrentAnimController->m_fPlaybackSpeedScale = gStartAnimPlaybackSpeed
                + InterpolateRangeClamped(0.0f, gStartTerrainSpeedBoost, 0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
        }
        else
        {
            SetRunningAnimState(0.1f);
            if (mUnidentified024.m_fActualSpeed < GetJogSpeed(m_pTweaks))
                mUnidentified024.m_fActualSpeed = mUnidentified024.m_fDesiredSpeed = GetJogSpeed(m_pTweaks);
        }
    }
}

void cFielder::SetWindupWBAnimState()
{
    if (GetFacingDeltaToPosition(m_pTeam->GetOtherNet()->m_v3NetLocation) < 0)
        SetAnimState(0x53, true, 0.2f, false, false);
    else
        SetAnimState(0x52, true, 0.2f, false, false);
}

void cFielder::SetStartWBAnimState()
{
    static int runStartAnims[4] = { 17, 17, 18, 16 };
    if ((IsCharacterSuperPowerActive(this, (eCharacterClass)1)
            || IsCharacterSuperPowerActive(this, (eCharacterClass)6)
            || IsCharacterSuperPowerActive(this, (eCharacterClass)11)) && m_bSuperPowerTankOn)
    {
        SetRunningWBAnimState(0.1f);
    }
    else
    {
        int direction = ((mUnidentified024.m_aDesiredFacingDirection - mUnidentified024.m_aActualFacingDirection + 0x2000) >> 14) & 3;
        if (direction != 0)
        {
            SetAnimState(runStartAnims[direction], true, 0.2f, false, false);
            s16 turnAdjust = CalcAnimTurnAdjust(mUnidentified024.m_aActualFacingDirection,
                mUnidentified024.m_aDesiredFacingDirection, m_eAnimID, 1.0f);
            InitMovementFromAnim(turnAdjust, v3Zero, 1.0f, false);
            m_pCurrentAnimController->m_fPlaybackSpeedScale = gStartAnimPlaybackSpeed
                + InterpolateRangeClamped(0.0f, gStartTerrainSpeedBoost, 0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
        }
        else
        {
            SetRunningWBAnimState(0.1f);
            if (mUnidentified024.m_fActualSpeed < GetJogSpeed(m_pTweaks))
                mUnidentified024.m_fActualSpeed = mUnidentified024.m_fDesiredSpeed = GetJogSpeed(m_pTweaks);
        }
    }
}

void cFielder::SetHardStopAnimState()
{
    if (!ShouldSkipHardStopAnim())
    {
        if (m_pBall != 0)
            SetAnimState(24, true, 0.2f, false, false);
        else
            SetAnimState(12, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        m_pCurrentAnimController->m_fPlaybackSpeedScale = gHardStopAnimPlaybackSpeed
            + InterpolateRangeClamped(0.0f, gHardStopTerrainSpeedBoost, 0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
    }
}

void cFielder::SetHardStopRecoverAnimState()
{
    if (m_pBall != 0)
    {
        SetAnimState(26, false, 0.03f, false, false);
    }
    else
    {
        SetAnimState(14, false, 0.03f, false, false);
    }
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gHardStopAnimPlaybackSpeed
        + InterpolateRangeClamped(0.0f, gHardStopTerrainSpeedBoost,
            0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
}

void cFielder::SetHardStopTurnAnimState()
{
    if (m_pBall != 0)
    {
        SetAnimState(25, false, 0.03f, false, false);
    }
    else
    {
        SetAnimState(13, false, 0.03f, false, false);
    }
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gHardStopAnimPlaybackSpeed
        + InterpolateRangeClamped(0.0f, gHardStopTerrainSpeedBoost,
            0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
}

void cFielder::SetRunBackwardsAnimState()
{
    SetAnimState(27, true, 0.067f, true, false);
    PlayerTweaks* tweaks = m_pTweaks;
    InitMovementStrafing(GetStrafeTurnSpeed(tweaks), GetStrafeTurnFalloff(tweaks),
        GetStrafeAccel(tweaks), GetStrafeDecel(tweaks));
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredMovementDirection;
}

void cFielder::SetRunToBackRunningAnimState()
{
    SetAnimState(33, true, 0.067f, true, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredMovementDirection;
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gBackRunningAnimPlaybackSpeed
        + InterpolateRangeClamped(0.0f, gBackRunningTerrainSpeedBoost,
            0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
}

void cFielder::SetBackRunningToRunAnimState()
{
    SetAnimState(34, true, 0.067f, true, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredMovementDirection;
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gBackRunningAnimPlaybackSpeed
        + InterpolateRangeClamped(0.0f, gBackRunningTerrainSpeedBoost,
            0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
}

void cFielder::SetBackRunningStopAnimState()
{
    SetAnimState(30, true, 0.067f, true, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredMovementDirection;
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gBackRunningAnimPlaybackSpeed
        + InterpolateRangeClamped(0.0f, gBackRunningTerrainSpeedBoost,
            0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
}

void cFielder::SetBackRunningStopStartAnimState()
{
    SetAnimState(31, true, 0.067f, true, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredMovementDirection;
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gBackRunningAnimPlaybackSpeed
        + InterpolateRangeClamped(0.0f, gBackRunningTerrainSpeedBoost,
            0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
}

void cFielder::SetBackRunningStopRecoverAnimState()
{
    SetAnimState(32, true, 0.067f, true, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredMovementDirection;
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gBackRunningAnimPlaybackSpeed
        + InterpolateRangeClamped(0.0f, gBackRunningTerrainSpeedBoost,
            0.33f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
}

void cFielder::SetStopAnimState()
{
    if (m_pBall != 0)
    {
        SetStoppingAnimState(this, 0x17);
    }
    else
    {
        SetStoppingAnimState(this, 5);
    }
}

void cFielder::SetStrafeRightStopAnimState()
{
    SetAnimState(36, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gStopAnimPlaybackSpeed;
}

void cFielder::SetStrafeLeftStopAnimState()
{
    SetAnimState(35, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    m_pCurrentAnimController->m_fPlaybackSpeedScale = gStopAnimPlaybackSpeed;
}

void cFielder::SetStrafeLeftAnimState()
{
    SetAnimState(28, true, 0.067f, true, false);
    PlayerTweaks* tweaks = m_pTweaks;
    InitMovementStrafing(GetStrafeTurnSpeed(tweaks), GetStrafeTurnFalloff(tweaks),
        GetStrafeAccel(tweaks), GetStrafeDecel(tweaks));
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredMovementDirection;
}

void cFielder::SetStrafeRightAnimState()
{
    SetAnimState(29, true, 0.067f, true, false);
    PlayerTweaks* tweaks = m_pTweaks;
    InitMovementStrafing(GetStrafeTurnSpeed(tweaks), GetStrafeTurnFalloff(tweaks),
        GetStrafeAccel(tweaks), GetStrafeDecel(tweaks));
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredMovementDirection;
}

void cFielder::SetIdleAnimState()
{
    SetAnimState(0, true, 0.2f, false, false);
    mUnidentified024.m_aDesiredFacingDirection = mUnidentified024.m_aActualFacingDirection;
    mUnidentified024.m_aDesiredMovementDirection = mUnidentified024.m_aActualFacingDirection;
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
    PlayerTweaks* tweaks = m_pTweaks;
    InitMovementRunning(GetJogTurnSpeed(tweaks), fn_8002CF10(tweaks),
        fn_8002C180(tweaks), fn_8002CF24(tweaks));
}

void cFielder::SetIdleWBAnimState()
{
    SetAnimState(15, true, 0.2f, false, false);
    mUnidentified024.m_aDesiredFacingDirection = mUnidentified024.m_aActualFacingDirection;
    mUnidentified024.m_aDesiredMovementDirection = mUnidentified024.m_aActualFacingDirection;
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
    PlayerTweaks* tweaks = m_pTweaks;
    InitMovementRunning(GetJogTurnSpeed(tweaks), GetRunWBTurnFalloff(tweaks),
        GetRunWBAccel(tweaks), GetRunWBDecel(tweaks));
}

void cFielder::RunningSABcallback(unsigned int parameter, cPN_SingleAxisBlender* blender)
{
    cFielder* fielder = (cFielder*)parameter;
    if (fielder->m_eAnimID == 4 || fielder->m_eAnimID == 9 || fielder->m_eAnimID == 0x14)
    {
        float weight = 0.5f * fielder->mUnidentified024.m_fLeanAmount + 0.5f;
        if (fielder->m_pCurrentAnimController->m_bMirror)
        {
            weight = 1.0f - weight;
        }
        if (IsConcurrentStateActive(fielder->m_pAIContext->mScriptMachine, 30))
        {
            weight = 1.0f - weight;
        }
        blender->m_fDesiredWeight = weight;
    }
    else
    {
        blender->m_fDesiredWeight = blender->m_fSmoothedWeight;
    }
}

void cFielder::SetRunningAnimState(float blendTime)
{
    mUnidentified024.m_aDesiredFacingDirection = mUnidentified024.m_aDesiredMovementDirection
        = mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
    const int runningAnims[3] = { 7, 4, 8 };
    const int superAnims[3] = { 10, 9, 11 };
    if (IsSuperGrowActive()
        || (GetCharacterClass() == BOWSER && m_bSuperPowerTankOn)
        || (GetCharacterClass() == WARIO && m_bSuperPowerTankOn)
        || (GetCharacterClass() == PETEY && m_bSuperPowerTankOn))
        SetRunLeanSAB(superAnims, 3, 1);
    else
        SetRunLeanSAB(runningAnims, 3, 1);
    PlayerTweaks* tweaks = m_pTweaks;
    InitMovementRunning(fn_8002C0AC(tweaks), fn_8002CF10(tweaks), fn_8002C180(tweaks), fn_8002CF24(tweaks));
}

void cFielder::SetRunningWBAnimState(float blendTime)
{
    mUnidentified024.m_aDesiredFacingDirection = mUnidentified024.m_aDesiredMovementDirection
        = mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
    int runningAnims[3] = { 0x15, 0x14, 0x16 };
    int superAnims[3] = { 10, 9, 11 };
    if (IsSuperGrowActive()
        || (GetCharacterClass() == BOWSER && m_bSuperPowerTankOn)
        || (GetCharacterClass() == PETEY && m_bSuperPowerTankOn))
        SetRunLeanSAB(superAnims, 3, 1);
    else
        SetRunLeanSAB(runningAnims, 3, 1);
    PlayerTweaks* tweaks = m_pTweaks;
    InitMovementRunning(GetRunWBTurnSpeed(tweaks), GetRunWBTurnFalloff(tweaks), GetRunWBAccel(tweaks), GetRunWBDecel(tweaks));
}

bool cFielder::ShouldIClearBall()
{
    return !CanShootWithoutPossession(this);
}

void cFielder::SetPosition(const nlVector3& v3Position)
{
    cCharacter::SetPosition(v3Position);
}

void cFielder::SetDesiredSpeed(float minSpeed, float maxSpeed)
{
    if (m_pController != 0)
    {
        float speed = 0.0f;
        if (m_pController->GetMovementStickMagnitude() > 0.0f)
        {
            speed = (maxSpeed - minSpeed) * m_pController->GetMovementStickMagnitude() + minSpeed;
        }
        mUnidentified024.m_fDesiredSpeed = speed;
    }
}

float cFielder::GetSpeedPowerupAdjusted(float speed)
{
    float multiplier = 1.0f;
    if (speed >= 0.0f)
    {
        if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 25))
        {
            multiplier *= GetMushroomSpeedBoost(m_pTweaks);
        }
        if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 24))
        {
            multiplier *= GetStarSpeedBoost(m_pTweaks);
        }
        if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 27))
        {
            multiplier *= ((DesireGooey*)GetConcurrentState(
                m_pAIContext->mScriptMachine, 27))->GetSpeedScale();
        }
        if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 28))
        {
            multiplier *= ((DesireShrink*)GetConcurrentState(
                m_pAIContext->mScriptMachine, 28))->GetSpeedScale();
        }
    }
    return multiplier * speed;
}

float cFielder::GetSlideAttackSpeed(int direction)
{
    float speed = GetSpeedPowerupAdjusted(GetSlideSpeed(m_pTweaks));
    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 25)
        || IsConcurrentStateActive(m_pAIContext->mScriptMachine, 24))
    {
        speed *= GetSuperSlideSpeedBonus(m_pTweaks);
    }
    return speed;
}

void cFielder::SetTangible(bool tangible, bool affectGoalLine)
{
    m_pPhysicsCharacter->m_CanCollideWithBall = tangible;
    m_pPhysicsCharacter->m_CanCollideWithCharacters = tangible;
    if (affectGoalLine)
    {
        m_pPhysicsCharacter->m_CanCollideWithGoalLine = tangible;
    }
    if (m_pBall != 0)
    {
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = tangible;
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = tangible;
    }
    mbTangible = tangible;
    if (m_pBall != 0)
    {
        m_pBall->m_bVisible = tangible;
    }
    mUnidentified17C = tangible;
}

void cFielder::EjectMonty(bool finished, unsigned short direction)
{
    nlVector3 velocity;
    nlSinCos(&velocity.y, &velocity.x, direction);
    velocity.x *= 24.0f;
    velocity.y *= 24.0f;
    velocity.z = 10.0f;
    SetVelocity(velocity);
    SetAnimState(0x7c, true, 0.2f, false, false);
    InitMovementCoast();
    m_bMontyDekeFinished = finished;
    SetTangible(true, false);
    mUnidentified178 = 1.0f;
}

void cFielder::EndMontyDeke()
{
    SetAnimState(0x50, false, 0.0f, false, false);
    m_pCurrentAnimController->SetTime(0.5f);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    nlVector3 position = mUnidentified024.m_v3Position;
    position.z = 0.0f;
    SetPosition(position);
    m_bMontyDekeFinished = true;
    mUnidentified178 = 1.0f;
    mUnidentified17C = true;
    EmitMontyDekeExit(this);
}

void cFielder::ShouldIWave()
{
    if (IsCaptain() && m_pBall == 0 && IsAvailableToReceivePass() && m_nPowerupAnimID < 0 && !fn_800976C4())
    {
        if (g_pBall->GetOwnerFielder() != 0
            && g_pBall->GetOwnerFielder()->mUnidentified1E4.m_tBallPossessionTimer.GetSeconds() > 0.5f
            && IsOnSameTeam(g_pBall->GetOwnerFielder())
            && g_pBall->GetOwnerFielder()->DoCalcCanDoPerfectPass(this, mUnidentified024.m_v3Position))
        {
            SetPowerupAnimState(0x5D);
            PlaySound(m_uSoundSlotId, 0x270203ED, 0, 0);
        }
    }
}

void cFielder::TestCollisionForInvincibility(cFielder* pOpponent)
{
    cFielder* pReactee = NULL;
    cFielder* pAttacker = NULL;
    if (!mbTangible)
        return;
    if (!pOpponent->mbTangible)
        return;
    if (IsFrozenStateActive())
        return;
    if (pOpponent->IsFrozenStateActive())
        return;

    bool bUnidentified = false;
    if (pOpponent->m_pBall != NULL)
    {
        if (IsSlideAttacking() && !IsSuperGrowActive())
        {
            float fPlayerScale = mUnidentified024.m_fPlayerScale;
            float fUnidentified = 0.18f
                + fn_8002BFA8(GetTweaks(), fPlayerScale);
            fUnidentified += gSlideBallContactRadiusBuffer;
            if (nlVec3DistanceSquared2D(mUnidentified024.m_v3Position, g_pBall->m_v3Position)
                < fUnidentified * fUnidentified)
                bUnidentified = true;
        }
    }
    else if (m_pBall != NULL)
    {
        if (pOpponent->IsSlideAttacking() && !pOpponent->IsSuperGrowActive())
        {
            float fPlayerScale = pOpponent->mUnidentified024.m_fPlayerScale;
            float fUnidentified = 0.18f
                + fn_8002BFA8(pOpponent->GetTweaks(), fPlayerScale);
            fUnidentified += gSlideBallContactRadiusBuffer;
            if (nlVec3DistanceSquared2D(pOpponent->mUnidentified024.m_v3Position, g_pBall->m_v3Position)
                < fUnidentified * fUnidentified)
                bUnidentified = true;
        }
    }

    if (IsAboveFielder(pOpponent))
        return;
    if (pOpponent->IsAboveFielder(this))
        return;

    if (IsInvincible() && !pOpponent->IsInvincible())
    {
        pReactee = pOpponent;
        pAttacker = this;
    }
    else if (pOpponent->IsInvincible() && !IsInvincible())
    {
        pReactee = this;
        pAttacker = pOpponent;
    }
    else if (IsSuperGrowActive() && !pOpponent->IsSuperGrowActive()
        && !IsFrozenStateActive() && !bUnidentified
        && !pOpponent->IsSuperGrowActive() && !pOpponent->IsInFallAction())
    {
        pReactee = pOpponent;
        pAttacker = this;
        pOpponent->InitActionKnockdownReact(v3Zero);

        PlayerAttackData* pAttackData = g_PlayerAttackDataPool.Allocate();
        pAttackData->pAttacker = this;
        u8 bHasGlobalPad = GetGlobalPad() != NULL;
        pAttackData->nAttackerPadID = bHasGlobalPad ? GetGlobalPad()->GetPadID() : -1;
        pAttackData->pTarget = pOpponent;
        pAttackData->mUnidentified0C = 2;
        pAttackData->mUnidentified10 = false;
        QueueAttackSuccessEvent(g_pGame, pAttackData);
    }
    else if (pOpponent->IsSuperGrowActive() && !IsSuperGrowActive()
        && !pOpponent->IsFrozenStateActive() && !bUnidentified && !IsInFallAction())
    {
        pReactee = this;
        pAttacker = pOpponent;
        InitActionKnockdownReact(v3Zero);

        PlayerAttackData* pAttackData = g_PlayerAttackDataPool.Allocate();
        pAttackData->pAttacker = pOpponent;
        u8 bHasGlobalPad = pOpponent->GetGlobalPad() != NULL;
        pAttackData->nAttackerPadID = bHasGlobalPad ? pOpponent->GetGlobalPad()->GetPadID() : -1;
        pAttackData->pTarget = this;
        pAttackData->mUnidentified0C = 2;
        pAttackData->mUnidentified10 = false;
        QueueAttackSuccessEvent(g_pGame, pAttackData);
    }
    else if (IsInvincibleChars() && !pOpponent->IsInvincibleChars())
    {
        pReactee = pOpponent;
        pAttacker = this;
    }
    else if (pOpponent->IsInvincibleChars() && !IsInvincibleChars())
    {
        pReactee = this;
        pAttacker = pOpponent;
    }

    if (pReactee == NULL)
        return;
    if (pReactee->IsFallenDown())
        return;

    fn_800470B4(pReactee, pAttacker);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.0f);
    if (pAttacker->CanPickupBall(g_pBall, pAttacker->IsSlideAttacking()))
        pAttacker->PickupBall(g_pBall);
}

void cFielder::TestButtonsToQueueActions(float deltaTime)
{
    unsigned short direction = 0;
    if (GetGlobalPad() != 0 && m_pBall != 0)
    {
        if (GetGlobalPad()->JustPressed(27, true))
        {
            bIsModified = bIsModified || IsActionModifierPressed();
            mUnidentified1E4.m_eLastPadAction = 27;
        }
        else if (GetGlobalPad()->JustPressed(28, true))
            mUnidentified1E4.m_eLastPadAction = 28;
        else if (IsDekeRequested(&direction) && m_bDekeReset && m_bSlideAttackReset)
        {
            m_aDekeDirection = direction;
            mUnidentified1E4.m_eLastPadAction = 29;
        }
    }
}

bool cFielder::TestQueuedActions()
{
    bool result = false;
    if (mUnidentified1E4.m_eLastPadAction == 28 && GetGlobalPad() != 0
        && !GetGlobalPad()->IsPressed(28, true))
    {
        result = InitActionShot(bIsModified, false);
    }
    else if (mUnidentified1E4.m_eLastPadAction == 27)
    {
        bool volley = bIsModified;
        result = InitActionPass(fn_80096F54(this, volley), volley, 0, false);
    }
    else if (mUnidentified1E4.m_eLastPadAction == 29)
        result = fn_800447C0(m_aDekeDirection);
    return result;
}

void cFielder::TestLooseBallControls(bool forceContact)
{
    if (mUnidentified1E4.m_bCanTestController && GetGlobalPad() != 0)
    {
        unsigned short hitDirection = 0;
        unsigned short dekeDirection = 0;
        if (GetGlobalPad()->JustPressed(27, true))
        {
            if (CanContactLooseBall(false) || forceContact)
            {
                bool volley = IsActionModifierPressed();
                cFielder* target = static_cast<cFielder*>(fn_80096F54(this, volley));
                InitActionLooseBallPass(target, volley);
                if (m_eActionState != ACTION_LOOSE_BALL_PASS && m_eActionState != ACTION_LOOSE_BALL_SHOT
                    && m_pTeam->mfBallInTimes[mUnidentified1E4.m_ID] <= gLooseBallContactBufferTime)
                {
                    UnidentifiedVariantCollection parameters;
                    parameters.Set(7, FuzzyVariant(0.1f + gLooseBallContactBufferTime));
                    parameters.Set(14, FuzzyVariant(g_pBall));
                    parameters.Set(16, FuzzyVariant(volley));
                    parameters.Set(0, FuzzyVariant((cPlayer*)target));
                    parameters.Set(1, FuzzyVariant(false));
                    parameters.Set(10, FuzzyVariant((void*)TransDesireLooseBallContact));
                    m_pAIContext->mScriptMachine->ActivateState(13, &parameters, true);
                    mUnidentified1E4.m_bCanTestController = false;
                }
            }
        }
        else if (GetGlobalPad()->JustPressed(28, true))
        {
            if (CanContactLooseBall(false) || forceContact)
            {
                bool modified = IsActionModifierPressed();
                InitActionLooseBallShot(modified);
                if (m_eActionState != ACTION_LOOSE_BALL_PASS && m_eActionState != ACTION_LOOSE_BALL_SHOT
                    && m_pTeam->mfBallInTimes[mUnidentified1E4.m_ID] <= gLooseBallContactBufferTime)
                {
                    UnidentifiedVariantCollection parameters;
                    parameters.Set(7, FuzzyVariant(0.1f + gLooseBallContactBufferTime));
                    parameters.Set(14, FuzzyVariant(g_pBall));
                    parameters.Set(16, FuzzyVariant(modified));
                    parameters.Set(1, FuzzyVariant(true));
                    parameters.Set(10, FuzzyVariant((void*)TransDesireLooseBallContact));
                    m_pAIContext->mScriptMachine->ActivateState(13, &parameters, true);
                }
            }
        }
        else if (IsReceivePassDekeRequested(&dekeDirection))
            InitActionSlideAttack(0, -1.0f, dekeDirection);
        else if (IsReceivePassHitRequested(&hitDirection))
        {
            unsigned short direction = GetActualFacing();
            if (m_pController != 0 && m_pController->GetMovementStickMagnitude() > 0.001f)
                direction = m_pController->GetMovementStickDirection();
            InitActionHit(0, direction);
        }
    }
}

void cFielder::TestButtonsWindup()
{
    unsigned short direction = 0;
    if (GetGlobalPad()->JustPressed(27, true))
    {
        bool modified = IsActionModifierPressed();
        InitActionPass(fn_80096F54(this, modified), modified, 0, false);
    }
    else if (IsDekeRequested(&direction))
        fn_800447C0(direction);
}

void cFielder::TestButtonsRunningWB(float deltaTime)
{
    if (GetCharacterClass() == (eCharacterClass)8 && !GetGlobalPad()->IsPressed(28, true))
        bYoshiInWindup = false;
    unsigned short direction = 0;
    if (GetGlobalPad()->JustPressed(27, true))
    {
        bool modified = IsActionModifierPressed();
        InitActionPass(fn_80096F54(this, modified), modified, 0, false);
    }
    else if (GetGlobalPad()->IsPressed(28, true))
    {
        if (GetCharacterClass() == (eCharacterClass)8)
        {
            if (!bYoshiInWindup)
                fn_8004B658();
        }
        else
            fn_8004B658();
    }
    else if (IsDekeRequested(&direction))
        fn_800447C0(direction);

    if (GetGlobalPad()->JustPressed(23, true))
        GetGlobalPad()->ResetButtonStateTicks(23, true);
    else if (GetGlobalPad()->IsPressed(23, true))
        mActionRunPassVars.nHeldTicks = GetGlobalPad()->GetButtonStateTicks(23, true);
    else if (GetGlobalPad()->JustReleased(23, true))
    {
        if (GetCurrentAnimID() == 24 || GetCurrentAnimID() == 25)
            mActionRunningWBVars.bCuePitch = true;
        else
            fn_8004B148();
    }
}

void cFielder::TestAnimBallContact()
{
    if (m_pCurrentAnimController->TestTrigger(m_fOneTimerAnimTime)
        && g_pBall->m_pOwner == 0 && g_pBall->m_tNoPickupTimer.m_uPackedTime == 0
        && g_pBall->meBallState != 10)
    {
        nlVector3 newBallPosition;
        g_pBall->m_pPhysicsBall->GetPosition(&newBallPosition);
        nlVector3 oldBallPosition = g_pBall->GetPosition();
        float contactRadius = g_pBall->fn_80014F38(GetPlayerScale());
        float ballRadius = g_pBall->fn_80014F38(1.0f);
        int jointIndex = m_nBallJointIndex;
        if (TestCollision(contactRadius, GetPrevJointPosition(jointIndex), GetJointPosition(jointIndex),
                ballRadius, oldBallPosition, newBallPosition))
        {
            g_pBall->SetPosition(GetJointPosition(m_nBallJointIndex));
            switch (m_eActionState)
            {
            case ACTION_RECEIVE_PASS:
                PickupBall(g_pBall);
                break;
            case ACTION_LOOSE_BALL_PASS:
            {
                g_pBall->SetOwner(this);
                mUnidentified1E4.m_tBallPossessionTimer.UnidentifiedClear();
                mUnidentified1E4.m_tBallUnPossessionTimer.UnidentifiedClear();
                float slow = GetSlowestVolleyPassSpeed(m_pTweaks);
                float fast = GetFastestVolleyPassSpeed(m_pTweaks);
                if (!bIsModified)
                {
                    slow = GetSlowestGroundPassSpeed(m_pTweaks);
                    fast = GetFastestGroundPassSpeed(m_pTweaks);
                }
                DoRegularPassing(mActionLooseBallPassVars.passTarget, bIsModified, true, false, false, slow, fast);
                m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
                break;
            }
            case ACTION_LOOSE_BALL_SHOT:
            case ACTION_ONETIMER:
            {
                m_pShotMeter->Reset(this);
                m_pShotMeter->m_fTime = 0.0f;
                bool perfectPass;
                switch (m_eAnimID)
                {
                case 60: case 61: case 62: case 63: case 64: case 65: case 66:
                case 67: case 68: case 69: case 70: case 71: case 72: case 73:
                    perfectPass = true;
                    break;
                default:
                    perfectPass = false;
                    break;
                }
                m_pShotMeter->CalcOneTimerValue(this, perfectPass);
                g_pBall->SetOwner(this);
                mUnidentified1E4.m_tBallPossessionTimer.UnidentifiedClear();
                mUnidentified1E4.m_tBallUnPossessionTimer.UnidentifiedClear();
                if (!ShouldIClearBall())
                {
                    DoRegularShooting(false);
                    DeliverShotPresentationEndEvent(g_pGame);
                    EmitBallShot(this, (eBallShotEffectType)2, 0, false, true);
                }
                else
                {
                    DoClearBall();
                    EmitBallShot(this, (eBallShotEffectType)1, 0, false, false);
                }
                FixedUpdateTask::GetTargetTimeScale();
                break;
            }
            }
        }
    }
}

PlayerTweaks* cFielder::GetTweaks() const
{
    return m_pTweaks;
}

bool cFielder::IsSuperPowerActive() const
{
    return IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x17);
}

bool cFielder::IsStarActive() const
{
    return IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x18);
}

bool cFielder::IsConfused() const
{
    return IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1E);
}

bool cFielder::IsMushroomActive() const
{
    return IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x19);
}

bool cFielder::IsSlippery() const
{
    return IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1A);
}

bool cFielder::IsShrunk() const
{
    return IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1C);
}

bool cFielder::IsSuperGrowActive() const
{
    return IsMarioSuperPowerActive() || IsLuigiSuperPowerActive();
}

bool cFielder::IsMarioSuperPowerActive() const
{
    return GetCharacterClass() == (eCharacterClass)0 && IsSuperPowerActive();
}

bool cFielder::IsLuigiSuperPowerActive() const
{
    return GetCharacterClass() == (eCharacterClass)4 && IsSuperPowerActive();
}

bool IsBowserSuperPowerActive(const cFielder* pFielder)
{
    bool active;
    GetCharacterSpecialActive(pFielder, (eCharacterClass)1, active);
    return active;
}

bool cFielder::IsPeachSuperPowerActive() const
{
    bool active;
    GetCharacterSpecialActive(this, PEACH, active);
    return active;
}

bool IsWaluigiSuperPowerActive(const cFielder* pFielder)
{
    bool active;
    GetCharacterSpecialActive(pFielder, WALUIGI, active);
    return active;
}

bool cFielder::IsWarioSuperPowerActive() const
{
    bool active;
    GetCharacterSpecialActive(this, WARIO, active);
    return active;
}

bool cFielder::IsPeteySuperPowerActive() const
{
    bool active;
    GetCharacterSpecialActive(this, PETEY, active);
    return active;
}

bool cFielder::IsDKSuperPowerActive() const
{
    bool result = false;
    if (GetCharacterClass() == DONKEYKONG
        && m_eActionState == (eFielderActionState)0x1D)
    {
        result = true;
    }
    return result;
}

bool cFielder::IsYoshiSuperPowerActive() const
{
    bool active;
    GetCharacterSpecialActive(this, YOSHI, active);
    return active;
}

void cFielder::UpdateTimers(float deltaTime)
{
    bool isGameplay = g_pGame->IsGameplayOrOvertime();
    if (isGameplay)
    {
        if (mtPowerupThrowTime.m_uPackedTime != 0)
        {
            if (mtPowerupThrowTime.Countdown(deltaTime, 0.0f))
            {
                DesireUsePowerup* desire = (DesireUsePowerup*)GetConcurrentState(m_pAIContext->mScriptMachine, 17);
                ThrowPowerup(desire);
            }
        }
        if (!mUnidentified1E4.m_bSkipActionUpdate && GetCharacterClass() == (eCharacterClass)16
            && mtPostDekeTimer.m_uPackedTime != 0)
        {
            mtPostDekeTimer.Countdown(deltaTime, 0.0f);
            float seconds = mtPostDekeTimer.GetSeconds();
            float fraction = seconds / gBooDekeFadeTime;
            if (fraction < 0.1f)
                mUnidentified178 = 1.0f - (1.0f - gBooDekeAlpha) * (fraction / 0.1f);
            else
                mUnidentified178 = gBooDekeAlpha;
        }
    }
}

bool cFielder::ActivateDesire(int state, UnidentifiedVariantCollection* parameters)
{
    return m_pAIContext->mScriptMachine->ActivateState(state, parameters, true) != 0;
}

bool cFielder::InitDesire(eFielderDesireState eDesireType, float fConfidence,
    float fDuration, const FuzzyVariant& opt1, const FuzzyVariant& opt2)
{
    UnidentifiedVariantCollection params;
    params.Set(7, FuzzyVariant(fDuration));
    params.Set(4, FuzzyVariant(fConfidence));
    params.Set(0, FuzzyVariant(opt1));
    params.Set(1, FuzzyVariant(opt2));

    bool bDesireInitSuccess = m_pAIContext->mScriptMachine
        ->ActivateState(eDesireType, &params, true) != 0;
    return bDesireInitSuccess;
}

void cFielder::PreUpdate(float fTime)
{
    cPlayer::PreUpdate(fTime);
    m_bHasBeenUpdated = false;
    mbWasHitByPowerupThisFrame = false;
}

void cFielder::PrePhysicsUpdate()
{
    cPlayer::PrePhysicsUpdate();

    DesireFrozen* pAction = (DesireFrozen*)
        GetConcurrentState(m_pAIContext->mScriptMachine, 0x1D);
    bool bActionActive = false;
    if (pAction != 0 && pAction->mActive
        && pAction->meFrozenState != DesireFrozen::FROZEN_NONE)
    {
        bActionActive = true;
    }

    if (!bActionActive
        && (GetActionState() == ACTION_RECEIVE_PASS
            || GetActionState() == ACTION_ONETIMER
            || GetActionState() == ACTION_LOOSE_BALL_SHOT
            || GetActionState() == ACTION_LOOSE_BALL_PASS))
    {
        TestAnimBallContact();
    }

    Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
    if (pGoalie->mGoalieActionState == GOALIEACTION_UNIDENTIFIED_13
        && pGoalie->mpTarget == this)
    {
        pGoalie->fn_80080BFC(0.0f);
    }
}

void cFielder::Update(float fDeltaT)
{
    SetPlayerAudioController(this);
    UpdateTimers(fDeltaT);
    cPlayer::Update(fDeltaT);
    m_pAIContext->Update(true, fDeltaT);

    if (!mUnidentified1E4.m_bSkipActionUpdate)
    {
        UpdateActionState(fDeltaT);
        UpdateHeadTracking(fDeltaT);
    }
    else
    {
        SetPosition(mUnidentified024.m_v3PrevPosition);
    }

    if (!mUnidentified1E4.m_bSkipAnimUpdate)
    {
        cCharacter::Update(fDeltaT);
    }
    else if (mUnidentified1E4.m_bForceFeatherUpdate && fn_800976C4())
    {
        cCharacter::Update(0.0f);
        m_pPowerupLayer->SetChild(
            1, m_pPowerupLayer->GetChild(1)->Update(fDeltaT));
    }

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)6
        && mWaluigiWallState.mUnidentified08 != 0)
    {
        mWaluigiWallState.mUnidentified08->Update(fDeltaT);
    }
    UpdateController(fDeltaT);
    m_bHasBeenUpdated = true;
}

ePowerUpType cFielder::GetPowerupType()
{
    return ((DesireUsePowerup*)GetConcurrentState(
        m_pAIContext->mScriptMachine, 0x11))->GetPowerupType();
}

void cFielder::UseCaptainPowerup()
{
    if (IsCaptain())
        fn_800D38D0((DesireUsePowerup*)GetConcurrentState(m_pAIContext->mScriptMachine, 17));
    else
        m_pTeam->GetCaptain()->UseCaptainPowerup();
}

void cFielder::UseTeamPowerup(cFielder* pTarget)
{
    ((DesireUsePowerup*)GetConcurrentState(
        m_pAIContext->mScriptMachine, 0x11))->fn_800D3968(
        pTarget, POWER_UP_NONE, true);
}

void cFielder::UpdateActionState(float dt)
{
    switch (m_eActionState)
    {
    case 1:
        fn_80044BEC(dt);
        break;
    case ACTION_ELECTROCUTION:
        ActionElectrocution(dt);
        break;
    case 3:
        fn_8004643C(dt);
        break;
    case 0x18:
        fn_80045C74(dt);
        break;
    case ACTION_HIT:
        ActionHit(dt);
        break;
    case 0:
        fn_80043C18(dt);
        break;
    case 0x23:
        fn_80044290(dt);
        break;
    case 5:
    case ACTION_HIT_REACT:
        fn_800474FC(dt);
        break;
    case ACTION_LATE_ONETIMER_FROM_VOLLEY:
        ActionLateOneTimerFromVolley(dt);
        break;
    case ACTION_IDLE_TURN:
        ActionIdleTurn(dt);
        break;
    case ACTION_LOOSE_BALL_PASS:
        fn_80048484(dt);
        break;
    case ACTION_LOOSE_BALL_SHOT:
        fn_800486DC(dt);
        break;
    case ACTION_ONETIMER:
        fn_80049EA0(dt);
        break;
    case ACTION_ONETOUCH_PASS_FROM_VOLLEY:
        ActionOneTouchPassFromVolley(dt);
        break;
    case ACTION_PASS:
        ActionPass(dt);
        break;
    case ACTION_POST_WHISTLE:
        ActionPostWhistle(dt);
        break;
    case 0x11:
        ActionReceivePass(dt);
        break;
    case ACTION_RUNNING:
        ActionRunning(dt);
        break;
    case ACTION_RUNNING_WB:
        ActionRunningWB(dt);
        break;
    case 0x13:
        fn_8004B2E4(dt);
        break;
    case ACTION_UNKNOWN_15:
        fn_8004C02C(dt);
        break;
    case ACTION_SLIDE_ATTACK:
        fn_8004C88C(dt);
        break;
    case ACTION_SLIDE_ATTACK_REACT:
        ActionSlideAttackReact(dt);
        break;
    case ACTION_BOMB_REACT:
        ActionBombReact(dt);
        break;
    case 0x1B:
        ActionBananaReact(dt);
        break;
    case 0x1A:
        ActionShellReact(dt);
        break;
    case 0x1C:
        ActionKnockdownReact(dt);
        break;
    case 0x1D:
        if (!g_pGame->IsGameplayOrOvertime() && ShouldStartCrossBlend(4))
        {
            StartRunning();
        }
        break;
    case ACTION_WAIT:
        ActionWait(dt);
        break;
    case ACTION_UNKNOWN_30:
        fn_8004BB80(dt);
        break;
    case ACTION_UNKNOWN_31:
        fn_8004E228();
        break;
    case ACTION_UNKNOWN_32:
        fn_8004EAB4(dt);
        break;
    case 0x21:
        fn_8004EE48(dt);
        break;
    case ACTION_UNKNOWN_34:
        fn_8004F2FC(dt);
        break;
    }
}

void cFielder::UpdateHeadTracking(float fDeltaT)
{
    m_pHeadTrack->m_fSmoothTime = gHeadTrackingSmoothTime;

    if ((IsBowserSuperPowerActive(this) || IsPeteySuperPowerActive()) && m_bSuperPowerTankOn)
    {
        if (gUseBowserPeteyHeadTracking)
        {
            cPlayer* pUnidentified = g_pBall->m_pOwner;
            if (pUnidentified != 0)
            {
                if (IsOnSameTeam(pUnidentified)
                    || nlVec3DistanceSquared2D(pUnidentified->mUnidentified024.m_v3Position,
                           mUnidentified024.m_v3Position) > 36.0f
                    || fn_800DDF54(this, pUnidentified) < 0.6f)
                {
                    pUnidentified = 0;
                }
            }
            if (pUnidentified == 0)
            {
                FindHeadTrackingHitTarget(this, pUnidentified);
            }

            nlVector3 v3Unidentified;
            if (pUnidentified == 0)
            {
                nlVector3 v3Unidentified0 = GetJointPosition(m_nHeadJointIndex);
                const nlMatrix4& m4Unidentified
                    = m_pPoseAccumulator->GetNodeMatrix(m_nHeadJointIndex);
                nlVector3 v3Unidentified1;
                nlVec3Set(v3Unidentified1,
                    m4Unidentified.m11, m4Unidentified.m12, m4Unidentified.m13);
                nlVec3ScaleAdd(v3Unidentified, 5.0f,
                    v3Unidentified1, v3Unidentified0);
            }
            else
            {
                v3Unidentified = pUnidentified->mUnidentified024.m_v3Position;
            }
            v3Unidentified.z = gSuperPowerHeadTrackingHeight;
            m_pHeadTrack->m_v3OOI = v3Unidentified;
            m_pHeadTrack->m_bTrackOOI = true;
        }
        else
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        return;
    }

    if (IsSuperGrowActive() && m_pBall == 0)
    {
        cPlayer* hitTarget;
        FindHeadTrackingHitTarget(this, hitTarget);
        if (hitTarget != 0)
        {
            m_pHeadTrack->m_v3OOI
                = hitTarget->GetJointPosition(hitTarget->m_nBip01JointIndex_0xA4);
            m_pHeadTrack->m_bTrackOOI = true;
            return;
        }
    }

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10)
    {
        m_pHeadTrack->m_bTrackOOI = false;
        return;
    }

    if (IsConcurrentStateActive(m_pAIContext->mScriptMachine, 0x1E)
        && !IsFallenDown()
        && (mUnidentified024.m_eCharacterClass != (eCharacterClass)0xC || m_pBall == 0))
    {
        float fUnidentified = (int)g_pGame->GetGameTime();
        nlVector3 v3Unidentified;
        if (g_pGame->GetGameTime() - fUnidentified < 0.25f
            || (g_pGame->GetGameTime() - fUnidentified > 0.5f
                && g_pGame->GetGameTime() - fUnidentified < GetHeadTrackingFinalPhaseTime()))
        {
            v3Unidentified = m_pTeam->m_pNet->m_v3NetLocation;
        }
        else
        {
            v3Unidentified = m_pTeam->GetOtherNet()->m_v3NetLocation;
        }
        v3Unidentified.z = 5.0f + nlRandomf(30.0f);
        m_pHeadTrack->m_v3OOI = v3Unidentified;
        m_pHeadTrack->m_bTrackOOI = true;
        return;
    }

    switch (m_eActionState)
    {
    case ACTION_HIT:
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)8)
        {
            m_pHeadTrack->m_fSmoothTime = 0.005f;
        }
    case 29:
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)9)
        {
            m_pHeadTrack->m_fSmoothTime = 0.005f;
        }
    case ACTION_NEED_ACTION:
    case 0:
    case 1:
    case ACTION_ELECTROCUTION:
    case 5:
    case ACTION_HIT_REACT:
    case ACTION_LATE_ONETIMER_FROM_VOLLEY:
    case ACTION_SHOT:
    case ACTION_SHOOT_TO_SCORE:
    case ACTION_ONETOUCH_PASS_FROM_VOLLEY:
    case ACTION_UNKNOWN_15:
    case ACTION_SLIDE_ATTACK_REACT:
    case ACTION_BOMB_REACT:
    case ACTION_SHELL_REACT:
    case ACTION_BANANA_REACT:
    case 28:
    case ACTION_UNKNOWN_31:
    case ACTION_UNKNOWN_32:
    case 33:
    case ACTION_UNKNOWN_34:
    case 35:
        m_pHeadTrack->m_bTrackOOI = false;
        break;

    case 3:
    case 24:
        m_pHeadTrack->m_bTrackOOI = true;
        if (m_fFallingTime > 0.0f)
        {
            nlVector3 v3Unidentified = mUnidentified024.m_v3Position;
            v3Unidentified.z -= 20.0f;
            m_pHeadTrack->m_v3OOI = v3Unidentified;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->GetPosition();
        }
        break;

    case ACTION_UNKNOWN_30:
        m_pHeadTrack->m_v3OOI = m_pTeam->GetOtherNet()->m_v3NetLocation;
        m_pHeadTrack->m_bTrackOOI = true;
        break;

    case ACTION_ONETIMER:
        switch (m_eAnimID)
        {
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
            m_pHeadTrack->m_bTrackOOI = false;
            return;
        default:
            break;
        }

        if (m_pCurrentAnimController->m_fTime > m_fOneTimerAnimTime)
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->GetPosition();
            m_pHeadTrack->m_bTrackOOI = true;
        }
        break;

    case ACTION_RECEIVE_PASS:
        if (m_pCurrentAnimController->m_fTime > 0.5f * m_fOneTimerAnimTime)
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->GetPosition();
            m_pHeadTrack->m_bTrackOOI = true;
        }
        break;

    case ACTION_LOOSE_BALL_PASS:
    case ACTION_LOOSE_BALL_SHOT:
        if (m_pCurrentAnimController->m_fTime > m_fOneTimerAnimTime)
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->GetPosition();
            m_pHeadTrack->m_bTrackOOI = true;
        }
        break;

    case ACTION_PASS:
    case 19:
    case ACTION_SLIDE_ATTACK:
        if (m_pBall == 0)
        {
            if (m_eAnimID != 0x27)
            {
                m_pHeadTrack->m_v3OOI = g_pBall->GetPosition();
                m_pHeadTrack->m_bTrackOOI = true;
                break;
            }
        }
        m_pHeadTrack->m_bTrackOOI = false;
        break;

    case ACTION_RUNNING_WB:
        if (!gNPCManager->mpChainChomp->IsHidden())
        {
            m_pHeadTrack->m_v3OOI = gNPCManager->mpChainChomp->GetPosition();
            m_pHeadTrack->m_bTrackOOI = true;
        }
        else
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        break;

    case ACTION_IDLE_TURN:
    case ACTION_RUNNING:
        if (!gNPCManager->mpChainChomp->IsHidden())
        {
            m_pHeadTrack->m_v3OOI = gNPCManager->mpChainChomp->GetPosition();
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->GetPosition();
        }
        m_pHeadTrack->m_bTrackOOI = true;
        break;

    case ACTION_POST_WHISTLE:
    {
        cPlayer* pScorer = g_pGame->m_pScorer;
        if (pScorer != 0)
        {
            m_pHeadTrack->m_v3OOI = pScorer->mUnidentified024.m_v3Position;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->GetPosition();
        }
        m_pHeadTrack->m_bTrackOOI = true;
        break;
    }

    case ACTION_WAIT:
        m_pHeadTrack->m_v3OOI = g_pBall->GetPosition();
        m_pHeadTrack->m_bTrackOOI = true;
        break;
    }
}

void cFielder::UpdateController(float fDeltaT)
{
    bool bUnidentified = false;
    if (GetGlobalPad() != NULL
        && GetGlobalPad()->IsPressed(PAD_SWITCH, true))
    {
        int nUnidentified = GetGlobalPad()->GetButtonStateTicks(PAD_SWITCH, true);
        if (nUnidentified * FixedUpdateTask::GetPhysicsUpdateTick() > 0.33f
            && m_pTeam->GetCaptain() != this)
        {
            GetGlobalPad()->ResetButtonStateTicks(PAD_SWITCH, true);
            m_pTeam->GetCaptain()->mbIgnorePadSwitchRelease = true;
            bUnidentified = true;
        }
    }

    if (g_pGame->IsGameplayOrOvertime())
    {
        if (GetGlobalPad() != NULL
            && ((GetGlobalPad()->JustPressed(PAD_SWITCH, true)
                    && !(GetGlobalPad() != NULL
                            ? GetGlobalPad()->IsPressed(0x17, true)
                            : false))
                || bUnidentified))
        {
            switch (m_eActionState)
            {
            case ACTION_NEED_ACTION:
            case ACTION_PASS:
            case ACTION_POST_WHISTLE:
            case (eFielderActionState)0x13:
            {
                if (IsStuck())
                {
                    if (m_pBall == NULL)
                    {
                        SwapController(bUnidentified);
                    }
                }
                break;
            }

            case ACTION_SHOT:
            case ACTION_SHOOT_TO_SCORE:
            case ACTION_RUNNING_WB:
            case ACTION_UNKNOWN_30:
            {
                if (IsStuck())
                {
                    if (m_pBall == NULL)
                    {
                        SwapController(bUnidentified);
                        break;
                    }
                }

                if (m_pBall == NULL)
                {
                    SwapController(bUnidentified);
                }
                break;
            }

            case (eFielderActionState)0x00:
            case (eFielderActionState)0x01:
            case ACTION_ELECTROCUTION:
            case (eFielderActionState)0x03:
            case ACTION_HIT:
            case (eFielderActionState)0x05:
            case ACTION_HIT_REACT:
            case ACTION_IDLE_TURN:
            case ACTION_LATE_ONETIMER_FROM_VOLLEY:
            case ACTION_LOOSE_BALL_PASS:
            case ACTION_LOOSE_BALL_SHOT:
            case ACTION_ONETIMER:
            case ACTION_ONETOUCH_PASS_FROM_VOLLEY:
            case ACTION_RECEIVE_PASS:
            case ACTION_RUNNING:
            case ACTION_UNKNOWN_15:
            case ACTION_SLIDE_ATTACK:
            case ACTION_SLIDE_ATTACK_REACT:
            case (eFielderActionState)0x18:
            case ACTION_BOMB_REACT:
            case ACTION_SHELL_REACT:
            case ACTION_BANANA_REACT:
            case (eFielderActionState)0x1C:
            case (eFielderActionState)0x1D:
            case ACTION_UNKNOWN_31:
            case ACTION_UNKNOWN_32:
            case (eFielderActionState)0x21:
            case ACTION_UNKNOWN_34:
            case (eFielderActionState)0x23:
            case ACTION_WAIT:
                if (m_pBall == NULL)
                {
                    SwapController(bUnidentified);
                }
                break;
            }
        }

        if (GetGlobalPad() != NULL
            && GetGlobalPad()->JustReleased(PAD_SWITCH, true))
        {
            mbIgnorePadSwitchRelease = false;
        }
    }
}

void cFielder::AddDesiredPosition(const nlVector3& position, float urgency, float weight)
{
    DesireSteering* steering = (DesireSteering*)GetConcurrentState(m_pAIContext->mScriptMachine, 34);
    if (steering != 0 && steering->mActive)
    {
        AddSteeringTarget(steering, position, urgency, weight);
    }
}

const nlVector3& cFielder::GetDesiredPosition()
{
    DesireSteering* steering = (DesireSteering*)GetConcurrentState(m_pAIContext->mScriptMachine, 34);
    if (steering != 0 && steering->mActive)
    {
        return *GetSteeringTargetPosition(steering);
    }
    return mUnidentified024.m_v3Position;
}

float cFielder::GetDistanceToDesiredPos()
{
    const nlVector3& desiredPosition = GetDesiredPosition();
    nlVector2 delta = { mUnidentified024.m_v3Position.x - desiredPosition.x,
        mUnidentified024.m_v3Position.y - desiredPosition.y };
    return nlVec2Length(delta);
}

const nlVector3& cFielder::GetDesiredVelocity()
{
    DesireSteering* steering = (DesireSteering*)GetConcurrentState(m_pAIContext->mScriptMachine, 34);
    if (steering != 0 && steering->mActive)
    {
        return steering->m_v3DesiredVel;
    }
    return v3Zero;
}

void cFielder::StartRunning()
{
    if (!IsRunning() && !IsRunningWithBall())
    {
        if (m_pBall != 0)
        {
            InitActionRunningWB(false);
            return;
        }
        InitActionRunning();
    }
}

void cFielder::Reset(const nlVector3& v3Position, unsigned short aDirection)
{
    cPlayer::Reset(v3Position, aDirection);
    m_pAIContext->mScriptMachine->Reset(false);
    CleanUpAction(ACTION_NEED_ACTION);
    mtPowerupThrowTime.UnidentifiedClear();
    m_tMoveToTurboTimer.UnidentifiedClear();
    ClearPowerupAnimState(true);

    mfAirInterceptHeight[0] = -1.0f;
    mfAirInterceptHeight[1] = -1.0f;
    m_bHasBeenUpdated = false;
    m_eActionState = ACTION_NEED_ACTION;
    m_bInPosition = false;
    m_nPowerupAnimID = -1;
    m_eRole = (eRole)0;
    mbWasHitByPowerupThisFrame = false;
    mbTangible = true;
    mbIgnorePadSwitchRelease = false;
    for (int i = 0; i < 4; i++)
    {
        m_pMark[i] = 0;
    }
    m_tMoveToTurboTimer.UnidentifiedClear();
    mtPostDekeTimer.UnidentifiedClear();
    mtPowerupThrowTime.UnidentifiedClear();
    muInvincibleStatus = 0;
    m_nMegaStrikeResultState = 0;
    mUnidentified178 = 1.0f;
    mActionCrowdVars.bHasBeenSuckedToMiddle = false;
    mActionCrowdVars.fStuckInRiotTime = -1.0f;
    m_aDekeDirection = 0;
    m_bDekeReset = false;
    m_nDPadDownCounter = 2;
    m_fElectrocutionTime = 0.0f;
    m_fElectrocutionLiftTime = 0.0f;
    m_bGroundElectrocution = false;
    m_fFallingTime = 0.0f;
    m_v3SuckToSpot = v3Zero;
    m_fHitDistance = 0.0f;
    m_bHitReactFrameLock = false;
    bYoshiInWindup = false;
    m_fMegaStrikeNumBalls = 0.0f;
    m_fMegaStrikeAccuracy = 0.0f;
    m_fMegaStrikeReceivedTimestamp = -1.0f;
    bIsModified = false;
    mActionLooseBallPassVars.passTarget = 0;
    m_fOneTimerAnimTime = 0.0f;
    m_pPassTarget = 0;
    m_bAllowLeadPass = false;
    m_bOneTouchPass = false;
    mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
    mActionRunningVars.bFirstCycleOfTurbo = false;
    mActionRunningWBVars.bWaitForAnimToFinish = false;
    mActionRunningWBVars.bCuePitch = false;
    m_eSlideAttackState = 0;
    bAttackSucceeded = false;
    m_bSlideAttackReset = true;
    m_nStunnedAngularAcceleration = 0;
    m_nStunnedAngularVelocity = 0;
    m_bSuperPowerTankOn = false;
    m_bSuperPowerTankShutdownPending = false;
    m_fSuperPowerTankLevel = 0.0f;
    m_fSuperPowerTankCapacity = 0.0f;
    mActionBowserSuper.nextFireballTime = 0.0f;
    mActionBowserSuper.fireballStageTime = 0.0f;
    mActionBowserSuper.fireballStageNum = 0;
    mActionBowserSuper.fn_800504A4();
    mWaluigiWallState.mUnidentified00 = 0.0f;
    mWaluigiWallState.mUnidentified04 = 0.0f;
    mWaluigiWallState.fn_800504A8();
    nlVec3Set(mActionBooSkillshot.v3StartPosition, 0.0f, 0.0f, 0.0f);
    mActionBooSkillshot.bFollowingBall = false;
    m_bMontyDekeFinished = false;
    m_fMegaStrikeFirstPressPosition = -1.0f;
    m_fMegaStrikeSecondPressPosition = -1.0f;
    m_fMegaStrikeGreenRegionWidth = -1.0f;
    m_fMegaStrikeMeterPosition = -1.0f;
    m_fMegaStrikePhaseTimeRemaining = 0.0f;
    m_fMegaStrikeSecondPhaseDuration = 0.0f;
    m_fMegaStrikeFirstPhaseDuration = 0.0f;
    m_bMegaStrikeSecondPhase = false;
    m_fLocalMegaStrikeNumBalls = 0.0f;
    m_fLocalMegaStrikeAccuracy = 0.0f;
    m_fMegaStrikeSegment1Position = 0.0f;
    m_fMegaStrikeSegment2Position = 0.0f;
    m_fMegaStrikeSegment3Position = 0.0f;
    m_fMegaStrikeSegment4Position = 0.0f;
    mUnidentified3D4 = 0.0f;
    InitDesire(
        (eFielderDesireState)0x1F, 0.5f, -1.0f, fvNotSet, fvNotSet);
    InitActionWait();
}

void cFielder::ResetEffects()
{
    cCharacter::ResetEffects();
    if (mWaluigiWallState.mUnidentified08 != 0)
    {
        mWaluigiWallState.mUnidentified08->ClearWalls();
    }
    if (m_pBulletBill != 0)
    {
        m_pBulletBill->Hide(true);
    }
}

void cFielder::SetMegaStrikeResult(float numBalls, float accuracy)
{
    m_fMegaStrikeNumBalls = numBalls;
    m_nMegaStrikeResultState = 2;
    m_fMegaStrikeAccuracy = accuracy;
    m_fMegaStrikeReceivedTimestamp = GetFixedUpdateTask()->mSimulationTime;
}

u16 sFielderType = 0xFFFF;
u16 sActCrowdVarsType = 0xFFFF;
u16 sActDekeVarsType = 0xFFFF;
u16 sActElectVarsType = 0xFFFF;
u16 sActFallVarsType = 0xFFFF;
u16 sActHitVarsType = 0xFFFF;
u16 sActHitReactVarsType = 0xFFFF;
u16 sActSuperVarsType = 0xFFFF;
u16 sActShootPassCommonType = 0xFFFF;
u16 sActLooseBallPassType = 0xFFFF;
u16 sActOneTimerVarsType = 0xFFFF;
u16 sActPassingVarsType = 0xFFFF;
u16 sActRunPassVarsType = 0xFFFF;
u16 sActRunningVarsType = 0xFFFF;
u16 sActRunningWBVarsType = 0xFFFF;
u16 sActSlideAttackType = 0xFFFF;
u16 sActMegaStrikeMeterType = 0xFFFF;
u16 sActStunnedType = 0xFFFF;
u16 sActBowserSuperType = 0xFFFF;
u16 sActWarioSuperType = 0xFFFF;

inline void cFielder::RegisterDebugFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("DetFielder");
    REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
        m_bHasBeenUpdated, "m_bHasBeenUpdated");
    REGISTER_FIELDER_FIELD(14, m_bHasBeenUpdated,
        m_eActionState, "m_eActionState");
    REGISTER_FIELDER_FIELD(20, m_bHasBeenUpdated,
        m_tMoveToTurboTimer, "m_tMoveToTurboTimer");
    REGISTER_FIELDER_FIELD(20, m_bHasBeenUpdated,
        mtPostDekeTimer, "mtPostDekeTimer");
    REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
        m_bInPosition, "m_bInPosition");
    REGISTER_FIELDER_FIELD(17, m_bHasBeenUpdated,
        mfAirInterceptHeight[0], "mfAirInterceptHeight[0]");
    REGISTER_FIELDER_FIELD(17, m_bHasBeenUpdated,
        mfAirInterceptHeight[1], "mfAirInterceptHeight[1]");
    REGISTER_FIELDER_FIELD(8, m_bHasBeenUpdated,
        m_nPowerupAnimID, "m_nPowerupAnimID");
    REGISTER_FIELDER_FIELD(20, m_bHasBeenUpdated,
        mtPowerupThrowTime, "mtPowerupThrowTime");
    REGISTER_FIELDER_FIELD(8, m_bHasBeenUpdated,
        muInvincibleStatus, "muInvincibleStatus");
    REGISTER_FIELDER_FIELD(14, m_bHasBeenUpdated,
        m_eRole, "m_eRole");
    for (int i = 0; i < 4; i++)
    {
        cache->AddField(15, gDebugFieldTypes[15].size,
            (u8*)&m_pMark[i] - (u8*)&m_bHasBeenUpdated, "m_pMark[i]");
    }
    REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
        mbWasHitByPowerupThisFrame, "mbWasHitByPowerupThisFrame");
    REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
        mbTangible, "mbTangible");
    REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
        mbIgnorePadSwitchRelease, "mbIgnorePadSwitchRelease");
    cache->EndType();
}

inline void cFielder::RegisterActWarioSuperFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActWarioSuper");
    REGISTER_FIELDER_FIELD(17, m_fNextGasTime,
        m_fNextGasTime, "nextGasTime");
    cache->EndType();
}

inline void cFielder::RegisterActBowserSuperFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActBowserSuper");
    REGISTER_FIELDER_FIELD(17, mActionBowserSuper,
        mActionBowserSuper.nextFireballTime, "nextFireballTime");
    REGISTER_FIELDER_FIELD(17, mActionBowserSuper,
        mActionBowserSuper.fireballStageTime, "fireballStageTime");
    REGISTER_FIELDER_FIELD(9, mActionBowserSuper,
        mActionBowserSuper.fireballStageNum, "fireballStageNum");
    cache->EndType();
}

inline void cFielder::RegisterActStunnedFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActStunned");
    REGISTER_FIELDER_FIELD(10, m_nStunnedAngularAcceleration,
        m_nStunnedAngularAcceleration, "angAccel");
    REGISTER_FIELDER_FIELD(10, m_nStunnedAngularAcceleration,
        m_nStunnedAngularVelocity, "angVel");
    cache->EndType();
}

inline void cFielder::RegisterActMegaStrikeMeterFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActMegaStrikeMeter");
    REGISTER_FIELDER_FIELD(17, m_fMegaStrikeNumBalls,
        m_fMegaStrikeNumBalls, "fNumBalls");
    REGISTER_FIELDER_FIELD(17, m_fMegaStrikeNumBalls,
        m_fMegaStrikeAccuracy, "fAccuracy");
    REGISTER_FIELDER_FIELD(17, m_fMegaStrikeNumBalls,
        m_fMegaStrikeReceivedTimestamp, "fReceivedTimestamp");
    cache->EndType();
}

inline void cFielder::RegisterActSlideAttackFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActSlideAttack");
    REGISTER_FIELDER_FIELD(14, m_eSlideAttackState,
        m_eSlideAttackState, "eSlideAttackState");
    REGISTER_FIELDER_FIELD(16, m_eSlideAttackState,
        bAttackSucceeded, "bAttackSucceeded");
    REGISTER_FIELDER_FIELD(16, m_eSlideAttackState,
        m_bSlideAttackReset, "bIsReset");
    cache->EndType();
}

inline void cFielder::RegisterActRunningWBVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActRunningWBVars");
    REGISTER_FIELDER_FIELD(16, mActionRunningWBVars,
        mActionRunningWBVars.bWaitForAnimToFinish, "bWaitForAnimToFinish");
    REGISTER_FIELDER_FIELD(16, mActionRunningWBVars,
        mActionRunningWBVars.bCuePitch, "bCuePitch");
    cache->EndType();
}

inline void cFielder::RegisterActRunningVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActRunningVars");
    REGISTER_FIELDER_FIELD(14, mActionRunningVars,
        mActionRunningVars.eLastStrafeDirection, "eLastStrafeDirection");
    REGISTER_FIELDER_FIELD(16, mActionRunningVars,
        mActionRunningVars.bFirstCycleOfTurbo, "bFirstCycleOfTurbo");
    cache->EndType();
}

inline void cFielder::RegisterActRunPassVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActRunPassVars");
    REGISTER_FIELDER_FIELD(8, mActionRunPassVars,
        mActionRunPassVars.nHeldTicks, "nHeldTicks");
    REGISTER_FIELDER_FIELD(17, mActionRunPassVars,
        mActionRunPassVars.fSpeed, "fSpeed");
    cache->EndType();
}

inline void cFielder::RegisterActPassingVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActPassingVars");
    REGISTER_FIELDER_FIELD(15, m_pPassTarget,
        m_pPassTarget, "pPassTarget");
    REGISTER_FIELDER_FIELD(16, m_pPassTarget,
        m_bAllowLeadPass, "bAllowLeadPass");
    REGISTER_FIELDER_FIELD(16, m_pPassTarget,
        m_bOneTouchPass, "bIsOneTouchPass");
    cache->EndType();
}

inline void cFielder::RegisterActOneTimerVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActOneTimerVars");
    REGISTER_FIELDER_FIELD(17, m_fOneTimerAnimTime,
        m_fOneTimerAnimTime, "fOneTimerAnimTime");
    cache->EndType();
}

inline void cFielder::RegisterActLooseBallPassFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActLooseBallPass");
    REGISTER_FIELDER_FIELD(15, mActionLooseBallPassVars,
        mActionLooseBallPassVars.passTarget, "passTarget");
    cache->EndType();
}

inline void cFielder::RegisterActShootPassCommonFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActShootPassCommon");
    REGISTER_FIELDER_FIELD(16, bIsModified,
        bIsModified, "bIsModified");
    cache->EndType();
}

inline void cFielder::RegisterActSuperVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActSuperVars");
    REGISTER_FIELDER_FIELD(16, bYoshiInWindup,
        bYoshiInWindup, "bYoshiInWindup");
    cache->EndType();
}

inline void cFielder::RegisterActHitReactVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActHitReactVars");
    REGISTER_FIELDER_FIELD(16, m_bHitReactFrameLock,
        m_bHitReactFrameLock, "bDoFrameLock");
    cache->EndType();
}

inline void cFielder::RegisterActHitVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActHitVars");
    REGISTER_FIELDER_FIELD(17, m_fHitDistance,
        m_fHitDistance, "fHitDistance");
    cache->EndType();
}

inline void cFielder::RegisterActFallVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActFallVars");
    REGISTER_FIELDER_FIELD(17, m_fFallingTime,
        m_fFallingTime, "fallingTime");
    REGISTER_FIELDER_FIELD(22, m_fFallingTime,
        m_v3SuckToSpot, "v3SuckToSpot");
    cache->EndType();
}

inline void cFielder::RegisterActElectVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActElectVars");
    REGISTER_FIELDER_FIELD(17, m_fElectrocutionTime,
        m_fElectrocutionTime, "electrocutionTime");
    REGISTER_FIELDER_FIELD(17, m_fElectrocutionTime,
        m_fElectrocutionLiftTime, "electrocutionLiftTime");
    REGISTER_FIELDER_FIELD(16, m_fElectrocutionTime,
        m_bGroundElectrocution, "bIsGroundElectrocution");
    cache->EndType();
}

inline void cFielder::RegisterActDekeVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActDekeVars");
    REGISTER_FIELDER_FIELD(19, m_aDekeDirection,
        m_aDekeDirection, "aDekeDir");
    REGISTER_FIELDER_FIELD(16, m_aDekeDirection,
        m_bDekeReset, "bIsReset");
    REGISTER_FIELDER_FIELD(8, m_aDekeDirection,
        m_nDPadDownCounter, "nDPadDownCounter");
    cache->EndType();
}

inline void cFielder::RegisterActCrowdVarsFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("ActCrowdVars");
    REGISTER_FIELDER_FIELD(16, mActionCrowdVars,
        mActionCrowdVars.bHasBeenSuckedToMiddle, "bHasBeenSuckedToMiddle");
    REGISTER_FIELDER_FIELD(17, mActionCrowdVars,
        mActionCrowdVars.fStuckInRiotTime, "fStuckInRiotTime");
    cache->EndType();
}

inline void FielderDesireShdState::RegisterDebugFields(unsigned short* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("FielderDesireShdState");
    REGISTER_FIELDER_FIELD(2, *this,
        m_nTransitionFuncHash, "m_nTransitionFuncHash");
    REGISTER_FIELDER_FIELD(2, *this,
        m_nLastActiveTime, "m_nLastActiveTime");
    REGISTER_FIELDER_FIELD(17, *this,
        m_fMaxDuration, "m_fMaxDuration");
    REGISTER_FIELDER_FIELD(17, *this,
        m_fMinDuration, "m_fMinDuration");
    REGISTER_FIELDER_FIELD(17, *this, m_fAge, "m_fAge");
    cache->EndType();
}

#include "Game/EventDataTypes.inl"
#include "Game/AI/FuzzyVariant.inl"
#include "Game/AI/ScriptMachine.inl"
#include "Game/AI/DesireDebug.inl"

void cFielder::SyncLog(void* context, DebugWriteCache* cache)
{
    cPlayer::SyncLog(context, cache);

    if (sFielderType == 0xFFFF)
    {
        RegisterDebugFields(&sFielderType, cache);
    }

    void* data = cache->WriteData(sFielderType, &m_bHasBeenUpdated,
        offsetof(cFielder, m_nMegaStrikeResultState) - offsetof(cFielder, m_bHasBeenUpdated));
    if (data != 0)
    {
        cFielder** marks = (cFielder**)((u8*)data
            + offsetof(cFielder, m_pMark) - offsetof(cFielder, m_bHasBeenUpdated));
        for (int i = 0; i < 4; i++)
        {
            marks[i] = (cFielder*)(m_pMark[i] == 0
                ? -1
                : m_pMark[i]->mUnidentified120);
        }
        cache->ChecksumData(sFielderType, data, context);
    }

    if (sActCrowdVarsType == 0xFFFF)
    {
        RegisterActCrowdVarsFields(&sActCrowdVarsType, cache);
    }
    cache->ChecksumData(sActCrowdVarsType, &mActionCrowdVars, context);
    cache->WriteData(sActCrowdVarsType, &mActionCrowdVars, sizeof(mActionCrowdVars));

    if (sActDekeVarsType == 0xFFFF)
    {
        RegisterActDekeVarsFields(&sActDekeVarsType, cache);
    }
    cache->ChecksumData(sActDekeVarsType, &m_aDekeDirection, context);
    cache->WriteData(sActDekeVarsType, &m_aDekeDirection,
        offsetof(cFielder, m_fElectrocutionTime) - offsetof(cFielder, m_aDekeDirection));

    if (sActElectVarsType == 0xFFFF)
    {
        RegisterActElectVarsFields(&sActElectVarsType, cache);
    }
    cache->ChecksumData(sActElectVarsType, &m_fElectrocutionTime, context);
    cache->WriteData(sActElectVarsType, &m_fElectrocutionTime,
        offsetof(cFielder, m_fFallingTime) - offsetof(cFielder, m_fElectrocutionTime));

    if (sActFallVarsType == 0xFFFF)
    {
        RegisterActFallVarsFields(&sActFallVarsType, cache);
    }
    cache->ChecksumData(sActFallVarsType, &m_fFallingTime, context);
    cache->WriteData(sActFallVarsType, &m_fFallingTime,
        offsetof(cFielder, m_fHitDistance) - offsetof(cFielder, m_fFallingTime));

    if (sActHitVarsType == 0xFFFF)
    {
        RegisterActHitVarsFields(&sActHitVarsType, cache);
    }
    cache->ChecksumData(sActHitVarsType, &m_fHitDistance, context);
    cache->WriteData(sActHitVarsType, &m_fHitDistance, sizeof(m_fHitDistance));

    if (sActHitReactVarsType == 0xFFFF)
    {
        RegisterActHitReactVarsFields(&sActHitReactVarsType, cache);
    }
    cache->ChecksumData(sActHitReactVarsType, &m_bHitReactFrameLock, context);
    cache->WriteData(sActHitReactVarsType, &m_bHitReactFrameLock, sizeof(m_bHitReactFrameLock));

    if (sActSuperVarsType == 0xFFFF)
    {
        RegisterActSuperVarsFields(&sActSuperVarsType, cache);
    }
    cache->ChecksumData(sActSuperVarsType, &bYoshiInWindup, context);
    cache->WriteData(sActSuperVarsType, &bYoshiInWindup, sizeof(bYoshiInWindup));

    if (sActShootPassCommonType == 0xFFFF)
    {
        RegisterActShootPassCommonFields(&sActShootPassCommonType, cache);
    }
    cache->ChecksumData(sActShootPassCommonType, &bIsModified, context);
    cache->WriteData(sActShootPassCommonType, &bIsModified, sizeof(bIsModified));

    if (sActLooseBallPassType == 0xFFFF)
    {
        RegisterActLooseBallPassFields(&sActLooseBallPassType, cache);
    }
    data = cache->WriteData(sActLooseBallPassType,
        &mActionLooseBallPassVars, sizeof(mActionLooseBallPassVars));
    if (data != 0)
    {
        ActLooseBallPass* copy = (ActLooseBallPass*)data;
        copy->passTarget = (cFielder*)(mActionLooseBallPassVars.passTarget == 0
            ? -1
            : mActionLooseBallPassVars.passTarget->mUnidentified120);
        cache->ChecksumData(sActLooseBallPassType, data, context);
    }

    if (sActOneTimerVarsType == 0xFFFF)
    {
        RegisterActOneTimerVarsFields(&sActOneTimerVarsType, cache);
    }
    cache->ChecksumData(sActOneTimerVarsType, &m_fOneTimerAnimTime, context);
    cache->WriteData(sActOneTimerVarsType, &m_fOneTimerAnimTime, sizeof(m_fOneTimerAnimTime));

    if (sActPassingVarsType == 0xFFFF)
    {
        RegisterActPassingVarsFields(&sActPassingVarsType, cache);
    }
    data = cache->WriteData(sActPassingVarsType, &m_pPassTarget,
        offsetof(cFielder, mActionRunPassVars) - offsetof(cFielder, m_pPassTarget));
    if (data != 0)
    {
        cFielder* copy = (cFielder*)((u8*)data - offsetof(cFielder, m_pPassTarget));
        copy->m_pPassTarget = (cPlayer*)(m_pPassTarget == 0
            ? -1
            : m_pPassTarget->mUnidentified120);
        cache->ChecksumData(sActPassingVarsType, data, context);
    }

    if (sActRunPassVarsType == 0xFFFF)
    {
        RegisterActRunPassVarsFields(&sActRunPassVarsType, cache);
    }
    cache->ChecksumData(sActRunPassVarsType, &mActionRunPassVars, context);
    cache->WriteData(sActRunPassVarsType, &mActionRunPassVars, sizeof(mActionRunPassVars));

    if (sActRunningVarsType == 0xFFFF)
    {
        RegisterActRunningVarsFields(&sActRunningVarsType, cache);
    }
    cache->ChecksumData(sActRunningVarsType, &mActionRunningVars, context);
    cache->WriteData(sActRunningVarsType, &mActionRunningVars, sizeof(mActionRunningVars));

    if (sActRunningWBVarsType == 0xFFFF)
    {
        RegisterActRunningWBVarsFields(&sActRunningWBVarsType, cache);
    }
    cache->ChecksumData(sActRunningWBVarsType, &mActionRunningWBVars, context);
    cache->WriteData(sActRunningWBVarsType, &mActionRunningWBVars, sizeof(mActionRunningWBVars));

    if (sActSlideAttackType == 0xFFFF)
    {
        RegisterActSlideAttackFields(&sActSlideAttackType, cache);
    }
    cache->ChecksumData(sActSlideAttackType, &m_eSlideAttackState, context);
    cache->WriteData(sActSlideAttackType, &m_eSlideAttackState,
        offsetof(cFielder, m_fMegaStrikeNumBalls) - offsetof(cFielder, m_eSlideAttackState));

    if (sActMegaStrikeMeterType == 0xFFFF)
    {
        RegisterActMegaStrikeMeterFields(&sActMegaStrikeMeterType, cache);
    }
    cache->ChecksumData(sActMegaStrikeMeterType, &m_fMegaStrikeNumBalls, context);
    cache->WriteData(sActMegaStrikeMeterType, &m_fMegaStrikeNumBalls,
        offsetof(cFielder, m_fMegaStrikeFirstPressPosition) - offsetof(cFielder, m_fMegaStrikeNumBalls));

    if (sActStunnedType == 0xFFFF)
    {
        RegisterActStunnedFields(&sActStunnedType, cache);
    }
    cache->ChecksumData(sActStunnedType, &m_nStunnedAngularAcceleration, context);
    cache->WriteData(sActStunnedType, &m_nStunnedAngularAcceleration,
        offsetof(cFielder, m_bSuperPowerTankOn) - offsetof(cFielder, m_nStunnedAngularAcceleration));

    if (sActBowserSuperType == 0xFFFF)
    {
        RegisterActBowserSuperFields(&sActBowserSuperType, cache);
    }
    cache->ChecksumData(sActBowserSuperType, &mActionBowserSuper, context);
    cache->WriteData(sActBowserSuperType, &mActionBowserSuper, sizeof(mActionBowserSuper));

    if (sActWarioSuperType == 0xFFFF)
    {
        RegisterActWarioSuperFields(&sActWarioSuperType, cache);
    }
    cache->ChecksumData(sActWarioSuperType, &m_fNextGasTime, context);
    cache->WriteData(sActWarioSuperType, &m_fNextGasTime, sizeof(m_fNextGasTime));

    DesireSteering* steering = (DesireSteering*)GetFielderDesire(this, 34);
    fn_8000F324(steering->m_pAvoidance, context, cache);

    for (int i = 0; i < 36; i++)
    {
        Desire* desire = GetFielderDesire(this, i);
        if (desire != 0 && desire->IsActive())
        {
            FielderDesireShdState state;
            const TransitionFunc& transition
                = !desire->mOverrideTransition.IsUnset()
                ? desire->mOverrideTransition
                : desire->mDefaultTransition;
            state.m_nTransitionFuncHash = transition.mFuncHash;
            state.m_nLastActiveTime = (u32)desire->mLastActiveTime;
            state.m_fMaxDuration = desire->mMaxDuration;
            state.m_fMinDuration = desire->mMinDuration;
            state.m_fAge = desire->mAgeTimer.GetSeconds();
            if (gFielderDesireStateDebugType == 0xFFFF)
            {
                state.RegisterDebugFields(&gFielderDesireStateDebugType, cache);
            }
            cache->ChecksumData(gFielderDesireStateDebugType, &state, context);
            cache->WriteData(gFielderDesireStateDebugType, &state, sizeof(state));
            desire->UnidentifiedVirtual7(context, cache);
        }
    }
}

void cFielder::ChecksumState(RunningChecksum* pChecksum)
{
    cPlayer::ChecksumState(pChecksum);
    pChecksum->ChecksumData(&m_eActionState, sizeof(m_eActionState));
    pChecksum->ChecksumData(&m_eRole, sizeof(m_eRole));
}

#undef REGISTER_FIELDER_FIELD
