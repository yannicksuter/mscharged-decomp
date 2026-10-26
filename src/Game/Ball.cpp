#include "Game/AI/FielderDesireTypes.h"
#include "NL/nlDLListContainer.inl"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include <stddef.h>
#include "Game/CharacterTriggers.h"
#include <stdlib.h>
#include "Game/Audio/GameStreams.h"
#include "Game/Audio/Plat3dSoundSrc.h"
#include "Game/RumbleActions.h"
#include <math.h>

#include "Game/Ball.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/ShotMeter.h"
#include "Game/BallTrail.h"
#include "Game/CharacterTweaks.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DebugWriteCache.h"
#include "Game/Drawable/DrawableModel.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmitterCallbacks.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/Field.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/ObjectBlur.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Player.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/NPCManager.h"
#include "Game/SHierarchy.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "Game/TweakValueFloat.h"
#include "NL/nlMain.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/utility.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/KoopaShellObject.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Render/StadiumLoading.h"
#include "NL/nlstring_tmpl.h"
#include "Game/SharedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "NL/nlFunction.inl"

extern unsigned int gNumBallTrails;

cBall* g_pBall = NULL;
unsigned char gRandomizeBallChargeOnReset;
float gInitialKickoffBallHeight;
unsigned char gForceFullBallCharge;
float gKickoffBallLateralSpeedRange;
float gKickoffBallVerticalSpeed;
float gKickoffBallSpeedRandomness;
bool gbUseShotClock;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static const char szPerfectPassBallBlurTexture[]
    = "global/perfectpassstreak";
static const char szPerfectPassBallBlurTexture2[]
    = "global/perfectpass2streak";
static const char szPerfectPassBallBlurTexture3[]
    = "global/perfectpass3streak";
static const char szPerfectPassBallBlurTexture4[]
    = "global/perfectpass4streak";
static const char szBowserShootToScoreBallBlurTexture[]
    = "global/bowsershoottoscorestreak";
static const char szBowserJrShootToScoreBallBlurTexture[]
    = "global/bowserjrshoottoscorestreak";
static const char szDaisyShootToScoreBallBlurTexture[]
    = "global/daisyshoottoscorestreak";
static const char szDonkeyKongShootToScoreBallBlurTexture[]
    = "global/dkshoottoscorestreak";
static const char szDiddyKongShootToScoreBallBlurTexture[]
    = "global/diddykongshoottoscorestreak";
static const char szLuigiShootToScoreBallBlurTexture[]
    = "global/luigishoottoscorestreak";
static const char szMarioShootToScoreBallBlurTexture[]
    = "global/marioshoottoscorestreak";
static const char szPeachShootToScoreBallBlurTexture[]
    = "global/peachshoottoscorestreak";
static const char szPeteyShootToScoreBallBlurTexture[]
    = "global/peteyshoottoscorestreak";
static const char szWaluigiShootToScoreBallBlurTexture[]
    = "global/washoottoscorestreak";
static const char szWarioShootToScoreBallBlurTexture[]
    = "global/warioshoottoscorestreak";
static const char szYoshiShootToScoreBallBlurTexture[]
    = "global/yoshishoottoscorestreak";
extern const nlVector3 gDefaultSkillshotDeflectionTarget = { 12.5f, 0.0f, 0.18f };
static nlMatrix3 m3Ident
    = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

bool gbUsePassCharging = true;
bool gbUseShotCharging = true;
float gMinPlaymakerPassCharge = .55f;
float gMaxPlaymakerPassCharge = .55f;
float gFullBallChargeDecayTime = 6.0f;
float gMaxBallChargeScale = 1.1f;
static float sBallContactRadiusScale = 2.5f;
float gDefaultSkillshotDeflectionTimeScale = .075f;
float gKoopaSkillshotDeflectionTimeScale = .105f;
float gBirdoSkillshotDeflectionTimeScale = .05f;
float gShyGuySkillshotDeflectionTime = 1.33f;
float gDryBonesSkillshotDeflectionSpread = 1.0f;
float gKoopaSkillshotDeflectionSpread = 35.0f;
float gBirdoSkillshotDeflectionSpread = 8.0f;
float gShyGuySkillshotDeflectionSpread = 12.5f;
float gDefaultSkillshotDeflectionSpread = 35.0f;
float gMinSkillshotDeflectionTime = .85f;
float gMaxSkillshotDeflectionTime = 1.75f;
unsigned char gPreserveBallChargeOnReset = 1;
float gKickoffBallLongitudinalSpeed = 2.0f;
float gBallTrailFadeTime = .1f;
float gBallTrailWidthScale = 2.2f;
int gBallTrailMaxPositionEntries = 150;
float gfShotClockTime = 10.0f;
float gfShotClockFrozenTime = 10.0f;
float gBallSidelineSteeringStrength = .8f;
float gMinBallSidelineSpeed = 7.5f;
float gMaxBallSidelineSpeed = 22.5f;
float gBallSidelineTargetY = 11.5f;
float gLowRatingShotAirResistanceScale = 24.0f;
float gClearBallAirResistanceScale = 15.0f;
float gBallHitDamageSpeed = 30.0f;
float gBallMediumHitReactionSpeed = 40.0f;
float gBallHeavyHitReactionSpeed = 50.0f;
float gBallPredictionAirResistanceScale = 1.0f;
float gBallPredictionGravityScale = 1.025f;
float gBallForwardSpinBase = .5f;
float gBallBackSpinBase = -3.0f;
float gLavaBallHorizontalSpeedScale = 1.0f;
float gLavaBallVerticalSpeed = 18.0f;
float gMaxLavaBallHorizontalSpeed = 9.0f;
float gLavaBallRandomDirectionSpeedThreshold = .2f;
float gHeaderTargetPredictionHeight = .8f;
bool gHeaderTargetEnabled = true;
float gHeaderTargetRedisplayDelay = .33f;
float gHeaderTargetInitialDelay = .33f;

static TweakBoolBinding sUsePassChargingTweak(
    "gbUsePassCharging", "Game/Gameplay/Charging/Pass", &gbUsePassCharging, true);
static TweakBoolBinding sUseShotChargingTweak(
    "gbUseShotCharging", "Game/Gameplay/Charging/Shot", &gbUseShotCharging, true);
static TweakFloatBinding sShotClockTimeTweak(
    "gfShotClockTime", "Game/Gameplay/Charging/Shot Clock", &gfShotClockTime, true);
static TweakFloatBinding sShotClockFrozenTimeTweak(
    "gfShotClockFrozenTime", "Game/Gameplay/Charging/Shot Clock", &gfShotClockFrozenTime, true);
static TweakBoolBinding sUseShotClockTweak(
    "gbUseShotClock", "Game/Gameplay/Charging/Shot Clock", &gbUseShotClock, true);
LiveBallTrail gBallTrails[10];

static inline float FullBallCharge()
{
    return 4.0f;
}

cBall::cBall()
    : m_tShotTimer(0.0f)
    , m_tLightningTimer(0.0f)
    , m_tNoPickupTimer(0.0f)
    , m_tPassTargetTimer(0.0f)
    , mtNoChargeLossTimer(0.0f)
    , mtStuckInRiotTimer(0.0f)
    , mtShotClockTimer(0.0f)
{
    m_bVisible = true;
    m_bBallPathChangeCount = 0;
    m_bBallDeflectCount = 0;
    m_fTotalPassTime = 0.0f;
    m_uGoalType = GOAL_UNCLASSIFIED;
    m_uVoiceID = 0;
    m_CurrentGlowEffect = 0;
    mfChargeValue = 0.0f;
    mfSkillShotTime = 0.0f;
    meBallState = BALL_STATE_LOOSE;
    mePrevBallState = BALL_STATE_LOOSE;
    m_pOwner = NULL;
    m_pPrevOwner = NULL;
    m_pLastTouch = NULL;
    m_pPassTarget = NULL;
    m_pShooter = NULL;
    mpDamageTarget = NULL;
    m_iConsecutiveVolleyPasses = 0;

    m_tNoPickupTimer.SetSeconds(0.0f);
    m_tShotTimer.SetSeconds(0.0f);
    m_tLightningTimer.SetSeconds(0.0f);
    m_tPassTargetTimer.SetSeconds(0.0f);
    mtStuckInRiotTimer.SetSeconds(0.0f);
    mtNoChargeLossTimer.SetSeconds(0.0f);
    mtShotClockTimer.SetSeconds(0.0f);

    mnShotClockTeam = -1;
    mbStuckInRiotDone = false;
    mbBallOnFire = false;
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    mbBallFrozen = false;
#endif

    m_v3Position.x = 0.0f;
    m_v3Position.y = 0.0f;
    m_v3Position.z = 0.18f;
    m_v3PrevPosition = m_v3Position;
    m_v3PassIntercept.x = 0.0f;
    m_v3PassIntercept.y = 0.0f;
    m_v3PassIntercept.z = 0.0f;
    m_qOrientation.z = 0.0f;
    m_qOrientation.y = 0.0f;
    m_qOrientation.x = 0.0f;
    m_qOrientation.w = 1.0f;
    m_v3Velocity.x = 0.0f;
    m_v3Velocity.y = 0.0f;
    m_v3Velocity.z = 0.0f;
    m_v3ShotTarget.x = 0.0f;
    m_v3ShotTarget.y = 0.0f;
    m_v3ShotTarget.z = 0.0f;
    m_v3ShotOrigin.x = 0.0f;
    m_v3ShotOrigin.y = 0.0f;
    m_v3ShotOrigin.z = 0.0f;

    m_pBlurHandler = NULL;
    m_uGlowSoundCue = 0;
    m_pDrawableBall = (DrawableModel*)FindStadiumDrawableObject(
        nlStringHash("gameplay/ball"));

    m_pPhysicsBall = new (8, false) PhysicsAIBall(0.18f);
    m_pPhysicsBall->m_pAIBall = this;
    m_pPhysicsBall->SetPosition(
        m_v3Position, PhysicsObject::WORLD_COORDINATES);
    m_v3ShotOrigin = m_v3Position;
    m_pPhysicsBall->SetLinearVelocity(m_v3Velocity);
    m_pPhysicsBall->SetAngularVelocity(v3Zero);

    m_pSoundOwner = CreateAudioSoundOwner(g_pAudioSystem);
    m_pSoundOwner->m_SpatialBits |= 0x00800000;
    m_pSoundOwner->SetPosition(&m_v3Position);
}

cBall::~cBall()
{
    ChangeBallState(this, BALL_STATE_LOOSE);
    ClearBallEffects();
    SetBallCharge(this, 0.0f);
    StopSound(m_uGlowSoundCue, this);
    m_uGlowSoundCue = 0;
    ReleaseAudioSoundOwner(g_pAudioSystem, m_pSoundOwner);
    delete m_pPhysicsBall;
}

void cBall::ClearOwner()
{
    m_pPrevOwner = m_pOwner;
    m_pOwner->SetBall(NULL);
    m_pOwner = NULL;
    m_pPhysicsBall->EnableCollisions();
#if defined(VERSION_R4QK01)
    g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
    g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
#endif
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsBall->GetPosition(&m_v3Position);
    m_pPhysicsBall->GetLinearVelocity(&m_v3Velocity);
    ++m_bBallPathChangeCount;
}

void cBall::ClearBallEffects()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.25f);
        m_pBlurHandler = NULL;
    }
    KillBallShot("skillshot_ball_meteor", 0);
    KillBallShot("skillshot_ball_drybones", 0);
    KillBallShot("skillshot_ball_boo", 0);
}

static inline bool IsClass15BallShot(cBall* pBall, cPlayer* pShooter)
{
    // Keep declaration and initialization order distinct for the nested inline.
    bool bState8ShotWithShooter;
    bool bClassShot;
    bClassShot = false;
    bState8ShotWithShooter = false;
    bool bState8Shot = pBall->IsSkillShotActive();
    if (bState8Shot && pShooter != NULL)
    {
        bState8ShotWithShooter = true;
    }
    if (bState8ShotWithShooter
        && pShooter->m_DetChar.m_eCharacterClass == TOAD)
    {
        bClassShot = true;
    }
    return bClassShot;
}

