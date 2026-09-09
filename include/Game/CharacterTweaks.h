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

    /* 0x044 */ TweakFloatBinding mUnidentified044;
    /* 0x054 */ TweakFloatBinding mUnidentified054;
    /* 0x064 */ TweakFloatBinding mUnidentified064;
    /* 0x074 */ TweakFloatBinding mUnidentified074;
    /* 0x084 */ TweakFloatBinding mUnidentified084;
    /* 0x094 */ TweakFloatBinding mUnidentified094;
    /* 0x0A4 */ TweakFloatBinding mUnidentified0A4;
    /* 0x0B4 */ TweakFloatBinding mUnidentified0B4;
    /* 0x0C4 */ TweakFloatBinding mUnidentified0C4;
    /* 0x0D4 */ TweakFloatBinding mUnidentified0D4;
    /* 0x0E4 */ TweakFloatBinding mUnidentified0E4;
    /* 0x0F4 */ TweakFloatBinding mUnidentified0F4;
    /* 0x104 */ TweakFloatBinding mUnidentified104;
    /* 0x114 */ TweakFloatBinding mUnidentified114;
    /* 0x124 */ TweakFloatBinding mUnidentified124;
    /* 0x134 */ TweakFloatBinding mUnidentified134;
    /* 0x144 */ TweakFloatBinding mUnidentified144;
    /* 0x154 */ TweakFloatBinding mUnidentified154;
    /* 0x164 */ TweakFloatBinding mUnidentified164;
    /* 0x174 */ TweakFloatBinding mUnidentified174;
    /* 0x184 */ TweakFloatBinding mUnidentified184;
    /* 0x194 */ TweakFloatBinding mUnidentified194;
    /* 0x1A4 */ TweakFloatBinding mUnidentified1A4;
    /* 0x1B4 */ TweakFloatBinding mUnidentified1B4;
    /* 0x1C4 */ TweakFloatBinding mUnidentified1C4;
    /* 0x1D4 */ TweakFloatBinding mUnidentified1D4;
    /* 0x1E4 */ TweakFloatBinding mUnidentified1E4;
    /* 0x1F4 */ TweakFloatBinding mUnidentified1F4;
    /* 0x204 */ TweakFloatBinding mUnidentified204;
    /* 0x214 */ TweakFloatBinding mUnidentified214;
    /* 0x224 */ TweakFloatBinding mUnidentified224;
    /* 0x234 */ TweakFloatBinding mUnidentified234;
    /* 0x244 */ TweakFloatBinding mUnidentified244;
    /* 0x254 */ TweakFloatBinding mUnidentified254;
    /* 0x264 */ TweakFloatBinding mUnidentified264;
    /* 0x274 */ TweakFloatBinding mUnidentified274;
    /* 0x284 */ TweakFloatBinding mUnidentified284;
    /* 0x294 */ TweakFloatBinding mUnidentified294;
    /* 0x2A4 */ TweakFloatBinding mUnidentified2A4;
    /* 0x2B4 */ TweakFloatBinding mUnidentified2B4;
    /* 0x2C4 */ TweakFloatBinding mUnidentified2C4;
    /* 0x2D4 */ TweakFloatBinding mUnidentified2D4;
    /* 0x2E4 */ TweakFloatBinding mUnidentified2E4;
    /* 0x2F4 */ TweakFloatBinding mUnidentified2F4;
    /* 0x304 */ TweakFloatBinding mUnidentified304;
    /* 0x314 */ TweakFloatBinding mUnidentified314;
    /* 0x324 */ TweakFloatBinding mUnidentified324;
    /* 0x334 */ TweakFloatBinding mUnidentified334;
    /* 0x344 */ TweakFloatBinding mUnidentified344;
    /* 0x354 */ TweakFloatBinding mUnidentified354;
    /* 0x364 */ TweakFloatBinding mUnidentified364;
    /* 0x374 */ TweakFloatBinding mUnidentified374;
    /* 0x384 */ TweakFloatBinding mUnidentified384;
    /* 0x394 */ TweakFloatBinding mUnidentified394;
    /* 0x3A4 */ TweakFloatBinding mUnidentified3A4;
    /* 0x3B4 */ TweakFloatBinding mUnidentified3B4;
    /* 0x3C4 */ TweakFloatBinding mUnidentified3C4;
    /* 0x3D4 */ TweakFloatBinding mUnidentified3D4;
    /* 0x3E4 */ TweakFloatBinding mUnidentified3E4;
    /* 0x3F4 */ TweakFloatBinding fGreenShellSpeed;
    /* 0x404 */ TweakFloatBinding mUnidentified404;
    /* 0x414 */ TweakFloatBinding mUnidentified414;
    /* 0x424 */ TweakFloatBinding mUnidentified424;
    /* 0x434 */ TweakFloatBinding mUnidentified434;
    /* 0x444 */ TweakFloatBinding mUnidentified444;
    /* 0x454 */ TweakFloatBinding mUnidentified454;
    /* 0x464 */ TweakFloatBinding mUnidentified464;
    /* 0x474 */ float mUnidentified474;
    /* 0x478 */ float mUnidentified478;
    /* 0x47C */ float mUnidentified47C;
    /* 0x480 */ float mUnidentified480;
    /* 0x484 */ float mUnidentified484;
    /* 0x488 */ float mUnidentified488;
    /* 0x48C */ float mUnidentified48C;
    /* 0x490 */ float mUnidentified490;
    /* 0x494 */ float mUnidentified494;
    /* 0x498 */ float mUnidentified498;
    /* 0x49C */ float mUnidentified49C;
    /* 0x4A0 */ float mUnidentified4A0;
    /* 0x4A4 */ float mUnidentified4A4;
    /* 0x4A8 */ float mUnidentified4A8;
    /* 0x4AC */ float mUnidentified4AC;
    /* 0x4B0 */ float mUnidentified4B0;
    /* 0x4B4 */ float mUnidentified4B4;
    /* 0x4B8 */ float mUnidentified4B8;
    /* 0x4BC */ float mUnidentified4BC;
    /* 0x4C0 */ float mUnidentified4C0;
    /* 0x4C4 */ float mUnidentified4C4;
    /* 0x4C8 */ float mUnidentified4C8;
    /* 0x4CC */ float mUnidentified4CC;
    /* 0x4D0 */ float mUnidentified4D0;
    /* 0x4D4 */ float mUnidentified4D4;
    /* 0x4D8 */ float mUnidentified4D8;
    /* 0x4DC */ float mUnidentified4DC;

