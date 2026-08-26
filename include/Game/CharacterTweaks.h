#ifndef GAME_CHARACTER_TWEAKS_H
#define GAME_CHARACTER_TWEAKS_H

#include "types.h"

class TweakValue_804F4DC8
{
public:
    operator float() const
    {
        return *m_pValue;
    }

private:
    /* 0x00 */ u8 mUnidentified000[0x0C];
    /* 0x0C */ float* m_pValue;
}; // total size: 0x10

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
protected:
    /* 0x00 */ u8 mUnidentified000[0x44];
};

class GoalieTweaks : public PlayerTweaks
{
public:
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

private:
    /* 0x144 */ u8 mUnidentified144[0x10];

public:
    /* 0x154 */ TweakValue_804F4DC8 fGoalieBallTime;

private:
    /* 0x164 */ u8 mUnidentified164[0x30];

public:
    /* 0x194 */ TweakValue_804F4DC8 fSaveDirectionSeekSpeed;
    /* 0x1A4 */ TweakValue_804F4DC8 fSaveDirectionSeekFalloff;

private:
    /* 0x1B4 */ u8 mUnidentified1B4[0x30];

public:
    /* 0x1E4 */ TweakValue_804F4DC8 fLobShotStumbleChance;

private:
    /* 0x1F4 */ u8 mUnidentified1F4[0x10];

public:
    /* 0x204 */ TweakValue_804F4DC8 fSaveCatchTolerance;

private:
    /* 0x214 */ u8 mUnidentified214[0x124];

public:
    /* 0x338 */ TweakValue_804F4DC8 fRunningDirectionSeekSpeed;
    /* 0x348 */ TweakValue_804F4DC8 fRunningDirectionSeekFalloff;
};

#endif // GAME_CHARACTER_TWEAKS_H
