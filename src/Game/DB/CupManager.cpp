#include "Game/DB/GameProgress.h"
#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHCupNews.h"
#include "Game/Team.h"
#include "NL/nlMath.h"
#include "revolution/os/OSTime_fwd.h"
#include <string.h>

struct CupMatchup
{
    int mHome;
    int mAway;
};

const CupMatchup FOUR_TEAM_MATCHUPS[12] = {
    { 0, 1 },
    { 2, 3 },
    { 3, 1 },
    { 2, 0 },
    { 0, 3 },
    { 1, 2 },
    { 3, 2 },
    { 1, 0 },
    { 0, 2 },
    { 1, 3 },
    { 2, 1 },
    { 3, 0 },
};

const CupMatchup SIX_TEAM_MATCHUPS[30] = {
    { 0, 1 },
    { 5, 4 },
    { 3, 2 },
    { 3, 5 },
    { 1, 4 },
    { 2, 0 },
    { 4, 2 },
    { 0, 3 },
    { 5, 1 },
    { 1, 3 },
    { 5, 2 },
    { 4, 0 },
    { 0, 5 },
    { 2, 1 },
    { 3, 4 },
    { 1, 0 },
    { 4, 5 },
    { 2, 3 },
    { 5, 3 },
    { 4, 1 },
    { 0, 2 },
    { 2, 4 },
    { 3, 0 },
    { 1, 5 },
    { 3, 1 },
    { 2, 5 },
    { 0, 4 },
    { 5, 0 },
    { 1, 2 },
    { 4, 3 },
};

const CupMatchup TEN_TEAM_MATCHUPS[45] = {
    { 1, 5 },
    { 9, 0 },
    { 4, 8 },
    { 7, 3 },
    { 2, 6 },
    { 0, 5 },
    { 7, 2 },
    { 8, 1 },
    { 3, 4 },
    { 9, 6 },
    { 2, 4 },
    { 8, 5 },
    { 3, 9 },
    { 1, 7 },
    { 6, 0 },
    { 0, 8 },
    { 4, 1 },
    { 3, 6 },
    { 5, 2 },
    { 7, 9 },
    { 9, 2 },
    { 8, 3 },
    { 7, 6 },
    { 0, 1 },
    { 4, 5 },
    { 8, 7 },
    { 5, 9 },
    { 2, 0 },
    { 6, 4 },
    { 1, 3 },
    { 4, 7 },
    { 2, 8 },
    { 1, 9 },
    { 6, 5 },
    { 0, 3 },
    { 5, 3 },
    { 9, 4 },
    { 0, 7 },
    { 8, 6 },
    { 2, 1 },
    { 6, 1 },
    { 3, 2 },
    { 5, 7 },
    { 9, 8 },
    { 4, 0 },
};

const int gFireCupStadiums[6] = { 13, 14, 4, 11, 15, 5 };

const int gCrystalCupStadiums[7] = { 13, 14, 4, 7, 11, 15, 5 };

const int gStrikerCupStadiums[8] = { 13, 14, 4, 7, 3, 11, 15, 5 };

const int gCommonCupStadiums[7] = { 0, 1, 2, 6, 8, 10, 12 };

const int gCupSkillLevelCounts[6][4] = {
    { 0, 0, 0, 0 },
    { 2, 0, 0, 0 },
    { 1, 2, 0, 0 },
    { 0, 3, 2, 0 },
    { 0, 0, 6, 0 },
    { 0, 0, 1, 0 },
};

const int gPlayoffSkillLevels[4][3] = {
    { 2, 2, 2 },
    { 3, 3, 3 },
    { 4, 4, 5 },
    { 0, 0, 0 },
};

const int gFinalOpponentSkillLevels[3] = { 2, 4, 5 };

CupManager* g_pCupManager;

static inline CupSidekicks GetRandomCupSidekicks()
{
    CupSidekicks sidekicks;
    for (int i = 0; i < 3; i++)
    {
        sidekicks.mValues[i] = (eSidekickID)nlRandom(8, &nlDefaultSeed);
    }
    return sidekicks;
}

static inline bool IsInStadiumGroup(int stadium)
{
    for (int i = 0; i < 7; i++)
    {
        if (stadium == gCommonCupStadiums[i])
        {
            return true;
        }
    }
    return false;
}

inline void CupManager::IncreaseRoundNumber()
{
    int previousType = mCurrentCup->mRoundType;
    int nextType = -1;
    mCurrentCup->mRoundNumber = GetNextRoundNumber(&nextType);
    mCurrentCup->mRoundType = nextType;
    if (previousType != 1 && nextType == 1)
    {
        SetupPlayoffSchedule();
        if (mState == 16)
        {
            mCurrentCup->mRoundNumber = -5;
        }
    }
    mCurrentCup->mGameNumber = 0;
}

inline void CupManager::IncreaseGameNumber(bool shouldIncreaseRound)
{
    mCurrentCup->mGameNumber++;
    int numGames = GetNumGamesPerRound(mCurrentCup->mRoundType, mCurrentCup->mRoundNumber);
    if (mCurrentCup->mGameNumber == numGames && shouldIncreaseRound)
    {
        IncreaseRoundNumber();
    }
}

