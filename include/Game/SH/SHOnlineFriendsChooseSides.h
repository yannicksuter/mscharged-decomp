#ifndef GAME_SH_SHONLINEFRIENDSCHOOSESIDES_H
#define GAME_SH_SHONLINEFRIENDSCHOOSESIDES_H

#include "Game/BaseSceneHandler.h"
#include "Game/NetworkMessages.h"
#include "NL/nlColour.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feTimer.h"

class TLComponentInstance;
class TLTextInstance;

struct OnlineSidePlayer
{
    OnlineSidePlayer()
        : mMachineIndex(-1)
        , mIsGuest(false)
    {
    }

    int mMachineIndex;
    bool mIsGuest;
}; // size 0x8

class SHOnlineFriendsChooseSides : public BaseSceneHandler
{
public:
    SHOnlineFriendsChooseSides();
    virtual ~SHOnlineFriendsChooseSides();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void SetDraftMessage(NetMessageDraft message) { mDraftMessage = message; }

    void OnCountdownTick(FETimer* timer);
    void InitializeButtons();
    void OnSidePointerEnter(int index, void* context);
    void OnSidePointerLeave(int index, void* context);
    void OnSidePointerInside(int index, void* context);
    void OnSidePointerPress(int index, void* context);
    void OnDonePointerEnter(int index, void* context);
    void OnDonePointerInside(int index, void* context);
    void OnDonePointerLeave(int index, void* context);
    void OnDonePointerPress(int index, void* context);
    void UpdateDoneButton();
    int GetOnlinePlayerIndex(int pad);
    void OnSidesChanged(NetMessageSidesChanged* message);
    void DoChangeSides(int newSide, int oldSide, int index);
    void OnDisconnectPopupClosed();

    /* 0x01C */ bool mDisconnectPopupActive;
    /* 0x01D */ u8 mPadding01D[3];
    /* 0x020 */ NetMessageDraft mDraftMessage;
    /* 0x434 */ bool mButtonsInitialized;
    /* 0x435 */ bool mDoneButtonAnimating;
    /* 0x436 */ bool mDraftStarted;
    /* 0x437 */ u8 mPadding437;
    /* 0x438 */ FEPointerButton mSideButtons[2];
    /* 0x5A0 */ FEPointerButton mDoneButton;
    /* 0x654 */ TLComponentInstance* mSideInstances[2];
    /* 0x65C */ TLComponentInstance* mDoneButtonInstance;
    /* 0x660 */ TLTextInstance* mSelectSideText;
    /* 0x664 */ int mPlayerSides[4];
    /* 0x674 */ nlColour mPlayerColours[4];
    /* 0x684 */ OnlineSidePlayer mOnlinePlayers[4];
    /* 0x6A4 */ int mPointerInsideCounts[4];
    /* 0x6B4 */ int mPlayerCount;
    /* 0x6B8 */ int mSidePlayerIndices[2][3];
    /* 0x6D0 */ unsigned short mPlayerNames[4][14];
    /* 0x740 */ unsigned short mTimerText[8];
    /* 0x750 */ FETimer mCountdownTimer;
    /* 0x76C */ bool mTimerTextDirty;
    /* 0x76D */ u8 mPadding76D[3];
    /* 0x770 */ int mSecondsRemaining;
}; // size 0x774

#endif // GAME_SH_SHONLINEFRIENDSCHOOSESIDES_H