void cBall::CollideWithCharacterCallback(
    cPlayer* pCharacter, const nlVector3& v3PreBallVelocity)
{
    bool bCanDamage;
    switch (meBallState)
    {
    case BALL_STATE_SHOT:
        if (nlSqrt(m_v3Velocity.GetLengthSq3D(), true) > gBallHitDamageSpeed)
        {
            bCanDamage = true;
        }
        else
        {
            bCanDamage = false;
        }
        break;
    case BALL_STATE_SKILLSHOT:
        bCanDamage = true;
        break;
    default:
        bCanDamage = false;
        break;
    }

    if (bCanDamage && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pCharacterFielder = (cFielder*)pCharacter;
        nlVector3 v3BallDirection;
        nlVec3Sub(v3BallDirection, m_v3Position,
            m_pPrevOwner->m_DetChar.m_v3Position);
        unsigned short aBallDirection
            = (unsigned short)(int)(10430.378f
                * nlATan2f(v3BallDirection.y, v3BallDirection.x));
        bool bReactToHit = true;
        bool bDeflectBall = true;

        bool bInvincible = !pCharacterFielder->IsStuck()
            && (pCharacterFielder->muInvincibleStatus & 4) != 0;
        if (!bInvincible)
        {
            if (pCharacter->m_pBall != NULL)
            {
                pCharacter->ReleaseBall(BALL_STATE_LOOSE);
            }

            cPlayer* pShooter = m_pShooter;
            if (IsClass15BallShot(this, pShooter))
            {
                StartPlayerFire(pCharacter, 9999.9f);
            }
            else if (IsDryBonesSkillshot(this))
            {
                DeflectSkillshotBall(this, pShooter);
                pCharacterFielder->InitActionGroundElectrocution(pShooter->m_DetChar.m_v3Position);
                if (StadiumHasCrowdAudio(
                        GameInfoManager::Instance()->GetStadium()))
                {
                    unsigned long soundID = 0xCE269987;
                    if (pCharacter->m_pTeam->m_nSide == 0)
                    {
                        soundID = 0x5089F33E;
                    }
                    PlayCrowdReaction(soundID);
                }
                bReactToHit = false;
                bDeflectBall = false;
            }
            else if (mbBallOnFire)
            {
                StartPlayerFire(pCharacter, 9999.9f);
            }

            if (bReactToHit)
            {
                int nReact = HIT_REACTION_LIGHT;
                if (nlSqrt(m_v3Velocity.GetLengthSq3D(), true) > gBallHeavyHitReactionSpeed)
                {
                    nReact = HIT_REACTION_HEAVY;
                }
                else if (nlSqrt(m_v3Velocity.GetLengthSq3D(), true) > gBallMediumHitReactionSpeed)
                {
                    nReact = HIT_REACTION_MEDIUM;
                }
                pCharacterFielder->InitActionHitReact(m_pPrevOwner,
                    aBallDirection, nReact, false, false);
            }
        }
        else
        {
            pCharacter->SetNoPickUpTime(0.3f);
        }

        if (bDeflectBall)
        {
            if (m_pOwner != NULL)
            {
                ChangeBallState(this, BALL_STATE_POSSESSED);
            }
            else
            {
                bool bPassTarget = (meBallState == BALL_STATE_VOLLEY_PASS
                                       || meBallState == BALL_STATE_GROUND_PASS)
                    && m_pPassTarget != NULL;
                if (bPassTarget
                    && ReceivingPass((cFielder*)m_pPassTarget))
                {
                    cFielder* pFielder;
                    if (m_pPassTarget != NULL
                        && m_pPassTarget->m_eClassType == FIELDER)
                    {
                        pFielder = (cFielder*)m_pPassTarget;
                    }
                    else
                    {
                        pFielder = NULL;
                    }
                    DesireReceivePass* pReceivePass
                        = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
                    if (pReceivePass == NULL
                        || !pReceivePass->IsActive()
                        || pReceivePass->meDesireSubState != RECEIVE_PASS_APPROACH)
                    {
                        ChangeBallState(this, BALL_STATE_LOOSE);
                    }
                }
                else
                {
                    ChangeBallState(this, BALL_STATE_LOOSE);
                }
            }

            m_tNoPickupTimer.SetSeconds(0.0f);

            nlVector3 v3Velocity;
            nlVector3 v3AngularVelocity;
            if (nlVec3DotProduct(v3BallDirection, m_v3Velocity) > 0.0f)
            {
                nlVec3Scale(v3Velocity, m_v3Velocity, -0.1f);
                nlVector3 v3CharacterToBall;
                nlVec3Sub(v3CharacterToBall, m_v3Position, pCharacter->m_DetChar.m_v3Position);
                if (nlVec3DotProduct(v3CharacterToBall, v3Velocity) < 0.0f)
                {
                    const nlVector3& previousPosition = m_v3PrevPosition;
                    SetPosition(previousPosition);
                }
            }
            else
            {
                nlVec3Scale(v3Velocity, m_v3Velocity, 0.1f);
            }

            v3Velocity.z += 4.0f + nlRandomf(3.0f);
            v3Velocity.y += -4.0f + nlRandomf(8.0f);

            m_pPhysicsBall->GetAngularVelocity(&v3AngularVelocity);
            SetVelocity(v3Velocity, SPINTYPE_PARAMETER,
                &v3AngularVelocity);
        }
    }

    if (meBallState == BALL_STATE_GOALIE_DEFLECTION && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pCharacter;
        if (pFielder->m_eActionState >= ACTION_MEGA_STRIKE
            || pFielder->m_eActionState < ACTION_LOOSE_BALL_PASS)
        {
            if (m_pOwner != NULL)
            {
                ChangeBallState(this, BALL_STATE_POSSESSED);
            }
            else
            {
                bool bPassTarget = (meBallState == BALL_STATE_VOLLEY_PASS
                                       || meBallState == BALL_STATE_GROUND_PASS)
                    && m_pPassTarget != NULL;
                if (bPassTarget
                    && ReceivingPass((cFielder*)m_pPassTarget))
                {
                    cFielder* pPassTarget;
                    if (m_pPassTarget != NULL
                        && m_pPassTarget->m_eClassType == FIELDER)
                    {
                        pPassTarget = (cFielder*)m_pPassTarget;
                    }
                    else
                    {
                        pPassTarget = NULL;
                    }
                    DesireReceivePass* pReceivePass
                        = (DesireReceivePass*)GetFielderDesire(
                            pPassTarget, FIELDER_DESIRE_RECEIVE_PASS);
                    if (pReceivePass == NULL
                        || !pReceivePass->IsActive()
                        || pReceivePass->meDesireSubState != RECEIVE_PASS_APPROACH)
                    {
                        ChangeBallState(this, BALL_STATE_LOOSE);
                    }
                }
                else
                {
                    ChangeBallState(this, BALL_STATE_LOOSE);
                }
            }
        }
    }

    if (m_tShotTimer.m_uPackedTime != 0
        || HasPassTarget())
    {
        if (pCharacter->m_eClassType == FIELDER)
        {
            m_uGoalType = GOAL_DEFLECTION;
        }

        if (m_pOwner != NULL)
        {
            ChangeBallState(this, BALL_STATE_POSSESSED);
        }
        else
        {
            bool bHasPassTarget = (meBallState == BALL_STATE_VOLLEY_PASS
                                      || meBallState == BALL_STATE_GROUND_PASS)
                && m_pPassTarget != NULL;
            if (bHasPassTarget
                && ReceivingPass((cFielder*)m_pPassTarget))
            {
                cFielder* pFielder;
                if (m_pPassTarget != NULL
                    && m_pPassTarget->m_eClassType == FIELDER)
                {
                    pFielder = (cFielder*)m_pPassTarget;
                }
                else
                {
                    pFielder = NULL;
                }
                DesireReceivePass* pReceivePass
                    = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
                if (pReceivePass == NULL
                    || !pReceivePass->IsActive()
                    || pReceivePass->meDesireSubState != RECEIVE_PASS_APPROACH)
                {
                    ChangeBallState(this, BALL_STATE_LOOSE);
                }
            }
            else
            {
                ChangeBallState(this, BALL_STATE_LOOSE);
            }
        }
    }

    cFielder* pOwnerFielder = (cFielder*)m_pOwner;
    if (pOwnerFielder != NULL && pOwnerFielder->m_eClassType == FIELDER)
    {
        pOwnerFielder = (cFielder*)m_pOwner;
    }
    else
    {
        pOwnerFielder = NULL;
    }

    if (pOwnerFielder != NULL && pOwnerFielder != pCharacter
        && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pCharacterFielder = (cFielder*)pCharacter;
        pOwnerFielder->TestCollisionForInvincibility(pCharacterFielder);

        if (!pCharacterFielder->IsOnSameTeam(pOwnerFielder))
        {
            if (pCharacterFielder->IsSlideAttacking())
            {
                nlVector3 v3ContactLocation
                    = pCharacter->m_DetChar.m_v3Position;
                nlVector3 v3PhysicsRadialSpot;
                float fPlayerScale
                    = pCharacter->m_DetChar.m_fPlayerScale;
                const unsigned short aActualFacingDirection
                    = pCharacter->m_DetChar.m_aActualFacingDirection;
                float fRadius = GetScaledPlayerWidth(
                    pCharacterFielder->GetTweaks(), fPlayerScale);
                nlPolarToCartesian(v3PhysicsRadialSpot.x,
                    v3PhysicsRadialSpot.y,
                    aActualFacingDirection, fRadius);
                v3PhysicsRadialSpot.z = 0.0f;
                nlVec3Add(v3ContactLocation, v3ContactLocation,
                    v3PhysicsRadialSpot);

                s16 nHitterContactLocationFacingDelta
                    = pCharacter->GetFacingDeltaToPosition(
                        v3ContactLocation);
                u16 absFacingDelta
                    = nHitterContactLocationFacingDelta < 0
                    ? -nHitterContactLocationFacingDelta
                    : nHitterContactLocationFacingDelta;
                if (absFacingDelta < 0x2000)
                {
                    if (pOwnerFielder->IsSlideAttacking())
                    {
                        s16 nHitteeContactLocationFacingDelta
                            = pOwnerFielder->GetFacingDeltaToPosition(
                                v3ContactLocation);
                        u16 absOwnerFacingDelta
                            = nHitteeContactLocationFacingDelta < 0
                            ? -nHitteeContactLocationFacingDelta
                            : nHitteeContactLocationFacingDelta;
                        if (absOwnerFacingDelta < 0x2000)
                        {
                            if (pOwnerFielder->GetTweaks()
                                    ->fDefenseSize
                                < pCharacterFielder->GetTweaks()
                                      ->fDefenseSize)
                            {
                                pOwnerFielder->InitActionSlideAttackReact(
                                    pCharacterFielder, false);
                                pCharacterFielder->SetSlideAttackSuccessFlag();
                                pCharacterFielder->PickupBall(g_pBall);
                            }
                            else if (pOwnerFielder->GetTweaks()
                                         ->fDefenseSize
                                > pCharacterFielder->GetTweaks()
                                      ->fDefenseSize)
                            {
                                pCharacterFielder
                                    ->InitActionSlideAttackReact(
                                        pOwnerFielder, false);
                                pOwnerFielder->SetSlideAttackSuccessFlag();
                            }
                            else
                            {
                                float hitterSpeed = pCharacterFielder->m_DetChar.m_fActualSpeed;
                                float hitteeSpeed = pOwnerFielder->m_DetChar.m_fActualSpeed;
                                if (hitteeSpeed < hitterSpeed)
                                {
                                    pOwnerFielder->InitActionSlideAttackReact(
                                        pCharacterFielder, false);
                                    pCharacterFielder->SetSlideAttackSuccessFlag();
                                    pCharacterFielder->PickupBall(g_pBall);
                                }
                                else
                                {
                                    pCharacterFielder
                                        ->InitActionSlideAttackReact(
                                            pOwnerFielder, false);
                                    pOwnerFielder->SetSlideAttackSuccessFlag();
                                }
                            }
                        }
                        else
                        {
                            pOwnerFielder->InitActionSlideAttackReact(
                                pCharacterFielder, false);
                            pCharacterFielder->DoPenaltyCardBooking(
                            pOwnerFielder, PEN_TYPE_SLIDE_WITH_BALL);
                            pCharacterFielder->SetSlideAttackSuccessFlag();
                            if (pCharacterFielder->CanPickupBall(
                                    g_pBall, false))
                            {
                                pCharacterFielder->PickupBall(g_pBall);
                            }
                        }
                    }
                    else
                    {
                        pOwnerFielder->InitActionSlideAttackReact(
                            pCharacterFielder, false);
                        pCharacterFielder->DoPenaltyCardBooking(
                            pOwnerFielder, PEN_TYPE_SLIDE_WITH_BALL);
                        pCharacterFielder->SetSlideAttackSuccessFlag();
                        if (pCharacterFielder->CanPickupBall(
                                g_pBall, false))
                        {
                            pCharacterFielder->PickupBall(g_pBall);
                        }
                    }
                }
            }
            else if (pOwnerFielder->IsSlideAttacking()
                && !pCharacterFielder->IsHitting())
            {
                pCharacterFielder->InitActionSlideAttackReact(
                    pOwnerFielder, false);
                pOwnerFielder->SetSlideAttackSuccessFlag();
            }
        }
        else
        {
            if (pOwnerFielder->IsSlideAttacking()
                && !pOwnerFielder->IsSuperGrowActive())
            {
                bool bInvincible = !pOwnerFielder->IsStuck()
                    && (pOwnerFielder->muInvincibleStatus & 1) != 0;
                if (!bInvincible)
                {
                    pOwnerFielder->FinishSlideAttack();
                }
            }
            if (pCharacterFielder->IsSlideAttacking()
                && !pCharacterFielder->IsSuperGrowActive())
            {
                bool bInvincible = !pCharacterFielder->IsStuck()
                    && (pCharacterFielder->muInvincibleStatus & 1) != 0;
                if (!bInvincible)
                {
                    pCharacterFielder->FinishSlideAttack();
                }
            }
        }
    }

    if (m_pOwner == NULL)
    {
        m_pLastTouch = pCharacter;
        FakeBallWorld::InvalidateBallCache();
        ++m_bBallDeflectCount;
    }

    ++m_bBallPathChangeCount;

    if (pCharacter->m_eClassType == FIELDER)
    {
        m_v3ShotOrigin = m_v3Position;
    }
}

