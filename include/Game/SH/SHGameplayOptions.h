#ifndef GAME_SH_SHGAMEPLAYOPTIONS_H
#define GAME_SH_SHGAMEPLAYOPTIONS_H

#include "Game/BaseSceneHandler.h"
#include "Game/DB/UserOptions.h"
#include "Game/FE/feBackButton.h"

struct FEPageControls;
class TLComponentInstance;

class SHGameplayOptions : public BaseSceneHandler
{
public:
    SHGameplayOptions();
    virtual ~SHGameplayOptions();
    virtual void SceneCreated();
    virtual void Update(float dt);
    void ToggleOptionsView();
    void UpdateLimitView(bool value);
    void InitializeSelections();
    void ApplyOptionSelection(int item);
    void UpdateLimitText(int type, int value);
    void InitializePointerButtons();
    void OnOptionPointerEnter(unsigned int index, void* context);
    void OnOptionPointerLeave(unsigned int index, void* context);
    void OnOptionPointerPress(unsigned int index, void* context);
    void OnCheatPointerEnter(unsigned int index, void* context);
    void OnCheatPointerLeave(unsigned int index, void* context);
    void OnCheatPointerPress(unsigned int index, void* context);
    void OnDonePointerEnter(unsigned int index, void* context);
    void OnDonePointerLeave(unsigned int index, void* context);
    void OnDonePointerPress(unsigned int index, void* context);
    void CommitSettings();
    void UpdateCheatText();

    /* 0x001C */ FEPointerButton mOptionButtons[24];
    /* 0x10FC */ FEPointerButton mCheatButtons[3];
    /* 0x1318 */ TLComponentInstance* mOptionInstances[24];
    /* 0x1378 */ TLComponentInstance* mCheatInstances[3];
    /* 0x1384 */ TLComponentInstance* mDoneButtonInstance;
    /* 0x1388 */ TLInstance* mGoalsSection;
    /* 0x138C */ TLInstance* mMinutesSection;
    /* 0x1390 */ TLInstance* mSkillSection;
    /* 0x1394 */ TLInstance* mSeriesSection;
    /* 0x1398 */ FEBackButton mNavigation;
    /* 0x1470 */ FEPointerButton mDoneButton;
    /* 0x1524 */ FEPageControls* mPageControls;
    /* 0x1528 */ int mPointerInsideCounts[4];
    /* 0x1538 */ unsigned short mSeriesText[32];
    /* 0x1578 */ unsigned short mLimitText[32];
    /* 0x15B8 */ int mViewIndex;
    /* 0x15BC */ bool mInitialized;
    /* 0x15BD */ bool mGoalLimitSelected;
    /* 0x15C0 */ GameplaySettings mSettings;
    /* 0x15DC */ CheatSettings mPowerupSettings;
    /* 0x15E8 */ int mSelectedSkill;
    /* 0x15EC */ int mSelectedSeries;
    /* 0x15F0 */ int mSelectedLimitType;
    /* 0x15F4 */ int mSelectedTime;
    /* 0x15F8 */ int mSelectedGoals;
    /* 0x15FC */ int mFlowState;
}; // size 0x1600

#endif // GAME_SH_SHGAMEPLAYOPTIONS_H
