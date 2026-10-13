#ifndef GAME_RENDER_NPCMANAGER_INL
#define GAME_RENDER_NPCMANAGER_INL

#include "Game/Render/NPCManager.h"

inline NPCManager* NPCManager::Instance()
{
    return gNPCManager;
}

#endif // GAME_RENDER_NPCMANAGER_INL