static inline void HandleBallGroundContactImpl(cBall* pBall)
{
    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    bool bPassTarget = pBall->HasPassTarget();
    if (bPassTarget)
    {
        if (ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL
                && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

void HandleBallGroundContact(cBall* pBall)
{
    HandleBallGroundContactImpl(pBall);
}

void SetBallFallState(cBall* pBall)
{
    if (pBall->meBallState != BALL_STATE_FALLING)
    {
        ChangeBallState(pBall, BALL_STATE_FALLING);
    }
}

void HandleBallWallContact(cBall* pBall)
{
    if (pBall->m_tNoPickupTimer.m_uPackedTime != 0)
    {
        return;
    }

    if (pBall->meBallState == BALL_STATE_GOALIE_DEFLECTION)
    {
        return;
    }

    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                           || pBall->meBallState == BALL_STATE_GROUND_PASS)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL
                && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

static inline float clampAbove(float minVal, float x)
{
    if (minVal >= x)
    {
        return minVal;
    }
    return x;
}

void cBall::PostPhysicsUpdate(float fDeltaT)
{
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsBall->GetPosition(&m_v3Position);
    m_pPhysicsBall->GetLinearVelocity(&m_v3Velocity);

    bool bCanDamage;
    switch (meBallState)
    {
    case BALL_STATE_SHOT:
        if (nlSqrt(m_v3Velocity.GetLengthSq3D(), true)
            > gBallHitDamageSpeed)
        {
            bCanDamage = true;
        }
        else
        {
            bCanDamage = false;
        }
        break;
    case BALL_STATE_SKILLSHOT:
        bCanDamage = true;
        break;
    default:
        bCanDamage = false;
        break;
    }

    if (bCanDamage && mpDamageTarget != NULL)
    {
        nlVector3 v3HitSpot;
        nlVector3 targetDelta;
        nlVector3 currentDelta;
        nlVector3 v3CurPos;
        nlVector3 v3PrevPos;
        float fPercent;
        nlVector3 v3BallVel;
        float fPrevZVel;

        v3HitSpot = mpDamageTarget->GetJointPosition(
            mpDamageTarget->m_pPoseAccumulator->m_BaseSHierarchy
                ->m_nPelvisNodeIndex);
        v3HitSpot.z = clampAbove(0.3f, v3HitSpot.z + 0.05f);

        v3CurPos = m_v3Position;
        v3PrevPos = m_v3PrevPosition;

        if (v3CurPos.z < 0.3f)
        {
            v3CurPos.z = 0.3f;
        }

        if (v3PrevPos.z < 0.3f)
        {
            v3PrevPos.z = 0.3f;
        }

        nlVec3Sub(targetDelta, v3HitSpot, v3PrevPos);
        nlVec3Sub(currentDelta, v3CurPos, v3PrevPos);

        float targetDist = nlSqrt(targetDelta.GetLengthSq3D(), true);
        float currentDist = nlSqrt(currentDelta.GetLengthSq3D(), true);

        fPercent = 0.5f;
        if (targetDist < currentDist)
        {
            nlVec3Scale(currentDelta, targetDist / targetDist);
        }
        else
        {
            nlVec3Scale(targetDelta, currentDist / targetDist);
        }

        if (targetDist < 5.0f)
        {
            fPercent += 0.5f * (1.0f - targetDist / 5.0f);
        }

        nlVecLerp(targetDelta, currentDelta, targetDelta, fPercent);
        nlVec3Add(v3CurPos, v3PrevPos, targetDelta);

        m_v3Position = v3CurPos;
        m_pPhysicsBall->SetPosition(
            v3CurPos, PhysicsObject::WORLD_COORDINATES);
        m_pPhysicsBall->SetRotation(m3Ident);

        FakeBallWorld::InvalidateBallCache();
        m_bBallPathChangeCount = m_bBallPathChangeCount + 1;

        const nlVector3& ballVelocity = m_v3Velocity;
        fPrevZVel = ballVelocity.z;
        nlVec3Project(v3BallVel, ballVelocity, targetDelta);
        v3BallVel.z = fPrevZVel;

        float speedSq = v3BallVel.GetLengthSq3D();
        if (speedSq < 400.0f)
        {
            float speed = nlSqrt(speedSq, true);
            nlVec3Scale(v3BallVel, 20.0f / speed);
        }

        if (v3CurPos.z < 0.4f && v3BallVel.z < 0.0f)
        {
            v3BallVel.z = 0.0f;
        }

        m_v3Velocity = v3BallVel;
        m_pPhysicsBall->SetLinearVelocity(v3BallVel);
    }

    UpdateOrientation(fDeltaT);

    bool bBooSkillshot = m_tShotTimer.m_uPackedTime != 0
        && meBallState == BALL_STATE_SKILLSHOT && m_pShooter != NULL
        && m_pShooter->m_DetChar.m_eCharacterClass == BOO;
    if (bBooSkillshot)
    {
        cFielder* pFielder = (cFielder*)m_pShooter;
        if (pFielder->m_eActionState == ACTION_BOO_SKILLSHOT
            && pFielder->mActionBooSkillshot.bFollowingBall)
        {
            nlVector3 v3JointPosition = pFielder->GetJointPosition(
                pFielder->m_nBip01JointIndex_0xA4);
            nlVector2 v2Delta;
            v2Delta.x = v3JointPosition.x - pFielder->m_DetChar.m_v3Position.x;
            v2Delta.y = v3JointPosition.y - pFielder->m_DetChar.m_v3Position.y;
            float fDistance
                = nlSqrt(v2Delta.x * v2Delta.x + v2Delta.y * v2Delta.y,
                    true);
            float fHeight
                = pFielder->m_DetChar.m_v3Position.z - v3JointPosition.z;

            nlVector3 v3Position;
            nlVector3 v3Velocity = m_v3Velocity;
            pFielder->SetFacingDirection(
                (unsigned short)(int)(10430.378f
                    * nlATan2f(v3Velocity.y, v3Velocity.x)),
                true);
            pFielder->SetVelocity(v3Velocity);

            v3Velocity.z = 0.0f;
            nlVec3Normalize(v3Velocity, v3Velocity);

            nlVec3ScaleAdd(
                v3Position, -fDistance, v3Velocity, m_v3Position);
            v3Position.z += fHeight;
            if (v3Position.z < 0.0f)
            {
                v3Position.z = 0.0f;
            }
            pFielder->SetPosition(v3Position);
        }
    }

    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->AddViewOrientedPoint(
            m_v3Position, m_v3Velocity);
    }

    KoopaShellObject* pKoopaShell = gNPCManager->mpKoopaShell;
    if (pKoopaShell != NULL && pKoopaShell->mVisible)
    {
        pKoopaShell->mVelocity = m_v3Velocity;
        pKoopaShell->SetPosition(m_v3Position);
    }

    BirdoEggObject* pState = gNPCManager->mpBirdoEgg;
    if (pState != NULL && pState->mVisible)
    {
        pState->mVelocity = m_v3Velocity;
        pState->SetPosition(m_v3Position);
    }
}

static inline bool UpdateBallPassLockImpl(cBall* pBall)
{
    bool bPassLockedIn = false;
    bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                           || pBall->meBallState == BALL_STATE_GROUND_PASS)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        cPlayer* pPassTarget = pBall->m_pPassTarget;
        cFielder* pFielder;
        if (pPassTarget != NULL
            && pPassTarget->m_eClassType == FIELDER)
        {
            pFielder = (cFielder*)pPassTarget;
        }
        else
        {
            pFielder = NULL;
        }
        DesireReceivePass* pReceivePass
            = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
        if (pReceivePass != NULL
            && pReceivePass->IsActive())
        {
            bPassLockedIn = pReceivePass->meDesireSubState != RECEIVE_PASS_APPROACH;
        }
    }

    if (bPassLockedIn != pBall->m_pPhysicsBall->mbPassLockedIn)
    {
        pBall->m_pPhysicsBall->mbPassLockedIn = bPassLockedIn;
        FakeBallWorld::InvalidateBallCache();
        ++pBall->m_bBallPathChangeCount;
    }
    return bPassLockedIn;
}

bool UpdateBallPassLock(cBall* pBall)
{
    return UpdateBallPassLockImpl(pBall);
}

bool IsBallDangerousShot(cBall* pBall)
{
    switch (pBall->meBallState)
    {
    case BALL_STATE_SHOT:
        return nlSqrt(pBall->m_v3Velocity.GetLengthSq3D(), true)
            > gBallHitDamageSpeed;
    case BALL_STATE_SKILLSHOT:
        return true;
    default:
        return false;
    }
}

bool IsBallEffectPlaying(
    cBall* pBall, const EffectsGroup* pEffectsGroup)
{
    return EmissionManager::Instance()->IsPlaying(
        (unsigned long)pBall, pEffectsGroup);
}

static inline cFielder* GetOwnerFielderImpl(cBall* pBall)
{
    cPlayer* player = pBall->m_pOwner;
    if ((player != NULL) && (player->m_eClassType == FIELDER))
    {
        return (cFielder*)player;
    }
    return NULL;
}

static inline cFielder* GetPassTargetFielderImpl(const cBall* pBall)
{
    cPlayer* player = pBall->m_pPassTarget;
    if ((player != NULL) && (player->m_eClassType == FIELDER))
    {
        return (cFielder*)player;
    }
    return NULL;
}

nlVector3* cBall::GetAIVelocity() const
{
    cPlayer* temp_r4 = m_pOwner;
    if (temp_r4 != NULL)
    {
        return &(temp_r4->m_DetChar.m_v3Velocity);
    }
    return (nlVector3*)&(m_v3Velocity);
}

nlVector3* cBall::GetDrawablePosition() const
{
    const nlMatrix4& mtx = *m_pDrawableBall->GetWorldMatrix();
    return (nlVector3*)&(mtx.e2[3][0]);
}

float cBall::GetScaledContactRadius(float fScale) const
{
    return fScale * (0.18f * sBallContactRadiusScale);
}

cFielder* cBall::GetOwnerFielder()
{
    return GetOwnerFielderImpl(this);
}

cPlayer* cBall::GetOwnerGoalie()
{
    cPlayer* player = m_pOwner;
    if ((player == NULL) || (player->m_eClassType != GOALIE))
    {
        return NULL;
    }
    return player;
}

cFielder* cBall::GetPassTargetFielder() const
{
    return GetPassTargetFielderImpl(this);
}

bool cBall::GetInNet(int& nSide)
{
    cGame* gameState = g_pGame;
    if (gameState->mbCaptainShotToScoreOn == 0)
    {
        if (m_pPhysicsBall->mbIsInsideNet)
        {
            float fDirection = g_pTeams[0]->m_pNet->m_fDirection;
            nSide = !(m_v3Position.x * fDirection > 1.0f);
            return true;
        }
    }
    else if (gameState->m_uMegastrikeCurShot > gameState->m_uMegastrikeNumShots
             && gameState->m_uMegastrikeGoals != 0)
    {
        float fDirection = g_pTeams[0]->m_pNet->m_fDirection;
        nSide = !(m_v3Position.x * fDirection > 1.0f);
        return true;
    }

    return false;
}

void cBall::InitiateBallBlur(
    eBallShotEffectType effectType, cPlayer* pPlayer)
{
    if (m_pBlurHandler != NULL)
    {
        BlurManager::DestroyHandler(m_pBlurHandler, 0.1f);
        m_pBlurHandler = NULL;
    }

    switch (effectType)
    {
    case BALL_EFFECT_S2S_SUPER_SHOT:
    case BALL_EFFECT_S2S_SHOT:
    case BALL_EFFECT_PERFECT_SHOT:
    default:
        if (mfChargeValue >= 1.0f)
        {
            char textureName[64] = "";
            int nLength;
            if (mfChargeValue < 2.0f)
            {
                nlStrNCpy(textureName,
                    szPerfectPassBallBlurTexture,
                    sizeof(textureName));
                nLength = 8;
            }
            else if (mfChargeValue < 3.0f)
            {
                nlStrNCpy(textureName,
                    szPerfectPassBallBlurTexture2,
                    sizeof(textureName));
                nLength = 12;
            }
            else if (mfChargeValue < 4.0f)
            {
                nlStrNCpy(textureName,
                    szPerfectPassBallBlurTexture3,
                    sizeof(textureName));
                nLength = 18;
            }
            else
            {
                nlStrNCpy(textureName,
                    szPerfectPassBallBlurTexture4,
                    sizeof(textureName));
                nLength = 24;
            }

            m_pBlurHandler = BlurManager::GetNewHandler(
                textureName, 0.18f, nLength, true);
        }
        break;
    case BALL_EFFECT_PERFECT_PASS:
    case BALL_EFFECT_REGULAR_SHOT:
    case BALL_EFFECT_ONETIMER_SHOT:
    case BALL_EFFECT_CHIP_SHOT:
        break;
    }
}

void EmitBallChargeTransition(cBall* pBall)
{
    static unsigned long sHashBallShotWindupTrans0To1
        = nlStringLowerHash("ball_shot_windup_trans_0_1");
    static unsigned long sHashBallShotWindupTrans1To2
        = nlStringLowerHash("ball_shot_windup_trans_1_2");
    static unsigned long sHashBallShotWindupTrans2To3
        = nlStringLowerHash("ball_shot_windup_trans_2_3");
    static unsigned long sHashBallShotWindupTrans3ToMax
        = nlStringLowerHash("ball_shot_windup_trans_3_max");
    static unsigned long sHashBallShotWindupTransMax
        = nlStringLowerHash("ball_shot_windup_trans_max");

    if (pBall->mfChargeValue < 1.0f)
    {
        EmitBallWindupTransition(sHashBallShotWindupTrans0To1);
    }
    else if (pBall->mfChargeValue < 2.0f)
    {
        EmitBallWindupTransition(sHashBallShotWindupTrans1To2);
    }
    else if (pBall->mfChargeValue < 3.0f)
    {
        EmitBallWindupTransition(sHashBallShotWindupTrans2To3);
    }
    else if (pBall->mfChargeValue < 4.0f)
    {
        EmitBallWindupTransition(sHashBallShotWindupTrans3ToMax);
    }
    else
    {
        EmitBallWindupTransition(sHashBallShotWindupTransMax);
    }
}

void ResetBallCharge(cBall* pBall, bool bParam)
{
    if (pBall->mfChargeValue >= 0.0f && !bParam)
    {
        EmitBallChargeTransition(pBall);
    }

    if (pBall->m_pBlurHandler != NULL)
    {
        pBall->m_pBlurHandler->Die(0.25f);
        pBall->m_pBlurHandler = NULL;
    }
    KillBallShot("skillshot_ball_meteor", 0);
    KillBallShot("skillshot_ball_drybones", 0);
    KillBallShot("skillshot_ball_boo", 0);

    if (gForceFullBallCharge || GameInfoManager::Instance()->IsWhiteBallEnabled())
    {
        pBall->mfChargeValue = 4.0f;
    }
    else
    {
        pBall->mfChargeValue = 0.0f;
    }

    float fMaxCharge = gMaxBallChargeScale * FullBallCharge();
    float fValue = pBall->mfChargeValue;
    if (fValue >= fMaxCharge)
    {
        pBall->mfChargeValue = fMaxCharge;
    }
    else if (fValue < 0.0f)
    {
        pBall->mfChargeValue = 0.0f;
    }

    UpdateBallGlow(pBall);
}

// A named scale keeps the multiply operand order used by the state-change
// inline without changing SetBallCharge or its other callers.
static inline void SetBallChargeWithScale(cBall* pBall, float fParam)
{
    if (gForceFullBallCharge || GameInfoManager::Instance()->IsWhiteBallEnabled())
    {
        pBall->mfChargeValue = 4.0f;
    }
    else
    {
        pBall->mfChargeValue = fParam;
    }

    float fMaxChargeScale = 4.0f;
    float fMaxCharge = gMaxBallChargeScale * fMaxChargeScale;
    float fValue = pBall->mfChargeValue;
    if (fValue >= fMaxCharge)
    {
        pBall->mfChargeValue = fMaxCharge;
    }
    else if (pBall->mfChargeValue < 0.0f)
    {
        pBall->mfChargeValue = 0.0f;
    }

    UpdateBallGlow(pBall);
}

void SetBallCharge(cBall* pBall, float fParam)
{
    if (gForceFullBallCharge || GameInfoManager::Instance()->IsWhiteBallEnabled())
    {
        pBall->mfChargeValue = 4.0f;
    }
    else
    {
        pBall->mfChargeValue = fParam;
    }

    float fMaxCharge = gMaxBallChargeScale * FullBallCharge();
    float fValue = pBall->mfChargeValue;
    if (fValue >= fMaxCharge)
    {
        pBall->mfChargeValue = fMaxCharge;
    }
    else if (pBall->mfChargeValue < 0.0f)
    {
        pBall->mfChargeValue = 0.0f;
    }

    UpdateBallGlow(pBall);
}

float GetBallChargeValue(cBall* pBall, int nParam)
{
    if (nParam != 0)
    {
        if (!pBall->m_bVisible)
        {
            return 0.0f;
        }

        if (pBall->GetOwnerFielder() != NULL
            && pBall->GetOwnerFielder()->m_DetChar.m_eCharacterClass == BIRDO)
        {
            return 0.0f;
        }

        if (pBall->GetOwnerFielder() != NULL
            && pBall->GetOwnerFielder()->m_DetChar.m_eCharacterClass
                == SHYGUY
            && pBall->GetOwnerFielder()->m_eActionState == ACTION_SKILLSHOT)
        {
            return 0.0f;
        }
    }

    return pBall->mfChargeValue;
}

