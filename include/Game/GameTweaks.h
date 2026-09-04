#ifndef GAME_GAME_TWEAKS_H
#define GAME_GAME_TWEAKS_H

#include "Game/CharacterTweaks.h"
#include "NL/nlAVLTree.h"

class cPlayer;

class SkillTweaks;

class SkillTweak
{
public:
    float fn_800A0D6C();

public:
    /* 0x00 */ char mNameInFile[0x20];
    /* 0x20 */ unsigned long mUnidentified020;
    /* 0x24 */ unsigned long mUnidentified024;
    /* 0x28 */ SkillTweaks* mUnidentified028;
    /* 0x2C */ float mUnidentified02C;
    /* 0x30 */ bool mUnidentified030;
    /* 0x31 */ u8 mPadding031[3];
    /* 0x34 */ float* mUnidentified034;
    /* 0x38 */ unsigned long mUnidentified038;
    /* 0x3C */ float* mUnidentified03C;
}; // total size: 0x40

class SkillTweaks
{
public:
    SkillTweaks(const char* category);
    ~SkillTweaks();

    float fn_800A327C(unsigned long param1, cPlayer* param2);
    bool fn_800A3350(unsigned long param1, float* param2, cPlayer* param3);
    float* fn_800A3404();
    float fn_800A3474(cPlayer* param1);

public:
    /* 0x000 */ SkillTweak* mUnidentified000[4];
    /* 0x010 */ SkillTweak* Def_Marking;
    /* 0x014 */ SkillTweak* Off_Avoidance;
    /* 0x018 */ SkillTweak* Def_SlideAttackChance;
    /* 0x01C */ SkillTweak* Off_Reaction;
    /* 0x020 */ SkillTweak* mUnidentified020;
    /* 0x024 */ SkillTweak* mUnidentified024;
    /* 0x028 */ SkillTweak* PowerupUsageChance[3][10];
    /* 0x0A0 */ SkillTweak* mUnidentified0A0[4];
    /* 0x0B0 */ SkillTweak* mUnidentified0B0[4];
    /* 0x0C0 */ float mUnidentified0C0[14];
    /* 0x0F8 */ TweakValueImpl_804F4DC8 fShotValue1;
    /* 0x108 */ TweakValueImpl_804F4DC8 fShotValue2;
    /* 0x118 */ TweakValueImpl_804F4DC8 fShotValue3;
    /* 0x128 */ TweakValueImpl_804F4DC8 fShotChance0;
    /* 0x138 */ TweakValueImpl_804F4DC8 fShotChance1;
    /* 0x148 */ TweakValueImpl_804F4DC8 fShotChance2;
    /* 0x158 */ TweakValueImpl_804F4DC8 fShotChance3;
    /* 0x168 */ TweakValueImpl_804F4DC8 fShotChance4;
    /* 0x178 */ TweakValueImpl_804F4DC8 fAttackCarrierDistance;
    /* 0x188 */ TweakValueImpl_804F4DC8 fLooseBallChaseDistance;
    /* 0x198 */ TweakValueImpl_804F4DC8 fGoalieCanInterceptPass;
    /* 0x1A8 */ TweakValueImpl_804F4DC8 fGoalieDekeChance;
    /* 0x1B8 */ TweakValueImpl_804F4DC8 fGoalieDekeSpeed;
    /* 0x1C8 */ const char* mUnidentified1C8;
    /* 0x1CC */ char mszFileName[0x40];
    /* 0x20C */ float mUnidentified20C[4];
    /* 0x21C */ nlAVLTreeSlotPool<unsigned long, SkillTweak*,
        DefaultKeyCompare<unsigned long> > mSkillTweaksList;
    /* 0x240 */ cPlayer* mUnidentified240;
    /* 0x244 */ float mUnidentified244;
}; // total size: 0x248

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

class GameTweaks : public TweaksBase
{
public:
    GameTweaks(const char* name, const char* category);
    virtual ~GameTweaks();
    virtual void Init();

private:
    /* 0x044 */ u8 mUnidentified044[0x2C];

public:
    /* 0x070 */ UnidentifiedFormationTweakVector2 vGetInPositionKeyFielderDist;
    /* 0x090 */ UnidentifiedFormationTweakVector2 vGetInPositionInRadius;
    /* 0x0B0 */ UnidentifiedFormationTweakVector2 vGetInPositionOutRadius;

