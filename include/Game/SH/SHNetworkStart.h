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

    void SetActionButtons(int state);
    void SelectMenuItem(TLComponentInstance* component);
    void DeselectMenuItem(TLComponentInstance* component);

    /* 0x020 */ MenuList<TLComponentInstance> mMenuItems;
    /* 0x234 */ int mState;
    /* 0x238 */ bool mUnidentified238;
    /* 0x23C */ TLTextInstance* mPlayerText[7];
    /* 0x258 */ unsigned short mPlayerNames[7][11];
}; // size: 0x2F4

extern bool gNetworkStartWaitingForDialog;
extern bool gNetworkStartResetRequested;

void ResetNetworkStart();
void ResumeNetworkStart();

#endif // GAME_SH_SH_NETWORK_START_H