float GetBallChargeBonus(cBall* pBall)
{
    float fParam = pBall->mfChargeValue - 1.0f;
    float fResult = 0.0f;
    if (fParam > 0.0f)
    {
        fResult = fParam / 3.0f;
    }

    fResult = fResult >= 0.0f ? fResult : 0.0f;
    return fResult <= 1.0f ? fResult : 1.0f;
}

static inline void ShootAtFastImpl(cBall* pBall, nlVector3& v3Vel,
    const nlVector3& v3Target, float fDesiredTime)
{
    float gravity = pBall->m_pPhysicsBall->m_gravity;
    float airResistance = pBall->m_pPhysicsBall->mfBallAirResistance;
    float k = gBallPredictionAirResistanceScale * airResistance;
    float g = gBallPredictionGravityScale * gravity;
    float eToTheNegativeKT = Exp(-k * fDesiredTime);
    float kSquaredOverOneMinusEToTheNegativeKT
        = (k * k) / (1.0f - eToTheNegativeKT);
    float oneOverK = 1.0f / k;

    v3Vel.x = kSquaredOverOneMinusEToTheNegativeKT
        * (oneOverK * (v3Target.x - pBall->m_v3Position.x));
    v3Vel.y = kSquaredOverOneMinusEToTheNegativeKT
        * (oneOverK * (v3Target.y - pBall->m_v3Position.y));
    v3Vel.z = kSquaredOverOneMinusEToTheNegativeKT
            * (oneOverK * (v3Target.z - pBall->m_v3Position.z
                              - g * fDesiredTime / k))
        + g / k;
}

void DeflectSkillshotBall(cBall*, cPlayer* pShooter)
{
    g_pBall->m_pPhysicsBall->RestoreBallForces();

    nlVector3 v3Position = gDefaultSkillshotDeflectionTarget;
    v3Position.x *= AIsgn(g_pBall->m_v3Position.x);

    eCharacterClass eClass = CHARACTER_CLASS_INVALID;
    if (pShooter != NULL)
    {
        eClass = pShooter->m_DetChar.m_eCharacterClass;
        v3Position = pShooter->m_DetChar.m_v3Position;
    }

    float fTimeScale;
    float fOriginalX = v3Position.x;
    switch (eClass)
    {
    case CHARACTER_CLASS_INVALID:
    default:
        v3Position.x += nlRandomf(gDefaultSkillshotDeflectionSpread)
            - 0.5f * gDefaultSkillshotDeflectionSpread;
        v3Position.y += nlRandomf(gDefaultSkillshotDeflectionSpread)
            - 0.5f * gDefaultSkillshotDeflectionSpread;
        fTimeScale = gDefaultSkillshotDeflectionTimeScale;
        break;
    case KOOPA:
        v3Position.x += nlRandomf(gKoopaSkillshotDeflectionSpread)
            - 0.5f * gKoopaSkillshotDeflectionSpread;
        v3Position.y += nlRandomf(gKoopaSkillshotDeflectionSpread)
            - 0.5f * gKoopaSkillshotDeflectionSpread;
        fTimeScale = gKoopaSkillshotDeflectionTimeScale;
        break;
    case BIRDO:
        v3Position.x += nlRandomf(gBirdoSkillshotDeflectionSpread)
            - 0.5f * gBirdoSkillshotDeflectionSpread;
        v3Position.y += nlRandomf(gBirdoSkillshotDeflectionSpread)
            - 0.5f * gBirdoSkillshotDeflectionSpread;
        fTimeScale = gBirdoSkillshotDeflectionTimeScale;
        break;
    case DRYBONES:
        v3Position.x += nlRandomf(gDryBonesSkillshotDeflectionSpread)
            - 0.5f * gDryBonesSkillshotDeflectionSpread;
        v3Position.y += nlRandomf(gDryBonesSkillshotDeflectionSpread)
            - 0.5f * gDryBonesSkillshotDeflectionSpread;
        fTimeScale = gDefaultSkillshotDeflectionTimeScale;
        break;
    case SHYGUY:
        v3Position.x += nlRandomf(gShyGuySkillshotDeflectionSpread)
            - 0.5f * gShyGuySkillshotDeflectionSpread;
        v3Position.y += nlRandomf(gShyGuySkillshotDeflectionSpread)
            - 0.5f * gShyGuySkillshotDeflectionSpread;
        fTimeScale = gShyGuySkillshotDeflectionTime;
        break;
    }

    if (v3Position.x < 0.0f)
    {
        v3Position.x = nlMinEquals(
            nlMaxEquals(v3Position.x, g_pBall->m_v3Position.x),
            fOriginalX);
    }
    else
    {
        v3Position.x = nlMinEquals(
            nlMaxEquals(v3Position.x, fOriginalX),
            g_pBall->m_v3Position.x);
    }

    cField::FixOutOfBoundsPosition(v3Position, 0.2f, true);

    nlVector3 v3Velocity;
    nlVector2 v2Delta;
    v2Delta.x = v3Position.x - g_pBall->m_v3Position.x;
    v2Delta.y = v3Position.y - g_pBall->m_v3Position.y;
    float fDistance = nlSqrt(
        v2Delta.x * v2Delta.x + v2Delta.y * v2Delta.y, true);
    float fDesiredTime = fTimeScale * fDistance;
    if (eClass == SHYGUY)
    {
        fDesiredTime = fTimeScale;
    }
    else if (fDesiredTime < gMinSkillshotDeflectionTime)
    {
        fDesiredTime = gMinSkillshotDeflectionTime;
    }
    else if (fDesiredTime > gMaxSkillshotDeflectionTime)
    {
        fDesiredTime = gMaxSkillshotDeflectionTime;
    }

    ShootAtFastImpl(g_pBall, v3Velocity, v3Position, fDesiredTime);

    eSpinType spinType;
    if (nlRandom(100) > 50)
    {
        spinType = SPINTYPE_FORWARD;
    }
    else
    {
        spinType = SPINTYPE_BACK;
    }
    g_pBall->SetVelocity(v3Velocity, spinType, NULL);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.15f);
    PhysicsBall* pPhysicsBall = g_pBall->m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;
    ChangeBallState(g_pBall, BALL_STATE_GOALIE_DEFLECTION);
}

