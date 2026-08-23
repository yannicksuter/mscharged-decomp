#ifndef GAME_SH_SH_TITLE_SCREEN_H
#define GAME_SH_SH_TITLE_SCREEN_H

#include "Game/BaseGameSceneManager.h"

class TitleScene : public BaseSceneHandler
{
public:
    TitleScene(ScreenMovement movement);
    virtual ~TitleScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    /* 0x01C */ float mTimeElapsed;
    /* 0x020 */ u8 mUnknown20[0xC0];
    /* 0x0E0 */ ScreenMovement mMovement;
    /* 0x0E4 */ u32 mControllerDefaults[9];
    /* 0x108 */ bool mControllerReady[9];
    /* 0x111 */ u8 mPadding111[3];
}; // size 0x114

#endif // GAME_SH_SH_TITLE_SCREEN_H
