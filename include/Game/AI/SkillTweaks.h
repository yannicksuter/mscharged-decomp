#ifndef GAME_AI_SKILL_TWEAKS_H
#define GAME_AI_SKILL_TWEAKS_H

#include "Game/TweakValue.h"
#include "NL/nlAVLTree.h"
#include "NL/nlPiecewiseLinearCurve.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

class cPlayer;
class Config;

class SkillTweaks;

class SkillTweak
{
public:
    SkillTweak(const char* sNameInFile)
    {
        mOverride = -9999.9f;
        mModifier = 0;
        mpSkillTweaks = 0;
        mHash = nlStringLowerHash(sNameInFile);
        nlStrNCpy(mNameInFile, sNameInFile, sizeof(mNameInFile));
        mOwnsCurve = false;
        mpCurvePoints = 0;
    }

    ~SkillTweak()
    {
        if (mOwnsCurve)
            delete[] mpCurvePoints;
    }

    void ParseCurve(const char* text, int length);
    float GetValue();

public:
    /* 0x00 */ char mNameInFile[0x20];
    /* 0x20 */ unsigned long mHash;
    /* 0x24 */ int mModifier;
    /* 0x28 */ SkillTweaks* mpSkillTweaks;
    /* 0x2C */ float mOverride;
    /* 0x30 */ bool mOwnsCurve;
    /* 0x34 */ nlPiecewiseLinearCurve mCurve;
    /* 0x3C */ nlVector2* mpCurvePoints;
}; // total size: 0x40

class SkillTweaks
{
public:
    SkillTweaks(const char* category);
    ~SkillTweaks();

    void CreateTweaks(bool reload);
    void Init(int difficulty, bool blend, bool reload);
    SkillTweak* AddTweak(const char* name);
    float GetSkillValue(unsigned long key, cPlayer* pPlayer);
    bool GetSkillValue(unsigned long key, float* value, cPlayer* pPlayer);
    float* GetDecisionWeights();
    float GetReaction(cPlayer* pPlayer);

public:
    /* 0x000 */ SkillTweak* Decision_Choice[4];
    /* 0x010 */ SkillTweak* Def_Marking;
    /* 0x014 */ SkillTweak* Off_Avoidance;
    /* 0x018 */ SkillTweak* Def_SlideAttackChance;
    /* 0x01C */ SkillTweak* Off_Reaction;
    /* 0x020 */ SkillTweak* Def_Reaction;
    /* 0x024 */ SkillTweak* Loose_Reaction;
    /* 0x028 */ SkillTweak* PowerupUsageChance[3][10];
    /* 0x0A0 */ SkillTweak* MegaGoalChance[4];
    /* 0x0B0 */ SkillTweak* MegaGoalAccuracy[4];
    /* 0x0C0 */ float mDifficulty[14];
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
    /* 0x1C8 */ const char* mCategory;
    /* 0x1CC */ char mszFileName[0x40];
    /* 0x20C */ float mDecisionWeights[4];
    /* 0x21C */ nlAVLTreeSlotPool<unsigned long, SkillTweak*,
        DefaultKeyCompare<unsigned long> >
        mSkillTweaksList;
    /* 0x240 */ cPlayer* mpCurrentPlayer;
    /* 0x244 */ float mCharacterWeight;
}; // total size: 0x248

#endif // GAME_AI_SKILL_TWEAKS_H
