#include "Game/CharacterTweaks.h"

#include "Game/TweakFileLoader.h"
#include "Game/UnidentifiedStaticStorage.h"

GoalieTweaks::GoalieTweaks(const char* name, const char* category)
    : TweaksBase(name)
    , mUnidentified358(category)
{
    Init();
    gTweakFileLoader.LoadFileAsync(mszFileName, mUnidentified358);
}

GoalieTweaks::~GoalieTweaks()
{
}

inline float max_float(float a, float b)
{
    if (a >= b)
    {
        return a;
    }
    return b;
}

void GoalieTweaks::fn_800277A0()
{
    // Find the maximum fatigue value
    fShotFatigueMax = max_float(fShotFatigueDefault, fShotFatigueStandCatch);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueDiveCatch);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueStandDeflect);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueDiveDeflect);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueStandPunch);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueLegSave);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueSTSSave);
    fShotFatigueMax = max_float(fShotFatigueMax, fShotFatigueSTSStun);
}

void GoalieTweaks::Init()
{
    fJoggingSpeed.BindWithDefault("Jogging Speed", 5.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fJoggingSpeed.BindWithDefault("Jogging Speed", 5.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fRunningSpeed.BindWithDefault("Running Speed", 7.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fThrowingDirectionSeekSpeed.BindWithDefault("Throwing Turn Speed", 50000.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fThrowingDirectionSeekFalloff.BindWithDefault("Throwing Turn falloff", 4000.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fKickDistanceMin.BindWithDefault("Minimum Kick Distance", 13.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fOverhandThrowDistanceMin.BindWithDefault("Minimum Overhand Throw Distance", 7.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fKickVelocityMin.BindWithDefault("Minimum Kick Velocity", 12.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fKickVelocityMax.BindWithDefault("Maximum Kick Velocity", 16.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fKickAngleMin.BindWithDefault("Minimum Kick Angle", 50.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fKickAngleMax.BindWithDefault("Maximum Kick Angle", 60.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fFatigueRecoverRate.BindWithDefault("Fatigue Recover Rate", 2.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fFatigueCatchThreshold.BindWithDefault("Fatigue Catch Threshold", 60.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fCatchSaveMaxSpeed.BindWithDefault("Catch Save Max Speed", 0.5f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fGetupEnergyHigh.BindWithDefault("High Getup Energy", 80.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fGetupEnergyLow.BindWithDefault("Low Getup Energy", 20.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fGetupSpeedLow.BindWithDefault("Low Getup Speed", 0.5f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fStrafeSpeedLow.BindWithDefault("Low Strafe Speed", 0.8f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fGoalieBallTime.BindWithDefault("Goalie Ball Time", 5.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fGoalieStunTimeMin.BindWithDefault("Min STS Stun Time", 1.5f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fGoalieStunTimeMax.BindWithDefault("Max STS Stun Time", 3.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fLooseBallShotDistance.BindWithDefault("Loose Ball Shot Distance", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fSaveDirectionSeekSpeed.BindWithDefault("Save Turn Speed", 100000.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fSaveDirectionSeekFalloff.BindWithDefault("Save Turn Falloff", 4000.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fSaveBackRunTimeScale.BindWithDefault("Save Back Run Scale", 1.5f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fSaveIgnoreMargin.BindWithDefault("Save Ignore Margin", 1.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fSaveMissDelay.BindWithDefault("Miss Save Delay", 0.1f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fLobShotStumbleChance.BindWithDefault("Lob Shot Stumble Chance", 0.3f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fInterceptSaveTolerance.BindWithDefault("Intercept Save Tolerance", 0.5f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fSaveCatchTolerance.BindWithDefault("Catch Save Tolerance", 0.7f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueDefault.BindWithDefault("Shot Fatigue Default", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueStandCatch.BindWithDefault("Shot Fatigue Stand Catch", 5.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueDiveCatch.BindWithDefault("Shot Fatigue Dive Catch", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueStandDeflect.BindWithDefault("Shot Fatigue Stand Deflection", 5.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueDiveDeflect.BindWithDefault("Shot Fatigue Dive Deflection", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueStandPunch.BindWithDefault("Shot Fatigue Stand Punch", 7.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueLegSave.BindWithDefault("Shot Fatigue Leg Save", 6.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueSTSSave.BindWithDefault("Shot Fatigue STS Save", 15.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fShotFatigueSTSStun.BindWithDefault("Shot Fatigue STS Stun", 20.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);

    fShotFatigueMax = 0.0f;

    mUnidentified2A8.BindWithDefault("Min On Fire Time", 5.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    mUnidentified2B8.BindWithDefault("Max On Fire Time", 8.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fPounceRange.BindWithDefault("Pounce Range", 1.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fPhysCapsuleHeight.BindWithDefault("Capsule Height", 1.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fPhysCapsuleRadius.BindWithDefault("Capsule Width", 1.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fPassGroundSpeedMax.BindWithDefault("Pass Ground Speed Max", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fPassGroundSpeedMin.BindWithDefault("Pass Ground Speed Min", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fPassVolleySpeedMax.BindWithDefault("Pass Volley Speed Max", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fPassVolleySpeedMin.BindWithDefault("Pass Volley Speed Min", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fRunningDirectionSeekSpeed.BindWithDefault("Running Turn Speed", 10.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
    fRunningDirectionSeekFalloff.BindWithDefault("Running Direction Seek Falloff", 4000.0f, mUnidentified358, false, 0.0f, 0.0f, 0.0f);
}