static inline void CancelBallShotImpl(cBall* pBall, bool bParam)
{
    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    if (!bParam)
    {
        bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                               || pBall->meBallState == BALL_STATE_GROUND_PASS)
            && pBall->m_pPassTarget != NULL;
        if (bPassTarget && ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

void CancelBallShot(cBall* pBall, bool bParam)
{
    CancelBallShotImpl(pBall, bParam);
}

static inline void ClearBallStateTargets(cBall* pBall)
{
    pBall->m_tShotTimer.Clear();
    pBall->mpDamageTarget = NULL;
    if (pBall->m_pPassTarget != NULL)
    {
        pBall->m_pPassTarget = NULL;
    }

    pBall->m_v3PassIntercept.x = 0.0f;
    pBall->m_v3PassIntercept.y = 0.0f;
    pBall->m_v3PassIntercept.z = 0.0f;
    pBall->m_tPassTargetTimer.Clear();
    pBall->m_fTotalPassTime = 0.0f;
    if (pBall->m_uVoiceID != 0)
    {
        pBall->m_uVoiceID = 0;
    }
}

static inline void UpdateBallShotClock(cBall* pBall)
{
    if (pBall->m_pOwner != NULL)
    {
        if (!pBall->m_pOwner->IsOnSameTeam(pBall->m_pPrevOwner))
        {
            pBall->mtShotClockTimer.SetSeconds(gfShotClockTime);
            pBall->mnShotClockTeam = pBall->m_pOwner->m_pTeam->m_nSide;
        }
    }
    else
    {
        pBall->mnShotClockTeam = -1;
        pBall->mtShotClockTimer.SetSeconds(0.0f);
    }
}

void ChangeBallState(cBall* pBall, int nBallState)
{
    ImmediateEvent<void(int, int)>* event
        = &g_pGame->mEventQueue.mBallStateChangeEvent;
    event->Deliver(pBall->meBallState, nBallState);

    if (pBall->meBallState == BALL_STATE_LIGHTNING)
    {
        pBall->m_pPhysicsBall->m_gravity = -22.5f;
        OnBallLightningEnd(0);
        pBall->m_tLightningTimer.Clear();
    }

    if ((pBall->meBallState == BALL_STATE_SHOT || pBall->meBallState == BALL_STATE_CHIP_SHOT)
        && nBallState != BALL_STATE_SHOT && nBallState != BALL_STATE_CHIP_SHOT && nBallState != BALL_STATE_FALLING)
    {
        EmitBallChargeTransition(pBall);
        pBall->m_pPhysicsBall->ResetBallAirResistance();
    }
    else if (pBall->meBallState == BALL_STATE_CLEARING && nBallState != BALL_STATE_FALLING)
    {
        pBall->m_pPhysicsBall->ResetBallAirResistance();
    }
    else if (pBall->meBallState == BALL_STATE_FALLING)
    {
        pBall->m_pPhysicsBall->ResetBallAirResistance();
    }

    switch (nBallState)
    {
    case BALL_STATE_POSSESSED:
        UpdateBallShotClock(pBall);
        ClearBallStateTargets(pBall);
        if (pBall->m_pBlurHandler != NULL)
        {
            pBall->m_pBlurHandler->Die(0.25f);
            pBall->m_pBlurHandler = NULL;
        }
        break;
    case BALL_STATE_FALLING:
        UpdateBallShotClock(pBall);
        ClearBallStateTargets(pBall);
        break;
    case BALL_STATE_LIGHTNING:
    {
        EmitLightningBall();
        pBall->SetVelocity(v3Zero, SPINTYPE_NONE, NULL);
        pBall->m_pPhysicsBall->m_gravity = 0.0f;
        pBall->m_tLightningTimer.SetSeconds(0.5f);

        SetBallChargeWithScale(pBall, 4.0f);

        if (pBall->m_pBlurHandler != NULL)
        {
            pBall->m_pBlurHandler->Die(0.25f);
            pBall->m_pBlurHandler = NULL;
        }
    }
    case BALL_STATE_LOOSE:
        UpdateBallShotClock(pBall);
        ClearBallStateTargets(pBall);
        break;
    case BALL_STATE_GOALIE_DEFLECTION:
        ClearBallStateTargets(pBall);
        pBall->InitiateBallBlur(BALL_EFFECT_S2S_SUPER_SHOT, NULL);
        break;
    case BALL_STATE_CLEARING:
        UpdateBallShotClock(pBall);
        if (pBall->m_pPrevOwner != NULL
            && pBall->m_pPrevOwner->m_eClassType == FIELDER)
        {
            pBall->m_pPhysicsBall->mfBallAirResistance
                = pBall->m_pPhysicsBall->GetDefaultBallAirResistance() * gClearBallAirResistanceScale;
        }
        break;
    case BALL_STATE_SHOT:
        UpdateBallShotClock(pBall);
        cPlayer* pPrevOwner = pBall->m_pPrevOwner;
        if (pPrevOwner != NULL
            && pPrevOwner->m_eClassType == FIELDER)
        {
            float resistance = pBall->m_pPhysicsBall->GetDefaultBallAirResistance();
            PlayerTweaks* tweaks
                = ((cFielder*)pPrevOwner)->GetTweaks();
            pBall->m_pPhysicsBall->mfBallAirResistance
                = resistance * Interpolate(gLowRatingShotAirResistanceScale, 1.0f,
                    (float)tweaks->fShooting);
        }
        break;
    case BALL_STATE_CHIP_SHOT:
        UpdateBallShotClock(pBall);
        break;
    case BALL_STATE_GROUND_PASS:
    case BALL_STATE_VOLLEY_PASS:
    case BALL_STATE_SKILLSHOT:
    default:
        break;
    }

    if (nBallState == BALL_STATE_POSSESSED || nBallState == BALL_STATE_GOALIE_DEFLECTION)
    {
        PlayOwnedSound(0, 0xBF92BBAF,
            (XSoundOwner*)pBall->m_pSoundOwner, NULL, NULL);
    }

    if (pBall->m_pPrevOwner != NULL
        && pBall->m_pPrevOwner->m_eClassType == FIELDER)
    {
        StopSound(0x65321E47, pBall);
        if (nBallState == BALL_STATE_CHIP_SHOT)
        {
            PlayOwnedSound(0, 0xDE8EC45D,
                (XSoundOwner*)pBall->m_pSoundOwner, NULL, NULL);
        }
        else if (nBallState == BALL_STATE_SHOT)
        {
            PlayOwnedSound(0, 0xDE8EC45D,
                (XSoundOwner*)pBall->m_pSoundOwner, NULL, NULL);
            if (pBall->mfChargeValue >= 1.0f
                && pBall->mfChargeValue < 2.0f)
            {
                PlayOwnedSound(0, 0xDE8EC45E,
                    (XSoundOwner*)pBall->m_pSoundOwner, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 2.0f
                && pBall->mfChargeValue < 3.0f)
            {
                PlayOwnedSound(0, 0xDE8EC45F,
                    (XSoundOwner*)pBall->m_pSoundOwner, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 3.0f
                && pBall->mfChargeValue < 4.0f)
            {
                PlayOwnedSound(0, 0xDE8EC460,
                    (XSoundOwner*)pBall->m_pSoundOwner, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 4.0f)
            {
                PlayOwnedSound(0, 0xDE8EC461,
                    (XSoundOwner*)pBall->m_pSoundOwner, NULL,
                    NULL);
            }
        }
        else if (nBallState == BALL_STATE_VOLLEY_PASS || nBallState == BALL_STATE_GROUND_PASS || nBallState == BALL_STATE_CLEARING)
        {
            PlayOwnedSound(0, 0x874F86F2,
                (XSoundOwner*)pBall->m_pSoundOwner, NULL, NULL);
            if (pBall->mfChargeValue >= 1.0f
                && pBall->mfChargeValue < 2.0f)
            {
                PlayOwnedSound(0, 0x71406564,
                    (XSoundOwner*)pBall->m_pSoundOwner, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 2.0f
                && pBall->mfChargeValue < 3.0f)
            {
                PlayOwnedSound(0, 0x71406565,
                    (XSoundOwner*)pBall->m_pSoundOwner, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 3.0f
                && pBall->mfChargeValue < 4.0f)
            {
                PlayOwnedSound(0, 0x71406566,
                    (XSoundOwner*)pBall->m_pSoundOwner, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 4.0f)
            {
                PlayOwnedSound(0, 0x71406567,
                    (XSoundOwner*)pBall->m_pSoundOwner, NULL,
                    NULL);
            }

            if (nBallState == BALL_STATE_VOLLEY_PASS)
            {
                PlayOwnedSound(0, 0x65321E47,
                    (XSoundOwner*)pBall->m_pSoundOwner,
                    "Volley Pass", pBall);
            }
        }
        else if (nBallState == BALL_STATE_SKILLSHOT)
        {
            PlayOwnedSound(pBall->m_pPrevOwner->m_uSoundSlotId,
                0x3D267BDF,
                (XSoundOwner*)pBall->m_pSoundOwner, NULL, NULL);
        }
    }

    pBall->mePrevBallState = pBall->meBallState;
    pBall->meBallState = static_cast<eBallState>(nBallState);
}

void cBall::ClearBallBlur()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.25f);
        m_pBlurHandler = NULL;
    }
}

bool IsDryBonesSkillshot(cBall* pBall)
{
    return pBall->m_tShotTimer.m_uPackedTime != 0
        && pBall->meBallState == BALL_STATE_SKILLSHOT && pBall->m_pShooter != NULL
        && pBall->m_pShooter->m_DetChar.m_eCharacterClass
        == DRYBONES;
}

bool IsBooSkillshotBall(cBall* pBall)
{
    return pBall->m_tShotTimer.m_uPackedTime != 0
        && pBall->meBallState == BALL_STATE_SKILLSHOT && pBall->m_pShooter != NULL
        && pBall->m_pShooter->m_DetChar.m_eCharacterClass
        == BOO;
}

bool IsBallShotActive(cBall* pBall)
{
    return pBall->m_tShotTimer.m_uPackedTime != 0;
}

void cBall::SetOwner(cPlayer* pOwner)
{
    m_pOwner = pOwner;
    pOwner->SetBall(this);
    m_pLastTouch = pOwner;
    ChangeBallState(this, BALL_STATE_POSSESSED);

    if (pOwner->m_eClassType != GOALIE)
    {
        g_pGame->SetPotentialScorer(pOwner);
    }

    PhysicsBall* pPhysicsBall = m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;

#if defined(VERSION_R4QK01)
    if (pOwner->m_eClassType == FIELDER)
    {
        if (((cFielder*)pOwner)->mbTangible)
        {
            g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
            g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        }
    }
    else
    {
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
    }
#endif
}

void cBall::SetPosition(const nlVector3& pos)
{
    m_v3Position = pos;
    m_pPhysicsBall->SetPosition(pos, PhysicsObject::WORLD_COORDINATES);
    m_pPhysicsBall->SetRotation(m3Ident);
    FakeBallWorld::InvalidateBallCache();
    ++m_bBallPathChangeCount;
}

void cBall::SetVelocity(const nlVector3& velocity, eSpinType spin,
    const nlVector3* pAngularVelocity)
{
    nlVector3 v3AngVel;
    float fSpinRand;

    m_v3Velocity = velocity;
    m_pPhysicsBall->SetLinearVelocity(velocity);

    if (spin == SPINTYPE_NONE)
    {
        v3AngVel.x = 0.0f;
        v3AngVel.y = 0.0f;
        v3AngVel.z = 0.0f;
    }
    else if (spin == SPINTYPE_FORWARD)
    {
        fSpinRand = gBallForwardSpinBase + nlRandomf(2.0f);

        nlVector3 v3Up = { 0.0f, 0.0f, 0.0f };
        v3Up.z = fSpinRand;

        nlVec3CrossProductAlt(v3AngVel, v3Up, velocity);
        nlVec3Set(v3AngVel, v3AngVel.z, v3AngVel.y, v3AngVel.x);
    }
    else if (spin == SPINTYPE_BACK)
    {
        fSpinRand = gBallBackSpinBase + nlRandomf(2.0f);

        nlVector3 v3Up = { 0.0f, 0.0f, 0.0f };
        v3Up.z = fSpinRand;

        nlVec3CrossProductAlt(v3AngVel, v3Up, velocity);
        nlVec3Set(v3AngVel, v3AngVel.z, v3AngVel.y, v3AngVel.x);
    }
    else if (spin == SPINTYPE_ROLLING)
    {
        m_pPhysicsBall->CalcAngularFromLinearVelocity(v3AngVel);
        nlVec3Set(v3AngVel, 0.92f * v3AngVel.x,
            0.92f * v3AngVel.y, 0.92f * v3AngVel.z);
    }
    else if (spin == SPINTYPE_PARAMETER)
    {
        v3AngVel = *pAngularVelocity;
    }

    m_pPhysicsBall->SetAngularVelocity(v3AngVel);
    m_pPhysicsBall->SetUseAngularVelocity(true);
    m_pPhysicsBall->SetRotation(m3Ident);
    FakeBallWorld::InvalidateBallCache();
    m_bBallPathChangeCount = m_bBallPathChangeCount + 1;
    UpdateBallPassLockImpl(this);
    m_v3ShotOrigin = m_v3Position;
}

void cBall::Shoot(cPlayer* pShooter, const nlVector3& v3Dir,
    const nlVector3& v3Spin, eSpinType spinType, int nBallState, bool bParam6)
{
    nlVector3 v3PredPos;
    nlVector3 v3PredVel;
    nlVector3 v3ToDir;
    nlVector3 v3FromDir;
    nlQuaternion qRot;
    nlVector3 v3TargetOffset;

    SetVelocity(v3Dir, spinType, &v3Spin);
    m_tNoPickupTimer.SetSeconds(0.1f);
    m_tShotTimer.SetSeconds(2.0f);
    ChangeBallState(this, nBallState);
    m_pShooter = pShooter;

    if (bParam6)
    {
        m_pPhysicsBall->ResetBallAirResistance();
    }

    if (m_pPhysicsBall->mbUseMagnusEffect)
    {
        nlVec3Set(v3TargetOffset,
            m_v3Position.x - m_v3ShotTarget.x,
            m_v3Position.y - m_v3ShotTarget.y,
            m_v3Position.z - m_v3ShotTarget.z);
        float fDist = nlSqrt(v3TargetOffset.GetLengthSq3D(), true);

        DisablePredictedGoaliePlanes();
        FakeBallWorld::GetPredictedPosAtDistance(
            fDist, v3PredPos, v3PredVel, true);

        nlVec3Sub(v3ToDir, m_v3ShotTarget, m_v3Position);
        nlVec3Sub(v3FromDir, v3PredPos, m_v3Position);

        GetRotationBetweenVectors(qRot, v3FromDir, v3ToDir);
        RotateVector(m_v3Velocity, v3Dir, qRot);

        if (m_v3Velocity.z < 1.0f && m_v3Position.z < 1.0f)
        {
            m_v3Velocity.z = 1.0f;
        }

        float fSidelineY = cField::GetSidelineY(1) - 0.5f;
        if (m_v3Position.y > fSidelineY && m_v3Velocity.y > -0.1f)
        {
            m_v3Velocity.y = -0.1f;
        }
        else if (m_v3Position.y < -cField::GetSidelineY(1) + 0.5f
            && m_v3Velocity.y < 0.1f)
        {
            m_v3Velocity.y = 0.1f;
        }

        m_pPhysicsBall->SetLinearVelocity(m_v3Velocity);
        FakeBallWorld::InvalidateBallCache();
    }

    if (!g_pGame->mbCaptainShotToScoreOn)
    {
        Goalie* pGoalie = m_pPrevOwner->m_pTeam->GetOtherTeam()->GetGoalie();
        pGoalie->InitActionSaveSetup(true);
    }
}

void ReleaseBallForPass(cBall* pBall, cPlayer* pPlayer,
    nlVector3* pVelocity, int nSpinType, bool bVolleyPass, bool bParam)
{
    if (bVolleyPass && pBall->mePrevBallState == BALL_STATE_VOLLEY_PASS)
    {
        ++pBall->m_iConsecutiveVolleyPasses;
    }
    else
    {
        pBall->m_iConsecutiveVolleyPasses = 0;
    }

    int nReleaseReason = bVolleyPass ? BALL_STATE_VOLLEY_PASS : BALL_STATE_GROUND_PASS;
    if (pPlayer->m_pBall != NULL)
    {
        pPlayer->ReleaseBall(nReleaseReason);
    }
    else
    {
        ChangeBallState(pBall, nReleaseReason);
    }

    pBall->SetVelocity(*pVelocity, (eSpinType)nSpinType, NULL);
    pBall->m_tNoPickupTimer.SetSeconds(0.1f);
    PhysicsBall* pPhysicsBall = pBall->m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;

    if (gbUsePassCharging && pPlayer->m_eClassType == FIELDER)
    {
        float fValue = GetPlaymakerRating(((cFielder*)pPlayer)->GetTweaks());
        float fPercent = InterpolateRangeClamped(
            0.0f, 1.0f, 0.5f, 1.0f, fValue);
        float fCharge = Interpolate(gMinPlaymakerPassCharge, gMaxPlaymakerPassCharge, fPercent);
        float fCurrentCharge = pBall->mfChargeValue;
        if (gForceFullBallCharge
            || GameInfoManager::Instance()->IsWhiteBallEnabled())
        {
            pBall->mfChargeValue = 4.0f;
        }
        else
        {
            pBall->mfChargeValue = fCharge + fCurrentCharge;
        }

        float fMaxCharge = gMaxBallChargeScale * FullBallCharge();
        fValue = pBall->mfChargeValue;
        if (fValue >= fMaxCharge)
        {
            pBall->mfChargeValue = fMaxCharge;
        }
        else if (fValue < 0.0f)
        {
            pBall->mfChargeValue = 0.0f;
        }

        UpdateBallGlow(pBall);
    }
}

void cBall::ShootRelease(const nlVector3& v3Velocity, eSpinType SpinType)
{
    SetVelocity(v3Velocity, SpinType, NULL);
    m_tNoPickupTimer.SetSeconds(0.1f);
    PhysicsBall* pPhysicsBall = m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;
}

// Keep this inline separate: DeflectSkillshotBall needs a different local order
// to preserve its floating-point register allocation.
static inline void CalculateFastShotVelocity(cBall* pBall, nlVector3& v3Vel,
    const nlVector3& v3Target, float fDesiredTime)
{
    float gravity = pBall->m_pPhysicsBall->m_gravity;
    float airResistance = pBall->m_pPhysicsBall->mfBallAirResistance;
    float g;
    float k;
    k = gBallPredictionAirResistanceScale * airResistance;
    g = gBallPredictionGravityScale * gravity;
    float eToTheNegativeKT = Exp(-k * fDesiredTime);
    float kSquaredOverOneMinusEToTheNegativeKT
        = (k * k) / (1.0f - eToTheNegativeKT);
    float oneOverK = 1.0f / k;

    v3Vel.x = kSquaredOverOneMinusEToTheNegativeKT
            * (oneOverK * (v3Target.x - pBall->m_v3Position.x));
    v3Vel.y = kSquaredOverOneMinusEToTheNegativeKT
            * (oneOverK * (v3Target.y - pBall->m_v3Position.y));
    v3Vel.z = kSquaredOverOneMinusEToTheNegativeKT
                * (oneOverK * (v3Target.z - pBall->m_v3Position.z - g * fDesiredTime / k))
            + g / k;
}

void cBall::ShootAtFast(nlVector3& v3Vel, const nlVector3& v3Target,
    float fDesiredTime)
{
    CalculateFastShotVelocity(this, v3Vel, v3Target, fDesiredTime);
}

// Separate products preserve the original rounding before summing the speed.
static inline float BallVelocityLength(float x, float y, float z)
{
    float xSquared = x * x;
    float ySquared = y * y;
    float zSquared = z * z;
    return nlSqrt(xSquared + ySquared + zSquared, true);
}

void SteerBallToSideline(cBall* pBall)
{
    if (nlAbs(pBall->m_v3Position.y) - gBallSidelineTargetY < 0.0f)
    {
        HandleBallGroundContactImpl(pBall);
        return;
    }

    if (pBall->m_v3Position.z < 0.36f)
    {
        nlVector3 v3Position = pBall->m_v3Position;
        v3Position.z = 0.36f;
        pBall->SetPosition(v3Position);
    }

    nlVector3 v3Position = pBall->m_v3Position;
    v3Position.y = AIsgn(v3Position.y) * gBallSidelineTargetY;
    cField::FixOutOfBoundsX(v3Position, true, 3.0f);

    nlVector3 v3Direction;
    nlVec3Sub(v3Direction, v3Position, pBall->m_v3Position);
    nlVec3Scale(v3Direction,
        nlRecipSqrt(v3Direction.GetLengthSq3D(), true));
    nlVec3Scale(v3Direction, gBallSidelineSteeringStrength);

    float fSpeed = BallVelocityLength(pBall->m_v3Velocity.x, pBall->m_v3Velocity.y, pBall->m_v3Velocity.z);
    if (fSpeed < gMinBallSidelineSpeed)
    {
        fSpeed = gMinBallSidelineSpeed;
    }
    else if (fSpeed > gMaxBallSidelineSpeed)
    {
        fSpeed = gMaxBallSidelineSpeed;
    }

    nlVector3 v3Velocity;
    nlVec3Add(v3Velocity, pBall->m_v3Velocity, v3Direction);
    nlVec3Scale(v3Velocity,
        nlRecipSqrt(v3Velocity.GetLengthSq3D(), true));
    nlVec3Scale(v3Velocity, fSpeed);

    nlVector3 v3AngularVelocity;
    pBall->m_pPhysicsBall->GetAngularVelocity(&v3AngularVelocity);
    pBall->SetVelocity(
        v3Velocity, SPINTYPE_PARAMETER, &v3AngularVelocity);
}

void UpdateBallStateAndTimers(cBall* pBall, float fDeltaT)
{
    bool bIsGameplay = g_pGame->IsGameplayOrOvertime();

    if (bIsGameplay)
    {
        if (pBall->meBallState == BALL_STATE_FALLING)
        {
            SteerBallToSideline(pBall);
        }

        pBall->m_tNoPickupTimer.Countdown(fDeltaT, 0.0f);

        if (pBall->m_tLightningTimer.m_uPackedTime != 0
            && pBall->m_tLightningTimer.Countdown(fDeltaT, 0.0f))
        {
            eBallState& ballState = pBall->meBallState;
            if (ballState == BALL_STATE_LIGHTNING)
            {
                HandleBallGroundContact(pBall);
            }
            else
            {
                ResetBall(pBall, false);
            }
        }

        if (pBall->m_tShotTimer.m_uPackedTime != 0
            && pBall->m_tShotTimer.Countdown(fDeltaT, 0.0f))
        {
            HandleBallGroundContact(pBall);
        }

        if (pBall->m_tPassTargetTimer.m_uPackedTime != 0
            && pBall->m_tPassTargetTimer.Countdown(fDeltaT, 0.0f))
        {
            pBall->m_fTotalPassTime = 0.0f;
        }

        if (pBall->mtStuckInRiotTimer.m_uPackedTime != 0
            && pBall->mtStuckInRiotTimer.Countdown(fDeltaT, 0.0f))
        {
            pBall->mbStuckInRiotDone = true;
        }

        if (pBall->mtNoChargeLossTimer.m_uPackedTime != 0)
        {
            pBall->mtNoChargeLossTimer.Countdown(fDeltaT, 0.0f);
        }

        if (pBall->mtShotClockTimer.m_uPackedTime != 0
            && pBall->mtShotClockTimer.Countdown(fDeltaT, 0.0f)
            && gbUseShotClock)
        {
            cFielder* pFielder = NULL;
            if (GetOwnerFielderImpl(pBall) != NULL)
            {
                pFielder = GetOwnerFielderImpl(pBall);
            }
            else if (GetPassTargetFielderImpl(pBall) != NULL)
            {
                pFielder = GetPassTargetFielderImpl(pBall);
            }
            if (pFielder != NULL)
            {
                SetFielderFrozenState(pFielder, 1, gfShotClockFrozenTime);
            }
        }
    }
}

static bool sHeaderTargetVisible;

#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
static inline void RestoreFrozenBallPosition(cBall* pBall)
{
    const nlVector3& position = pBall->m_v3PrevPosition;
    pBall->SetPosition(position);
}
#endif

void cBall::Update(float fDeltaT)
{
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    // Hold the ball in place during the Mega Strike freeze.
    if (mbBallFrozen)
    {
        RestoreFrozenBallPosition(this);
    }
    else
#endif
    {
        UpdateBallStateAndTimers(this, fDeltaT);
        DecayBallCharge(this);

        bool bKillHeaderTarget = true;
        static Timer tHeaderTargetTimer(0.33f);

        bool bIsGameplay = g_pGame->IsGameplayOrOvertime();

        if (bIsGameplay)
        {
            if (gHeaderTargetEnabled
                && meBallState != BALL_STATE_SHOT
                && meBallState != BALL_STATE_CHIP_SHOT
                && meBallState != BALL_STATE_SKILLSHOT
                && meBallState != BALL_STATE_LIGHTNING
                && meBallState != BALL_STATE_POSSESSED
                && meBallState != BALL_STATE_FALLING)
            {
                if (sHeaderTargetVisible)
                {
                    bKillHeaderTarget = m_v3Position.z < 0.4f;
                }
                else if (m_v3Position.z > gHeaderTargetPredictionHeight
                    && tHeaderTargetTimer.Countdown(fDeltaT, 0.0f))
                {
                    nlVector3 v3LandingSpot;
                    PredictLandingSpotAndTime(v3LandingSpot,
                        NULL, NULL, gHeaderTargetPredictionHeight);
                    EmitHeaderTarget(this, &v3LandingSpot, false);
                    sHeaderTargetVisible = true;
                    bKillHeaderTarget = false;
                }
            }

            if (sHeaderTargetVisible && bKillHeaderTarget)
            {
                KillHeaderTarget(this, false);
                sHeaderTargetVisible = false;
                tHeaderTargetTimer.SetSeconds(gHeaderTargetRedisplayDelay);
            }
        }
        else
        {
            KillHeaderTarget(this, false);
            sHeaderTargetVisible = false;
            tHeaderTargetTimer.SetSeconds(gHeaderTargetInitialDelay);
        }

        if (meBallState != BALL_STATE_VOLLEY_PASS
            || (meBallState == BALL_STATE_POSSESSED && m_pOwner != NULL
                && m_pOwner->m_DetPlayer.m_tBallPossessionTimer.GetSeconds() > 0.1f))
        {
            m_iConsecutiveVolleyPasses = 0;
        }

        m_pSoundOwner->count.zeroVelocity
            = !g_pGame->IsGameplayOrOvertime();
    }
}

static inline void CalcBallRotationFromVelocity(
    nlQuaternion& qOrientationDelta, const nlVector3& v3Velocity,
    float fDeltaT)
{
    qOrientationDelta.z = 0.0f;
    qOrientationDelta.y = 0.0f;
    qOrientationDelta.x = 0.0f;
    qOrientationDelta.w = 1.0f;

    float fVel = nlSqrt(v3Velocity.GetLengthSq3D(), true);
    if (fVel > 0.0001f)
    {
        float angle = fDeltaT * (fVel / 0.18f);

        nlVector3 v3NormalizedVelocity = v3Velocity;
        nlVector3 v3Up;
        nlVector3 v3RotationAxis;

        float velocityX = v3NormalizedVelocity.x;
        v3NormalizedVelocity.x = velocityX / fVel;
        v3NormalizedVelocity.y /= fVel;
        v3NormalizedVelocity.z /= fVel;

        v3Up.x = 0.0f;
        v3Up.y = 0.0f;
        v3Up.z = 1.0f;

        float fAxisX;
        float fAxisY;
        float fAxisZ;

        fAxisX = v3Up.y * v3NormalizedVelocity.z
               - v3Up.z * v3NormalizedVelocity.y;
        fAxisY = -v3Up.x * v3NormalizedVelocity.z
               + v3Up.z * v3NormalizedVelocity.x;
        fAxisZ = v3Up.x * v3NormalizedVelocity.y
               - v3Up.y * v3NormalizedVelocity.x;
        nlVec3Set(v3RotationAxis, fAxisX, fAxisY, fAxisZ);

        nlMakeQuat(qOrientationDelta, v3RotationAxis, (unsigned short)(int)(10430.378f * angle));
    }
}

void cBall::UpdateOrientation(float fDeltaT)
{
    nlQuaternion qOrientationDelta;
    nlVector3 v3AngVel;
    float fInvAng;
    nlQuaternion qNewOrientation;

    if (m_pOwner == NULL)
    {
        u8 bUseAngularVel = 0;
        PhysicsBall* physics = m_pPhysicsBall;
        if (physics->mbUseAngularVel != 0
            || physics->mfSpinTimer > 0.0f)
        {
            bUseAngularVel = 1;
        }

        if (bUseAngularVel != 0)
        {
            physics->GetAngularVelocity(&v3AngVel);

            float fAng = nlSqrt(v3AngVel.x * v3AngVel.x
                                    + v3AngVel.y * v3AngVel.y
                                    + v3AngVel.z * v3AngVel.z,
                true);
            if (fAng > 0.01f)
            {
                fInvAng = 1.0f / fAng;
                nlVec3Scale(v3AngVel, fInvAng);
                nlMakeQuat(qOrientationDelta, v3AngVel, (unsigned short)(int)(10430.378f * (fAng * fDeltaT)));
            }
            else
            {
                qOrientationDelta.z = 0.0f;
                qOrientationDelta.y = 0.0f;
                qOrientationDelta.x = 0.0f;
                qOrientationDelta.w = 1.0f;
            }
        }
        else
        {
            CalcBallRotationFromVelocity(
                qOrientationDelta, m_v3Velocity, fDeltaT);
        }
    }
    else
    {
        m_pPhysicsBall->SetUseAngularVelocity(false);

        switch (m_pOwner->m_DetPlayer.m_eBallRotationMode)
        {
        case BRM_ANIMATED:
            m_pOwner->GetAnimatedBallOrientation(m_qOrientation);
            return;
        case BRM_MATCH_VELOCITY:
        {
            CalcBallRotationFromVelocity(
                qOrientationDelta, m_v3Velocity, fDeltaT);
            break;
        }
        }
    }

    nlMultQuat(qNewOrientation, qOrientationDelta, m_qOrientation);
    nlQuatNormalize(m_qOrientation, qNewOrientation);
}

void cBall::WarpTo(const nlVector3& toPos)
{
    m_v3Position = toPos;
    m_pPhysicsBall->SetPosition(toPos, PhysicsObject::WORLD_COORDINATES);
    m_pPhysicsBall->SetRotation(m3Ident);
    FakeBallWorld::InvalidateBallCache();
    m_bBallPathChangeCount = m_bBallPathChangeCount + 1;
    m_v3PrevPosition = toPos;
}

void cBall::SetPassTarget(
    cPlayer* passTargetPlayer, const nlVector3& pos, bool bVolley)
{
    m_pPassTarget = passTargetPlayer;
    m_v3PassIntercept = pos;
}

void cBall::SetPassTargetTimer(float seconds)
{
    m_tPassTargetTimer.SetSeconds(seconds);
    if (m_fTotalPassTime == 0.0f)
    {
        m_fTotalPassTime = seconds;
    }
}

void DecayBallCharge(cBall* pBall)
{
    if (pBall->mtNoChargeLossTimer.m_uPackedTime != 0)
    {
        return;
    }

    bool bLoseCharge = false;
    switch (pBall->meBallState)
    {
    case BALL_STATE_LOOSE:
    case BALL_STATE_CLEARING:
    case BALL_STATE_GOALIE_DEFLECTION:
        bLoseCharge = true;
        break;
    case BALL_STATE_POSSESSED:
        if (pBall->GetOwnerFielder() != NULL)
        {
            cFielder* pOwnerFielder = pBall->GetOwnerFielder();
            bool bIsShotActive = true;
            eShotMeterState state
                = pOwnerFielder->m_pShotMeter->m_eShotMeterState;
            if (state != SHOT_METER_ACTIVE
                && state != SHOT_METER_STS_ACTIVE)
            {
                bIsShotActive = false;
            }
            if (!bIsShotActive)
            {
                bLoseCharge = true;
            }
        }
        else
        {
            bLoseCharge = true;
        }
        break;
    }

    if (!bLoseCharge)
    {
        return;
    }

    float fValue = pBall->mfChargeValue;
    float fChargeLoss = (g_fSimulationTick / gFullBallChargeDecayTime) * FullBallCharge();
    if (gForceFullBallCharge || GameInfoManager::Instance()->IsWhiteBallEnabled())
    {
        pBall->mfChargeValue = 4.0f;
    }
    else
    {
        pBall->mfChargeValue = fValue - fChargeLoss;
    }

    float fMaxCharge = gMaxBallChargeScale * FullBallCharge();
    fValue = pBall->mfChargeValue;
    if (fValue >= fMaxCharge)
    {
        pBall->mfChargeValue = fMaxCharge;
    }
    else if (fValue < 0.0f)
    {
        pBall->mfChargeValue = 0.0f;
    }

    UpdateBallGlow(pBall);
}

void cBall::KillBlurHandler()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.f);
        m_pBlurHandler = NULL;
    }
}

void PredictBallPosition(
    cBall* pBall, nlVector3* pPosition, float fTime)
{
    float airResistance = pBall->m_pPhysicsBall->mfBallAirResistance;
    float gravity = pBall->m_pPhysicsBall->m_gravity;
    float g;
    float k;
    k = gBallPredictionAirResistanceScale * airResistance;
    g = gBallPredictionGravityScale * gravity;
    float eToTheNegativeKT = Exp(-k * fTime);
    float oneMinusEToTheNegativeKTOverK
        = (1.0f / k) * (1.0f - eToTheNegativeKT);

    pPosition->x = pBall->m_v3Position.x
        + pBall->m_v3Velocity.x * oneMinusEToTheNegativeKTOverK;
    pPosition->y = pBall->m_v3Position.y
        + pBall->m_v3Velocity.y * oneMinusEToTheNegativeKTOverK;
    pPosition->z = pBall->m_v3Position.z + fTime * g / k
        + (1.0f / k) * oneMinusEToTheNegativeKTOverK
            * (k * pBall->m_v3Velocity.z - g);
    pPosition->z = nlMaxEquals(0.18f, pPosition->z);
}

float cBall::PredictLandingSpotAndTime(nlVector3& v3Dest,
    int* pNumSolutions, float* pTimes, float fHeight)
{
    float fTime = 0.0f;

    if (!nlNear(fHeight, 0.0f)
        || !nlNear(m_v3Position.z, 0.18f)
        || (m_v3Position.z <= fHeight && m_v3Velocity.z <= 0.0f))
    {
        int numSolutions;
        float times[2];

        float fGravity = m_pPhysicsBall->m_gravity;
        SolveQuadratic(0.5f * fGravity, m_v3Velocity.z,
            m_v3Position.z - fHeight,
            numSolutions, times[0], times[1]);

        if (pNumSolutions != NULL && pTimes != NULL)
        {
            *pNumSolutions = 0;
            float* root = times;
            for (int i = 0; i < numSolutions; i++)
            {
                if (*root > 0.0f)
                {
                    pTimes[*pNumSolutions] = *root;
                    (*pNumSolutions)++;
                }
                root++;
            }
            return 0.0f;
        }

        if (numSolutions == 2)
        {
            float solution1 = times[1];
            fTime = times[0];
            fTime = (fTime >= solution1) ? fTime : solution1;
        }
        else if (numSolutions == 1)
        {
            fTime = times[0];
        }
        else
        {
            return -9999.9f;
        }

        float k = gBallPredictionAirResistanceScale * m_pPhysicsBall->GetBallAirResistance();
        float g = gBallPredictionGravityScale * m_pPhysicsBall->GetGravity();
        float eToTheNegativeKT = Exp(-k * fTime);
        float oneOverK = 1.0f / k;
        float oneMinusEToTheNegativeKTOverK
            = oneOverK * (1.0f - eToTheNegativeKT);

        v3Dest.x = m_v3Position.x
            + m_v3Velocity.x * oneMinusEToTheNegativeKTOverK;
        v3Dest.y = m_v3Position.y
            + m_v3Velocity.y * oneMinusEToTheNegativeKTOverK;
        v3Dest.z = m_v3Position.z + fTime * g / k
            + oneOverK * oneMinusEToTheNegativeKTOverK
                * (k * m_v3Velocity.z - g);
        v3Dest.z = nlMaxEquals(0.18f, v3Dest.z);
        cField::FixOutOfBoundsPosition(v3Dest, 0.18f, true);
    }
    else
    {
        v3Dest = m_v3Position;
    }

    return fTime;
}

void ResetBall(cBall* pBall, bool bParam)
{
    nlVector3 v3Pos = { 0.0f, 0.0f, 0.18f };
    if (g_pTeams[0]->m_nScore == 0 && g_pTeams[1]->m_nScore == 0)
    {
        v3Pos.z = gInitialKickoffBallHeight;
    }

    float fChargeValue = 0.0f;
    if (gPreserveBallChargeOnReset)
    {
        fChargeValue = pBall->mfChargeValue;
    }

    if (pBall->m_pOwner != NULL)
    {
        pBall->m_pOwner->ReleaseBall(BALL_STATE_LOOSE);
    }

    pBall->m_pPhysicsBall->Unknown0();
    CancelBallShotImpl(pBall, false);

    pBall->m_bVisible = true;
    pBall->m_bBallPathChangeCount = 0;
    pBall->m_bBallDeflectCount = 0;
    pBall->m_fTotalPassTime = 0.0f;
    pBall->m_uGoalType = GOAL_UNCLASSIFIED;
    pBall->m_uVoiceID = 0;
    pBall->m_CurrentGlowEffect = 0;
    pBall->mfChargeValue = 0.0f;
    pBall->mfSkillShotTime = 0.0f;
    pBall->meBallState = BALL_STATE_LOOSE;
    pBall->mePrevBallState = BALL_STATE_LOOSE;
    pBall->m_pOwner = NULL;
    pBall->m_pPrevOwner = NULL;
    pBall->m_pLastTouch = NULL;
    pBall->m_pPassTarget = NULL;
    pBall->m_pShooter = NULL;
    pBall->mpDamageTarget = NULL;
    pBall->m_iConsecutiveVolleyPasses = 0;

    pBall->m_tNoPickupTimer.SetSeconds(0.0f);
    pBall->m_tShotTimer.SetSeconds(0.0f);
    pBall->m_tLightningTimer.SetSeconds(0.0f);
    pBall->m_tPassTargetTimer.SetSeconds(0.0f);
    pBall->mtStuckInRiotTimer.SetSeconds(0.0f);
    pBall->mtNoChargeLossTimer.SetSeconds(0.0f);
    pBall->mtShotClockTimer.SetSeconds(0.0f);

    pBall->m_v3Position.x = 0.0f;
    pBall->m_v3Position.y = 0.0f;
    pBall->m_v3Position.z = 0.18f;
    pBall->mnShotClockTeam = -1;
    pBall->mbStuckInRiotDone = false;
    pBall->mbBallOnFire = false;
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    pBall->mbBallFrozen = false;
#endif
    pBall->m_v3PrevPosition = pBall->m_v3Position;
    pBall->m_v3PassIntercept.x = 0.0f;
    pBall->m_v3PassIntercept.y = 0.0f;
    pBall->m_v3PassIntercept.z = 0.0f;
    pBall->m_qOrientation.z = 0.0f;
    pBall->m_qOrientation.y = 0.0f;
    pBall->m_qOrientation.x = 0.0f;
    pBall->m_qOrientation.w = 1.0f;
    pBall->m_v3Velocity.x = 0.0f;
    pBall->m_v3Velocity.y = 0.0f;
    pBall->m_v3Velocity.z = 0.0f;
    pBall->m_v3ShotTarget.x = 0.0f;
    pBall->m_v3ShotTarget.y = 0.0f;
    pBall->m_v3ShotTarget.z = 0.0f;
    pBall->m_v3ShotOrigin.x = 0.0f;
    pBall->m_v3ShotOrigin.y = 0.0f;
    pBall->m_v3ShotOrigin.z = 0.0f;

    pBall->WarpTo(v3Pos);
    pBall->ClearBallEffects();
    SetBallCharge(pBall, 0.0f);

    if (pBall->m_pOwner != NULL)
    {
        if (!pBall->m_pOwner->IsOnSameTeam(pBall->m_pPrevOwner))
        {
            pBall->mtShotClockTimer.SetSeconds(gfShotClockTime);
            pBall->mnShotClockTeam = pBall->m_pOwner->m_pTeam->m_nSide;
        }
    }
    else
    {
        pBall->mnShotClockTeam = -1;
        pBall->mtShotClockTimer.SetSeconds(0.0f);
    }

    if (gPreserveBallChargeOnReset && bParam)
    {
        SetBallCharge(pBall, fChargeValue);
    }
    else if (gRandomizeBallChargeOnReset)
    {
        SetBallCharge(pBall, nlRandomf(4.0f));
    }

    KillHeaderTarget(pBall, false);
}

void StopBallGlowSound(cBall* pBall)
{
    StopSound(pBall->m_uGlowSoundCue, pBall);
    pBall->m_uGlowSoundCue = 0;
}

static const nlVector3 sBallTrailUpVector = { 0.0f, 1.0f, 0.0f };

void RegisterBallEventCallbacks()
{
    FindEvent<void>("BallFall", -1)->Add(Function<void*>(OnBallFall), 0, -1);
    FindEvent<void(int, int)>("BallStateChange", -1)->Add(Function<void(int, int)>(OnBallStateChange), 0, -1);
    FindEvent<void>("ResetEffects", -1)->Add(Function<void*>(OnBallResetEffects), 0, -1);
    FindEvent<NoEventData>("Kickoff", -1)->Add(Function<FnVoidVoid>(OnBallKickoff), 0, -1);
    FindEvent<void>("GetReadyForKickoff", -1)->Add(Function<void*>(OnBallGetReadyForKickoff), 0, -1);
    FindEvent<NoEventData>("GameOver", -1)->Add(Function<FnVoidVoid>(OnBallGameOver), 0, -1);
    FindEvent<void>("CollisionBallTronWall", -1)->Add(Function<void*>(OnBallTronWallCollision), 0, -1);
    FindEvent<CollisionEggData>("CollisionEggBall", -1)->Add(Function<CollisionEggData*>(OnBallEggCollision), 0, -1);
    FindEvent<void>("CollisionDebrisBall", -1)->Add(Function<void*>(OnBallDebrisCollision), 0, -1);
    FindEvent<PhysicsPatch>("CollisionPatchBall", -1)->Add(Function<PhysicsPatch*>(OnBallPatchCollision), 0, -1);
    FindEvent<void>("CollisionThwompBall", -1)->Add(Function<void*>(OnBallThwompCollision), 0, -1);

    gNumBallTrails = 0;
    unsigned int i = 0;
    LiveBallTrail* pBallTrail = gBallTrails;
    for (; i < 10; ++i)
    {
        pBallTrail->visible = false;
        if (!pBallTrail->visible)
        {
            EmissionManager::Instance()->Destroy(
                (unsigned long)pBallTrail, NULL);
            if (pBallTrail->blurHandler != NULL)
            {
                pBallTrail->blurHandler->Die(gBallTrailFadeTime);
                pBallTrail->blurHandler = NULL;
            }
        }
        ++pBallTrail;
    }
}

void OnBallGameOver()
{
    cBall* pBall = g_pBall;
    if (pBall == NULL)
    {
        return;
    }

    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                           || pBall->meBallState == BALL_STATE_GROUND_PASS)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL
                && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

void OnBallResetEffects(void*)
{
    cBall* pBall = g_pBall;
    if (pBall == NULL)
    {
        return;
    }

    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                           || pBall->meBallState == BALL_STATE_GROUND_PASS)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL
                && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

void OnBallGetReadyForKickoff(void*)
{
    if (g_pBall == NULL)
    {
        return;
    }

    cFielder* pCaptain
        = g_pTeams[g_pGame->m_nLastTeamToScore]->GetCaptain();
    cFielder* pOtherCaptain
        = pCaptain->m_pTeam->GetOtherTeam()->GetCaptain();

    if (pCaptain->GetGlobalPad() != NULL)
    {
        SetPlayerAudioController(pCaptain);
        PlaySound(0, 0xCC32C1A8, NULL, NULL);
        PlayRumbleAction(RUMBLE_SMALL_CONTACT, pCaptain->GetGlobalPad());
    }

    if (pOtherCaptain->GetGlobalPad() != NULL)
    {
        SetPlayerAudioController(pOtherCaptain);
        PlaySound(0, 0xCC32C1A8, NULL, NULL);
        PlayRumbleAction(RUMBLE_SMALL_CONTACT, pOtherCaptain->GetGlobalPad());
    }

    if ((g_pTeams[0]->m_nScore > 0 || g_pTeams[1]->m_nScore > 0)
        && pOtherCaptain->CanReceivePass())
    {
        pOtherCaptain->PickupBall(g_pBall);
    }
}

void OnBallKickoff()
{
    if (g_pBall == NULL)
    {
        return;
    }
    if (g_pTeams[0]->m_nScore != 0)
    {
        return;
    }
    if (g_pTeams[1]->m_nScore != 0)
    {
        return;
    }
    if (g_pBall->m_pOwner != NULL)
    {
        return;
    }

    float fSpeedX
        = nlRandomf(gKickoffBallLongitudinalSpeed * gKickoffBallSpeedRandomness)
        + gKickoffBallLongitudinalSpeed * (1.0f - gKickoffBallSpeedRandomness);
    float fSpreadY
        = nlRandomf(gKickoffBallLateralSpeedRange * gKickoffBallSpeedRandomness)
        + gKickoffBallLateralSpeedRange * (1.0f - gKickoffBallSpeedRandomness);

    nlVector3 v3Velocity = { fSpeedX, 0.0f, 0.0f };
    v3Velocity.y
        = 0.5f * fSpreadY - nlRandomf(fSpreadY);
    v3Velocity.x = 0.0f;
    v3Velocity.z
        = 0.75f * gKickoffBallVerticalSpeed
        + nlRandomf(0.25f * gKickoffBallVerticalSpeed);

    g_pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, NULL);
}

void OnBallFall(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->meBallState != BALL_STATE_FALLING)
    {
        ChangeBallState(pBall, BALL_STATE_FALLING);
    }
}