inline void CupManager::SetUserSelectedCupTeam(int team)
{
    mCurrentCup->mUserSelectedTeam = team;
    if (team != -1)
    {
        mCurrentCup->mHumanTeams = 0;
        mCurrentCup->mHumanTeams |= 1 << team;
    }
}

inline void CupManager::SetUserSelectedCupSidekicks(CupSidekicks sidekicks)
{
    mCurrentCup->mUserSelectedSidekick = sidekicks;
}

CupManager::CupManager()
    : mForceHighestSkillLevel(false)
    , mCurrentMode(-1)
    , mCurrentCup(0)
    , mShowCupPhasePopup(false)
    , mPendingCupTeam(-1)
    , mPreGameUnlockedState(0)
{
    mFireCupSeries.mRoundNumber = -6;
    mCrystalCupSeries.mRoundNumber = -6;
    mStrikerCupSeries.mRoundNumber = -6;
    mPreviousGameTeams[0] = -1;
    mPreviousGameTeams[1] = -1;
}

CupManager::~CupManager()
{
}

u16 CupManager::GetNumPlayingTeams() const
{
    return mCurrentCup->GetNumTeams();
}

TeamStats CupManager::GetTeamStatsByIndex(u16 index)
{
    if (index == GetNumPlayingTeams())
    {
        return *mCurrentCup->GetPreviousTeamStats();
    }
    return *mCurrentCup->GetTeamStats(index);
}

TeamStats* CupManager::pGetTeamStatsByIndex(u16 index) const
{
    if (index == mCurrentCup->GetNumTeams())
    {
        return mCurrentCup->GetPreviousTeamStats();
    }
    return mCurrentCup->GetTeamStats(index);
}

TeamStats CupManager::GetTeamStats(int team)
{
    TeamStats result;
    result.Initialize((eTeamID)team);
    if (team == mCurrentCup->GetPreviousTeamStats()->mTeamIndex)
    {
        return *mCurrentCup->GetPreviousTeamStats();
    }
    for (int i = 0; i < mCurrentCup->GetNumTeams(); i++)
    {
        TeamStats stats = GetTeamStatsByIndex(i);
        if (stats.mTeamIndex == team)
        {
            result = stats;
            break;
        }
    }
    return result;
}

BasicGameInfo* CupManager::GetMatchupInfo(int phase, short round, int matchup) const
{
    return mCurrentCup->GetGameInfo(phase, round, matchup);
}

BasicGameInfo* CupManager::GetGameInfo(int phase, int matchup)
{
    if (phase == 1)
    {
        matchup += GetNumGames(0);
    }
    else if (phase == 2)
    {
        matchup += GetNumGames(0);
        matchup += GetNumGames(1);
    }
    return mCurrentCup->GetGameInfo(matchup);
}

bool CupManager::HasGameBeenPlayed(int phase, int matchup)
{
    BasicGameInfo* gameInfo = GetGameInfo(phase, matchup);
    if (gameInfo->mFinalScore[0] != 0 || gameInfo->mFinalScore[1] != 0)
    {
        return true;
    }
    return false;
}

BasicGameInfo* CupManager::GetCurrentGameInfo()
{
    return mCurrentCup->GetGameInfo(mCurrentCup->mRoundNumber, mCurrentCup->mGameNumber);
}

eTeamID CupManager::GetUserSelectedCupTeam() const
{
    return (eTeamID)mCurrentCup->mUserSelectedTeam;
}

int CupManager::GetFinalOpponentTeam() const
{
    return mCurrentCup->GetPreviousTeamStats()->mTeamIndex;
}

int CupManager::GetCurrentRoundType() const
{
    return mCurrentCup->mRoundType;
}

s16 CupManager::GetCurrentRoundNumber() const
{
    return mCurrentCup->mRoundNumber;
}

s16 CupManager::GetNextRoundNumber(int* roundType)
{
    int currentType = mCurrentCup->mRoundType;
    s16 currentRound = mCurrentCup->mRoundNumber;
    int numRounds = mCurrentCup->GetNumRoundsForType(currentType);
    if (currentRound == -5)
    {
        *roundType = currentType;
        return -5;
    }
    if (currentRound < numRounds - 1)
    {
        *roundType = currentType;
        return currentRound + 1;
    }

    s16 nextRound = 0;
    if (currentType == 0)
    {
        *roundType = 1;
    }
    else if (currentType == 1)
    {
        *roundType = 2;
    }
    else if (currentType == 2)
    {
        nextRound = -5;
    }
    return nextRound;
}

u16 CupManager::GetNumGamesPerRound(int phase, int round) const
{
    u16 returnValue;
    if (phase == 1)
    {
        u16 numRounds = mCurrentCup->GetNumPlayoffRounds();
        if (round == numRounds - 1)
        {
            returnValue = 1;
        }
        else if (round == numRounds - 2)
        {
            returnValue = 2;
        }
        else if (round == numRounds - 3)
        {
            returnValue = 4;
        }
    }
    else if (phase == 0)
    {
        returnValue = mCurrentCup->GetNumTeams() >> 1;
    }
    else if (phase == 2)
    {
        returnValue = 1;
    }
    return returnValue;
}

