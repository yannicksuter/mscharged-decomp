#ifndef GAME_CHARACTER_TWEAKS_H
#define GAME_CHARACTER_TWEAKS_H

#include "Game/TweakValue.h"
#include "Game/TweaksBase.h"

class FielderTweaks : public TweaksBase
{
public:
    FielderTweaks(const char* name, const char* category);
    virtual ~FielderTweaks();
    virtual void Init();

    /* 0x044 */ TweakFloatBinding fRunSpeedMin;
    /* 0x054 */ TweakFloatBinding fRunSpeedMax;
    /* 0x064 */ TweakFloatBinding fJogSpeedMin;
    /* 0x074 */ TweakFloatBinding fJogSpeedMax;
    /* 0x084 */ TweakFloatBinding fRunAccelMin;
    /* 0x094 */ TweakFloatBinding fRunAccelMax;
    /* 0x0A4 */ TweakFloatBinding fRunTurnSpeedMin;
    /* 0x0B4 */ TweakFloatBinding fRunTurnSpeedMax;
    /* 0x0C4 */ TweakFloatBinding fTurboTurnSpeedMin;
    /* 0x0D4 */ TweakFloatBinding fTurboTurnSpeedMax;
    /* 0x0E4 */ TweakFloatBinding fTurboSpeedMin;
    /* 0x0F4 */ TweakFloatBinding fTurboSpeedMax;
    /* 0x104 */ TweakFloatBinding fRunWBTurnSpeedMin;
    /* 0x114 */ TweakFloatBinding fRunWBTurnSpeedMax;
    /* 0x124 */ TweakFloatBinding fRunWBSpeedMin;
    /* 0x134 */ TweakFloatBinding fRunWBSpeedMax;
    /* 0x144 */ TweakFloatBinding fRunWBAccelMin;
    /* 0x154 */ TweakFloatBinding fRunWBAccelMax;
    /* 0x164 */ TweakFloatBinding fTurboWBMin;
    /* 0x174 */ TweakFloatBinding fTurboWBMax;
    /* 0x184 */ TweakFloatBinding fFastestGroundPassSpeedMin;
    /* 0x194 */ TweakFloatBinding fFastestGroundPassSpeedMax;
    /* 0x1A4 */ TweakFloatBinding fFastestVolleyPassSpeedMin;
    /* 0x1B4 */ TweakFloatBinding fFastestVolleyPassSpeedMax;
    /* 0x1C4 */ TweakFloatBinding fSlowestShotSpeedMin;
    /* 0x1D4 */ TweakFloatBinding fSlowestShotSpeedMax;
    /* 0x1E4 */ TweakFloatBinding fOneTimerMaxSpeedMin;
    /* 0x1F4 */ TweakFloatBinding fOneTimerMaxSpeedMax;
    /* 0x204 */ TweakFloatBinding fClearBallMinZSpeed;
    /* 0x214 */ TweakFloatBinding fClearMinSpeed;
    /* 0x224 */ TweakFloatBinding fFastestShotSpeedMin;
    /* 0x234 */ TweakFloatBinding fFastestShotSpeedMax;
    /* 0x244 */ TweakFloatBinding fFastestChipShotSpeedMin;
    /* 0x254 */ TweakFloatBinding fFastestChipShotSpeedMax;
    /* 0x264 */ TweakFloatBinding fFastestClearSpeedMin;
    /* 0x274 */ TweakFloatBinding fFastestClearSpeedMax;
    /* 0x284 */ TweakFloatBinding fShotNetOpenWeight;
    /* 0x294 */ TweakFloatBinding fShotPlayerDistanceWeight;
    /* 0x2A4 */ TweakFloatBinding fChipShotGoalieOutWeight;
    /* 0x2B4 */ TweakFloatBinding fChipShotNetOpenWeight;
    /* 0x2C4 */ TweakFloatBinding fShotNetOpenAngle;
    /* 0x2D4 */ TweakFloatBinding fShotRatingsWeight;
    /* 0x2E4 */ TweakFloatBinding fSTSYellowDistance;
    /* 0x2F4 */ TweakFloatBinding fSlideTimeMin;
    /* 0x304 */ TweakFloatBinding fSlideTimeMax;
    /* 0x314 */ TweakFloatBinding fSlideSpeedMin;
    /* 0x324 */ TweakFloatBinding fSlideSpeedMax;
    /* 0x334 */ TweakFloatBinding fSlideDecelTimeMin;
    /* 0x344 */ TweakFloatBinding fSlideDecelTimeMax;
    /* 0x354 */ TweakFloatBinding fSlideDecel;
    /* 0x364 */ TweakFloatBinding fSuperSlideSpeedBonus;
    /* 0x374 */ TweakFloatBinding fHitEffectiveMaxFrame;
    /* 0x384 */ TweakFloatBinding fHitEffectiveFirstFrame;
    /* 0x394 */ TweakFloatBinding fHitEffectiveLastFrameMin;
    /* 0x3A4 */ TweakFloatBinding fHitEffectiveLastFrameMax;
    /* 0x3B4 */ TweakFloatBinding fMushroomEffectTime;
    /* 0x3C4 */ TweakFloatBinding fMushroomSpeedBoost;
    /* 0x3D4 */ TweakFloatBinding fStarEffectTime;
    /* 0x3E4 */ TweakFloatBinding fStarSpeedBoost;
    /* 0x3F4 */ TweakFloatBinding fGreenShellSpeed;
    /* 0x404 */ TweakFloatBinding fShellTime;
    /* 0x414 */ TweakFloatBinding fBowserExplodeRadius;
    /* 0x424 */ TweakFloatBinding fTerrainMinSpeedAdjust;
    /* 0x434 */ TweakFloatBinding fTerrainMaxSpeedAdjust;
    /* 0x444 */ TweakFloatBinding fTerrainMinSlipperyAdjust;
    /* 0x454 */ TweakFloatBinding fTerrainMaxSlipperyAdjust;
    /* 0x464 */ TweakFloatBinding fTerrainMaxSlipperyMomentum;
    /* 0x474 */ float fRunTurnFalloff;
    /* 0x478 */ float fRunDecel;
    /* 0x47C */ float mUnidentified47C;
    /* 0x480 */ float fStrafeTurnFalloff;
    /* 0x484 */ float fStrafeDecel;
    /* 0x488 */ float fStrafeAccel;
    /* 0x48C */ float fJogTurnSpeedMin;
    /* 0x490 */ float fJogTurnSpeedMax;
    /* 0x494 */ float mUnidentified494;
    /* 0x498 */ float fStrafeTurnSpeed;
    /* 0x49C */ float mUnidentified49C;
    /* 0x4A0 */ float mUnidentified4A0;
    /* 0x4A4 */ float mUnidentified4A4;
    /* 0x4A8 */ float mUnidentified4A8;
    /* 0x4AC */ float fRunWBTurnFalloff;
    /* 0x4B0 */ float fRunWBDecel;
    /* 0x4B4 */ float mUnidentified4B4;
    /* 0x4B8 */ float fSlowestGroundPassSpeed;
    /* 0x4BC */ float fSlowestVolleyPassSpeed;
    /* 0x4C0 */ float fShotWindupTurnSpeed;
    /* 0x4C4 */ float fShotWindupTurnFalloff;
    /* 0x4C8 */ float fShotWindupDecel;
    /* 0x4CC */ float mUnidentified4CC;
    /* 0x4D0 */ float mUnidentified4D0;
    /* 0x4D4 */ float mUnidentified4D4;
    /* 0x4D8 */ float mUnidentified4D8;
    /* 0x4DC */ float mUnidentified4DC;

private:
    /* 0x4E0 */ const char* mCategory;
}; // total size: 0x4E4