void OnBallTronWallCollision(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                           || pBall->meBallState == BALL_STATE_GROUND_PASS)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL
                && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

void OnBallEggCollision(CollisionEggData*)
{
    cBall* pBall = g_pBall;
    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                           || pBall->meBallState == BALL_STATE_GROUND_PASS)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL
                && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

// The reference keeps the reset inlines from reusing the predicate's ball load.
static inline cBall*& GetPatchBall()
{
    return g_pBall;
}

void OnBallPatchCollision(PhysicsPatch* pPatch)
{
    int patchType = pPatch->GetType();
    if (patchType == PATCH_FIRE_BALL && g_pBall->m_pOwner == NULL
        && !IsBooSkillshotBall(g_pBall))
    {
        CancelBallShot(GetPatchBall(), false);

        nlVector3 v3Velocity;
        float fLengthSquared = pPatch->m_Velocity.GetLengthSq3D();
        if (fLengthSquared > 4.0f)
        {
            nlVec3Scale(v3Velocity, pPatch->m_Velocity,
                nlRecipSqrt(fLengthSquared, true));
            v3Velocity.z = nlRandomf(0.3f);
            nlVec3Scale(v3Velocity, 4.0f + nlRandomf(5.0f));
        }
        else
        {
            MakeRandomDirection2D(v3Velocity, 2.0f + nlRandomf(3.0f));
            v3Velocity.z = 5.0f + nlRandomf(5.0f);
        }

        cBall* pBall = g_pBall;
        pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, NULL);
        pBall->m_tNoPickupTimer.SetSeconds(0.1f);
        PhysicsBall* pPhysicsBall = pBall->m_pPhysicsBall;
        pPhysicsBall->mbUseMagnusEffect = false;
        pPhysicsBall->mfChargeBonus = 0.0f;
    }
    else if (patchType == PATCH_CHAIN_LIGHTNING)
    {
        SetBallChargeWithScale(g_pBall, 4.0f);
    }

    if (pPatch->GetPosition().z != 0.0f)
    {
        return;
    }
    if (!(g_pBall->m_pPhysicsBall->GetPosition().z < 0.207f))
    {
        return;
    }

    int nBallState = g_pBall->meBallState;
    switch (nBallState)
    {
    case BALL_STATE_POSSESSED:
    case BALL_STATE_SHOT:
    case BALL_STATE_CHIP_SHOT:
    case BALL_STATE_SKILLSHOT:
        return;
    default:
        break;
    }

    int nPatchType = pPatch->m_Type;
    PhysicsPatchInfo* pPatchInfo = GetPhysicsPatchInfo(nPatchType);
    if (pPatchInfo->mFriction != 0.0f)
    {
        nlVector3 v3Force;
        g_pBall->m_pPhysicsBall->GetLinearVelocity(&v3Force);
        nlVec3Scale(
            v3Force, -10.0f * pPatchInfo->mFriction);
        g_pBall->m_pPhysicsBall->AddForceAtCentreOfMass(v3Force);
    }

    if (pPatch->m_Type != PATCH_LAVA_HOLE)
    {
        return;
    }
    if (g_pBall->m_pOwner != NULL)
    {
        return;
    }
    if (IsBooSkillshotBall(g_pBall))
    {
        return;
    }

    CancelBallShot(GetPatchBall(), false);
    SetBallChargeWithScale(g_pBall, 4.0f);

    nlVector3 v3Velocity;
    g_pBall->m_pPhysicsBall->GetLinearVelocity(&v3Velocity);
    v3Velocity.z = 0.0f;
    float fLengthSquared = v3Velocity.GetLengthSq3D();
    if (fLengthSquared < gLavaBallRandomDirectionSpeedThreshold * gLavaBallRandomDirectionSpeedThreshold)
    {
        MakeRandomDirection2D(v3Velocity, gMaxLavaBallHorizontalSpeed);
    }
    else
    {
        float fLength = nlSqrt(fLengthSquared, true);
        if (fLength > gMaxLavaBallHorizontalSpeed)
        {
            fLength = gMaxLavaBallHorizontalSpeed;
        }
        nlVec3Scale(v3Velocity,
            nlRecipSqrt(v3Velocity.GetLengthSq3D(), false));
        nlVec3Scale(v3Velocity, fLength * gLavaBallHorizontalSpeedScale);
    }

    v3Velocity.z = gLavaBallVerticalSpeed;
    g_pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, NULL);
}

