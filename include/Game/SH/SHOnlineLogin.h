#ifndef GAME_SH_ONLINE_LOGIN_H
#define GAME_SH_ONLINE_LOGIN_H

#include "Game/BaseSceneHandler.h"
#include "Game/NetworkSession.h"
#include "Game/FE/feBackButton.h"

class SHOnlineLogin : public BaseSceneHandler, public NetworkLoginListener
{
public:
    SHOnlineLogin();
    virtual ~SHOnlineLogin();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    virtual void OnLoginResult(int result);
    virtual void OnStatsResult(bool success);

    void OnErrorDismissed();

    /* 0x020 */ bool mPopupActive;
    /* 0x024 */ FEBackButton mBackButton;
    /* 0x0FC */ TLComponentInstance* mLoginComponent;
    /* 0x100 */ int mState;
    /* 0x104 */ float mElapsedTime;
    /* 0x108 */ float mSlideCompleteTime;
}; // size 0x10C


#endif // GAME_SH_ONLINE_LOGIN_H
