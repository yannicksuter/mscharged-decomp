#include "Game/GameTweaks.h"
#include "Game/GameInfo.h"
#include "Game/TweakConfig.h"
#include "Game/TweakFileLoader.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakValueInt.h"
#include "NL/nlPrint.h"

#include <stdlib.h>

GameTweaks::GameTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mCategory(category)
{
    Init();
    RegisterTweaks(true);
}

void GameTweaks::RegisterTweaks(bool registerTweaks)
{
    if (registerTweaks)
        gTweakFileLoader.LoadFileAsync(mszFileName, mCategory);
    else
        LoadTweakConfigFile(mszFileName, mCategory, true);
}

TweakIntBinding::TweakIntBinding(int* value)
    : m_pValue(value)
{
}

bool TweakIntBinding::BindWithDefault(const char* name, int defaultValue,
    const char* group, bool reload, float value, float min, float max)
{
    bool found = Bind(name, value, group, reload, min, max);
    if (!found)
    {
        *m_pValue = defaultValue;
    }
    return found;
}

int TweakIntBinding::GetDefault()
{
    return 0;
}

int TweakIntBinding::GetValueType()
{
    return 3;
}

int TweakIntBinding::GetStorageKind()
{
    return 2;
}

void TweakIntBinding::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

TweakValueBase* TweakIntBinding::CreateValue(
    const char* name, void* entry)
{
    TweakValueInt* created = new (
        gTweakValueAllocator->Allocate(sizeof(TweakValueInt)))
        TweakValueInt(name, 0);
    AddTweakValue((TweakEntry*)entry, created);
    return created;
}

void* TweakIntBinding::GetValueAddress()
{
    return m_pValue;
}

void TweakIntBinding::FormatValue(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", *m_pValue);
}

void TweakIntBinding::ParseValue(const char* value)
{
    *m_pValue = atoi(value);
}

void TweakIntBinding::CopyValueFrom(
    TweakValueBase* other)
{
    switch (other->GetStorageKind())
    {
    case 1:
        *m_pValue = ((TweakValueInt*)other)->value;
        break;
    case 2:
        *m_pValue = *((TweakIntBinding*)other)->m_pValue;
        break;
    }
}

int TweakIntBinding::IsBound()
{
    return m_pValue != 0;
}

void TweakIntBinding::BindValueAddress(void* value)
{
    m_pValue = (int*)value;
}

