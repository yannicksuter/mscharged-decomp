#ifndef GAME_SH_SH_OPTIONS_H
#define GAME_SH_SH_OPTIONS_H

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;

class OptionsScene : public BaseSceneHandler
{
public:
    OptionsScene();
    virtual ~OptionsScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void OnButtonPointerPress(int index, void* context);
    void OnButtonPointerEnter(int index, void* context);
    void OnButtonPointerLeave(int index, void* context);
    void InitializePointerButtons();

    /* 0x01C */ TLComponentInstance* mOptionInstances[3];
    /* 0x028 */ FEPointerButton mOptionButtons[3];
    /* 0x244 */ FEBackButton mBackButton;
    /* 0x31C */ bool mInitialized;
    /* 0x31D */ u8 mPadding31D[3];
    /* 0x320 */ int mState;
    /* 0x324 */ SceneList mNextScene;
}; // size 0x328

#endif // GAME_SH_SH_OPTIONS_H