u16 CupManager::GetNumGames(int phase) const
{
    int result;
    if (phase == 0)
    {
        int rounds = mCurrentCup->GetNumRegularRounds();
        result = GetNumGamesPerRound(phase, 0);
        result *= rounds;
    }
    else if (phase == 1)
    {
        int rounds = mCurrentCup->GetNumPlayoffRounds();
        if (rounds == 1)
        {
            result = 1;
        }
        else if (rounds == 2)
        {
            result = 3;
        }
        else
        {
            result = 7;
        }
    }
    else
    {
        result = 1;
    }
    return result;
}

int CupManager::PickStadium(bool isLastRound) const
{
    int mode = mCurrentMode;
    const int* stadiums = 0;
    int count = 0;
    if (mode == 0)
    {
        if (isLastRound)
        {
            return 7;
        }
        stadiums = gFireCupStadiums;
        count = 6;
    }
    else if (mode == 1)
    {
        if (isLastRound)
        {
            return 3;
        }
        stadiums = gCrystalCupStadiums;
        count = 7;
    }
    else if (mode == 2)
    {
        if (isLastRound)
        {
            return 9;
        }
        stadiums = gStrikerCupStadiums;
        count = 8;
    }

    int index = nlRandom(count + 1, &nlDefaultSeed);
    if (index < count)
    {
        return stadiums[index];
    }
    return gCommonCupStadiums[nlRandom(7, &nlDefaultSeed)];
}

void CupManager::PickRoundStadiums(int* stadiums)
{
    const int* choices = 0;
    int count = 0;
    int rounds = mCurrentCup->GetNumRegularRounds();
    int first = 0;
    if (mCurrentMode == 0)
    {
        choices = gFireCupStadiums;
        count = 6;
    }
    else if (mCurrentMode == 1)
    {
        choices = gCrystalCupStadiums;
        count = 7;
    }
    else if (mCurrentMode == 2)
    {
        choices = gStrikerCupStadiums;
        count = 8;
    }

    for (int i = 0; i < rounds; i++)
    {
        if (i % count == 0)
        {
            first = i;
        }
        for (;;)
        {
            bool available = true;
            int index = nlRandom(count + 1, &nlDefaultSeed);
            if (index < count)
            {
                int stadium = choices[index];
                for (int j = first; j < i; j++)
                {
                    if (stadium == stadiums[j])
                    {
                        available = false;
                        break;
                    }
                }
                if (available == true)
                {
                    stadiums[i] = stadium;
                    break;
                }
            }
            else
            {
                for (int j = first; j < i; j++)
                {
                    if (IsInStadiumGroup(stadiums[j]))
                    {
                        available = false;
                        break;
                    }
                }
                if (available == true)
                {
                    stadiums[i] = gCommonCupStadiums[nlRandom(7, &nlDefaultSeed)];
                    break;
                }
            }
        }
    }
}

void CupManager::SetupRoundRobinSchedule(int* lineup, CupSidekicks* sklineup)
{
    int numplayingteams = mCurrentCup->GetNumTeams();
    int numRounds = mCurrentCup->GetNumRegularRounds();
    int numGamesPerRound = GetNumGamesPerRound(0, 0);
    mState = 15;
    BasicGameInfo* g;
    int home;
    int away;
    int stadiums[10];
    PickRoundStadiums(stadiums);
    mCurrentCup->mRoundNumber = 0;
    mCurrentCup->mGameNumber = 0;
    mCurrentCup->mRoundType = 0;
    mCurrentCup->Reset();
    for (int round = 0; round < numRounds; round++)
    {
        for (int game = 0; game < numGamesPerRound; game++)
        {
            int index = round * numGamesPerRound + game;
            switch (mCurrentCup->GetNumTeams())
            {
            case 4:
                home = FOUR_TEAM_MATCHUPS[index].mHome;
                away = FOUR_TEAM_MATCHUPS[index].mAway;
                break;
            case 6:
                home = SIX_TEAM_MATCHUPS[index].mHome;
                away = SIX_TEAM_MATCHUPS[index].mAway;
                break;
            case 10:
                home = TEN_TEAM_MATCHUPS[index].mHome;
                away = TEN_TEAM_MATCHUPS[index].mAway;
                break;
            }
            g = mCurrentCup->GetGameInfo(0, round, game);
            g->mTeamIndex[0] = lineup[home];
            g->mTeamIndex[1] = lineup[away];
            for (int i = 0; i < 3; i++)
            {
                g->SetSidekick(0, sklineup[home].mValues[i], i);
                g->SetSidekick(1, sklineup[away].mValues[i], i);
            }
            int stadium;
            if (mCurrentCup->IsHumanTeam(g->GetTeam(0)) || mCurrentCup->IsHumanTeam(g->GetTeam(1)))
            {
                stadium = stadiums[round];
            }
            else
            {
                stadium = PickStadium(false);
            }
            g->mStadiumIndex = stadium;
        }
    }
    TeamStats* stats = mCurrentCup->GetTeamStats(0);
    for (int i = 0; i < numplayingteams; i++)
    {
        stats[i].Initialize((eTeamID)lineup[i]);
        stats[i].SetSidekicks(sklineup[i]);
    }
    AssignTeamSkillLevels();
}