void GameTweaks::Init()
{
    fGameDuration.BindWithDefault("Game Duration", (float)GameInfoManager::Instance()->GetCurrentSettings()->GameTime, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFielderAttributeWeight.BindWithDefault("Fielder Attributes Weight", 0.2f, mCategory, false, 0.0f, 0.0f, 0.0f);
    vGetInPositionKeyFielderDistX.BindWithDefault("Get In Position Key Fielder Min Distance", 2.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    vGetInPositionKeyFielderDistY.BindWithDefault("Get In Position Key Fielder Max Distance", 12.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    vGetInPositionInRadiusX.BindWithDefault("At Target Position Min Radius", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    vGetInPositionInRadiusY.BindWithDefault("At Target Position Max Radius", 0.6f, mCategory, false, 0.0f, 0.0f, 0.0f);
    vGetInPositionOutRadiusX.BindWithDefault("At Target Remain At Position Min Radius", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    vGetInPositionOutRadiusY.BindWithDefault("At Target Remain At Position Max Radius", 4.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nStrafeToRunInDirectionDelta.BindWithDefault("Run To Strafe Angle", 11500.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nBackwardsToStrafeRunInDirectionDelta.BindWithDefault("Backwards To Strafe Angle", 23000.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nStrafeToRunOutDirectionDelta.BindWithDefault("Strafe To Run Angle", 9000.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nBackwardsToStrafeRunOutDirectionDelta.BindWithDefault("Strafe To Backwards Angle", 25000.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideAttackRadius.BindWithDefault("Do Slide Attack Radius", 2.15f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fArrivalInRadius.BindWithDefault("Arrived In Radius", 0.45f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fArrivalOutRadius.BindWithDefault("Arrived Out Radius", 0.85f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearSeekInRadius.BindWithDefault("Near In Radius", 1.55f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fNearSeekOutRadius.BindWithDefault("Near Out Radius", 2.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSwapControllerTime.BindWithDefault("Swap Controller No Swap Back Time", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSwapFacingTime.BindWithDefault("Swap Controller Dampen Facing Time", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFreezeShellFrozenTime.BindWithDefault("Freeze Shell Frozen Time", 3.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaActiveTime.BindWithDefault("Banana Time", 5.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombActiveTime.BindWithDefault("Bobomb Time", 8.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fChainChompActiveTime.BindWithDefault("Chain Chomp Time", 8.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fChainChompFallTime.BindWithDefault("Chain Chomp Fall Time", 8.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fChainChompSpeed.BindWithDefault("Chain Chomp Speed", 8.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nScoreDifferenceMaximum.BindWithDefault("Score Difference Maximum", 1, mCategory, false, 0.0f, 0.0f, 0.0f);
    nScoreDifferenceMinimum.BindWithDefault("Score Difference Minimum", 5, mCategory, false, 0.0f, 0.0f, 0.0f);
    nPowerupsNumForPenalty.BindWithDefault("Powerups Awarded Penalty", 2, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaResistance.BindWithDefault("Banana Resistance", 5.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellBounceGround.BindWithDefault("Shell Bounce Ground", 0.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellBounce.BindWithDefault("Shell Bounce Other", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellSmallRadius.BindWithDefault("Shell Small Radius", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellMediumRadius.BindWithDefault("Shell Medium Radius", 0.6f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellBigRadius.BindWithDefault("Shell Big Radius", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellMediumChance.BindWithDefault("Shell Medium Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellBigChance.BindWithDefault("Shell Big Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellFiveChance.BindWithDefault("Shell Five Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellThreeChance.BindWithDefault("Shell Three Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellExplodeChance.BindWithDefault("Shell Explode Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaSmallRadius.BindWithDefault("Banana Small Radius", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaMediumRadius.BindWithDefault("Banana Medium Radius", 0.6f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaBigRadius.BindWithDefault("Banana Big Radius", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaMediumChance.BindWithDefault("Banana Medium Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaBigChance.BindWithDefault("Banana Big Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaFiveChance.BindWithDefault("Banana Five Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaThreeChance.BindWithDefault("Banana Three Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBananaExplodeChance.BindWithDefault("Banana Explode Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombSmallRadius.BindWithDefault("Bobomb Small Radius", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombMediumRadius.BindWithDefault("Bobomb Medium Radius", 0.6f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombBigRadius.BindWithDefault("Bobomb Big Radius", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombMediumChance.BindWithDefault("Bobomb Medium Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombBigChance.BindWithDefault("Bobomb Big Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombFiveChance.BindWithDefault("Bobomb Five Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombThreeChance.BindWithDefault("Bobomb Three Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBobombMineChance.BindWithDefault("Bobomb Mine Chance", 0.33f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupExplosionRadiusMin.BindWithDefault("Powerup Explosion Radius Min", 3.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupExplosionRadiusMax.BindWithDefault("Powerup Explosion Radius Max", 6.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupArrowThrowChance.BindWithDefault("Powerup Arrow Throw Chance", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupSpreadThrowChance.BindWithDefault("Powerup Spread Throw Chance", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupSurroundThrowChance.BindWithDefault("Powerup Surround Throw Chance", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupHorizontalLineThrowChance.BindWithDefault("Powerup H Line Throw Chance", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupHitWithBallMinAmount.BindWithDefault("Powerup Hit With Ball Min", 0.1f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupHitWithBallMaxAmount.BindWithDefault("Powerup Hit With Ball Max", 0.15f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupHitNoBallMinAmount.BindWithDefault("Powerup Hit No Ball Min", 0.25f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupHitNoBallMaxAmount.BindWithDefault("Powerup Hit No Ball Max", 0.35f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupSlideWithBallMinAmount.BindWithDefault("Powerup Slide With Ball Min", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupSlideWithBallMaxAmount.BindWithDefault("Powerup Slide With Ball Max", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupSlideNoBallMinAmount.BindWithDefault("Powerup Slide No Ball Min", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupSlideNoBallMaxAmount.BindWithDefault("Powerup Slide No Ball Max", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupPowerShotMinAmount.BindWithDefault("Powerup Power Shot Min", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupPowerShotMaxAmount.BindWithDefault("Powerup Power Shot Max", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupInterceptPassMinAmount.BindWithDefault("Powerup Intercept Pass Min", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupInterceptPassMaxAmount.BindWithDefault("Powerup Intercept Pass Max", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupPerfectPassMinAmount.BindWithDefault("Powerup Perfect Pass Min", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupPerfectPassMaxAmount.BindWithDefault("Powerup Perfect Pass Max", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupContextDekeMinAmount.BindWithDefault("Powerup Context Deke Min", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupContextDekeMaxAmount.BindWithDefault("Powerup Context Deke Max", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupIconSpeed.BindWithDefault("Powerup Icon Speed", 0.2f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nPowerupBoxMaxActiveOnField.BindWithDefault("Powerup Box Max Amount", 2, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupBoxDelayDropTime.BindWithDefault("Powerup Box Delay Drop Time", 10.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fChainChompRadius.BindWithDefault("Chain Chomp Radius", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fDrawPowerupIconTime.BindWithDefault("Draw Powerup Icon Time", 1.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fAngleWeighting.BindWithDefault("Powerup Angle Weighting", 0.0005f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fHitAngleWeighting.BindWithDefault("Hit Angle Weighting", 0.0005f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fDekeAngleWeighting.BindWithDefault("Deke Angle Weighting", 0.0005f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPassDistanceWeight.BindWithDefault("Pass Distance Weight", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPassIdealMinDistance.BindWithDefault("Pass Ideal Min Distance", 7.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPassIdealMinRange.BindWithDefault("Pass Ideal Min Range", 10.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPassIdealMaxDistance.BindWithDefault("Pass Ideal Max Distance", 7.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPassIdealMaxRange.BindWithDefault("Pass Ideal Max Range", 10.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nPassMinimumAngle.BindWithDefault("Pass Minimum Angle", 0, mCategory, false, 0.0f, 0.0f, 0.0f);
    nPassMaximumAngle.BindWithDefault("Pass Maximum Angle", 120, mCategory, false, 0.0f, 0.0f, 0.0f);
    fVolleyPassDistanceWeight.BindWithDefault("Volley Pass Distance Weight", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fVolleyPassIdealMinDistance.BindWithDefault("Volley Pass Ideal Min Distance", 7.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fVolleyPassIdealMinRange.BindWithDefault("Volley Pass Ideal Min Range", 10.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fVolleyPassIdealMaxDistance.BindWithDefault("Volley Pass Ideal Max Distance", 7.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fVolleyPassIdealMaxRange.BindWithDefault("Volley Pass Ideal Max Range", 10.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nVolleyPassMinimumAngle.BindWithDefault("Volley Pass Minimum Angle", 0, mCategory, false, 0.0f, 0.0f, 0.0f);
    nVolleyPassMaximumAngle.BindWithDefault("Volley Pass Maximum Angle", 120, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFrontAudibleSurrDist.BindWithDefault("Front Audible Surr Dist", 100.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBackAudibleSurrDist.BindWithDefault("Back Audible Surr Dist", 100.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSpeedOfSoundForDoppler.BindWithDefault("Speed Of Sound For Doppler", 200.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fEmitterDistFromListenerMaxVol.BindWithDefault("Emitter Dist From Listener Max Vol", 0.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fMaxAudibleEmitterDistance.BindWithDefault("Max Aubible Emitter Distance", 100.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fEmitterVolToDistanceValue.BindWithDefault("Emitter Vol To Distance Value", 0.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadeFilterSlowMoInTime.BindWithDefault("Slow Mo Filter Fade In Time", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadeFilterSlowMoOutTime.BindWithDefault("Slow Mo Filter Fade Out Time", 0.2f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadeFilterFreqMin.BindWithDefault("Filter Min Freq Pct", 0.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadeFilterFreqMax.BindWithDefault("Filter Max Freq Pct", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadeFilterDropoffDelayTime.BindWithDefault("Filter Dropoff Delay Time", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadeFilterDropoffTime.BindWithDefault("Filter Dropoff Time", 6.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadePitchMin.BindWithDefault("Dialogue Pitch Min Pct", 0.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadePitchMax.BindWithDefault("Dialogue Pitch Max Pct", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fMinBobombMoveSFXTime.BindWithDefault("Min Bobomb Move SFX Activation Time", 2.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPerfectPassProximityFilterDistSq.BindWithDefault("Perfect Pass SFX Proximity Filter Dist Sq", 16.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupSmallSizeVolCoeff.BindWithDefault("Small Powerup Volume Coefficient", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupMedSizeVolCoeff.BindWithDefault("Medium Powerup Volume Coefficient", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPowerupLargeSizeVolCoeff.BindWithDefault("Large Powerup Volume Coefficient", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFadePerfectPassTrailSFXStartTime.BindWithDefault("Perfect Pass Trail SFX Fade Start Time", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fMinHitIntensityForHardBodyHitSFX.BindWithDefault("Min Hit Intensity to play BODY_HIT_HARD SFX", 0.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideAttackHitReactionVolume.BindWithDefault("Slide Attack Hit Reaction Volume", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShootToScoreBallHitReactionVolume.BindWithDefault("S2S Ball Hit Reaction Volume", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBombHitReactionVolume.BindWithDefault("Bomb Hit Reaction Volume", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBombShockwaveReactionVolume.BindWithDefault("Bomb Shockwave Reaction Volume", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSmallShellHitReactionVolume.BindWithDefault("Small Shell Hit Reaction Volume", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fMediumShellHitReactionVolume.BindWithDefault("Medium Shell Hit Reaction Volume", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fLargeShellHitReactionVolume.BindWithDefault("Large Shell Hit Reaction Volume", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGoalieDropKickHitReactionVolume.BindWithDefault("Goalie Drop Kick Hit Reaction Volume", 0.75f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBallHitWallMaxAudibleVelocity.BindWithDefault("Ball Hit Wall Max Audible Velocity", 35.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBallHitWallMinAudibleVelocity.BindWithDefault("Ball Hit Wall Min Audible Velocity", 2.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBallHitWallMinVolume.BindWithDefault("Ball Hit Wall Min Volume", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBallHitWallMinTimeBeforeNextAudio.BindWithDefault("Ball Hit Wall Min Time Before Next Audio", 0.2f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBallHitNetMaxAudibleVelocity.BindWithDefault("Ball Hit Net Max Audible Velocity", 35.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBallHitNetMinAudibleVelocity.BindWithDefault("Ball Hit Net Min Audible Velocity", 3.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBallHitNetMinVolume.BindWithDefault("Ball Hit Net Min Volume", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBallHitNetMinTimeBeforeNextAudio.BindWithDefault("Ball Hit Net Min Time Before Next Audio", 1.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fPerfectPassSlowMo.BindWithDefault("Perfect Pass Slow Mo", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSkillshotSlowMoRate.BindWithDefault("Skillshot Slow Mo Rate", 0.1f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSkillshotSlowMoTime.BindWithDefault("Skillshot Slow Mo Time", 0.1f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSkillshotSlowMoDuration.BindWithDefault("Skillshot Slow Mo Duration", 0.1f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSkillshotSlowMoRamp.BindWithDefault("Skillshot Slow Mo Ramp", 0.1f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShootToScoreBallBlurWidth.BindWithDefault("S2S Ball Blur Width", 0.3f, mCategory, false, 0.0f, 0.0f, 0.0f);
    nShootToScoreBallBlurLength.BindWithDefault("S2S Ball Blur Length", 30, mCategory, false, 0.0f, 0.0f, 0.0f);
    fLeftTriggerDownPressure.BindWithDefault("Left Trigger Pressure", 0.7f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fIndicatorDistAboveHead.BindWithDefault("Indicator Distance Above Head", 0.4525f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fIndicatorDistInPixels.BindWithDefault("Indicator Pixel Clearance", 6.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotPostOffset.BindWithDefault("Shot Width Offset From Post", 0.25f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotHeightOffsetFromPost.BindWithDefault("Shot Height Offset From Post", 0.1f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotWidthVariance.BindWithDefault("Shot Width Variance", 0.12f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotHeightVariance.BindWithDefault("Shot Height Variance", 0.06f, mCategory, false, 0.0f, 0.0f, 0.0f);
}
