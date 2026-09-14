#ifndef GAME_GAME_SCENE_MANAGER_H
#define GAME_GAME_SCENE_MANAGER_H

#include "Game/BaseGameSceneManager.h"
#include "NL/nlSingleton.h"

class GameSceneManager : public BaseGameSceneManager, public nlSingleton<GameSceneManager>
{
public:
    GameSceneManager();
    virtual ~GameSceneManager();
    virtual void Pop();
};

inline GameSceneManager* fn_801CA660()
{
    return nlSingleton<GameSceneManager>::s_pInstance;
}

#endif // GAME_GAME_SCENE_MANAGER_H