class PlayerTweaks
{
public:
    float GetSkillRating(unsigned int index);
    float GetRunningSpeed();

    PlayerTweaks(const char* name, const char* category);
    virtual ~PlayerTweaks();

    /* 0x004 */ TweakFloatBinding mUnidentified004;
    /* 0x014 */ TweakFloatBinding fWidth;
    /* 0x024 */ TweakFloatBinding fMovementTurningRadius;
    /* 0x034 */ TweakFloatBinding fMovementSpeed;
    /* 0x044 */ TweakFloatBinding fMovementAcceleration;
    /* 0x054 */ TweakFloatBinding fDefenseSlideTackle;
    /* 0x064 */ TweakFloatBinding mUnidentified064;
    /* 0x074 */ TweakFloatBinding fDefenseHittingDistance;
    /* 0x084 */ TweakFloatBinding fOffenseShootingWindupTime;
    /* 0x094 */ TweakFloatBinding fOffenseShootingWindupTotalTime;
    /* 0x0A4 */ TweakFloatBinding fShooting;
    /* 0x0B4 */ TweakFloatBinding fPassing;
}; // total size: 0xC4

class GoalieTweaks : public TweaksBase
{
public:
    GoalieTweaks(const char* name, const char* category);
    virtual ~GoalieTweaks();
    void CalculateShotFatigueMax();
    virtual void Init();