void CupManager::AssignTeamSkillLevels()
{
    TeamStats* stats = mCurrentCup->GetTeamStats(0);
    int counts[6];
    for (int i = 0; i < 6; i++)
    {
        counts[i] = 0;
    }
    for (int i = 0; i < mCurrentCup->GetNumTeams(); i++)
    {
        if (stats[i].mTeamIndex != GetUserSelectedCupTeam())
        {
            int skillLevel;
            do
            {
                skillLevel = nlRandom(6, &nlDefaultSeed);
            } while (counts[skillLevel] >= gCupSkillLevelCounts[skillLevel][mCurrentMode]);
            stats[i].mSkillLevel = skillLevel;
            counts[skillLevel]++;
        }
    }
}

void CupManager::SetupPlayoffSchedule()
{
    int indices[10];
    StatsTracker::Instance()->GetSortedTeamStats(mCurrentCup->GetTeamStats(0), mCurrentCup->GetNumTeams(), indices, mCurrentCup->GetNumTeams());
    bool human = false;
    u16 playoffTeams = GetNumPlayoffTeams();
    for (int i = 0; i < GetNumGamesPerRound(1, 0); i++)
    {
        BasicGameInfo* info = mCurrentCup->GetGameInfo(1, 0, i);
        TeamStats* home = mCurrentCup->GetTeamStats(indices[i]);
        TeamStats* away = mCurrentCup->GetTeamStats(indices[playoffTeams - i - 1]);
        int homeTeam = home->mTeamIndex;
        int awayTeam = away->mTeamIndex;
        info->mTeamIndex[0] = homeTeam;
        info->mTeamIndex[1] = awayTeam;
        for (int sidekick = 0; sidekick < 3; sidekick++)
        {
            info->SetSidekick(0, home->mSidekicks.mValues[sidekick], sidekick);
            info->SetSidekick(1, away->mSidekicks.mValues[sidekick], sidekick);
        }
        info->mStadiumIndex = PickStadium(false);
        if (mCurrentCup->IsHumanTeam(info->GetTeam(0)) || mCurrentCup->IsHumanTeam(info->GetTeam(1)))
        {
            human = true;
        }
    }
    if (!human)
    {
        mState = 16;
    }
}

void CupManager::SetRoundResult(bool inOvertime, int winningSide)
{
    BasicGameInfo* info = GameInfoManager::Instance()->GetCurrentGameInfo();
    eTeamID winner = info->GetTeam(winningSide);
    eTeamID loser;
    if (winningSide == 0)
    {
        loser = info->GetTeam(1);
    }
    else
    {
        loser = info->GetTeam(0);
    }
    eTeamID team = GetUserSelectedCupTeam();
    bool userWon = winner == team;
    int phase = mCurrentCup->mRoundType;
    int round = mCurrentCup->mRoundNumber;
    if (phase == 1)
    {
        int numRounds = mCurrentCup->GetNumPlayoffRounds();
        if (round == numRounds - 1)
        {
            if (userWon == true)
            {
                TeamStats* stats = mCurrentCup->GetPreviousTeamStats();
                eTeamID captain = stats->mTeamIndex;
                CupSidekicks sidekicks = GetRandomCupSidekicks();
                stats->SetSidekicks(sidekicks);
                CupSidekicks userSidekicks = mCurrentCup->mUserSelectedSidekick;
                stats->mSkillLevel = gFinalOpponentSkillLevels[mCurrentMode];
                for (unsigned int i = 0; i < 3; i++)
                {
                    BasicGameInfo* next = mCurrentCup->GetGameInfo(2, i, 0);
                    short sides[2] = { i == 1 ? AWAY : HOME, i == 1 ? HOME : AWAY };
                    next->mTeamIndex[sides[0]] = captain;
                    next->mTeamIndex[sides[1]] = team;
                    for (int j = 0; j < 3; j++)
                    {
                        next->SetSidekick(sides[0], sidekicks.mValues[j], j);
                        next->SetSidekick(sides[1], userSidekicks.mValues[j], j);
                    }
                    next->mStadiumIndex = PickStadium(true);
                }
            }
            else
            {
                mState = 17;
                mCurrentCup->mRoundNumber = -5;
            }
        }
        else
        {
            int game = mCurrentCup->mGameNumber;
            BasicGameInfo* next = 0;
            int side = -1;
            if (loser == team)
            {
                mState = 17;
                mCurrentCup->mRoundNumber = -5;
            }
            if (round == numRounds - 2)
            {
                next = mCurrentCup->GetGameInfo(1, round + 1, 0);
                if (game == 0)
                {
                    side = 0;
                }
                else
                {
                    side = 1;
                }
            }
            else if (round == numRounds - 3)
            {
                if (game == 0)
                {
                    next = mCurrentCup->GetGameInfo(1, round + 1, 0);
                    side = 0;
                }
                else if (game == 1)
                {
                    next = mCurrentCup->GetGameInfo(1, round + 1, 0);
                    side = 1;
                }
                else if (game == 2)
                {
                    next = mCurrentCup->GetGameInfo(1, round + 1, 1);
                    side = 0;
                }
                else if (game == 3)
                {
                    next = mCurrentCup->GetGameInfo(1, round + 1, 1);
                    side = 1;
                }
            }
            next->mTeamIndex[side] = winner;
            for (int i = 0; i < 3; i++)
            {
                eSidekickID sidekick = info->GetSidekick(winningSide, i);
                next->SetSidekick(side, sidekick, i);
            }
            next->mStadiumIndex = PickStadium(false);
        }
    }
    else if (phase == 2 && round > 0)
    {
        int wins = 0;
        int losses = 0;
        for (int i = 0; i < round; i++)
        {
            BasicGameInfo* game = mCurrentCup->GetGameInfo(2, i, 0);
            eTeamSide side = (eTeamSide)game->GetWinningSide();
            if (team == game->GetTeam(side))
            {
                wins++;
            }
            if (team != game->GetTeam((int)side))
            {
                losses++;
            }
        }
        if (userWon == true)
        {
            wins++;
        }
        if (userWon != true)
        {
            losses++;
        }
        if (wins >= 2)
        {
            mState = 4;
            mCurrentCup->mRoundNumber = -5;
        }
        else if (losses >= 2)
        {
            mState = 18;
            mCurrentCup->mRoundNumber = -5;
        }
    }
}

