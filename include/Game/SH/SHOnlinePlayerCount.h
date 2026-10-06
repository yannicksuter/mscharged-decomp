#ifndef GAME_SH_SHONLINEPLAYERCOUNT_H
#define GAME_SH_SHONLINEPLAYERCOUNT_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;

class SHOnlinePlayerCount : public BaseSceneHandler
{
public:
    enum MatchMode
    {
        ModeRanked = 0,
        ModeUnranked = 1,
    };

    enum TransitionState
    {
        StateEntering = 0,
        StateInteractive = 1,
        StateForward = 2,
        StateBack = 3,
    };

    enum PlayerCountButton
    {
        ButtonOneLocalPlayer = 0,
        ButtonTwoLocalPlayers = 1,
    };

    SHOnlinePlayerCount(int mode);
    virtual ~SHOnlinePlayerCount();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void InitializeButtons();
    void OnButtonPointerEnter(unsigned int index, void* context);
    void OnButtonPointerLeave(unsigned int index, void* context);
    void OnButtonPointerPress(unsigned int index, void* context);

    /* 0x01C */ FEBackButton mBackButton;
    /* 0x0F4 */ FEPointerButton mButtons[2];
    /* 0x25C */ TLComponentInstance* mButtonInstances[2];
    /* 0x264 */ int mMode;
    /* 0x268 */ int mPointerInsideCount[4];
    /* 0x278 */ bool mButtonsInitialized;
    /* 0x279 */ unsigned char mPadding279[3];
    /* 0x27C */ int mButtonCount;
    /* 0x280 */ int mTransitionState;
    /* 0x284 */ int mNextScene;
}; // size 0x288

void InitializeOnlineMatch(bool twoLocalPlayers, unsigned char tournament, bool ranked);

#endif // GAME_SH_SHONLINEPLAYERCOUNT_H