    /* 0x044 */ TweakFloatBinding fJoggingSpeed;
    /* 0x054 */ TweakFloatBinding fRunningSpeed;
    /* 0x064 */ TweakFloatBinding fThrowingDirectionSeekSpeed;
    /* 0x074 */ TweakFloatBinding fThrowingDirectionSeekFalloff;
    /* 0x084 */ TweakFloatBinding fKickDistanceMin;
    /* 0x094 */ TweakFloatBinding fOverhandThrowDistanceMin;
    /* 0x0A4 */ TweakFloatBinding fKickVelocityMin;
    /* 0x0B4 */ TweakFloatBinding fKickVelocityMax;
    /* 0x0C4 */ TweakFloatBinding fKickAngleMin;
    /* 0x0D4 */ TweakFloatBinding fKickAngleMax;
    /* 0x0E4 */ TweakFloatBinding fFatigueRecoverRate;
    /* 0x0F4 */ TweakFloatBinding fFatigueCatchThreshold;
    /* 0x104 */ TweakFloatBinding fCatchSaveMaxSpeed;

public:
    /* 0x114 */ TweakFloatBinding fGetupEnergyHigh;
    /* 0x124 */ TweakFloatBinding fGetupEnergyLow;
    /* 0x134 */ TweakFloatBinding fGetupSpeedLow;

    /* 0x144 */ TweakFloatBinding fStrafeSpeedLow;
    /* 0x154 */ TweakFloatBinding fGoalieBallTime;
    /* 0x164 */ TweakFloatBinding fGoalieStunTimeMin;
    /* 0x174 */ TweakFloatBinding fGoalieStunTimeMax;
    /* 0x184 */ TweakFloatBinding fLooseBallShotDistance;
    /* 0x194 */ TweakFloatBinding fSaveDirectionSeekSpeed;
    /* 0x1A4 */ TweakFloatBinding fSaveDirectionSeekFalloff;
    /* 0x1B4 */ TweakFloatBinding fSaveBackRunTimeScale;
    /* 0x1C4 */ TweakFloatBinding fSaveIgnoreMargin;
    /* 0x1D4 */ TweakFloatBinding fSaveMissDelay;
    /* 0x1E4 */ TweakFloatBinding fLobShotStumbleChance;
    /* 0x1F4 */ TweakFloatBinding fInterceptSaveTolerance;
    /* 0x204 */ TweakFloatBinding fSaveCatchTolerance;

    /* 0x214 */ TweakFloatBinding fShotFatigueDefault;
    /* 0x224 */ TweakFloatBinding fShotFatigueStandCatch;
    /* 0x234 */ TweakFloatBinding fShotFatigueDiveCatch;
    /* 0x244 */ TweakFloatBinding fShotFatigueStandDeflect;
    /* 0x254 */ TweakFloatBinding fShotFatigueDiveDeflect;
    /* 0x264 */ TweakFloatBinding fShotFatigueStandPunch;
    /* 0x274 */ TweakFloatBinding fShotFatigueLegSave;
    /* 0x284 */ TweakFloatBinding fShotFatigueSTSSave;
    /* 0x294 */ TweakFloatBinding fShotFatigueSTSStun;
    /* 0x2A4 */ float fShotFatigueMax;
    /* 0x2A8 */ TweakFloatBinding mUnidentified2A8;
    /* 0x2B8 */ TweakFloatBinding mUnidentified2B8;
    /* 0x2C8 */ TweakFloatBinding fPounceRange;
    /* 0x2D8 */ TweakFloatBinding fPhysCapsuleRadius;
    /* 0x2E8 */ TweakFloatBinding fPhysCapsuleHeight;
    /* 0x2F8 */ TweakFloatBinding fPassGroundSpeedMax;
    /* 0x308 */ TweakFloatBinding fPassGroundSpeedMin;
    /* 0x318 */ TweakFloatBinding fPassVolleySpeedMax;
    /* 0x328 */ TweakFloatBinding fPassVolleySpeedMin;
    /* 0x338 */ TweakFloatBinding fRunningDirectionSeekSpeed;
    /* 0x348 */ TweakFloatBinding fRunningDirectionSeekFalloff;

private:
    /* 0x358 */ const char* mCategory;
};


