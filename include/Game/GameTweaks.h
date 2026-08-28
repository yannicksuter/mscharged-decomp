#ifndef GAME_GAME_TWEAKS_H
#define GAME_GAME_TWEAKS_H

#include "Game/CharacterTweaks.h"

class TweakValueInt_804F4DC8
{
public:
    operator int() const
    {
        return *m_pValue;
    }

private:
    /* 0x00 */ u8 mUnidentified000[0x0C];
    /* 0x0C */ int* m_pValue;
}; // total size: 0x10

class UnidentifiedFormationTweakValue
{
public:
    operator float() const
    {
        return *m_pValue;
    }

private:
    /* 0x00 */ float* m_pValue;
    /* 0x04 */ u8 mUnidentified004[0x0C];
}; // total size: 0x10

struct UnidentifiedFormationTweakVector2
{
    UnidentifiedFormationTweakValue x;
    UnidentifiedFormationTweakValue y;
}; // total size: 0x20

class GameTweaks
{
private:
    /* 0x000 */ u8 mUnidentified000[0x70];

public:
    /* 0x070 */ UnidentifiedFormationTweakVector2 vGetInPositionKeyFielderDist;
    /* 0x090 */ UnidentifiedFormationTweakVector2 vGetInPositionInRadius;
    /* 0x0B0 */ UnidentifiedFormationTweakVector2 vGetInPositionOutRadius;

private:
    /* 0x0D0 */ u8 mUnidentified0D0[0xB4];

public:
    /* 0x184 */ TweakValue_804F4DC8 fBananaActiveTime;
    /* 0x194 */ TweakValue_804F4DC8 fBobombActiveTime;

private:
    /* 0x1A4 */ u8 mUnidentified1A4[0x30];

public:
    /* 0x1D4 */ TweakValueInt_804F4DC8 nScoreDifferenceMaximum;
    /* 0x1E4 */ TweakValueInt_804F4DC8 nScoreDifferenceMinimum;

private:
    /* 0x1F4 */ u8 mUnidentified1F4[0x10];

public:
    /* 0x204 */ TweakValue_804F4DC8 fBananaResistance;
    /* 0x214 */ TweakValue_804F4DC8 fShellBounceGround;

private:
    /* 0x224 */ u8 mUnidentified224[0x10];

public:
    /* 0x234 */ TweakValue_804F4DC8 fShellSmallRadius;
    /* 0x244 */ TweakValue_804F4DC8 fShellMediumRadius;
    /* 0x254 */ TweakValue_804F4DC8 fShellBigRadius;
    /* 0x264 */ TweakValue_804F4DC8 fShellMediumChance;
    /* 0x274 */ TweakValue_804F4DC8 fShellBigChance;
    /* 0x284 */ TweakValue_804F4DC8 fShellFiveChance;
    /* 0x294 */ TweakValue_804F4DC8 fShellThreeChance;

public:
    /* 0x2A4 */ TweakValue_804F4DC8 fShellExplodeChance;

public:
    /* 0x2B4 */ TweakValue_804F4DC8 fBananaSmallRadius;
    /* 0x2C4 */ TweakValue_804F4DC8 fBananaMediumRadius;
    /* 0x2D4 */ TweakValue_804F4DC8 fBananaBigRadius;
    /* 0x2E4 */ TweakValue_804F4DC8 fBananaMediumChance;
    /* 0x2F4 */ TweakValue_804F4DC8 fBananaBigChance;
    /* 0x304 */ TweakValue_804F4DC8 fBananaFiveChance;
    /* 0x314 */ TweakValue_804F4DC8 fBananaThreeChance;

public:
    /* 0x324 */ TweakValue_804F4DC8 fBananaExplodeChance;

public:
    /* 0x334 */ TweakValue_804F4DC8 fBobombSmallRadius;
    /* 0x344 */ TweakValue_804F4DC8 fBobombMediumRadius;
    /* 0x354 */ TweakValue_804F4DC8 fBobombBigRadius;
    /* 0x364 */ TweakValue_804F4DC8 fBobombMediumChance;
    /* 0x374 */ TweakValue_804F4DC8 fBobombBigChance;
    /* 0x384 */ TweakValue_804F4DC8 fBobombFiveChance;
    /* 0x394 */ TweakValue_804F4DC8 fBobombThreeChance;

private:
    /* 0x3A4 */ u8 mUnidentified3A4[0x1A0];

public:
    /* 0x544 */ TweakValue_804F4DC8 fChainChompRadius;
};

struct unk_800A9274;

struct unk_8056CF08
{
private:
    /* 0x00 */ u8 mUnidentified00[0x04];

public:
    /* 0x04 */ unk_800A9274* mUnidentified04;

private:
    /* 0x08 */ u8 mUnidentified08[0x08];

public:
    /* 0x10 */ GameTweaks* m_pGameTweaks;
    /* 0x14 */ unk_80029F58* m_unk14;

private:
    /* 0x18 */ u8 mUnidentified18[0x08];
}; // total size: 0x20

extern unk_8056CF08 lbl_8056CF08;

#endif // GAME_GAME_TWEAKS_H