private:
    /* 0x4E0 */ const char* mUnidentified4E0;
}; // total size: 0x4E4

class PlayerTweaks
{
public:
    float GetSkillRating(unsigned int index);

    PlayerTweaks(const char* name, const char* category);
    virtual ~PlayerTweaks();

    /* 0x004 */ TweakFloatBinding mUnidentified004;
    /* 0x014 */ TweakFloatBinding mUnidentified014;
    /* 0x024 */ TweakFloatBinding mUnidentified024;
    /* 0x034 */ TweakFloatBinding mUnidentified034;
    /* 0x044 */ TweakFloatBinding mUnidentified044;
    /* 0x054 */ TweakFloatBinding mUnidentified054;
    /* 0x064 */ TweakFloatBinding mUnidentified064;
    /* 0x074 */ TweakFloatBinding mUnidentified074;
    /* 0x084 */ TweakFloatBinding mUnidentified084;
    /* 0x094 */ TweakFloatBinding mUnidentified094;
    /* 0x0A4 */ TweakFloatBinding fShooting;
    /* 0x0B4 */ TweakFloatBinding fPassing;
}; // total size: 0xC4

class GoalieTweaks : public TweaksBase
{
public:
    GoalieTweaks(const char* name, const char* category);
    virtual ~GoalieTweaks();
    void fn_800277A0();
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
    /* 0x358 */ const char* mUnidentified358;
};

#endif // GAME_CHARACTER_TWEAKS_H