bool CupManager::DetermineNextMatchups(int dnmflags)
{
    int round = mCurrentCup->mRoundNumber;
    int pad = GameInfoManager::Instance()->mMainUserPadNumber;
    int stopForHuman = dnmflags & 1;
    int simulate = dnmflags & 16;
    int advanceGame = dnmflags & 2;
    int advanceRound = dnmflags & 4;
    int stopAfterRound = dnmflags & 8;
    while (round != -5)
    {
        int phase = mCurrentCup->mRoundType;
        GetNumGamesPerRound(phase, mCurrentCup->mRoundNumber);
        BasicGameInfo* info = mCurrentCup->GetGameInfo(phase, round, mCurrentCup->mGameNumber);
        eTeamID home = info->GetTeam(0);
        eTeamID away = info->GetTeam(1);
        GameInfoManager::Instance()->mGameInfo[GameInfoManager::Instance()->mCurrentMode] = info;
        if (stopForHuman && (mCurrentCup->IsHumanTeam(home) || mCurrentCup->IsHumanTeam(away)))
        {
            GameInfoManager::Instance()->GetCurrentGameInfo()->mPadSides[(u16)pad] = home != mCurrentCup->mUserSelectedTeam;
            return true;
        }
        if (simulate)
        {
            StatsTracker::Instance()->SetBasicGameInfoPointer(info, true);
            StatsTracker::Instance()->SimulateGame();
            StatsTracker::Instance()->CompileEndOfGameStats();
        }
        if (advanceGame)
        {
            IncreaseGameNumber(advanceRound);
            round = mCurrentCup->mRoundNumber;
            if (mCurrentCup->mGameNumber == GetNumGamesPerRound(mCurrentCup->mRoundType, round) && stopAfterRound)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    return false;
}

void CupManager::UpdateCurrentCup()
{
    if (mCurrentMode == 0)
    {
        mCurrentCup = &mFireCupSeries;
    }
    else if (mCurrentMode == 1)
    {
        mCurrentCup = &mCrystalCupSeries;
    }
    else if (mCurrentMode == 2)
    {
        mCurrentCup = &mStrikerCupSeries;
    }
    else
    {
        mCurrentCup = 0;
    }
}

void CupManager::SetMode(int mode)
{
    mCurrentMode = mode;
    UpdateCurrentCup();
}

void CupManager::ResetCupRecord()
{
    mCupRecord.mCurrentRecord.mValues[0] = 0;
    mCupRecord.mCurrentRecord.mValues[1] = 0;
    mCupRecord.mCurrentRecord.mValues[2] = 0;
    mCupRecord.mSavedRecord.mValues[0] = 0;
    mCupRecord.mSavedRecord.mValues[1] = 0;
    mCupRecord.mSavedRecord.mValues[2] = 0;
}

void CupManager::SaveCupRecord()
{
    mCupRecord.mSavedRecord = mCupRecord.mCurrentRecord;
}

void CupManager::RestoreCupRecord()
{
    mCupRecord.mCurrentRecord = mCupRecord.mSavedRecord;
}

void CupManager::PrepareCurrentGame()
{
    eTeamID opponent;
    BasicGameInfo* info = GetCurrentGameInfo();
    eTeamID home = info->GetTeam(0);
    eTeamID away = info->GetTeam(1);
    eTeamID team = GetUserSelectedCupTeam();
    opponent = home == team ? away : home;
    eTeamSide side = home == team ? HOME : AWAY;
    CupSidekicks sidekicks = GetUserSelectedCupSidekicks();
    int skill;
    if (mForceHighestSkillLevel == true)
    {
        skill = 5;
    }
    else if (GetCurrentRoundType() == 1)
    {
        skill = gPlayoffSkillLevels[GetCurrentMode()][GetCurrentRoundNumber()];
    }
    else
    {
        skill = GetTeamStats(opponent).mSkillLevel;
    }
    mCurrentCup->mCupSettings.SkillLevel = (GameplaySettings::eSkillLevel)skill;
    info->SetSidekick((short)side, sidekicks.mValues[0], 0);
    info->SetSidekick((short)side, sidekicks.mValues[1], 1);
    info->SetSidekick((short)side, sidekicks.mValues[2], 2);
    SavePreGameUnlockState();
}

void CupManager::AdvanceToNextUserGame()
{
    IncreaseGameNumber(true);
    DetermineNextMatchups(23);
}

void CupManager::StartCupSeries()
{
    int team = mPendingCupTeam;
    CupSidekicks userSidekicks = GetPendingCupSidekicks();
    SelectFinalOpponents();
    mPendingCupTeam = -1;
    eTeamID usedTeams[10];
    for (int i = 0; i < 10; i++)
    {
        usedTeams[i] = TEAM_INVALID;
    }
    for (int mode = 0; mode < 3; mode++)
    {
        SetMode(mode);
        SetUserSelectedCupTeam(team);
        SetUserSelectedCupSidekicks(userSidekicks);
        int count = mCurrentCup->GetNumTeams();
        unsigned int userIndex = nlRandom(count, &nlDefaultSeed);
        unsigned int bossIndex = -1;
        CupSidekicks sidekicks[10];
        int teams[10];
        for (int i = 0; i < count; i++)
        {
            teams[i] = -1;
        }
        teams[userIndex] = team;
        sidekicks[userIndex] = userSidekicks;
        if (mode != 0)
        {
            bossIndex = nlRandom(count, &nlDefaultSeed);
            int boss = mFinalOpponentTeams[mode - 1];
            while (bossIndex == userIndex)
            {
                bossIndex = nlRandom(count, &nlDefaultSeed);
            }
            teams[bossIndex] = boss;
            sidekicks[bossIndex] = GetRandomCupSidekicks();
            if (mode == 1)
            {
                usedTeams[4 + bossIndex] = (eTeamID)boss;
            }
        }
        int excluded[2] = { mode == 0, 2 };
        excluded[1] = 2;
        eTeamID opponent = (eTeamID)mFinalOpponentTeams[mode];
        if (mode == 2)
        {
            excluded[1] = 1;
        }
        mCurrentCup->GetPreviousTeamStats()->Initialize(opponent);
        for (int i = 0; i < count; i++)
        {
            if (i == userIndex || i == bossIndex)
            {
                continue;
            }
            int candidate;
            for (;;)
            {
                candidate = nlRandom(12, &nlDefaultSeed);
                bool available = true;
                if (candidate == team || candidate == opponent)
                {
                    continue;
                }
                if (mode != 2 && (candidate == mFinalOpponentTeams[excluded[0]] || candidate == mFinalOpponentTeams[excluded[1]]))
                {
                    continue;
                }
                for (int j = 0; j < count; j++)
                {
                    if (candidate == teams[j])
                    {
                        available = false;
                        break;
                    }
                }
                if (mode == 1 && available)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (candidate == usedTeams[j])
                        {
                            available = false;
                            break;
                        }
                    }
                }
                else if (mode == 2 && available)
                {
                    available = false;
                    for (int j = 0; j < 10; j++)
                    {
                        if (candidate == usedTeams[j])
                        {
                            available = true;
                            break;
                        }
                    }
                }
                if (available)
                {
                    break;
                }
            }
            teams[i] = candidate;
            sidekicks[i] = GetRandomCupSidekicks();
            if (mode == 0)
            {
                usedTeams[i] = (eTeamID)candidate;
            }
            else if (mode == 1)
            {
                usedTeams[4u + i] = (eTeamID)candidate;
            }
        }
        SetupRoundRobinSchedule(teams, sidekicks);
    }
    SetMode(0);
    DetermineNextMatchups(19);
}

