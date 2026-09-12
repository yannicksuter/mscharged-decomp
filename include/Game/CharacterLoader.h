#ifndef GAME_CHARACTER_LOADER_H
#define GAME_CHARACTER_LOADER_H

#include "Game/CharacterTemplate.h"

class CharacterLoader_8056B290
{
public:
    struct Entry
    {
        /* 0x00 */ int nTeamID;
        /* 0x04 */ int nCharIdx;
        /* 0x08 */ int nPlayerID;
        /* 0x0C */ eCharacterClass cc;
        /* 0x10 */ bool bCaptain;
        /* 0x11 */ bool bGoalie;
        /* 0x12 */ bool bSidekick;
    }; // total size: 0x14

    CharacterLoader_8056B290()
    {
        for (int i = 0; i < 2; i++)
        {
            captain[i] = CHARACTER_CLASS_INVALID;
            for (int j = 0; j < 3; j++)
            {
                sidekick[i][j] = CHARACTER_CLASS_INVALID;
            }
            goalie[i] = CHARACTER_CLASS_INVALID;
        }
        mAudioRequestCount = -1;
        mAudioCompletedCount = -1;
    }
    ~CharacterLoader_8056B290();

    void fn_80009BC8();
    bool fn_80009EFC();
    bool fn_80009F48();
    void fn_80009FCC();
    void fn_8000A0A8();
    bool fn_8000A144();
    bool fn_8000A224();
    bool fn_8000A2FC();
    void fn_8000A324();
    bool fn_8000A378();
    void fn_8000A418();
    bool fn_8000A5D8();
    bool fn_8000A67C();
    bool fn_8000A790();
    bool fn_8000A870();
    void fn_8000A8E4(int nModel);
    bool fn_8000A9A4(int nModel);
    unsigned int fn_8000AAB8();
    bool fn_8000AB18();
    unsigned int fn_8000ACEC();
    bool fn_8000AD4C();
    bool fn_8000AE04();
    void fn_8000AE90();
    bool fn_8000B00C();
    void fn_8000B0E8();
    bool fn_8000B14C();
    bool fn_8000B1B8();
    unsigned int fn_8000B1F8();
    bool fn_8000B230();
    bool fn_8000B3C0();
    bool fn_8000B3E0();
    bool fn_8000B6C4();
    void fn_8000B8E8();
    bool fn_8000B9F4();
    void fn_8000BA00();
    void fn_8000BD70();
    bool fn_8000BD88();
    bool fn_8000BF04();
    bool fn_8000BFA0();
    void fn_8000C0FC();
    bool fn_8000C124();
    void fn_8000C130();
    bool fn_8000C1A4();
    void fn_8000C1B0();
    void fn_8000C22C();
    bool fn_8000C254();

    /* 0x000 */ Entry mEntries[10];
    /* 0x0C8 */ int mCurrentIndex;
    /* 0x0CC */ Entry* mCurrent;
    /* 0x0D0 */ tCharacterTemplate* mTemplate;
    /* 0x0D4 */ tCharacterTemplateInfo* mTemplateInfo;
    /* 0x0D8 */ eCharacterClass captain[2];
    /* 0x0E0 */ eCharacterClass sidekick[2][3];
    /* 0x0F8 */ eCharacterClass goalie[2];
    /* 0x100 */ void* mTextureData;
    /* 0x104 */ unsigned long mTextureSize;
    /* 0x108 */ void* mAltTextureData;
    /* 0x10C */ unsigned long mAltTextureSize;
    /* 0x110 */ void* mExtraTextureData;
    /* 0x114 */ unsigned long mExtraTextureSize;
    /* 0x118 */ void* mEffectsData;
    /* 0x11C */ unsigned int mEffectsLoad;
    /* 0x120 */ void* mEffectsNonResData;
    /* 0x124 */ unsigned int mEffectsNonResLoad;
    /* 0x128 */ void* mModelData[4];
    /* 0x138 */ unsigned long mModelSize[4];
    /* 0x148 */ void* mUnidentified148;
    /* 0x14C */ unsigned long mUnidentified14C;
    /* 0x150 */ void* mHierarchyData;
    /* 0x154 */ unsigned long mHierarchySize;
    /* 0x158 */ void* mPhysicsData;
    /* 0x15C */ unsigned long mPhysicsSize;
    /* 0x160 */ void* mAnimData;
    /* 0x164 */ unsigned long mAnimSize;
    /* 0x168 */ void* mTriggerData;
    /* 0x16C */ unsigned long mTriggerSize;
    /* 0x170 */ void* mAnimRetargetData;
    /* 0x174 */ unsigned long mAnimRetargetSize;
    /* 0x178 */ void* mSidekickTextureData;
    /* 0x17C */ unsigned long mSidekickTextureSize;
    /* 0x180 */ int mAudioRequestCount;
    /* 0x184 */ int mAudioCompletedCount;

    static CharacterLoader_8056B290 sUnidentifiedInstance;
}; // total size: 0x188

#endif // GAME_CHARACTER_LOADER_H
