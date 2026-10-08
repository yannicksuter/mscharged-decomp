#ifndef GAME_SH_SH_NETWORK_START_H
#define GAME_SH_SH_NETWORK_START_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feMenu.h"
#include "Game/LANLobbyListener.h"

class TLComponentInstance;
class TLTextInstance;

class NetworkStartScene : public BaseSceneHandler, public LANLobbyListener
{
public:
    enum State
    {
        STATE_ONLINE_OPTIONS = 0,
        STATE_CREATE_GAME = 1,
        STATE_JOIN_GAME = 2,
        STATE_WAIT_FOR_START = 3,
    };

    enum ActionButtons
    {
        ACTION_BUTTONS_HIDDEN = -1,
        ACTION_BUTTONS_A_AND_B = 0,
        ACTION_BUTTONS_A = 1,
        ACTION_BUTTONS_B = 2,
    };

    NetworkStartScene();
    virtual ~NetworkStartScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    virtual void OnGameCreated(int result);
    virtual void OnGameJoined(int result);
    virtual void OnGameLaunched(int result);
    virtual void OnGameFound(LANGameInfo* game) { }
    virtual void UnidentifiedVirtual10() { }
    virtual void OnGameExpired(LANGameInfo* game) { }
    virtual void OnLobbyShutdown() { }

    void SetActionButtons(int buttonState);
    void SelectMenuItem(TLComponentInstance* component);
    void DeselectMenuItem(TLComponentInstance* component);
    void EnterState(int state);
    void OnMenuItemApply(TLComponentInstance* component, int state);
    void UpdateMenuInput();
    void UpdateLobby();

    /* 0x020 */ MenuList<TLComponentInstance> mMenuItems;
    /* 0x234 */ int mState;
    /* 0x238 */ bool mShowPlayer2Controls;
    /* 0x23C */ TLTextInstance* mPlayerText[7];
    /* 0x258 */ unsigned short mPlayerNames[7][11];
}; // size: 0x2F4

extern bool gNetworkStartWaitingForDialog;
extern bool gNetworkStartResetRequested;

void ResetNetworkStart();
void ResumeNetworkStart();

#endif // GAME_SH_SH_NETWORK_START_H
