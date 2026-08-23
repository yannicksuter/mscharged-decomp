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

#endif // GAME_GAME_SCENE_MANAGER_H
