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
    virtual u16 GetNumRoundsForType(int phase) = 0;
    virtual u16 GetNumRounds() = 0;
    virtual u16 GetNumRegularRounds() = 0;
    virtual u16 GetNumPlayoffRounds() = 0;
    virtual u16 GetFirstRoundNumber() = 0;
    virtual void Reset() = 0;
    virtual void* SerializeData(void* dst) const
    {
        memcpy(dst, &mUserSelectedTeam, sizeof(mUserSelectedTeam));
        dst = (u8*)dst + sizeof(mUserSelectedTeam);
        memcpy(dst, &mUserSelectedSidekick, sizeof(mUserSelectedSidekick));
        dst = (u8*)dst + sizeof(mUserSelectedSidekick);
        memcpy(dst, &mRoundType, sizeof(mRoundType));
        dst = (u8*)dst + sizeof(mRoundType);
        memcpy(dst, &mRoundNumber, sizeof(mRoundNumber));
        dst = (u8*)dst + sizeof(mRoundNumber);
        memcpy(dst, &mGameNumber, sizeof(mGameNumber));
        dst = (u8*)dst + sizeof(mGameNumber);
        memcpy(dst, &mHumanTeams, sizeof(mHumanTeams));
        return (u8*)dst + sizeof(mHumanTeams);
    }
    virtual void* DeserializeData(void* src)
    {
        memcpy(&mUserSelectedTeam, src, sizeof(mUserSelectedTeam));
        src = (u8*)src + sizeof(mUserSelectedTeam);
        memcpy(&mUserSelectedSidekick, src, sizeof(mUserSelectedSidekick));
        src = (u8*)src + sizeof(mUserSelectedSidekick);
        memcpy(&mRoundType, src, sizeof(mRoundType));
        src = (u8*)src + sizeof(mRoundType);
        memcpy(&mRoundNumber, src, sizeof(mRoundNumber));
        src = (u8*)src + sizeof(mRoundNumber);
        memcpy(&mGameNumber, src, sizeof(mGameNumber));
        src = (u8*)src + sizeof(mGameNumber);
        memcpy(&mHumanTeams, src, sizeof(mHumanTeams));
        return (u8*)src + sizeof(mHumanTeams);
    }
    virtual int GetSaveDataSize() const
    {
        return 0x1A;
    }
};

template <u16 Teams, u16 Rounds>
struct Cup : public BaseCup
{
    virtual BasicGameInfo* GetGameInfo(int round, int matchup)
    {
        return GetGameInfo(mRoundType, round, matchup);
    }
    virtual BasicGameInfo* GetGameInfo(int index)
    {
        return &mGameInfo[0][index];
    }
    virtual BasicGameInfo* GetGameInfo(int phase, int round, int matchup)
    {
        BasicGameInfo* result = 0;
        switch (phase)
        {
        case 0:
        {
            int index = matchup + round * Teams / 2;
            result = &mGameInfo[0][index];
            break;
        }
        case 1:
        {
            int index = (Rounds - 2) * (Teams / 2) + Teams - 4;
            if (round != (Teams == 4 ? 0 : Teams == 6 ? 1 : 2))
            {
                if (round == (Teams == 4 ? -1 : Teams == 6 ? 0 : 1))
                {
                    index = (Rounds - 2) * (Teams / 2) + Teams - 6 + matchup;
                }
                else if (round == (Teams == 4 ? -2 : Teams == 6 ? -1 : 0))
                {
                    index = (Rounds - 2) * (Teams / 2) + Teams - 10 + matchup;
                }
            }
            result = &mGameInfo[0][index];
            break;
        }
        case 2:
        {
            int index = Rounds * (Teams / 2) - (3 - round);
            result = &mGameInfo[0][index];
            break;
        }
        }
        return result;
    }
    virtual TeamStats* GetTeamStats(int index)
    {
        return &mTeamStats[index];
    }
    virtual TeamStats* GetPreviousTeamStats()
    {
        return &mPreviousTeamStats;
    }
    virtual u16 GetNumTeams()
    {
        return Teams;
    }
    virtual u16 GetNumRoundsForType(int phase)
    {
        if (phase == 0)
        {
            return Rounds - 2;
        }
        if (phase == 1)
        {
            return Teams == 4 ? 1 : Teams == 6 ? 2 : 3;
        }
        if (phase == 2)
        {
            return 3;
        }
        return Rounds - 2;
    }
    virtual u16 GetNumRegularRounds()
    {
        return Rounds - 2;
    }
    virtual u16 GetNumPlayoffRounds()
    {
        return Teams == 4 ? 1 : Teams == 6 ? 2 : 3;
    }
    virtual u16 GetNumRounds()
    {
        return Rounds + (Teams == 4 ? 2 : Teams == 6 ? 3 : 4);
    }
    virtual u16 GetFirstRoundNumber()
    {
        return 0;
    }
    virtual void Reset()
    {
        for (int i = 0; i < Rounds * (Teams / 2); i++)
        {
            mGameInfo[0][i].Reset(true);
        }
    }
    virtual void* SerializeData(void* dst) const
    {
        dst = BaseCup::SerializeData(dst);
        memcpy(dst, mGameInfo, sizeof(mGameInfo));
        dst = (u8*)dst + sizeof(mGameInfo);
        memcpy(dst, mTeamStats, sizeof(mTeamStats));
        dst = (u8*)dst + sizeof(mTeamStats);
        memcpy(dst, &mPreviousTeamStats, sizeof(mPreviousTeamStats));
        return (u8*)dst + sizeof(mPreviousTeamStats);
    }
    virtual void* DeserializeData(void* src)
    {
        src = BaseCup::DeserializeData(src);
        memcpy(mGameInfo, src, sizeof(mGameInfo));
        src = (u8*)src + sizeof(mGameInfo);
        memcpy(mTeamStats, src, sizeof(mTeamStats));
        src = (u8*)src + sizeof(mTeamStats);
        memcpy(&mPreviousTeamStats, src, sizeof(mPreviousTeamStats));
        return (u8*)src + sizeof(mPreviousTeamStats);
    }
    virtual int GetSaveDataSize() const
    {
        return BaseCup::GetSaveDataSize() + sizeof(mGameInfo) + sizeof(mTeamStats) + sizeof(mPreviousTeamStats);
    }

    BasicGameInfo mGameInfo[Rounds][Teams / 2];
    TeamStats mTeamStats[Teams];
    TeamStats mPreviousTeamStats;
};

#endif // GAME_DB_CUP_H
