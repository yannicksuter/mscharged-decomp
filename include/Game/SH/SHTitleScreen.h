#ifndef GAME_SH_SH_TITLE_SCREEN_H
#define GAME_SH_SH_TITLE_SCREEN_H

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;

void StartTitleToMainMenuTransition();

class TitleScene : public BaseSceneHandler
{
public:
    TitleScene(ScreenMovement movement);
    virtual ~TitleScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void StartDemoMatch();
    void OnControllerPointerPress(int index, void* context);
    void OnControllerPointerEnter(int index, void* context);
    void OnControllerPointerLeave(int index, void* context);
    void InitializePointerButtons();

    /* 0x01C */ float m_fTimeElapsed;
    /* 0x020 */ u8 mUnidentified20[4];
    /* 0x024 */ FEPointerButton mControllerComponent;
    /* 0x0D8 */ TLComponentInstance* mTextPressStart;
    /* 0x0DC */ bool mStartedDemo;
    /* 0x0DD */ bool mStartedMovie;
    /* 0x0DE */ bool mInitialized;
    /* 0x0DF */ bool mPointerOverStartButton;
    /* 0x0E0 */ ScreenMovement mMovement;
    /* 0x0E4 */ int mUnlockInputSequence[9];
    /* 0x108 */ bool mUnlockInputEntered[9];
    /* 0x111 */ u8 mPadding111[3];
}; // size 0x114

class HealthWarningSceneV2 : public BaseSceneHandler
{
public:
    enum WarningPhase
    {
        PhaseFadeIn = 0,
        PhaseWaitForInput = 1,
        PhaseFadeOut = 2,
        PhaseFinished = 3,
    };

    HealthWarningSceneV2();
    virtual ~HealthWarningSceneV2();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x1C */ int mWarningPhase;
}; // size 0x20

#endif // GAME_SH_SH_TITLE_SCREEN_H
