#ifndef GAME_MII_MANAGER_INL
#define GAME_MII_MANAGER_INL

#include "Game/MiiManager.h"

inline MiiManager* MiiManager::Instance()
{
    return MiiManager::s_pInstance;
}

#endif // GAME_MII_MANAGER_INL
