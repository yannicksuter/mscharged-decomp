#ifndef GAME_SH_SH_TITLE_SCREEN_H
#define GAME_SH_SH_TITLE_SCREEN_H

#include "Game/BaseGameSceneManager.h"
#include "unclassified/tu_80219248.h"

class TLComponentInstance;

void StartMovieCB();

class TitleScene : public BaseSceneHandler
{
public:
    TitleScene(ScreenMovement movement);
    virtual ~TitleScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void fn_801D1F6C();
    void fn_801D22C8(int index, void* context);
    void fn_801D2478(int index, void* context);
    void fn_801D24EC(int index, void* context);

    /* 0x01C */ float m_fTimeElapsed;
    /* 0x020 */ u8 mUnidentified20[4];
    /* 0x024 */ TU80219248Component mControllerComponent;
    /* 0x0D8 */ TLComponentInstance* mTextPressStart;
    /* 0x0DC */ bool mStartedDemo;
    /* 0x0DD */ bool mStartedMovie;
    /* 0x0DE */ bool mUnidentifiedDE;
    /* 0x0DF */ bool mUnidentifiedDF;
    /* 0x0E0 */ ScreenMovement mMovement;
    /* 0x0E4 */ int mControllerDefaults[9];
    /* 0x108 */ bool mControllerReady[9];
    /* 0x111 */ u8 mPadding111[3];
}; // size 0x114

class HealthWarningSceneV2 : public BaseSceneHandler
{
public:
    HealthWarningSceneV2();
    virtual ~HealthWarningSceneV2();
    virtual void Update(float dt);
    virtual void SceneCreated();

    /* 0x1C */ int mState;
}; // size 0x20

#endif // GAME_SH_SH_TITLE_SCREEN_H
