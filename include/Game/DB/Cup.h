#ifndef GAME_DB_CUP_H
#define GAME_DB_CUP_H

#include <string.h>

#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/UserOptions.h"
#include "types.h"

struct BaseCup
{
    bool IsHumanTeam(eTeamID team) const { return (mHumanTeams & (1 << team)) != 0; }

    BaseCup()
        : mUserSelectedTeam(-1)
        , mUserSelectedSidekick()
        , mRoundType(0)
        , mRoundNumber(0)
        , mGameNumber(0)
        , mHumanTeams(0)
    {
    }

    /* 0x00 */ int mUserSelectedTeam;
    /* 0x04 */ CupSidekicks mUserSelectedSidekick;
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
    virtual u16 GetNumRounds(int phase) = 0;
    virtual u16 GetNumRounds() = 0;
    virtual u16 GetNumRegularRounds() = 0;
    virtual u16 GetNumPlayoffRounds() = 0;
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
    virtual u16 GetNumRounds(int phase);
    virtual u16 GetNumRounds();
    virtual u16 GetNumRegularRounds();
    virtual u16 GetNumPlayoffRounds();
    virtual u16 GetFirstRoundNumber();
    virtual void Reset();
    virtual void* SerializeData(void* dst) const;
    virtual void* DeserializeData(void* src);
    virtual int GetSaveDataSize() const;

    BasicGameInfo mGameInfo[Rounds][Teams / 2];
    TeamStats mTeamStats[Teams];
    TeamStats mPreviousTeamStats;
};

#endif // GAME_DB_CUP_H
