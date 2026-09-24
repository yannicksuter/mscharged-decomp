#ifndef GAME_GAMEINFO_INL
#define GAME_GAMEINFO_INL

#include "Game/GameInfo.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include "Game/FriendManager.h"
#include "Game/GameSceneManager.h"

inline bool UserInfo::IsWidescreen() const
{
    return ::IsWidescreen();
}

inline UserInfo& GameInfoManager::GetUserInfo()
{
    return mUserInfo;
}

inline int GameInfoManager::fn_801CA658() const
{
    return mCurrentMode;
}

inline GameSceneManager* fn_801CA660()
{
    return nlSingleton<GameSceneManager>::s_pInstance;
}

inline BaseGameSceneManager* GetOverlayManager()
{
    return g_pOverlayManager;
}

inline StrikerChallenge* fn_801CA670()
{
    return g_pStrikerChallenge;
}

inline FriendManager* GetFriendManager()
{
    return g_pFriendManager;
}

#endif
