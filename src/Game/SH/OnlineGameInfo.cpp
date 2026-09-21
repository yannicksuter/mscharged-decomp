#include "Game/GameInfo.h"
#include "Game/FriendManager.h"
#include "NL/nlPrint.h"

void* gOnlineFriendSelectionContext;

void SetOnlineRankedMatch(bool value)
{
    GameInfoManager::Instance()->mOnlineRankedMatch = value;
}

bool IsOnlineRankedMatch()
{
    return GameInfoManager::Instance()->mOnlineRankedMatch;
}

void SetOnlineTwoLocalPlayers(bool value)
{
    GameInfoManager::Instance()->mOnlineTwoLocalPlayers = value;
}

bool HasOnlineTwoLocalPlayers()
{
    return GameInfoManager::Instance()->mOnlineTwoLocalPlayers;
}

void SetOnlineFriendSelectionMode(bool value)
{
    GameInfoManager::Instance()->mOnlineFriendSelectionMode = value;
}

bool IsOnlineFriendSelectionMode()
{
    return GameInfoManager::Instance()->mOnlineFriendSelectionMode;
}

void SetOnlineFriendSelectionContext(void* context)
{
    gOnlineFriendSelectionContext = context;
}

void FormatFriendKey(unsigned long long friendKey, u16* output)
{
    if (friendKey != 0)
    {
        nlSNPrintf(output,
            14,
            (const unsigned short*)L"%.6llu %.6llu",
            friendKey / 1000000,
            friendKey % 1000000);
    }
    else
    {
        nlSNPrintf(output, 14, (const unsigned short*)L"UNKNOWN");
    }
}
