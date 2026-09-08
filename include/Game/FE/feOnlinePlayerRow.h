#ifndef GAME_FE_ONLINE_PLAYER_ROW_H
#define GAME_FE_ONLINE_PLAYER_ROW_H

#include "Game/NetworkStats.h"
#include <string.h>

class TLComponentInstance;

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
    mSearchState = 4;
    mStatus = 11;
    mCaptain = -1;
    mVisible = false;
    mSide = 0;
    mGuest = false;
    mShowCancel = false;
    mName[0] = 0;
    memset(mMiiData, 0, sizeof(mMiiData));
    mFriendIndex = -1;
    mStats.mScore = 0;
    mStats.mDisplayRank = 0;
    mStats.mWins = 0;
    mStats.mLosses = 0;
    mStats.mUnidentified14 = 0;
}

#endif // GAME_FE_ONLINE_PLAYER_ROW_H
