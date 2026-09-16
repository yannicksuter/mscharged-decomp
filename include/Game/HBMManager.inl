#ifndef GAME_HBM_MANAGER_INL
#define GAME_HBM_MANAGER_INL

#include "Game/HBMManager.h"

inline void HBMManager::Unblock()
{
    mBlocked = false;
}

inline HBMManager* fn_801FA19C()
{
    return gpHBMManager;
}

#endif // GAME_HBM_MANAGER_INL