void CupManager::RestartCupSeries()
{
    CupSidekicks userSidekicks = mCurrentCup->mUserSelectedSidekick;
    int team = mCurrentCup->mUserSelectedTeam;
    int originalMode = mCurrentMode;
    for (int mode = originalMode; mode < 3; mode++)
    {
        SetMode(mode);
        SetUserSelectedCupTeam(team);
        SetUserSelectedCupSidekicks(userSidekicks);
        int count = mCurrentCup->GetNumTeams();
        unsigned int userIndex = nlRandom(count, &nlDefaultSeed);
        CupSidekicks sidekicks[10];
        int teams[10];
        int previousTeams[10];
        for (int i = 0; i < count; i++)
        {
            teams[i] = -1;
            previousTeams[i] = mCurrentCup->GetTeamStats(i)->mTeamIndex;
        }
        teams[userIndex] = team;
        sidekicks[userIndex] = userSidekicks;
        TeamStats* stats = mCurrentCup->GetPreviousTeamStats();
        stats->Initialize(stats->mTeamIndex);
        for (int i = 0; i < count; i++)
        {
            if (previousTeams[i] != team)
            {
                unsigned int index;
                do
                {
                    index = nlRandom(count, &nlDefaultSeed);
                } while (teams[index] != -1);
                teams[index] = previousTeams[i];
                sidekicks[index] = GetRandomCupSidekicks();
            }
        }
        SetupRoundRobinSchedule(teams, sidekicks);
    }
    SetMode(originalMode);
    DetermineNextMatchups(19);
}

