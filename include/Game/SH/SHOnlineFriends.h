#ifndef GAME_SH_SHONLINEFRIENDS_H
#define GAME_SH_SHONLINEFRIENDS_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feOnlinePlayerRow.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feBackButton.h"

class TLTextInstance;

class SHOnlineFriends : public BaseSceneHandler
{
public:
    enum TransitionState
    {
        StateEntering = 0,
        StateInteractive = 1,
        StateForward = 2,
        StateBack = 3,
    };

    SHOnlineFriends();
    virtual ~SHOnlineFriends();
    virtual void SceneCreated();
    virtual void Update(float dt);
    void UpdateScrollRange();
    void UpdateFriend(int index);
    static int CompareFriendStatus(const void* a, const void* b);
    void InitializeButtons();
    void OnPointerPress(int index, void* context);
    void OnPointerEnter(int index, void* context);
    void OnPointerLeave(int index, void* context);
    void DeleteFriend(int index);
    void CancelDeleteFriend();
    void UpdateFriendCode();
    void OnDialogDismissed();
    void OnErrorDismissed();
    void ShowDialog(int type);
    void ConfirmDeleteFriend(int index);
    void ShowError(int error);
    void StartFriendInvite();
    void UpdateAddFriendRow();
    void UpdateVisibleRows();

    /* 0x001C */ int mUnidentified001C;
    /* 0x0020 */ int mScrollOffset;
    /* 0x0024 */ int mScrollRange;
    /* 0x0028 */ int mPointerHoverCount;
    /* 0x002C */ int mPressedItem;
    /* 0x0030 */ bool mInitialized;
    /* 0x0031 */ bool mPressHandled;
    /* 0x0034 */ float mRefreshTimer;
    /* 0x0038 */ FEPointerButton mRowButtons[4];
    /* 0x0308 */ FEScrollBar mScrollBar;
    /* 0x04BC */ FEBackButton mBackButton;
    /* 0x0594 */ TLTextInstance* mFriendCodeText;
    /* 0x0598 */ u16 mFriendCodeBuffer[64];
    /* 0x0618 */ TLComponentInstance* mRowInstances[4];
    /* 0x0628 */ u16 mRankText[4][32];
    /* 0x0728 */ u16 mRecordText[4][48];
    /* 0x08A8 */ FEOnlinePlayerRow mFriendRows[64];
    /* 0x2EA8 */ FEOnlinePlayerRow* mSortedFriendRows[64];
    /* 0x2FA8 */ bool mPopupActive;
    /* 0x2FAC */ int mTransitionState;
}; // size 0x2FB0

#endif // GAME_SH_SHONLINEFRIENDS_H
