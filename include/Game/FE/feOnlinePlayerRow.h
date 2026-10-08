#ifndef GAME_FE_ONLINE_PLAYER_ROW_H
#define GAME_FE_ONLINE_PLAYER_ROW_H

#include "Game/NetworkStats.h"
#include <string.h>

class TLComponentInstance;

enum eOnlineRowSearch
{
    ONLINE_ROW_SEARCHING = 0,
    ONLINE_ROW_CONNECTING = 1,
    ONLINE_ROW_INVITE = 2,
    ONLINE_ROW_ADD = 3,
    ONLINE_ROW_SEARCH_OFF = 4,
};

enum eOnlineRowStatus
{
    ONLINE_ROW_WAIT_FRIEND = 0,
    ONLINE_ROW_WAIT_OPPONENT = 1,
    ONLINE_ROW_AVAILABLE = 2,
    ONLINE_ROW_BUSY = 3,
    ONLINE_ROW_CHOOSE_CAPTAIN = 4,
    ONLINE_ROW_CHOOSE_SIDEKICKS = 5,
    ONLINE_ROW_CAPTAIN_NAMES = 6,
    ONLINE_ROW_ESTABLISHING = 7,
    ONLINE_ROW_OFFLINE = 8,
    ONLINE_ROW_DECLINED = 9,
    ONLINE_ROW_INVITING = 10,
    ONLINE_ROW_EMPTY = 11,
};

enum eOnlineRowSide
{
    ONLINE_ROW_NO_SIDE = 0,
    ONLINE_ROW_HOME = 1,
    ONLINE_ROW_AWAY = 2,
    ONLINE_ROW_GUEST = 3,
};

struct FEOnlinePlayerRow
{
    FEOnlinePlayerRow();
    void Reset();

    /* 0x00 */ u16 mName[14];
    /* 0x1C */ u8 mMiiData[0x4C];
    /* 0x68 */ int mFriendIndex;
    /* 0x6C */ int mSearchState;
    /* 0x70 */ int mStatus;
    /* 0x74 */ NetworkRankingMeta mStats;
    /* 0x8C */ int mCaptain;
    /* 0x90 */ int mSide;
    /* 0x94 */ bool mVisible;
    /* 0x95 */ bool mGuest;
    /* 0x96 */ bool mShowCancel;
}; // size 0x98

const char* GetOnlineCaptainSlideName(unsigned int captain);
void UpdateOnlinePlayerRow(FEOnlinePlayerRow* row, TLComponentInstance* instance,
    u16* name, int nameSize, u16* description, int descriptionSize, int index, bool value);

inline FEOnlinePlayerRow::FEOnlinePlayerRow()
{
    Reset();
}

inline void FEOnlinePlayerRow::Reset()
{
    mSearchState = ONLINE_ROW_SEARCH_OFF;
    mStatus = ONLINE_ROW_EMPTY;
    mCaptain = -1;
    mVisible = false;
    mSide = ONLINE_ROW_NO_SIDE;
    mGuest = false;
    mShowCancel = false;
    mName[0] = 0;
    memset(mMiiData, 0, sizeof(mMiiData));
    mFriendIndex = -1;
    mStats.mScore = 0;
    mStats.mDisplayRank = 0;
    mStats.mWins = 0;
    mStats.mLosses = 0;
    mStats.mOnlineRegion = 0;
}

#endif // GAME_FE_ONLINE_PLAYER_ROW_H
