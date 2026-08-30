#ifndef GAME_CHARACTER_TWEAKS_H
#define GAME_CHARACTER_TWEAKS_H

#include "Game/TweakValue.h"
#include "Game/TweaksBase.h"

class unk_80029F58
{
private:
    /* 0x000 */ u8 mUnidentified000[0x3F4];

public:
    /* 0x3F4 */ TweakValue_804F4DC8 fGreenShellSpeed;
    /* 0x404 */ TweakValue_804F4DC8 mUnidentified404;
};

class PlayerTweaks
{
public:
    virtual ~PlayerTweaks();

protected:
    /* 0x04 */ u8 mUnidentified004[0x3C];

public:
    /* 0x40 */ float* mUnidentified040;
};

class FielderTweaks : public PlayerTweaks
{
private:
    /* 0x044 */ u8 mUnidentified044[0x2C];

public:
    /* 0x070 */ float* mUnidentified070;

private:
    /* 0x074 */ u8 mUnidentified074[0x30];

public:
    /* 0x0A4 */ TweakValue_804F4DC8 fShooting;
};

class GoalieTweaks : public TweaksBase
{
public:
    GoalieTweaks(const char* name, const char* category);
    virtual ~GoalieTweaks();
    virtual void Init();

    /* 0x044 */ TweakValue_804F4DC8 fJoggingSpeed;
    /* 0x054 */ TweakValue_804F4DC8 fRunningSpeed;
    /* 0x064 */ TweakValue_804F4DC8 fThrowingDirectionSeekSpeed;
    /* 0x074 */ TweakValue_804F4DC8 fThrowingDirectionSeekFalloff;
    /* 0x084 */ TweakValue_804F4DC8 fKickDistanceMin;
    /* 0x094 */ TweakValue_804F4DC8 fOverhandThrowDistanceMin;
    /* 0x0A4 */ TweakValue_804F4DC8 fKickVelocityMin;
    /* 0x0B4 */ TweakValue_804F4DC8 fKickVelocityMax;
    /* 0x0C4 */ TweakValue_804F4DC8 fKickAngleMin;
    /* 0x0D4 */ TweakValue_804F4DC8 fKickAngleMax;
    /* 0x0E4 */ TweakValue_804F4DC8 fFatigueRecoverRate;
    /* 0x0F4 */ TweakValue_804F4DC8 fFatigueCatchThreshold;
    /* 0x104 */ TweakValue_804F4DC8 fCatchSaveMaxSpeed;

public:
    /* 0x114 */ TweakValue_804F4DC8 fGetupEnergyHigh;
    /* 0x124 */ TweakValue_804F4DC8 fGetupEnergyLow;
    /* 0x134 */ TweakValue_804F4DC8 fGetupSpeedLow;

    /* 0x144 */ TweakValue_804F4DC8 fStrafeSpeedLow;
    /* 0x154 */ TweakValue_804F4DC8 fGoalieBallTime;
    /* 0x164 */ TweakValue_804F4DC8 fGoalieStunTimeMin;
    /* 0x174 */ TweakValue_804F4DC8 fGoalieStunTimeMax;
    /* 0x184 */ TweakValue_804F4DC8 fLooseBallShotDistance;
    /* 0x194 */ TweakValue_804F4DC8 fSaveDirectionSeekSpeed;
    /* 0x1A4 */ TweakValue_804F4DC8 fSaveDirectionSeekFalloff;
    /* 0x1B4 */ TweakValue_804F4DC8 fSaveBackRunTimeScale;
    /* 0x1C4 */ TweakValue_804F4DC8 fSaveIgnoreMargin;
    /* 0x1D4 */ TweakValue_804F4DC8 fSaveMissDelay;
    /* 0x1E4 */ TweakValue_804F4DC8 fLobShotStumbleChance;
    /* 0x1F4 */ TweakValue_804F4DC8 fInterceptSaveTolerance;
    /* 0x204 */ TweakValue_804F4DC8 fSaveCatchTolerance;

    /* 0x214 */ TweakValue_804F4DC8 fShotFatigueDefault;
    /* 0x224 */ TweakValue_804F4DC8 fShotFatigueStandCatch;
    /* 0x234 */ TweakValue_804F4DC8 fShotFatigueDiveCatch;
    /* 0x244 */ TweakValue_804F4DC8 fShotFatigueStandDeflect;
    /* 0x254 */ TweakValue_804F4DC8 fShotFatigueDiveDeflect;
    /* 0x264 */ TweakValue_804F4DC8 fShotFatigueStandPunch;
    /* 0x274 */ TweakValue_804F4DC8 fShotFatigueLegSave;
    /* 0x284 */ TweakValue_804F4DC8 fShotFatigueSTSSave;
    /* 0x294 */ TweakValue_804F4DC8 fShotFatigueSTSStun;
    /* 0x2A4 */ float fShotFatigueMax;
    /* 0x2A8 */ TweakValue_804F4DC8 mUnidentified2A8;
    /* 0x2B8 */ TweakValue_804F4DC8 mUnidentified2B8;
    /* 0x2C8 */ TweakValue_804F4DC8 fPounceRange;
    /* 0x2D8 */ TweakValue_804F4DC8 fPhysCapsuleRadius;
    /* 0x2E8 */ TweakValue_804F4DC8 fPhysCapsuleHeight;
    /* 0x2F8 */ TweakValue_804F4DC8 fPassGroundSpeedMax;
    /* 0x308 */ TweakValue_804F4DC8 fPassGroundSpeedMin;
    /* 0x318 */ TweakValue_804F4DC8 fPassVolleySpeedMax;
    /* 0x328 */ TweakValue_804F4DC8 fPassVolleySpeedMin;
    /* 0x338 */ TweakValue_804F4DC8 fRunningDirectionSeekSpeed;
    /* 0x348 */ TweakValue_804F4DC8 fRunningDirectionSeekFalloff;

private:
    /* 0x358 */ const char* mUnidentified358;
};

#endif // GAME_CHARACTER_TWEAKS_H
