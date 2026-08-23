#ifndef GAME_BASE_GAME_SCENE_MANAGER_H
#define GAME_BASE_GAME_SCENE_MANAGER_H

#include "Game/BaseSceneHandler.h"

enum SceneList
{
    SCENE_INVALID = -2,
    SCENE_TITLE = 0,
    SCENE_MAIN_MENU = 1,
};

enum ScreenMovement
{
    SCREEN_NOTHING = 0,
    SCREEN_FORWARD = 1,
    SCREEN_BACK = 2,
};

class BaseGameSceneManager
{
public:
    BaseGameSceneManager();
    virtual ~BaseGameSceneManager();
    virtual BaseSceneHandler* Push(SceneList scene, ScreenMovement movement, bool popFirst);
    BaseSceneHandler* GetScene(SceneList scene);
    virtual void Pop();
    void PopEntireStack();
    int GetSceneType(BaseSceneHandler* handler);
    bool IsOnStack(SceneList scene);
    void PushLoadingScene(bool push);

    static const u32 MAX_SCENE_DEPTH = 32;

    /* 0x04 */ u32 mCurrentStackDepth;
    /* 0x08 */ SceneList mSceneStack[MAX_SCENE_DEPTH];
    /* 0x88 */ BaseSceneHandler* mSceneHandlerStack[MAX_SCENE_DEPTH];
}; // size 0x108

#endif // GAME_BASE_GAME_SCENE_MANAGER_H
