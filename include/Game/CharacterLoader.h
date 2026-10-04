#ifndef GAME_CHARACTER_LOADER_H
#define GAME_CHARACTER_LOADER_H

#include "Game/CharacterTemplate.h"

class CharacterLoader
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

    CharacterLoader()
    {
        for (int i = 0; i < 2; i++)
        {
            mCaptain[i] = CHARACTER_CLASS_INVALID;
            for (int j = 0; j < 3; j++)
            {
                mSidekick[i][j] = CHARACTER_CLASS_INVALID;
            }
            mGoalie[i] = CHARACTER_CLASS_INVALID;
        }
        mAudioRequestCount = -1;
        mAudioCompletedCount = -1;
    }
    ~CharacterLoader();

    void BuildCharacterList();
    bool NextCharacter();
    bool NeedsCharacterTextures();
    void StartLoadingCharacterTextures();
    void StartLoadingShockTextures();
    bool FinalizeLoadingCharacterTextures();
    bool FinalizeLoadingShockTexture();
    bool NeedsSharedTextures();
    void StartLoadingSharedTextures();
    bool FinalizeLoadingSharedTextures();
    void StartLoadingCharacterEffects();
    bool FinalizeLoadingCharacterEffects();
    bool StartLoadingExtraTextures();
    bool FinalizeLoadingExtraTextures();
    bool AcquireCurrentTemplate();
    void StartLoadingCharacterModel(int nModel);
    bool FinalizeLoadingCharacterModel(int nModel);
    unsigned int StartLoadingHierarchy();
    bool FinalizeLoadingHierarchy();
    unsigned int StartLoadingCharacterPhysicsElements();
    bool FinalizeLoadingCharacterPhysicsElements();
    bool ShareDuplicateAnimInventory();
    void StartLoadingCharacterAnimations();
    bool FinalizeLoadingCharacterAnimations();
    void StartLoadingCharacterTriggers();
    bool FinalizeLoadingCharacterTriggers();
    bool HasAnimRetarget();
    unsigned int StartLoadingAnimRetarget();
    bool FinalizeLoadingAnimRetarget();
    bool NeedsSidekickSwapTexture();
    bool StartLoadingSidekickSwapTexture();
    bool FinalizeLoadingSidekickSwapTexture();
    void StartLoadingCharINIFiles();
    bool FinalizeLoadingCharINIFiles();
    void CreateCharacterInstance();
    void fn_8000BD70();
    bool HasAlternateSwapTexture();
    bool StartLoadingCaptainOrGoalieAlternateSwapTexture();
    bool FinalizeLoadingCaptainOrGoalieAlternateSwapTexture();
    void StartLoadingAudioBank0();
    bool NeedsCaptainAudio();
    void StartLoadingCaptainAudio();
    bool NeedsSidekickAudio();
    void StartLoadingSidekickAudio();
    void StartLoadingAudioBank13();
    bool FinalizeAudio();

    /* 0x000 */ Entry mEntries[10];
    /* 0x0C8 */ int mCurrentIndex;
    /* 0x0CC */ Entry* mCurrent;
    /* 0x0D0 */ tCharacterTemplate* mTemplate;
    /* 0x0D4 */ tCharacterTemplateInfo* mTemplateInfo;
    /* 0x0D8 */ eCharacterClass mCaptain[2];
    /* 0x0E0 */ eCharacterClass mSidekick[2][3];
    /* 0x0F8 */ eCharacterClass mGoalie[2];
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

    static CharacterLoader sInstance;
}; // total size: 0x188

#endif // GAME_CHARACTER_LOADER_H
