#ifndef GAME_GAMEINFO_INL
#define GAME_GAMEINFO_INL

#include "Game/GameInfo.h"
#include "Game/Render/RLViewLayers.h"

inline bool UserInfo::IsWidescreen() const
{
    return ::IsWidescreen();
}

inline UserInfo& GameInfoManager::GetUserInfo()
{
    return mUserInfo;
}

#endif
