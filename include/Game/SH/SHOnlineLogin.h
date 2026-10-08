#ifndef GAME_SH_ONLINE_LOGIN_H
#define GAME_SH_ONLINE_LOGIN_H

#include "Game/BaseSceneHandler.h"
#include "Game/NetworkSession.h"
#include "Game/FE/feBackButton.h"

enum eOnlineLoginState
{
    ONLINE_LOGIN_WAIT_SLIDE = 0,
    ONLINE_LOGIN_WAIT_NETWORK = 1,
    ONLINE_LOGIN_CHECK_REQUEST = 2,
    ONLINE_LOGIN_WAIT_THREAD = 3,
    ONLINE_LOGIN_WAIT_RESULT = 4,
    ONLINE_LOGIN_WAIT_STATS = 5,
    ONLINE_LOGIN_SUCCESS = 6,
    ONLINE_LOGIN_NETWORK_ERROR = 7,
    ONLINE_LOGIN_LOGIN_ERROR = 8,
    ONLINE_LOGIN_STATS_ERROR = 9,
};

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
    /* 0x100 */ eOnlineLoginState mState;
    /* 0x104 */ float mElapsedTime;
    /* 0x108 */ float mSlideCompleteTime;
}; // size 0x10C


#endif // GAME_SH_ONLINE_LOGIN_H