// Shared functions and data from Game/CharacterTweaks.cpp.
extern "C" float fn_8002C758(PlayerTweaks* pTweaks);
extern "C" float fn_8002C780(PlayerTweaks* pTweaks);
extern "C" float fn_8002CC44(const PlayerTweaks* pTweaks);
extern "C" float fn_8002CD2C(const PlayerTweaks* pTweaks);
extern "C" float fn_8002D020(PlayerTweaks* pTweaks);
extern "C" float fn_8002D038(PlayerTweaks* pTweaks);
extern "C" float fn_8002D050(PlayerTweaks* pTweaks);


extern "C" float fn_8002BE38(PlayerTweaks* tweaks);
extern "C" float fn_8002BE84(const PlayerTweaks* tweaks);
extern "C" float fn_8002BFA8(PlayerTweaks* tweaks, float value);
extern "C" float fn_8002C0AC(PlayerTweaks* tweaks);
extern "C" float fn_8002C180(PlayerTweaks* tweaks);
extern "C" float fn_8002C254(const PlayerTweaks* tweaks);
extern "C" float fn_8002CF10(PlayerTweaks*);
extern "C" float fn_8002CF24(PlayerTweaks*);

// C++ accessors from Game/CharacterTweaks.cpp.
void InitPlayerTweaks(PlayerTweaks*, const char*, const char*, bool);
float GetOffensiveRating(PlayerTweaks*);
float GetPlaymakerRating(PlayerTweaks*);
float GetRunSpeed(PlayerTweaks*);
float GetJogTurnSpeed(PlayerTweaks*);
float GetRunWBAccel(PlayerTweaks*);
float GetRunWBTurnSpeed(PlayerTweaks*);
float GetTurboWBSpeed(PlayerTweaks*);
float GetFastestGroundPassSpeed(PlayerTweaks*);
float GetSlowestGroundPassSpeed(PlayerTweaks*);
float GetFastestVolleyPassSpeed(PlayerTweaks*);
float GetOneTimerMaxSpeed(PlayerTweaks*);
float GetMushroomEffectTime(PlayerTweaks*);
float GetShootingWindupTime(PlayerTweaks*);
float GetShootingWindupTotalTime(PlayerTweaks*);
float GetSlideTime(PlayerTweaks*);
float GetSlideDecelTime(PlayerTweaks*);
float GetSlideSpeed(const PlayerTweaks*);
float GetMushroomSpeedBoost(PlayerTweaks*);
float GetStarSpeedBoost(PlayerTweaks*);
float GetStrafeAccel(PlayerTweaks*);
float GetStrafeTurnSpeed(PlayerTweaks*);
float GetStrafeTurnFalloff(PlayerTweaks*);
float GetStrafeDecel(PlayerTweaks*);
float GetRunWBTurnFalloff(PlayerTweaks*);
float GetRunWBDecel(PlayerTweaks*);
float GetShotWindupTurnSpeed(PlayerTweaks*);
float GetShotWindupTurnFalloff(PlayerTweaks*);
float GetShotWindupDecel(PlayerTweaks*);
float GetSlowestVolleyPassSpeed(PlayerTweaks*);
float GetStarEffectTime(PlayerTweaks*);
float GetShellSpeed(PlayerTweaks*);
float GetSuperSlideSpeedBonus(PlayerTweaks*);

#endif // GAME_CHARACTER_TWEAKS_H