void OnBallDebrisCollision(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                           || pBall->meBallState == BALL_STATE_GROUND_PASS)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL
                && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

void OnBallThwompCollision(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->m_pOwner != NULL)
    {
        ChangeBallState(pBall, BALL_STATE_POSSESSED);
        return;
    }

    bool bPassTarget = (pBall->meBallState == BALL_STATE_VOLLEY_PASS
                           || pBall->meBallState == BALL_STATE_GROUND_PASS)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (ReceivingPass((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)GetFielderDesire(pFielder, FIELDER_DESIRE_RECEIVE_PASS);
            if (pReceivePass != NULL
                && pReceivePass->IsActive()
                && pReceivePass->meDesireSubState == RECEIVE_PASS_APPROACH)
            {
                return;
            }

            ChangeBallState(pBall, BALL_STATE_LOOSE);
            return;
        }
    }

    ChangeBallState(pBall, BALL_STATE_LOOSE);
}

void OnBallStateChange(int previousState, int currentState)
{
    if (previousState == BALL_STATE_SKILLSHOT && currentState != BALL_STATE_SKILLSHOT)
    {
        g_pBall->mfSkillShotTime = 0.0f;

        cBall* pBall = g_pBall;
        if (IsDryBonesSkillshot(pBall))
        {
            if (gForceFullBallCharge
                || GameInfoManager::Instance()->IsWhiteBallEnabled())
            {
                pBall->mfChargeValue = 4.0f;
            }
            else
            {
                pBall->mfChargeValue = 4.0f;
            }

            float fMaxChargeScale = 4.0f;
            float fMaxCharge = gMaxBallChargeScale * fMaxChargeScale;
            if (pBall->mfChargeValue >= fMaxCharge)
            {
                pBall->mfChargeValue = fMaxCharge;
            }
            else if (pBall->mfChargeValue < 0.0f)
            {
                pBall->mfChargeValue = 0.0f;
            }

            UpdateBallGlow(pBall);
        }

        KoopaShellObject* pKoopaShell
            = gNPCManager->mpKoopaShell;
        if (pKoopaShell != NULL && pKoopaShell->mVisible)
        {
            pKoopaShell->Deactivate(false);
        }

        BirdoEggObject* pState = gNPCManager->mpBirdoEgg;
        if (pState != NULL && pState->mVisible)
        {
            pState->Hide(false);
        }

        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_bVisible = true;
        g_pBall->m_pPhysicsBall->RestoreBallForces();

        if (g_pBall->mbBallOnFire)
        {
            g_pBall->mbBallOnFire = false;
            KillBallShot("skillshot_ball_meteor", 0);
        }

        g_pBall->ClearBallEffects();
    }
}

