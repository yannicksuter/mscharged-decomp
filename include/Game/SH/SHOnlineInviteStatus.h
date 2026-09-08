#ifndef GAME_SH_ONLINE_INVITE_STATUS_H
#define GAME_SH_ONLINE_INVITE_STATUS_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;

class SHOnlineInviteStatus : public BaseSceneHandler
{
public:
    SHOnlineInviteStatus();
    virtual ~SHOnlineInviteStatus();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void OnConnectionErrorDismissed();

    /* 0x01C */ int mStatus;
    /* 0x020 */ float mReturnDelay;
    /* 0x024 */ FEBackButton mBackButton;
    /* 0x0FC */ bool mCanCancel;
    /* 0x0FD */ bool mPopupActive;
    /* 0x0FE */ u8 mPaddingFE[2];
    /* 0x100 */ float mElapsedTime;
    /* 0x104 */ int mPointersInitialized;
    /* 0x108 */ TLComponentInstance* mStatusInstance;
}; // size 0x10C

#endif // GAME_SH_ONLINE_INVITE_STATUS_H
