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

    enum ScenePhase
    {
        PHASE_ENTERING = 0,
        PHASE_CHOOSING = 1,
        PHASE_EXITING_TO_OPTION = 2,
        PHASE_EXITING_TO_MAIN_MENU = 3,
    };

    /* 0x01C */ TLComponentInstance* mOptionInstances[3];
    /* 0x028 */ FEPointerButton mOptionButtons[3];
    /* 0x244 */ FEBackButton mBackButton;
    /* 0x31C */ bool mPointerButtonsInitialized;
    /* 0x31D */ u8 mPadding31D[3];
    /* 0x320 */ int mScenePhase;
    /* 0x324 */ SceneList mNextScene;
}; // size 0x328

#endif // GAME_SH_SH_OPTIONS_H
