#ifndef GAME_SH_SH_CHOOSE_CAPTAINS_H
#define GAME_SH_SH_CHOOSE_CAPTAINS_H

#include "Game/BaseGameSceneManager.h"

class ChooseCaptainsSceneV2 : public BaseSceneHandler
{
public:
    enum SceneType
    {
        ST_DOMINATION = 0,
        ST_STRIKER_CUP = 1,
    };

    ChooseCaptainsSceneV2(SceneType sceneType, ScreenMovement movement);
    virtual ~ChooseCaptainsSceneV2();
    virtual void Update(float dt);
    virtual void SceneCreated();

    /* 0x01C */ u8 mUnknown1C;
    /* 0x01D */ u8 mPadding1D[3];
    /* 0x020 */ SceneType mSceneType;
    /* 0x024 */ ScreenMovement mMovement;
    /* 0x028 */ u8 mUnknown28[0x135C];
}; // size 0x1384

class ChooseSidekicksSceneV2 : public BaseSceneHandler
{
public:
    ChooseSidekicksSceneV2(ChooseCaptainsSceneV2::SceneType sceneType, ScreenMovement movement);
    virtual ~ChooseSidekicksSceneV2();
    virtual void Update(float dt);
    virtual void SceneCreated();

    /* 0x01C */ u8 mUnknown1C[0x1C];
    /* 0x038 */ ScreenMovement mMovement;
    /* 0x03C */ ChooseCaptainsSceneV2::SceneType mSceneType;
    /* 0x040 */ u8 mUnknown40[0x19BC];
}; // size 0x19FC

#endif // GAME_SH_SH_CHOOSE_CAPTAINS_H
