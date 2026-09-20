#ifndef GAME_SH_ONLINE_GUEST_CONTROLLER_SELECT_H
#define GAME_SH_ONLINE_GUEST_CONTROLLER_SELECT_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;

class SHOnlineGuestControllerSelect : public BaseSceneHandler
{
public:
    SHOnlineGuestControllerSelect();
    virtual ~SHOnlineGuestControllerSelect();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateDoneButtonVisibility();
    void OnControllerPointerEnter(int index, void*);
    void OnControllerPointerLeave(int index, void*);
    void OnControllerPointerPress(int index, void*);
    void OnDonePointerEnter(int index, void*);
    void OnDonePointerInside(int index, void*);
    void OnDonePointerPress(int, void*);
    void InitializeButtons();
    void OnDonePointerLeave(int index, void*);

    bool IsLocalController(unsigned int index) const
    {
        return mPrimaryController == index || mGuestController == index;
    }

    /* 0x01C */ bool mRespondingToInvitation;
    /* 0x01D */ unsigned char mPadding1D[3];
    /* 0x020 */ int mState;
    /* 0x024 */ FEBackButton mNavigation;
    /* 0x0FC */ FEPointerButton mDoneButton;
    /* 0x1B0 */ FEPointerButton mControllerButton;
    /* 0x264 */ TLComponentInstance* mDoneButtonInstance;
    /* 0x268 */ TLComponentInstance* mHomeInstance;
    /* 0x26C */ unsigned short mGuestName[11];
    /* 0x282 */ unsigned char mPadding282[2];
    /* 0x284 */ int mPrimaryController;
    /* 0x288 */ int mGuestController;
    /* 0x28C */ int mHoverCounts[4];
    /* 0x29C */ bool mButtonsInitialized;
    /* 0x29D */ bool mDoneButtonEntering;
    /* 0x29E */ bool mSelectionConfirmed;
    /* 0x29F */ unsigned char mPadding29F;
}; // size 0x2A0

#endif // GAME_SH_ONLINE_GUEST_CONTROLLER_SELECT_H
