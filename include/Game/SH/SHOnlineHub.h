#ifndef GAME_SH_SHONLINEHUB_H
#define GAME_SH_SHONLINEHUB_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feButtonComponent.h"
#include "Game/NetworkStats.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;

class SHOnlineHub : public BaseSceneHandler
{
public:
    SHOnlineHub();
    virtual ~SHOnlineHub();
    virtual void SceneCreated();
    virtual void Update(float dt);
    void OnPointerPress(unsigned int index, void* context);
    void OnDialogDismissed();
    void OnErrorDismissed();
    void ShowError(int error);
    void UpdateFriendAndSeasonText();
    void UpdateLocalStats();
    void UpdateStrikerOfTheDay();
    void InitializeButtons();
    void OnPointerEnter(unsigned int index, void* context);
    void OnPointerLeave(unsigned int index, void* context);

    /* 0x01C */ u32 mUnidentified01C;
    /* 0x020 */ FEPointerButton mButtons[4];
    /* 0x2F0 */ TLComponentInstance* mButtonInstances[4];
    /* 0x300 */ FEPointerButton mHelpButton;
    /* 0x3B4 */ TLComponentInstance* mHelpButtonInstance;
    /* 0x3B8 */ FEBackButton mBackButton;
    /* 0x490 */ ButtonComponent mUnidentified490;
    /* 0x4B4 */ bool mInitialized;
    /* 0x4B8 */ int mHoverCounts[4];
    /* 0x4C8 */ u16 mFriendsText[48];
    /* 0x528 */ u16 mDaysRemainText[48];
    /* 0x588 */ float mRefreshTimer;
    /* 0x58C */ bool mPopupActive;
    /* 0x58D */ bool mHasStrikerOfTheDay;
    /* 0x590 */ NetworkRankingMeta mPointsStats;
    /* 0x5A8 */ NetworkRankingMeta mRankStats;
    /* 0x5C0 */ NetworkStatsPlayer mStrikerOfTheDay;
    /* 0x628 */ NetworkRankingMeta mStrikerOfTheDayStats;
    /* 0x640 */ u16 mPlayerNameText[24];
    /* 0x670 */ u16 mRankText[48];
    /* 0x6D0 */ u16 mTodayPointsText[48];
    /* 0x730 */ u16 mStrikerPointsText[48];
    /* 0x790 */ u16 mStrikerDescriptionText[128];
    /* 0x890 */ int mState;
    /* 0x894 */ int mPressedItem;
}; // size 0x898

static const char* sOnlineHubButtonNames[4] = {
    "BTN_UNRANKED", "BTN_RANKED", "BTN_LEADERBOARD", "BTN_FRIENDS"
};

#endif // GAME_SH_SHONLINEHUB_H