int CupManager::GetNumPlayoffTeams() const
{
    u16 rounds = mCurrentCup->GetNumPlayoffRounds();
    u16 teams = 0;
    if (rounds == 3)
    {
        teams = 8;
    }
    else if (rounds == 2)
    {
        teams = 4;
    }
    else if (rounds == 1)
    {
        teams = 2;
    }
    return teams;
}

int CupManager::GetSaveDataSize() const
{
    int size = mFireCupSeries.GetSaveDataSize();
    size += mCrystalCupSeries.GetSaveDataSize();
    size += mStrikerCupSeries.GetSaveDataSize();
    return size + GetProgressDataSize();
}

void* CupManager::SerializeData(void* dst) const
{
    dst = mFireCupSeries.SerializeData(dst);
    dst = mCrystalCupSeries.SerializeData(dst);
    dst = mStrikerCupSeries.SerializeData(dst);
    int size = GetProgressDataSize();
    memcpy(dst, &mState, size);
    return (char*)dst + size;
}

void* CupManager::DeserializeData(void* src)
{
    src = mFireCupSeries.DeserializeData(src);
    src = mCrystalCupSeries.DeserializeData(src);
    src = mStrikerCupSeries.DeserializeData(src);
    int size = GetProgressDataSize();
    memcpy(&mState, src, size);
    return (char*)src + size;
}

void CupManager::SelectFinalOpponents()
{
    int team = mPendingCupTeam;
    int bowserJr;
    int diddy = -1;
    int petey = -1;
    if (!IsDiddyKongUnlocked())
    {
        diddy = 10;
    }
    if (!IsPeteyUnlocked())
    {
        petey = 11;
    }
    if (IsBowserJrUnlocked())
    {
        do
        {
            bowserJr = nlRandom(12, &nlDefaultSeed);
        } while (bowserJr == team || bowserJr == diddy || bowserJr == petey);
    }
    else
    {
        bowserJr = 9;
    }
    mFinalOpponentTeams[0] = bowserJr;
    if (IsDiddyKongUnlocked())
    {
        do
        {
            diddy = nlRandom(12, &nlDefaultSeed);
        } while (diddy == team || diddy == bowserJr || diddy == petey);
    }
    mFinalOpponentTeams[1] = diddy;
    if (IsPeteyUnlocked())
    {
        do
        {
            petey = nlRandom(12, &nlDefaultSeed);
        } while (petey == team || petey == bowserJr || petey == diddy);
    }
    mFinalOpponentTeams[2] = petey;
}

bool CupManager::IsCupWinningGame(int team) const
{
    if (GetCurrentRoundType() == 2 && team == mCurrentCup->mUserSelectedTeam)
    {
        return mState == 4;
    }
    return false;
}

int CupManager::GetGoalsForLeader(int* statistic)
{
    int count = GetNumPlayingTeams();
    int index = 0;
    PlayerStats stats[10];
    for (int i = 0; i < count; i++)
    {
        stats[i] = GetTeamStatsByIndex(i).mPlayerTotalStats;
    }
    StatsTracker::Instance()->GetSortedStats(stats, count, &index, 1, STATS_GOALS_FOR, SORT_DESCENDING);
    *statistic = stats[index].mNumGoalsFor;
    return GetTeamStatsByIndex(index).mTeamIndex;
}

int CupManager::GetGoalsAgainstLeader(int* statistic)
{
    int count = GetNumPlayingTeams();
    int index = 0;
    PlayerStats stats[10];
    for (int i = 0; i < count; i++)
    {
        stats[i] = GetTeamStatsByIndex(i).mPlayerTotalStats;
    }
    StatsTracker::Instance()->GetSortedStats(stats, count, &index, 1, STATS_GOALS_AGAINST, SORT_ASCENDING);
    *statistic = stats[index].mNumGoalsAgainst;
    return GetTeamStatsByIndex(index).mTeamIndex;
}

void CupManager::ShowRoundNews()
{
    int roundType = GetCurrentRoundType();
    int round = GetCurrentRoundNumber();
    int state = mState;
    if (roundType == 0 && round == (mCurrentCup->GetNumRegularRounds() >> 1))
    {
        CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push((SceneList)39, SCREEN_NOTHING, false);
        scene->SetDisplayMode(2);
    }
    else if ((roundType == 1 && round == 0) || state == 16)
    {
        CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push((SceneList)39, SCREEN_NOTHING, false);
        scene->SetDisplayMode(3);
    }
    else if ((roundType == 2 && round == 0) || state == 17)
    {
        CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push((SceneList)39, SCREEN_NOTHING, false);
        scene->SetDisplayMode(4);
    }
    else if (round == -5)
    {
        CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push((SceneList)39, SCREEN_NOTHING, false);
        scene->SetDisplayMode(5);
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, false);
    }
}

