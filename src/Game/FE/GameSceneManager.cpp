#include "Game/GameSceneManager.h"

template <>
GameSceneManager* nlSingleton<GameSceneManager>::s_pInstance = 0;

GameSceneManager::GameSceneManager()
{
}

GameSceneManager::~GameSceneManager()
{
}

void GameSceneManager::Pop()
{
    BaseGameSceneManager::Pop();
}
