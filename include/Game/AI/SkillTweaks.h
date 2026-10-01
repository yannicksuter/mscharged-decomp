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
    float GetValue() const;

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

    float GetSkillRating(unsigned int index) const
    {
        float result = -9999.9f;
        switch (index)
        {
        case 1:
            result = mDifficulty[0];
            break;
        case 2:
            result = mDifficulty[1];
            break;
        case 3:
            result = mDifficulty[2];
            break;
        case 4:
            result = mDifficulty[3];
            break;
        case 5:
            result = mDifficulty[4];
            break;
        case 6:
            result = mDifficulty[5];
            break;
        case 7:
            result = mDifficulty[6];
            break;
        case 8:
            result = mDifficulty[7];
            break;
        case 9:
            result = mDifficulty[8];
            break;
        case 10:
            result = mDifficulty[9];
            break;
        case 11:
            result = mDifficulty[10];
            break;
        case 12:
            result = mDifficulty[11];
            break;
        case 13:
            result = mDifficulty[12];
            break;
        case 14:
            result = mDifficulty[13];
            break;
        }
        return result;
    }

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
    /* 0x0F8 */ TweakFloatBinding fShotValue1;
    /* 0x108 */ TweakFloatBinding fShotValue2;
    /* 0x118 */ TweakFloatBinding fShotValue3;
    /* 0x128 */ TweakFloatBinding fShotChance0;
    /* 0x138 */ TweakFloatBinding fShotChance1;
    /* 0x148 */ TweakFloatBinding fShotChance2;
    /* 0x158 */ TweakFloatBinding fShotChance3;
    /* 0x168 */ TweakFloatBinding fShotChance4;
    /* 0x178 */ TweakFloatBinding fAttackCarrierDistance;
    /* 0x188 */ TweakFloatBinding fLooseBallChaseDistance;
    /* 0x198 */ TweakFloatBinding fGoalieCanInterceptPass;
    /* 0x1A8 */ TweakFloatBinding fGoalieDekeChance;
    /* 0x1B8 */ TweakFloatBinding fGoalieDekeSpeed;
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
