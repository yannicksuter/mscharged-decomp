#ifndef GAME_MII_MANAGER_INL
#define GAME_MII_MANAGER_INL

#include "Game/MiiManager.h"

inline unsigned long MiiManager::GetIconTextureId(int index)
{
    return mIconTextureIds[index];
}

inline MiiManager* MiiManager::Instance()
{
    return g_pMiiManager;
}

#endif // GAME_MII_MANAGER_INL
