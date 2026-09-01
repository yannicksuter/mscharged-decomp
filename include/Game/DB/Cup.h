#ifndef GAME_DB_CUP_H
#define GAME_DB_CUP_H

#include <string.h>

#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/UserOptions.h"
#include "types.h"

struct BaseCup
{
    /* 0x00 */ int mUserSelectedTeam;
    /* 0x04 */ int mUserSelectedSidekick[3];
    /* 0x10 */ int mRoundType;
    /* 0x14 */ s16 mRoundNumber;
    /* 0x16 */ s16 mGameNumber;
    /* 0x18 */ u16 mHumanTeams;
    /* 0x1A */ u16 unknown_0x1A;
    /* 0x1C */ GameplaySettings mCupSettings;

    virtual BasicGameInfo* GetGameInfo(int round, int matchup) = 0;
    virtual BasicGameInfo* GetGameInfo(int phase, int round, int matchup) = 0;
    virtual BasicGameInfo* GetGameInfo(int index) = 0;
    virtual TeamStats* GetTeamStats(int index) = 0;
    virtual TeamStats* GetPreviousTeamStats() = 0;
    virtual u16 GetNumTeams() = 0;
    virtual u16 GetNumGamesPerRound(int phase) = 0;
    virtual u16 GetNumRounds() = 0;
    virtual u16 GetNumRegularGames() = 0;
    virtual u16 GetNumPlayoffGames() = 0;
    virtual u16 GetFirstRoundNumber() = 0;
    virtual void Reset() = 0;
    virtual void* SerializeData(void* dst) const;
    virtual void* DeserializeData(void* src);
    virtual int GetSaveDataSize() const;
};

template <u16 Teams, u16 Rounds>
struct Cup : public BaseCup
{
    virtual BasicGameInfo* GetGameInfo(int round, int matchup);
    virtual BasicGameInfo* GetGameInfo(int phase, int round, int matchup);
    virtual BasicGameInfo* GetGameInfo(int index);
    virtual TeamStats* GetTeamStats(int index);
    virtual TeamStats* GetPreviousTeamStats();
    virtual u16 GetNumTeams();
    virtual u16 GetNumGamesPerRound(int phase);
    virtual u16 GetNumRounds();
    virtual u16 GetNumRegularGames();
    virtual u16 GetNumPlayoffGames();
    virtual u16 GetFirstRoundNumber();
    virtual void Reset();
    virtual void* SerializeData(void* dst) const;
    virtual void* DeserializeData(void* src);
    virtual int GetSaveDataSize() const;

    BasicGameInfo mGameInfo[Rounds][Teams / 2];
    TeamStats mTeamStats[Teams];
    TeamStats mPreviousTeamStats;
};

template <u16 Teams, u16 Rounds>
BasicGameInfo* Cup<Teams, Rounds>::GetGameInfo(int round, int matchup)
{
    return GetGameInfo(mRoundType, round, matchup);
}

template <u16 Teams, u16 Rounds>
BasicGameInfo* Cup<Teams, Rounds>::GetGameInfo(int index)
{
    return &mGameInfo[0][index];
}

template <u16 Teams, u16 Rounds>
TeamStats* Cup<Teams, Rounds>::GetTeamStats(int index)
{
    return &mTeamStats[index];
}

template <u16 Teams, u16 Rounds>
TeamStats* Cup<Teams, Rounds>::GetPreviousTeamStats()
{
    return &mPreviousTeamStats;
}

template <u16 Teams, u16 Rounds>
u16 Cup<Teams, Rounds>::GetNumTeams()
{
    return Teams;
}

template <u16 Teams, u16 Rounds>
void Cup<Teams, Rounds>::Reset()
{
    for (int i = 0; i < Rounds * (Teams / 2); i++)
    {
        mGameInfo[0][i].Reset(true);
    }
}

template <u16 Teams, u16 Rounds>
void* Cup<Teams, Rounds>::SerializeData(void* dst) const
{
    memcpy(dst, &mUserSelectedTeam, sizeof(mUserSelectedTeam));
    dst = (u8*)dst + sizeof(mUserSelectedTeam);
    memcpy(dst, mUserSelectedSidekick, sizeof(mUserSelectedSidekick));
    dst = (u8*)dst + sizeof(mUserSelectedSidekick);
    memcpy(dst, &mRoundType, sizeof(mRoundType));
    dst = (u8*)dst + sizeof(mRoundType);
    memcpy(dst, &mRoundNumber, sizeof(mRoundNumber));
    dst = (u8*)dst + sizeof(mRoundNumber);
    memcpy(dst, &mGameNumber, sizeof(mGameNumber));
    dst = (u8*)dst + sizeof(mGameNumber);
    memcpy(dst, &mHumanTeams, sizeof(mHumanTeams));
    dst = (u8*)dst + sizeof(mHumanTeams);
    memcpy(dst, mGameInfo, sizeof(mGameInfo));
    dst = (u8*)dst + sizeof(mGameInfo);
    memcpy(dst, mTeamStats, sizeof(mTeamStats));
    dst = (u8*)dst + sizeof(mTeamStats);
    memcpy(dst, &mPreviousTeamStats, sizeof(mPreviousTeamStats));
    return (u8*)dst + sizeof(mPreviousTeamStats);
}

template <u16 Teams, u16 Rounds>
void* Cup<Teams, Rounds>::DeserializeData(void* src)
{
    memcpy(&mUserSelectedTeam, src, sizeof(mUserSelectedTeam));
    src = (u8*)src + sizeof(mUserSelectedTeam);
    memcpy(mUserSelectedSidekick, src, sizeof(mUserSelectedSidekick));
    src = (u8*)src + sizeof(mUserSelectedSidekick);
    memcpy(&mRoundType, src, sizeof(mRoundType));
    src = (u8*)src + sizeof(mRoundType);
    memcpy(&mRoundNumber, src, sizeof(mRoundNumber));
    src = (u8*)src + sizeof(mRoundNumber);
    memcpy(&mGameNumber, src, sizeof(mGameNumber));
    src = (u8*)src + sizeof(mGameNumber);
    memcpy(&mHumanTeams, src, sizeof(mHumanTeams));
    src = (u8*)src + sizeof(mHumanTeams);
    memcpy(mGameInfo, src, sizeof(mGameInfo));
    src = (u8*)src + sizeof(mGameInfo);
    memcpy(mTeamStats, src, sizeof(mTeamStats));
    src = (u8*)src + sizeof(mTeamStats);
    memcpy(&mPreviousTeamStats, src, sizeof(mPreviousTeamStats));
    return (u8*)src + sizeof(mPreviousTeamStats);
}

template <u16 Teams, u16 Rounds>
int Cup<Teams, Rounds>::GetSaveDataSize() const
{
    return 0x1A + sizeof(mGameInfo) + sizeof(mTeamStats) + sizeof(mPreviousTeamStats);
}

#endif // GAME_DB_CUP_H