int CupManager::GetUserTeamRank()
{
    return GetTeamRank(GetUserSelectedCupTeam());
}

int CupManager::GetTeamRank(int team)
{
    int count = GetNumPlayingTeams();
    int indices[10];
    StatsTracker::Instance()->GetSortedTeamStats(pGetTeamStatsByIndex(0), count, indices, count);
    for (int i = 0; i < count; i++)
    {
        if (team == GetTeamStatsByIndex(indices[i]).mTeamIndex)
        {
            count = i;
            break;
        }
    }
    return count;
}

void CupManager::AwardGoalTrophies()
{
    int roundType = GetCurrentRoundType();
    int round = GetCurrentRoundNumber();
    int state = mState;
    if ((roundType == 1 && round == 0) || state == 16)
    {
        int statistic0 = 0;
        int statistic1 = 0;
        int team0 = CupManager::Instance()->GetGoalsForLeader(&statistic0);
        int team1 = CupManager::Instance()->GetGoalsAgainstLeader(&statistic1);
        int team = mCurrentCup->mUserSelectedTeam;
        if (team0 == team || team1 == team)
        {
            int flag0;
            int flag1;
            if (GetCurrentMode() == 0)
            {
                flag0 = 16;
                flag1 = 8;
            }
            else if (GetCurrentMode() == 1)
            {
                flag0 = 64;
                flag1 = 32;
            }
            else
            {
                flag0 = 256;
                flag1 = 128;
            }
            if (team0 == team)
            {
                SetUnlockFlag(flag0);
            }
            if (team1 == team)
            {
                SetUnlockFlag(flag1);
            }
        }
    }
}

void CupManager::ForfeitCurrentGame()
{
    bool home = GameInfoManager::Instance()->GetTeam(0) == GetUserSelectedCupTeam();
    StatsTracker::Instance()->SetBasicGameInfoPointer(GameInfoManager::Instance()->GetCurrentGameInfo(), true);
    if (home)
    {
        StatsTracker::Instance()->TrackWinner(0);
    }
    else
    {
        StatsTracker::Instance()->TrackWinner(1);
    }

    IncreaseGameNumber(true);
    DetermineNextMatchups(23);
}

void CupManager::RecordCupUnlock(int index)
{
    CupSidekicks sidekicks = mCurrentCup->mUserSelectedSidekick;
    OSCalendarTime date;
    OSTicksToCalendarTime(OSGetTime(), &date);
    int captain = mCurrentCup->mUserSelectedTeam;
    TeamStats stats = GetTeamStats(captain);
    AppendCupHistoryRecord(&mCupRecord.mHistory, index, &date, captain, &sidekicks, &stats, mCupRecord);
}

void AppendCupHistoryRecord(CupHistory* history, int index, OSCalendarTime* date,
    int captain, CupSidekicks* sidekicks, TeamStats* stats, CupProgressRecord records)
{
    if (date->year < 2000)
    {
        date->year = 2000;
    }
    int goals = 0;
    history->mRecords[index][history->mWriteIndex[index]].mCaptain = captain;
    history->mRecords[index][history->mWriteIndex[index]].mSidekick1 = sidekicks->mValues[0];
    history->mRecords[index][history->mWriteIndex[index]].mSidekick2 = sidekicks->mValues[1];
    history->mRecords[index][history->mWriteIndex[index]].mSidekick3 = sidekicks->mValues[2];
    history->mRecords[index][history->mWriteIndex[index]].mDay = date->mday - 1;
    history->mRecords[index][history->mWriteIndex[index]].mMonth = date->month;
    history->mRecords[index][history->mWriteIndex[index]].mYearOffset = date->year - 2000;
    CupRecordCounters difference(
        records.mCurrentRecord.mValues[0] - records.mSavedRecord.mValues[0],
        records.mCurrentRecord.mValues[1] - records.mSavedRecord.mValues[1],
        records.mCurrentRecord.mValues[2] - records.mSavedRecord.mValues[2]);
    history->mRecords[index][history->mWriteIndex[index]].mWins = difference.mValues[0];
    history->mRecords[index][history->mWriteIndex[index]].mLosses = difference.mValues[1];
    history->mRecords[index][history->mWriteIndex[index]].mOvertimeLosses = difference.mValues[2];
    switch (index)
    {
    case 4:
    case 6:
    case 8:
        goals = stats->mPlayerTotalStats.mNumGoalsFor;
        break;
    case 3:
    case 5:
    case 7:
        goals = stats->mPlayerTotalStats.mNumGoalsAgainst;
        break;
    }
    history->mRecords[index][history->mWriteIndex[index]].mGoals = goals;
    if (history->mWriteIndex[index] < 11)
    {
        history->mWriteIndex[index]++;
    }
    else
    {
        history->mWriteIndex[index] = 0;
    }
}

int CupManager::GetCurrentMode() const
{
    return mCurrentMode;
}

int CupManager::GetCupPersona() const
{
    return 10;
}

u16 CupManager::GetNumPlayoffRounds() const
{
    return mCurrentCup->GetNumPlayoffRounds();
}

NetworkTournamentGame* CupManager::GetTournamentGame(int, int)
{
    return 0;
}