    /* 0x0D0 */ UnidentifiedFormationTweakValue nStrafeToRunInDirectionDelta;
    /* 0x0E0 */ UnidentifiedFormationTweakValue nBackwardsToStrafeRunInDirectionDelta;
    /* 0x0F0 */ UnidentifiedFormationTweakValue nStrafeToRunOutDirectionDelta;
    /* 0x100 */ UnidentifiedFormationTweakValue nBackwardsToStrafeRunOutDirectionDelta;
    /* 0x110 */ UnidentifiedFormationTweakValue fSlideAttackRadius;
    /* 0x120 */ UnidentifiedFormationTweakValue fArrivalInRadius;
    /* 0x130 */ UnidentifiedFormationTweakValue fArrivalOutRadius;
    /* 0x140 */ UnidentifiedFormationTweakValue fNearSeekInRadius;
    /* 0x150 */ UnidentifiedFormationTweakValue fNearSeekOutRadius;

private:
    /* 0x160 */ u8 mUnidentified160[0x14];

public:
    /* 0x174 */ TweakValueImpl_804F4DC8 mUnidentified174;
    /* 0x184 */ TweakValueImpl_804F4DC8 fBananaActiveTime;
    /* 0x194 */ TweakValueImpl_804F4DC8 fBobombActiveTime;

private:
    /* 0x1A4 */ u8 mUnidentified1A4[0x30];

public:
    /* 0x1D4 */ TweakValueIntImpl_804FD898 nScoreDifferenceMaximum;
    /* 0x1E4 */ TweakValueIntImpl_804FD898 nScoreDifferenceMinimum;

private:
    /* 0x1F4 */ u8 mUnidentified1F4[0x10];

public:
    /* 0x204 */ TweakValueImpl_804F4DC8 fBananaResistance;
    /* 0x214 */ TweakValueImpl_804F4DC8 fShellBounceGround;
    /* 0x224 */ TweakValueImpl_804F4DC8 fShellBounce;

public:
    /* 0x234 */ TweakValueImpl_804F4DC8 fShellSmallRadius;
    /* 0x244 */ TweakValueImpl_804F4DC8 fShellMediumRadius;
    /* 0x254 */ TweakValueImpl_804F4DC8 fShellBigRadius;
    /* 0x264 */ TweakValueImpl_804F4DC8 fShellMediumChance;
    /* 0x274 */ TweakValueImpl_804F4DC8 fShellBigChance;
    /* 0x284 */ TweakValueImpl_804F4DC8 fShellFiveChance;
    /* 0x294 */ TweakValueImpl_804F4DC8 fShellThreeChance;

public:
    /* 0x2A4 */ TweakValueImpl_804F4DC8 fShellExplodeChance;

public:
    /* 0x2B4 */ TweakValueImpl_804F4DC8 fBananaSmallRadius;
    /* 0x2C4 */ TweakValueImpl_804F4DC8 fBananaMediumRadius;
    /* 0x2D4 */ TweakValueImpl_804F4DC8 fBananaBigRadius;
    /* 0x2E4 */ TweakValueImpl_804F4DC8 fBananaMediumChance;
    /* 0x2F4 */ TweakValueImpl_804F4DC8 fBananaBigChance;
    /* 0x304 */ TweakValueImpl_804F4DC8 fBananaFiveChance;
    /* 0x314 */ TweakValueImpl_804F4DC8 fBananaThreeChance;

public:
    /* 0x324 */ TweakValueImpl_804F4DC8 fBananaExplodeChance;

public:
    /* 0x334 */ TweakValueImpl_804F4DC8 fBobombSmallRadius;
    /* 0x344 */ TweakValueImpl_804F4DC8 fBobombMediumRadius;
    /* 0x354 */ TweakValueImpl_804F4DC8 fBobombBigRadius;
    /* 0x364 */ TweakValueImpl_804F4DC8 fBobombMediumChance;
    /* 0x374 */ TweakValueImpl_804F4DC8 fBobombBigChance;
    /* 0x384 */ TweakValueImpl_804F4DC8 fBobombFiveChance;
    /* 0x394 */ TweakValueImpl_804F4DC8 fBobombThreeChance;

private:
    /* 0x3A4 */ u8 mUnidentified3A4[0x1A0];

public:
    /* 0x544 */ TweakValueImpl_804F4DC8 fChainChompRadius;

private:
    /* 0x554 */ u8 mUnidentified554[0x464];
}; // total size: 0x9B8

struct unk_800A9274
{
    virtual ~unk_800A9274();

    /* 0x04 */ TweakValueImpl_804F4DC8 mUnidentified04;
    /* 0x14 */ TweakValueImpl_804F4DC8 mUnidentified14;
    /* 0x24 */ TweakValueImpl_804F4DC8 mUnidentified24;
    /* 0x34 */ TweakValueImpl_804F4DC8 mUnidentified34;
}; // total size: 0x44

struct unk_8056CF08
{
    unk_8056CF08()
        : mUnidentified00(1)
        , mUnidentified04(0)
        , mUnidentified08(0)
        , mUnidentified0C(false)
        , m_pGameTweaks(0)
        , m_unk14(0)
        , mUnidentified18(0)
        , mUnidentified1C(0)
    {
    }

public:
    /* 0x00 */ u32 mUnidentified00;

    /* 0x04 */ unk_800A9274* mUnidentified04;

    /* 0x08 */ int mUnidentified08;
    /* 0x0C */ bool mUnidentified0C;
    /* 0x0D */ u8 mPadding0D[3];

    /* 0x10 */ GameTweaks* m_pGameTweaks;
    /* 0x14 */ FielderTweaks* m_unk14;

public:
    /* 0x18 */ SkillTweaks* mUnidentified18;
    /* 0x1C */ SkillTweaks* mUnidentified1C;
}; // total size: 0x20

extern unk_8056CF08 lbl_8056CF08;

#endif // GAME_GAME_TWEAKS_H
