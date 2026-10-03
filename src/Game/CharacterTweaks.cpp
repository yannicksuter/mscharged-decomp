#pragma pool_data off

#include "Game/CharacterTweaks.h"
#include "Game/Terrain.h"

#include "Game/AI/AiUtil.h"
#include "Game/Game.h"
#include "Game/GameTweaks.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakConfig.h"
#include "Game/TweakFileLoader.h"
#include "Game/TweakValue.inl"


float g_pTweaks[2] = {
    10.0f,
    0.0f,
};

FielderTweaks::FielderTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mCategory(category)
{
    Init();
    gTweakFileLoader.LoadFileAsync(mszFileName, mCategory);
}

FielderTweaks::~FielderTweaks()
{
}

void FielderTweaks::Init()
{
    fRunTurnFalloff = 4000.0f;
    fRunDecel = 12.5f;
    mUnidentified47C = 9.5f;
    fStrafeTurnFalloff = 4000.0f;
    fStrafeDecel = 15.0f;
    fStrafeAccel = 15.0f;
    fJogTurnSpeedMin = 30000.0f;
    fJogTurnSpeedMax = 40000.0f;
    mUnidentified494 = 0.85f;
    fStrafeTurnSpeed = 120000.0f;
    mUnidentified49C = 0.9f;
    mUnidentified4A0 = 2500.0f;
    mUnidentified4A4 = 22.5f;
    mUnidentified4A8 = 18.0f;
    fRunWBTurnFalloff = 3200.0f;
    fRunWBDecel = 18.0f;
    mUnidentified4B4 = 10.0f;
    fSlowestGroundPassSpeed = 12.0f;
    fSlowestVolleyPassSpeed = 12.0f;
    fShotWindupTurnSpeed = 75000.0f;
    fShotWindupTurnFalloff = 4000.0f;
    fShotWindupDecel = 6.0f;
    mUnidentified4CC = 26.0f;
    mUnidentified4D0 = 106.0f;
    mUnidentified4D4 = 27.0f;
    mUnidentified4D8 = 3.0f;
    mUnidentified4DC = g_pTweaks[0];

    fRunSpeedMin.BindWithDefault("Run Speed Min", 6.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunSpeedMax.BindWithDefault("Run Speed Max", 6.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fJogSpeedMin.BindWithDefault("Jog Speed Min", 4.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fJogSpeedMax.BindWithDefault("Jog Speed Max", 4.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunAccelMin.BindWithDefault("Run Accel Min", 6.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunAccelMax.BindWithDefault("Run Accel Max", 6.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunTurnSpeedMin.BindWithDefault("Run Turn Speed Min", 100000.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunTurnSpeedMax.BindWithDefault("Run Turn Speed Max", 100000.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTurboTurnSpeedMin.BindWithDefault("Turbo Turn Speed Min", 65000.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTurboTurnSpeedMax.BindWithDefault("Turbo Turn Speed Max", 65000.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTurboSpeedMin.BindWithDefault("Turbo Speed Min", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTurboSpeedMax.BindWithDefault("Turbo Speed Max", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunWBTurnSpeedMin.BindWithDefault("Run WB Turn Speed Min", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunWBTurnSpeedMax.BindWithDefault("Run WB Turn Speed Max", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunWBSpeedMin.BindWithDefault("Run WB Speed Min", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunWBSpeedMax.BindWithDefault("Run WB Speed Max", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunWBAccelMin.BindWithDefault("Run WB Accel Min", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fRunWBAccelMax.BindWithDefault("Run WB Accel Max", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTurboWBMin.BindWithDefault("Turbo WB Min", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTurboWBMax.BindWithDefault("Turbo WB Max", 7.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestGroundPassSpeedMin.BindWithDefault("Fastest Ground Pass Speed Min", 20.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestGroundPassSpeedMax.BindWithDefault("Fastest Ground Pass Speed Max", 20.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestVolleyPassSpeedMin.BindWithDefault("Fastest Volley Pass Speed Min", 11.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestVolleyPassSpeedMax.BindWithDefault("Fastest Volley Pass Speed Max", 11.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlowestShotSpeedMin.BindWithDefault("Slowest Shot Speed Min", 22.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlowestShotSpeedMax.BindWithDefault("Slowest Shot Speed Max", 22.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOneTimerMaxSpeedMin.BindWithDefault("One Timer Max Speed Min", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fOneTimerMaxSpeedMax.BindWithDefault("One Timer Max Speed Max", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fClearBallMinZSpeed.BindWithDefault("Clear Ball Min Z Speed", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fClearMinSpeed.BindWithDefault("Clear Min Speed", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestShotSpeedMin.BindWithDefault("Fastest Shot Speed Min", 28.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestShotSpeedMax.BindWithDefault("Fastest Shot Speed Max", 32.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestChipShotSpeedMin.BindWithDefault("Fastest Chip Shot Speed Min", 8.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestChipShotSpeedMax.BindWithDefault("Fastest Chip Shot Speed Max", 14.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestClearSpeedMin.BindWithDefault("Fastest Clear Speed Min", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fFastestClearSpeedMax.BindWithDefault("Fastest Clear Speed Max", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotNetOpenWeight.BindWithDefault("Shot Net Open Weight", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotPlayerDistanceWeight.BindWithDefault("Shot Player Distance Weight", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fChipShotGoalieOutWeight.BindWithDefault("Chip Shot Goalie Out Weight", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fChipShotNetOpenWeight.BindWithDefault("Chip Shot Net Open Weight", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotNetOpenAngle.BindWithDefault("Shot Net Open Angle", 45.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShotRatingsWeight.BindWithDefault("Shot Ratings Weight", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSTSYellowDistance.BindWithDefault("STS Yellow Distance", 0.05f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideTimeMin.BindWithDefault("Slide Time Min", 0.35f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideTimeMax.BindWithDefault("Slide Time Max", 0.35f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideSpeedMin.BindWithDefault("Slide Speed Min", 0.35f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideSpeedMax.BindWithDefault("Slide Speed Max", 0.35f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideDecelTimeMin.BindWithDefault("Slide Decel Time Min", 0.35f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideDecelTimeMax.BindWithDefault("Slide Decel Time Max", 0.35f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSlideDecel.BindWithDefault("Slide Decel", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fSuperSlideSpeedBonus.BindWithDefault("Super Slide Speed Bonus", 0.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fHitEffectiveMaxFrame.BindWithDefault("Hit Effective Max Frame", 7.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fHitEffectiveFirstFrame.BindWithDefault("Hit Effective First Frame", 4.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fHitEffectiveLastFrameMin.BindWithDefault("Hit Effective Last Frame Min", 14.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fHitEffectiveLastFrameMax.BindWithDefault("Hit Effective Last Frame Max", 14.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fMushroomEffectTime.BindWithDefault("Mushroom Effect Time", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fMushroomSpeedBoost.BindWithDefault("Mushroom Speed Boost", 15.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fStarEffectTime.BindWithDefault("Star Effect Time", 2.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fStarSpeedBoost.BindWithDefault("Star Speed Boost", 15.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fGreenShellSpeed.BindWithDefault("Shell Speed", 12.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fShellTime.BindWithDefault("Shell Time", 1.5f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fBowserExplodeRadius.BindWithDefault("Bowser Explode Radius", 5.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTerrainMinSpeedAdjust.BindWithDefault("Terrain Min Speed Adjust", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTerrainMaxSpeedAdjust.BindWithDefault("Terrain Max Speed Adjust", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTerrainMinSlipperyAdjust.BindWithDefault("Terrain Min Slippery Adjust", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTerrainMaxSlipperyAdjust.BindWithDefault("Terrain Max Slippery Adjust", 1.0f, mCategory, false, 0.0f, 0.0f, 0.0f);
    fTerrainMaxSlipperyMomentum.BindWithDefault("Terrain Max Slippery Momentum", 0.9f, mCategory, false, 0.0f, 0.0f, 0.0f);
}

PlayerTweaks::PlayerTweaks(const char* name, const char* category)
{
    InitPlayerTweaks(this, name, category, true);
}

PlayerTweaks::~PlayerTweaks()
{
}

void InitPlayerTweaks(PlayerTweaks* tweaks, const char* name,
    const char* category, bool registerTweaks)
{
    tweaks->mUnidentified004.BindWithDefault("mfHeight", 0.5f, category, true, 0.0f, 0.0f, 0.0f);
    tweaks->fWidth.BindWithDefault("mfWidth", 0.5f, category, true, 0.0f, 0.0f, 0.0f);
    tweaks->fMovementTurningRadius.BindWithDefault("mfMovement_TurningRadius", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->fMovementSpeed.BindWithDefault("mfMovement_Speed", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->fMovementAcceleration.BindWithDefault("mfMovement_Acceleration", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->fDefenseSlideTackle.BindWithDefault("mfDefense_SlideTackle", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->mUnidentified064.BindWithDefault("mfDefense_Size", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->fDefenseHittingDistance.BindWithDefault("mfDefense_HittingDistance", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->fOffenseShootingWindupTime.BindWithDefault("mfOffense_ShootingWindupTime", 0.5f, category, true, 0.0f, 4.0f, 0.05f);
    tweaks->fOffenseShootingWindupTotalTime.BindWithDefault("mfOffense_ShootingWindupTotalTime", 0.5f, category, true, 0.0f, 4.0f, 0.05f);
    tweaks->fShooting.BindWithDefault("mfOffense_Shooting", 0.5f, category, true, -4.0f, 4.0f, 0.05f);
    tweaks->fPassing.BindWithDefault("mfOffense_Passing", 0.5f, category, true, -4.0f, 4.0f, 0.05f);

    if (registerTweaks)
    {
        gTweakFileLoader.LoadFileAsync(name, category);
    }
    else
    {
        LoadTweakConfigFile(name, category, true);
    }
}

float GetOffensiveRating(PlayerTweaks* tweaks)
{
    return (tweaks->fPassing.GetValue()
               + tweaks->fShooting.GetValue())
         / 2.0f;
}

extern "C" float fn_8002BE38(PlayerTweaks* tweaks)
{
    float result = tweaks->fDefenseSlideTackle.GetValue()
                 + tweaks->fMovementSpeed.GetValue();
    return (result + tweaks->fDefenseHittingDistance.GetValue()) / 3.0f;
}

float GetPlaymakerRating(PlayerTweaks* tweaks)
{
    return (tweaks->fPassing.GetValue()
               + tweaks->fMovementSpeed.GetValue())
         / 2.0f;
}

extern "C" float fn_8002BE84(const PlayerTweaks* tweaks)
{
    float result = tweaks->fDefenseHittingDistance.GetValue()
                 + tweaks->fDefenseSlideTackle.GetValue();
    return (result + tweaks->fShooting.GetValue()) / 3.0f;
}

float PlayerTweaks::GetSkillRating(unsigned int index)
{
    float result = -9999.9f;
    switch (index)
    {
    case 1:
        result = fMovementSpeed;
        break;
    case 2:
        result = fDefenseSlideTackle;
        break;
    case 3:
        result = fDefenseHittingDistance;
        break;
    case 4:
        result = fShooting;
        break;
    case 5:
        result = fPassing;
        break;
    case 6:
        result = fDefenseSlideTackle.GetValue()
               + fMovementSpeed.GetValue();
        result = (result + fDefenseHittingDistance.GetValue())
               / 3.0f;
        break;
    case 7:
        result = (fPassing.GetValue()
                     + fShooting.GetValue())
               / 2.0f;
        break;
    case 8:
        result = (fPassing.GetValue()
                     + fMovementSpeed.GetValue())
               / 2.0f;
        break;
    case 9:
        result = fDefenseHittingDistance.GetValue()
               + fDefenseSlideTackle.GetValue();
        result = (result + fShooting.GetValue()) / 3.0f;
        break;
    }
    return result;
}

extern "C" float fn_8002BFA8(PlayerTweaks* tweaks, float value)
{
    return value * tweaks->fWidth;
}

float GetRunSpeed(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementSpeed;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fRunSpeedMax;
    float minimum = fielderTweaks->fRunSpeedMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float GetJogTurnSpeed(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementTurningRadius;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    return Interpolate(fielderTweaks->fJogTurnSpeedMin,
        fielderTweaks->fJogTurnSpeedMax,
        playerValue);
}

extern "C" float fn_8002C0AC(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementTurningRadius;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fRunTurnSpeedMax;
    float minimum = fielderTweaks->fRunTurnSpeedMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C180(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementAcceleration;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fRunAccelMax;
    float minimum = fielderTweaks->fRunAccelMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002C254(const PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementSpeed;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fTurboSpeedMax;
    float minimum = fielderTweaks->fTurboSpeedMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float PlayerTweaks::GetRunningSpeed()
{
    float playerValue = fMovementSpeed;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fRunWBSpeedMax;
    float minimum = fielderTweaks->fRunWBSpeedMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float GetRunWBAccel(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementAcceleration;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fRunWBAccelMax;
    float minimum = fielderTweaks->fRunWBAccelMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float GetRunWBTurnSpeed(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementTurningRadius;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fRunWBTurnSpeedMax;
    float minimum = fielderTweaks->fRunWBTurnSpeedMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float GetTurboWBSpeed(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementSpeed;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fTurboWBMax;
    float minimum = fielderTweaks->fTurboWBMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float GetFastestGroundPassSpeed(PlayerTweaks* tweaks)
{
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float scale = 0.25f * terrain + 0.75f;
    return Interpolate(
        scale * gGameTweaks.mFielderTweaks->fFastestGroundPassSpeedMin.GetValue(),
        scale * (float)gGameTweaks.mFielderTweaks->fFastestGroundPassSpeedMax,
        tweaks->fPassing);
}

float GetSlowestGroundPassSpeed(PlayerTweaks*)
{
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    return gGameTweaks.mFielderTweaks->fSlowestGroundPassSpeed
         * (0.25f * terrain + 0.75f);
}

float GetFastestVolleyPassSpeed(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->fFastestVolleyPassSpeedMin,
        gGameTweaks.mFielderTweaks->fFastestVolleyPassSpeedMax,
        tweaks->fPassing);
}

extern "C" float fn_8002C758(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->fFastestShotSpeedMin,
        gGameTweaks.mFielderTweaks->fFastestShotSpeedMax,
        tweaks->fShooting);
}

extern "C" float fn_8002C780(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->fSlowestShotSpeedMin,
        gGameTweaks.mFielderTweaks->fSlowestShotSpeedMax,
        tweaks->fShooting);
}

float GetOneTimerMaxSpeed(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->fOneTimerMaxSpeedMin,
        gGameTweaks.mFielderTweaks->fOneTimerMaxSpeedMax,
        tweaks->fShooting);
}

float GetMushroomEffectTime(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fMushroomEffectTime;
}

float GetShootingWindupTime(PlayerTweaks* tweaks)
{
    return tweaks->fOffenseShootingWindupTime;
}

float GetShootingWindupTotalTime(PlayerTweaks* tweaks)
{
    return tweaks->fOffenseShootingWindupTotalTime;
}

float GetSlideTime(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fDefenseSlideTackle;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fSlideTimeMax;
    float minimum = fielderTweaks->fSlideTimeMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSlipperyAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSlipperyAdjust;
    float terrain = g_pGame->mpTerrain->GetSlideFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float GetSlideDecelTime(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fDefenseSlideTackle;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fSlideDecelTimeMax;
    float minimum = fielderTweaks->fSlideDecelTimeMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSlipperyAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSlipperyAdjust;
    float terrain = g_pGame->mpTerrain->GetSlideFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float GetSlideSpeed(const PlayerTweaks* tweaks)
{
    float result = fn_8002C254(tweaks);
    result *= Interpolate(gGameTweaks.mFielderTweaks->fSlideSpeedMin,
        gGameTweaks.mFielderTweaks->fSlideSpeedMax,
        tweaks->fDefenseSlideTackle);
    if (fn_8002BE84(tweaks) > 0.9f)
    {
        result *= 1.175f;
    }
    return result * Interpolate(gGameTweaks.mFielderTweaks->fTerrainMinSpeedAdjust, gGameTweaks.mFielderTweaks->fTerrainMaxSpeedAdjust, g_pGame->mpTerrain->GetSpeedFactor());
}

float GetMushroomSpeedBoost(PlayerTweaks*)
{
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * (float)gGameTweaks.mFielderTweaks->fMushroomSpeedBoost;
}

float GetStarSpeedBoost(PlayerTweaks*)
{
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * (float)gGameTweaks.mFielderTweaks->fStarSpeedBoost;
}

extern "C" float fn_8002CC44(const PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementSpeed;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fRunSpeedMax;
    float minimum = fielderTweaks->fRunSpeedMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return gGameTweaks.mFielderTweaks->mUnidentified494 * terrainScale
         * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002CD2C(const PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementSpeed;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fRunSpeedMax;
    float minimum = fielderTweaks->fRunSpeedMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return gGameTweaks.mFielderTweaks->mUnidentified49C * terrainScale
         * Interpolate(minimum, maximum, playerValue);
}

extern "C" float fn_8002CE14(PlayerTweaks* tweaks)
{
    float playerValue = tweaks->fMovementSpeed;
    FielderTweaks* fielderTweaks = gGameTweaks.mFielderTweaks;
    float maximum = fielderTweaks->fJogSpeedMax;
    float minimum = fielderTweaks->fJogSpeedMin;
    float terrainMaximum = fielderTweaks->fTerrainMaxSpeedAdjust;
    float terrainMinimum = fielderTweaks->fTerrainMinSpeedAdjust;
    float terrain = g_pGame->mpTerrain->GetSpeedFactor();
    float terrainScale = Interpolate(terrainMinimum, terrainMaximum, terrain);
    return terrainScale * Interpolate(minimum, maximum, playerValue);
}

float GetStrafeAccel(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fStrafeAccel;
}

float GetStrafeTurnSpeed(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fStrafeTurnSpeed;
}

extern "C" float fn_8002CF10(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fRunTurnFalloff;
}

extern "C" float fn_8002CF24(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fRunDecel;
}

float GetStrafeTurnFalloff(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fStrafeTurnFalloff;
}

float GetStrafeDecel(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fStrafeDecel;
}

float GetRunWBTurnFalloff(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fRunWBTurnFalloff;
}

float GetRunWBDecel(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fRunWBDecel;
}

float GetShotWindupTurnSpeed(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fShotWindupTurnSpeed;
}

float GetShotWindupTurnFalloff(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fShotWindupTurnFalloff;
}

float GetShotWindupDecel(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fShotWindupDecel;
}

float GetSlowestVolleyPassSpeed(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fSlowestVolleyPassSpeed;
}

float GetStarEffectTime(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fStarEffectTime;
}

float GetShellSpeed(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fGreenShellSpeed;
}

float GetSuperSlideSpeedBonus(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fSuperSlideSpeedBonus;
}

extern "C" float fn_8002D020(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fHitEffectiveFirstFrame;
}

extern "C" float fn_8002D038(PlayerTweaks*)
{
    return gGameTweaks.mFielderTweaks->fHitEffectiveMaxFrame;
}

extern "C" float fn_8002D050(PlayerTweaks* tweaks)
{
    return Interpolate(gGameTweaks.mFielderTweaks->fHitEffectiveLastFrameMin,
        gGameTweaks.mFielderTweaks->fHitEffectiveLastFrameMax,
        tweaks->fDefenseHittingDistance);
}
