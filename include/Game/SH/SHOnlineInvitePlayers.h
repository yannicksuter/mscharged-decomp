#ifndef GAME_SH_SHONLINEINVITEPLAYERS_H
#define GAME_SH_SHONLINEINVITEPLAYERS_H

#include "Game/BaseSceneHandler.h"
#include "Game/NetworkStatsManager.h"
#include "Game/FE/feOnlinePlayerRow.h"
#include "Game/FE/feScrollBar.h"
#include "NL/nlString.h"
#include "Game/FE/feBackButton.h"

struct UnidentifiedDraftEntry;

struct OnlineInviteSlot
{
    /* 0x00 */ bool mLocal;
    /* 0x01 */ bool mGuest;
    /* 0x04 */ UnidentifiedDraftEntry* mEntry;
}; // size 0x08

class SHOnlineInvitePlayers : public BaseSceneHandler
{
public:
    SHOnlineInvitePlayers();
    virtual ~SHOnlineInvitePlayers();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void RefreshRows();
    void InitializeButtons();
    void OnInvitePointerEnter(int index, void* context);
    void OnInvitePointerLeave(int index, void* context);
    void OnInvitePointerPress(int index, void* context);
    void OnCancelPointerEnter(int index, void* context);
    void OnCancelPointerLeave(int index, void* context);
    void OnCancelPointerPress(int index, void* context);
    void OnStartPointerEnter(int index, void* context);
    void OnStartPointerLeave(int index, void* context);
    void OnStartPointerPress(int index, void* context);
    void OnInvitationErrorDismissed();
    bool RefreshLobbySlots();
    void SetPlayerRow(int value, int index);
    void HidePlayerPortrait(int index);
    void OnLobbyErrorDismissed();

    /* 0x01C */ bool mIsHost;
    /* 0x01D */ bool mStartFriendServer;
    /* 0x01E */ bool mPopupActive;
    /* 0x020 */ int mHoverCounts[4];
    /* 0x030 */ bool mInitialized;
    /* 0x034 */ float mDeclinedDisplayTime;
    /* 0x038 */ int mDeclinedFriendIndex;
    /* 0x03C */ float mInvitationTimeout;
    /* 0x040 */ bool mStartingMatch;
    /* 0x041 */ bool mStartHovered;
    /* 0x042 */ bool mBackEnabled;
    /* 0x044 */ FEBackButton mBackButton;
    /* 0x11C */ FEScrollBar mScrollBar;
    /* 0x2D0 */ TLComponentInstance* mBackButtonInstance;
    /* 0x2D4 */ FEPointerButton mStartButton;
    /* 0x388 */ TLComponentInstance* mStartButtonInstance;
    /* 0x38C */ TLComponentInstance* mRowInstances[4];
    /* 0x39C */ FEPointerButton mInviteButtons[4];
    /* 0x66C */ TLComponentInstance* mCancelInstances[4];
    /* 0x67C */ FEPointerButton mCancelButtons[4];
    /* 0x94C */ int mSlotCount;
    /* 0x950 */ OnlineInviteSlot mSlots[4];
    /* 0x970 */ int mMachineCount;
    /* 0x974 */ FEOnlinePlayerRow mRows[4];
    /* 0xBD4 */ u16 mRankText[4][0x20];
    /* 0xCD4 */ u16 mRecordText[4][0x30];
    /* 0xE54 */ u8 mUnidentifiedE54[8];
}; // size 0xE5C

#endif // GAME_SH_SHONLINEINVITEPLAYERS_H