static unsigned short sBallDebugType = 0xFFFF;
char gTweakFloatBindingFormat[] __attribute__((aligned(4))) = "%.3f";

inline void RegisterBallDebugFields(cBall* ball, DebugWriteCache* cache)
{
    cache->AddField(16, gDebugFieldTypes[16].size, 0,
        "m_bVisible");
    cache->AddField(9, gDebugFieldTypes[9].size,
        (u8*)&ball->m_bBallPathChangeCount - (u8*)ball,
        "m_bBallPathChangeCount");
    cache->AddField(9, gDebugFieldTypes[9].size,
        (u8*)&ball->m_bBallDeflectCount - (u8*)ball,
        "m_bBallDeflectCount");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&ball->m_tLightningTimer - (u8*)ball, "m_tLightningTimer");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&ball->m_tShotTimer - (u8*)ball,
        "m_tShotTimer");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&ball->m_tNoPickupTimer - (u8*)ball,
        "m_tNoPickupTimer");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&ball->m_tPassTargetTimer - (u8*)ball,
        "m_tPassTargetTimer");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&ball->mtStuckInRiotTimer - (u8*)ball,
        "mtStuckInRiotTimer");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&ball->mtNoChargeLossTimer - (u8*)ball,
        "mtNoChargeLossTimer");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&ball->mtShotClockTimer - (u8*)ball,
        "mtShotClockTimer");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&ball->mnShotClockTeam - (u8*)ball, "mnShotClockTeam");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&ball->mbStuckInRiotDone - (u8*)ball,
        "mbStuckInRiotDone");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&ball->mbBallOnFire - (u8*)ball, "mbBallOnFire");
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&ball->mbBallFrozen - (u8*)ball, "mbBallFrozen");
#endif
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&ball->m_fTotalPassTime - (u8*)ball, "m_fTotalPassTime");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&ball->m_iConsecutiveVolleyPasses - (u8*)ball,
        "m_iConsecutiveVolleyPasses");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&ball->m_v3Position - (u8*)ball, "m_v3Position");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&ball->m_v3PrevPosition - (u8*)ball, "m_v3PrevPosition");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&ball->m_v3Velocity - (u8*)ball, "m_v3Velocity");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&ball->m_v3PassIntercept - (u8*)ball, "m_v3PassIntercept");
    cache->AddField(24, gDebugFieldTypes[24].size,
        (u8*)&ball->m_qOrientation - (u8*)ball, "m_qOrientation");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&ball->m_v3ShotTarget - (u8*)ball, "m_v3ShotTarget");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&ball->m_v3ShotOrigin - (u8*)ball, "m_v3ShotOrigin");
    cache->AddField(2, gDebugFieldTypes[2].size,
        (u8*)&ball->m_uGoalType - (u8*)ball, "m_uGoalType");
    cache->AddField(2, gDebugFieldTypes[2].size,
        (u8*)&ball->m_uVoiceID - (u8*)ball, "m_uVoiceID");
    cache->AddField(2, gDebugFieldTypes[2].size,
        (u8*)&ball->m_CurrentGlowEffect - (u8*)ball,
        "m_CurrentGlowEffect");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&ball->mfChargeValue - (u8*)ball, "mfChargeValue");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&ball->mfSkillShotTime - (u8*)ball, "mfSkillShotTime");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&ball->meBallState - (u8*)ball, "meBallState");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&ball->mePrevBallState - (u8*)ball, "mePrevBallState");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&ball->m_pOwner - (u8*)ball, "m_pOwner");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&ball->m_pPrevOwner - (u8*)ball, "m_pPrevOwner");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&ball->m_pLastTouch - (u8*)ball, "m_pLastTouch");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&ball->m_pPassTarget - (u8*)ball, "m_pPassTarget");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&ball->m_pShooter - (u8*)ball, "m_pShooter");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&ball->mpDamageTarget - (u8*)ball, "mpDamageTarget");
}

void cBall::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sBallDebugType == 0xFFFF)
    {
        sBallDebugType = cache->BeginType("DetBall");
        RegisterBallDebugFields(this, cache);
        cache->EndType();
    }

    cBall* copy = (cBall*)cache->WriteData(sBallDebugType, this, offsetof(cBall, m_pBlurHandler));
    if (copy != NULL)
    {
        *(int*)&copy->m_pOwner
            = m_pOwner == NULL ? -1 : m_pOwner->m_nCharacterIndex;
        *(int*)&copy->m_pPrevOwner
            = m_pPrevOwner == NULL ? -1 : m_pPrevOwner->m_nCharacterIndex;
        *(int*)&copy->m_pLastTouch
            = m_pLastTouch == NULL ? -1 : m_pLastTouch->m_nCharacterIndex;
        *(int*)&copy->m_pPassTarget
            = m_pPassTarget == NULL ? -1 : m_pPassTarget->m_nCharacterIndex;
        *(int*)&copy->m_pShooter
            = m_pShooter == NULL ? -1 : m_pShooter->m_nCharacterIndex;
        *(int*)&copy->mpDamageTarget
            = mpDamageTarget == NULL ? -1 : mpDamageTarget->m_nCharacterIndex;
        cache->ChecksumData(sBallDebugType, copy, context);
    }
}

void cBall::ChecksumState(RunningChecksum* runningChecksum)
{
    runningChecksum->ChecksumData(&m_v3Position, sizeof(m_v3Position));
    runningChecksum->ChecksumData(&m_v3Velocity, sizeof(m_v3Velocity));
    runningChecksum->ChecksumData(&m_qOrientation, sizeof(m_qOrientation));
    runningChecksum->ChecksumData(&meBallState, sizeof(meBallState));
    runningChecksum->ChecksumData(&mfChargeValue, sizeof(mfChargeValue));
    runningChecksum->ChecksumData(
        &mfSkillShotTime, sizeof(mfSkillShotTime));
}

LiveBallTrail::LiveBallTrail()
{
    drawable = NULL;
    blurHandler = NULL;
    visible = false;
    orientation.z = 0.0f;
    orientation.y = 0.0f;
    orientation.x = 0.0f;
    orientation.w = 1.0f;
    position = v3Zero;
    velocity = v3Zero;
    angularVelocity = v3Zero;
}

LiveBallTrail::~LiveBallTrail()
{
    if (blurHandler != NULL)
    {
        blurHandler->Die(gBallTrailFadeTime);
        blurHandler = NULL;
    }
}

void SetBallTrailVisible(LiveBallTrail* pBallTrail, bool bParam)
{
    pBallTrail->visible = bParam;
    if (!pBallTrail->visible)
    {
        EmissionManager::Instance()->Destroy(
            (unsigned long)pBallTrail, NULL);
        if (pBallTrail->blurHandler != NULL)
        {
            pBallTrail->blurHandler->Die(gBallTrailFadeTime);
            pBallTrail->blurHandler = NULL;
        }
    }
}

void UpdateBallTrail(LiveBallTrail* pBallTrail, float fParam)
{
    nlVec3ScaleAdd(pBallTrail->position, fParam,
        pBallTrail->velocity, pBallTrail->position);

    if (pBallTrail->blurHandler != NULL)
    {
        nlVector3 v3Position;
        float fVelocityLengthSq
            = pBallTrail->velocity.GetLengthSq3D();
        if (fVelocityLengthSq > 0.1f)
        {
            nlVec3Normalize(v3Position, pBallTrail->velocity);
            nlVec3Scale(v3Position, 0.36f);
            nlVec3Add(
                v3Position, v3Position, pBallTrail->position);
        }
        else
        {
            v3Position = pBallTrail->position;
        }

        nlVector3 v3Up = sBallTrailUpVector;
        if (pBallTrail->velocity.GetLengthSq3D() < 0.1f)
        {
            v3Up = v3Zero;
        }
        pBallTrail->blurHandler->AddViewOrientedPoint(
            v3Position, v3Up);
    }

    if (fabsf(pBallTrail->position.x) > 100.0f
        || fabsf(pBallTrail->position.y) > 100.0f
        || fabsf(pBallTrail->position.z) > 100.0f)
    {
        pBallTrail->visible = false;
        pBallTrail->position = v3Zero;
        pBallTrail->velocity = v3Zero;
    }

    nlVector3 v3Rotation;
    nlVec3Scale(
        v3Rotation, pBallTrail->angularVelocity, fParam);
    nlQuaternion qOrientation;
    qOrientation.x = pBallTrail->orientation.x
        + 0.5f * (v3Rotation.x * pBallTrail->orientation.w
                     + v3Rotation.y * pBallTrail->orientation.z
                     - v3Rotation.z * pBallTrail->orientation.y);
    qOrientation.y = pBallTrail->orientation.y
        + 0.5f * (v3Rotation.y * pBallTrail->orientation.w
                     + v3Rotation.z * pBallTrail->orientation.x
                     - v3Rotation.x * pBallTrail->orientation.z);
    qOrientation.z = pBallTrail->orientation.z
        + 0.5f * (v3Rotation.z * pBallTrail->orientation.w
                     + v3Rotation.x * pBallTrail->orientation.y
                     - v3Rotation.y * pBallTrail->orientation.x);
    qOrientation.w = pBallTrail->orientation.w
        - 0.5f * (v3Rotation.x * pBallTrail->orientation.x
                     + v3Rotation.y * pBallTrail->orientation.y
                     + v3Rotation.z * pBallTrail->orientation.z);
    nlQuatNormalize(pBallTrail->orientation, qOrientation);
}

void InitializeMegaStrikeBallTrail(
    LiveBallTrail* pBallTrail, cFielder* pFielder)
{
    char effectName[64];
    char textureName[64];

    nlSNPrintf(effectName, sizeof(effectName),
        "%s_megastrike_home_3_gameplay",
        pFielder->m_pCharacterInfo->mName);

    EffectsGroup* pEffectsGroup = EmissionManager::Instance()->GetEffectsGroup(effectName);
    EmissionController* pController = EmissionManager::Instance()->Create(pEffectsGroup, 0, true, 0);
    pController->m_fGround = 0.02f;
    pController->SetPosition(pBallTrail->position);
    pController->m_uUserData = (u32)pBallTrail;
    pController->SetUpdateCallback(
        Function1<void, EmissionController&>(UpdateEmitterFromBallTrail));

    pEffectsGroup = EmissionManager::Instance()->GetEffectsGroup("megastrike_ball_launch");
    pController = EmissionManager::Instance()->Create(pEffectsGroup, 0, true, 0);
    pController->SetPosition(pBallTrail->position);
    pController->SetVelocity(v3Zero);

    if (pBallTrail->blurHandler != NULL)
    {
        pBallTrail->blurHandler->Die(gBallTrailFadeTime);
        pBallTrail->blurHandler = NULL;
    }

    switch (pFielder->m_DetChar.m_eCharacterClass)
    {
    case BOWSER:
        nlStrNCpy(textureName,
            szBowserShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case BOWSERJR:
        nlStrNCpy(textureName,
            szBowserJrShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case DAISY:
        nlStrNCpy(textureName,
            szDaisyShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case DONKEYKONG:
        nlStrNCpy(textureName,
            szDonkeyKongShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case DIDDYKONG:
        nlStrNCpy(textureName,
            szDiddyKongShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case LUIGI:
        nlStrNCpy(textureName,
            szLuigiShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case MARIO:
        nlStrNCpy(textureName,
            szMarioShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case PEACH:
        nlStrNCpy(textureName,
            szPeachShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case PETEY:
        nlStrNCpy(textureName,
            szPeteyShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case WALUIGI:
        nlStrNCpy(textureName,
            szWaluigiShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case WARIO:
        nlStrNCpy(textureName,
            szWarioShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case YOSHI:
        nlStrNCpy(textureName,
            szYoshiShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    }

    pBallTrail->blurHandler = BlurManager::GetNewHandler(
        textureName,
        0.18f * gBallTrailWidthScale * 0.925f,
        gBallTrailMaxPositionEntries,
        true);
}

LiveBallTrail* GetBallTrail(unsigned int nIndex)
{
    return &gBallTrails[nIndex];
}

void UpdateBallTrails(float fParam)
{
    LiveBallTrail* pBallTrail = gBallTrails;
    for (unsigned int i = 0; i < gNumBallTrails; ++i)
    {
        if (pBallTrail->visible)
        {
            UpdateBallTrail(pBallTrail, fParam);
        }
        ++pBallTrail;
    }
}

unsigned int GetNumBallTrails()
{
    return gNumBallTrails;
}

void InitializeBallTrails(unsigned int nNumTrails)
{
    gNumBallTrails = nNumTrails;
    nlVector3 v3ZeroValue = v3Zero;

    unsigned int i = 0;
    for (; i < nNumTrails; ++i)
    {
        LiveBallTrail* pBallTrail = &gBallTrails[i];
        SetBallTrailVisible(pBallTrail, false);
        pBallTrail->position = v3ZeroValue;
        pBallTrail->velocity = v3ZeroValue;
        pBallTrail->drawable = (DrawableModel*)GetBallRenderObject(i);
    }

    for (; i < 10; ++i)
    {
        LiveBallTrail* pBallTrail = &gBallTrails[i];
        SetBallTrailVisible(pBallTrail, false);
    }
}

unsigned int gNumBallTrails = 0;
