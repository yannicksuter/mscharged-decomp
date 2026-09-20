#include "Game/DB/GameProgress.h"

#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHCupNews.h"
#include "Game/Team.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/TweakRegistry.h"
#include "NL/gl/glMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlMath.h"
#include "revolution/os/OSTime_fwd.h"

#include <string.h>
#include "Game/UnidentifiedStaticStorage.h"

struct StrikerChallengeDefinition
{
    /* 0x00 */ const char* mConfigPath;
    /* 0x04 */ int mCaptain;
    /* 0x08 */ const char* mName;
    /* 0x0C */ const char* mTitle;
    /* 0x10 */ const char* mDifficulty;
};

extern const StrikerChallengeDefinition gStrikerChallengeDefinitions[22];

static inline CupSidekicks GetRandomCupSidekicks()
{
    CupSidekicks sidekicks;
    for (int i = 0; i < 3; i++)
    {
        sidekicks.mValues[i] = (eSidekickID)nlRandom(8, &nlDefaultSeed);
    }
    return sidekicks;
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

void CupManager::RestoreCupRecord()
{
    mCupRecord.mUnidentified86A0 = mCupRecord.mUnidentified86A6;
}

void* CupManager::SerializeData(void* dst) const
{
    dst = mFireCupSeries.SerializeData(dst);
    dst = mCrystalCupSeries.SerializeData(dst);
    dst = mStrikerCupSeries.SerializeData(dst);
    int size = UnidentifiedSize_8010D6CC();
    memcpy(dst, &mState, size);
    return (char*)dst + size;
}

void* BaseCup::SerializeData(void* dst) const
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

void* CupManager::DeserializeData(void* src)
{
    src = mFireCupSeries.DeserializeData(src);
    src = mCrystalCupSeries.DeserializeData(src);
    src = mStrikerCupSeries.DeserializeData(src);
    int size = UnidentifiedSize_8010D6CC();
    memcpy(&mState, src, size);
    return (char*)src + size;
}

void* BaseCup::DeserializeData(void* src)
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

int CupManager::GetSaveDataSize() const
{
    int size = mFireCupSeries.GetSaveDataSize();
    size += mCrystalCupSeries.GetSaveDataSize();
    size += mStrikerCupSeries.GetSaveDataSize();
    return size + UnidentifiedSize_8010D6CC();
}

int BaseCup::GetSaveDataSize() const
{
    return 0x1A;
}

s16 CupManager::GetNextRoundNumber(int* roundType)
{
    int currentType = mCurrentCup->mRoundType;
    s16 currentRound = mCurrentCup->mRoundNumber;
    int numRounds = mCurrentCup->GetNumRounds(currentType);
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

u16 CupManager::GetNumPlayingTeams() const
{
    return mCurrentCup->GetNumTeams();
}

BasicGameInfo* CupManager::GetMatchupInfo(int phase, short round, int matchup) const
{
    return mCurrentCup->GetGameInfo(phase, round, matchup);
}

BasicGameInfo* CupManager::GetCurrentGameInfo()
{
    return mCurrentCup->GetGameInfo(mCurrentCup->mRoundNumber, mCurrentCup->mGameNumber);
}

eTeamID CupManager::GetUserSelectedCupTeam() const
{
    return (eTeamID)mCurrentCup->mUserSelectedTeam;
}

bool CupManager::IsCupWinningGame(int team) const
{
    if (GetCurrentRoundType() == 2 && team == mCurrentCup->mUserSelectedTeam)
    {
        return mState == 4;
    }
    return false;
}

s16 CupManager::GetCurrentRoundNumber() const
{
    return mCurrentCup->mRoundNumber;
}

int CupManager::GetCurrentRoundType() const
{
    return mCurrentCup->mRoundType;
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

StrikerChallenge::StrikerChallenge()
{
    mRemainingTime = 0;
    mAIDifficulty = 1;
    mCondition = 0;
    mCaptain = 0;
    mWinParameter = 0;
    mCustomPowerups = 0;
    mCurrentChallenge = -1;
    mUnlocks.mUnlockedChallenges = 0;
    memset(mUnlocks.mCompletionDates, 0, sizeof(mUnlocks.mCompletionDates));
    mUnidentified6C = 0;
    mHeadlineVariant = -1;
    mScore[0] = 0;
    mScore[1] = 0;
    mMissingSidekicks[0] = 0;
    mMissingSidekicks[1] = 0;
    mHomePowerupsEnabled = true;
    mAwayPowerupsEnabled = true;
    mHomeMegastrikeEnabled = true;
    mAwayMegastrikeEnabled = true;
    mHomeSkillshotDisabled = true;
    mAwaySkillshotDisabled = true;
    mStunnedHomeGoalies = false;
    mStunnedAwayGoalies = false;
}

StrikerChallenge::~StrikerChallenge()
{
}

int StrikerChallenge::GetCaptain(int challenge) const
{
    return gStrikerChallengeDefinitions[challenge].mCaptain;
}

void StrikerChallenge::SetCurrentChallenge(int challenge)
{
    mCurrentChallenge = challenge;
    mCaptain = gStrikerChallengeDefinitions[challenge].mCaptain;
}

void StrikerChallenge::LoadSettings()
{
    BasicGameInfo* info = GameInfoManager::Instance()->GetCurrentGameInfo();
    info->mTeamIndex[0] = ConvertToTeamID(GetTweakString("challenge/home", "mario"));
    info->mTeamIndex[1] = ConvertToTeamID(GetTweakString("challenge/away", "luigi"));
    info->mStadiumIndex = ConvertToStadiumID(GetTweakString("challenge/stadium", "vice"));

    char name[64];
    for (int side = 0; side < 2; side++)
    {
        for (int sidekick = 0; sidekick < 3; sidekick++)
        {
            nlSNPrintf(name, sizeof(name), side == 0 ? "challenge/sidekickhome%d" : "challenge/sidekickaway%d", sidekick);
            int id = ConvertToSidekickID(GetTweakString(name, "toad"));
            info->SetSidekick((short)side, id, sidekick);
        }
    }

    mRemainingTime = GetTweakInt("challenge/remainingtime", 180);
    mAIDifficulty = GetTweakInt("challenge/ai", 1);
    mCondition = GetTweakInt("challenge/condition", 0);
    mWinParameter = GetTweakInt("challenge/winparameter", 0);
    mScore[0] = GetTweakInt("challenge/homescore", 0);
    mScore[1] = GetTweakInt("challenge/awayscore", 0);
    mMissingSidekicks[0] = GetTweakInt("challenge/homemissingsidekicks", 0);
    mMissingSidekicks[1] = GetTweakInt("challenge/awaymissingsidekicks", 0);
    mHomePowerupsEnabled = !GetTweakBool("challenge/homepowerups", false);
    mAwayPowerupsEnabled = !GetTweakBool("challenge/awaypowerups", false);
    mHomeMegastrikeEnabled = !GetTweakBool("challenge/homemegastrike", false);
    mAwayMegastrikeEnabled = !GetTweakBool("challenge/awaymegastrike", false);
    mHomeSkillshotDisabled = !GetTweakBool("challenge/homeskillshot", false);
    mAwaySkillshotDisabled = !GetTweakBool("challenge/awayskillshot", false);
    mStunnedHomeGoalies = GetTweakBool("challenge/stunnedhomegoalies", false);
    mStunnedAwayGoalies = GetTweakBool("challenge/stunnedawaygoalies", false);
    mCustomPowerups = GetTweakInt("challenge/custompowerups", 0);
}

bool StrikerChallenge::IsUnlocked(int challenge) const
{
    switch (challenge)
    {
    case 10:
        return IsUnlockFlagSet(0x200);
    case 18:
        return IsUnlockFlagSet(0x400);
    case 14:
        return IsUnlockFlagSet(0x800);
    case 12:
        return IsUnlockFlagSet(0x1000);
    case 11:
        return IsUnlockFlagSet(0x2000);
    case 13:
        return IsUnlockFlagSet(0x4000);
    case 16:
        return IsUnlockFlagSet(0x8000);
    case 15:
        return IsUnlockFlagSet(0x10000);
    case 17:
        return IsUnlockFlagSet(0x20000);
    case 20:
        return IsUnlockFlagSet(0x40000);
    case 21:
        return IsUnlockFlagSet(0x80000);
    case 19:
        return IsUnlockFlagSet(0x100000);
    case 0:
        return IsUnlockFlagSet(0x200000);
    case 1:
        return IsUnlockFlagSet(0x400000);
    case 2:
        return IsUnlockFlagSet(0x800000);
    case 3:
        return IsUnlockFlagSet(0x1000000);
    case 4:
        return IsUnlockFlagSet(0x2000000);
    case 5:
        return IsUnlockFlagSet(0x4000000);
    case 6:
        return IsUnlockFlagSet(0x8000000);
    case 7:
        return IsUnlockFlagSet(0x10000000);
    case 8:
        return IsUnlockFlagSet(0x20000000);
    case 9:
        return IsUnlockFlagSet(0x40000000);
    default:
        return false;
    }
}

bool StrikerChallenge::UnlockCurrentChallenge()
{
    bool unlocked;
    u32 unlockFlag;
    switch (mCurrentChallenge)
    {
    case 10:
        unlockFlag = 0x200;
        break;
    case 18:
        unlockFlag = 0x400;
        break;
    case 14:
        unlockFlag = 0x800;
        break;
    case 12:
        unlockFlag = 0x1000;
        break;
    case 11:
        unlockFlag = 0x2000;
        break;
    case 13:
        unlockFlag = 0x4000;
        break;
    case 16:
        unlockFlag = 0x8000;
        break;
    case 15:
        unlockFlag = 0x10000;
        break;
    case 17:
        unlockFlag = 0x20000;
        break;
    case 20:
        unlockFlag = 0x40000;
        break;
    case 21:
        unlockFlag = 0x80000;
        break;
    case 19:
        unlockFlag = 0x100000;
        break;
    case 0:
        unlockFlag = 0x200000;
        break;
    case 1:
        unlockFlag = 0x400000;
        break;
    case 2:
        unlockFlag = 0x800000;
        break;
    case 3:
        unlockFlag = 0x1000000;
        break;
    case 4:
        unlockFlag = 0x2000000;
        break;
    case 5:
        unlockFlag = 0x4000000;
        break;
    case 6:
        unlockFlag = 0x8000000;
        break;
    case 7:
        unlockFlag = 0x10000000;
        break;
    case 8:
        unlockFlag = 0x20000000;
        break;
    case 9:
        unlockFlag = 0x40000000;
        break;
    default:
        return false;
    }

    unlocked = !IsUnlockFlagSet(unlockFlag);
    if (unlocked)
    {
        SetUnlockFlag(unlockFlag);
    }
    return unlocked;
}

const char* StrikerChallenge::GetConfigPath(int challenge) const
{
    return gStrikerChallengeDefinitions[challenge].mConfigPath;
}

const char* StrikerChallenge::GetName() const
{
    return gStrikerChallengeDefinitions[mCurrentChallenge].mName;
}

const char* StrikerChallenge::GetTitle() const
{
    return gStrikerChallengeDefinitions[mCurrentChallenge].mTitle;
}

const char* StrikerChallenge::GetDifficulty() const
{
    return gStrikerChallengeDefinitions[mCurrentChallenge].mDifficulty;
}

const char* StrikerChallenge::GetDifficulty(int challenge) const
{
    return gStrikerChallengeDefinitions[challenge].mDifficulty;
}

void* StrikerChallenge::SerializeData(void* dst) const
{
    const u32 size = sizeof(mUnlocks.mCompletionDates) + sizeof(mUnlocks.mUnlockedChallenges);
    memcpy(dst, mUnlocks.mCompletionDates, size);
    return (u8*)dst + size;
}

void* StrikerChallenge::DeserializeData(void* src)
{
    const u32 size = sizeof(mUnlocks.mCompletionDates) + sizeof(mUnlocks.mUnlockedChallenges);
    memcpy(mUnlocks.mCompletionDates, src, size);
    return (u8*)src + size;
}

bool GLResourcePool::GetPoolMemoryInfo(unsigned long, const char**,
    unsigned long*, unsigned long*, unsigned long*, const char**)
{
    return false;
}

// Cup specializations provide the three configured cup sizes.
// Storage sizes follow the game and team record layouts; the shared virtual
// interface follows the declarations in Cup.h.

template <>
BasicGameInfo* Cup<4, 8>::GetGameInfo(int round, int matchup)
{
    return GetGameInfo(mRoundType, round, matchup);
}
template <>
BasicGameInfo* Cup<6, 12>::GetGameInfo(int round, int matchup)
{
    return GetGameInfo(mRoundType, round, matchup);
}
template <>
BasicGameInfo* Cup<10, 11>::GetGameInfo(int round, int matchup)
{
    return GetGameInfo(mRoundType, round, matchup);
}

template <>
BasicGameInfo* Cup<4, 8>::GetGameInfo(int index)
{
    return &mGameInfo[0][index];
}
template <>
BasicGameInfo* Cup<6, 12>::GetGameInfo(int index)
{
    return &mGameInfo[0][index];
}
template <>
BasicGameInfo* Cup<10, 11>::GetGameInfo(int index)
{
    return &mGameInfo[0][index];
}

template <>
TeamStats* Cup<4, 8>::GetTeamStats(int index)
{
    return &mTeamStats[index];
}
template <>
TeamStats* Cup<6, 12>::GetTeamStats(int index)
{
    return &mTeamStats[index];
}
template <>
TeamStats* Cup<10, 11>::GetTeamStats(int index)
{
    return &mTeamStats[index];
}

template <>
TeamStats* Cup<4, 8>::GetPreviousTeamStats()
{
    return &mPreviousTeamStats;
}
template <>
TeamStats* Cup<6, 12>::GetPreviousTeamStats()
{
    return &mPreviousTeamStats;
}
template <>
TeamStats* Cup<10, 11>::GetPreviousTeamStats()
{
    return &mPreviousTeamStats;
}

template <>
u16 Cup<4, 8>::GetNumTeams()
{
    return 4;
}
template <>
u16 Cup<6, 12>::GetNumTeams()
{
    return 6;
}
template <>
u16 Cup<10, 11>::GetNumTeams()
{
    return 10;
}

template <>
BasicGameInfo* Cup<4, 8>::GetGameInfo(int phase, int round, int matchup)
{
    BasicGameInfo* result = 0;
    switch (phase)
    {
    case 0:
    {
        int index = matchup + round * 4 / 2;
        result = &mGameInfo[0][index];
        break;
    }
    case 1:
    {
        int index = 12;
        if (round != 0)
        {
            if (round == -1)
            {
                index = 10 + matchup;
            }
            else if (round == -2)
            {
                index = 6 + matchup;
            }
        }
        result = &mGameInfo[0][index];
        break;
    }
    case 2:
    {
        int index = 16 - (3 - round);
        result = &mGameInfo[0][index];
        break;
    }
    }
    return result;
}
template <>
BasicGameInfo* Cup<6, 12>::GetGameInfo(int phase, int round, int matchup)
{
    BasicGameInfo* result = 0;
    switch (phase)
    {
    case 0:
    {
        int index = matchup + round * 6 / 2;
        result = &mGameInfo[0][index];
        break;
    }
    case 1:
    {
        int index = 32;
        if (round != 1)
        {
            if (round == 0)
            {
                index = 30 + matchup;
            }
            else if (round == -1)
            {
                index = 26 + matchup;
            }
        }
        result = &mGameInfo[0][index];
        break;
    }
    case 2:
    {
        int index = 36 - (3 - round);
        result = &mGameInfo[0][index];
        break;
    }
    }
    return result;
}
template <>
BasicGameInfo* Cup<10, 11>::GetGameInfo(int phase, int round, int matchup)
{
    BasicGameInfo* result = 0;
    switch (phase)
    {
    case 0:
    {
        int index = matchup + round * 10 / 2;
        result = &mGameInfo[0][index];
        break;
    }
    case 1:
    {
        int index = 51;
        if (round != 2)
        {
            if (round == 1)
            {
                index = 49 + matchup;
            }
            else if (round == 0)
            {
                index = 45 + matchup;
            }
        }
        result = &mGameInfo[0][index];
        break;
    }
    case 2:
    {
        int index = 55 - (3 - round);
        result = &mGameInfo[0][index];
        break;
    }
    }
    return result;
}

template <>
u16 Cup<4, 8>::GetNumRounds(int phase)
{
    if (phase == 0)
    {
        return 6;
    }
    if (phase == 1)
    {
        return 1;
    }
    if (phase == 2)
    {
        return 3;
    }
    return 6;
}
template <>
u16 Cup<6, 12>::GetNumRounds(int phase)
{
    if (phase == 0)
    {
        return 10;
    }
    if (phase == 1)
    {
        return 2;
    }
    if (phase == 2)
    {
        return 3;
    }
    return 10;
}
template <>
u16 Cup<10, 11>::GetNumRounds(int phase)
{
    if (phase == 0)
    {
        return 9;
    }
    if (phase == 1)
    {
        return 3;
    }
    if (phase == 2)
    {
        return 3;
    }
    return 9;
}

template <>
u16 Cup<4, 8>::GetNumRounds()
{
    return 10;
}
template <>
u16 Cup<6, 12>::GetNumRounds()
{
    return 15;
}
template <>
u16 Cup<10, 11>::GetNumRounds()
{
    return 15;
}

template <>
u16 Cup<4, 8>::GetNumRegularRounds()
{
    return 6;
}
template <>
u16 Cup<6, 12>::GetNumRegularRounds()
{
    return 10;
}
template <>
u16 Cup<10, 11>::GetNumRegularRounds()
{
    return 9;
}

template <>
u16 Cup<4, 8>::GetNumPlayoffRounds()
{
    return 1;
}
template <>
u16 Cup<6, 12>::GetNumPlayoffRounds()
{
    return 2;
}
template <>
u16 Cup<10, 11>::GetNumPlayoffRounds()
{
    return 3;
}

template <>
u16 Cup<4, 8>::GetFirstRoundNumber()
{
    return 0;
}
template <>
u16 Cup<6, 12>::GetFirstRoundNumber()
{
    return 0;
}
template <>
u16 Cup<10, 11>::GetFirstRoundNumber()
{
    return 0;
}

template <>
void Cup<4, 8>::Reset()
{
    for (int i = 0; i < 8 * (4 / 2); i++)
    {
        mGameInfo[0][i].Reset(true);
    }
}
template <>
void Cup<6, 12>::Reset()
{
    for (int i = 0; i < 12 * (6 / 2); i++)
    {
        mGameInfo[0][i].Reset(true);
    }
}
template <>
void Cup<10, 11>::Reset()
{
    for (int i = 0; i < 11 * (10 / 2); i++)
    {
        mGameInfo[0][i].Reset(true);
    }
}

template <>
void* Cup<4, 8>::SerializeData(void* dst) const
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
    dst = (u8*)dst + sizeof(mHumanTeams);
    memcpy(dst, mGameInfo, sizeof(mGameInfo));
    dst = (u8*)dst + sizeof(mGameInfo);
    memcpy(dst, mTeamStats, sizeof(mTeamStats));
    dst = (u8*)dst + sizeof(mTeamStats);
    memcpy(dst, &mPreviousTeamStats, sizeof(mPreviousTeamStats));
    return (u8*)dst + sizeof(mPreviousTeamStats);
}
template <>
void* Cup<6, 12>::SerializeData(void* dst) const
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
    dst = (u8*)dst + sizeof(mHumanTeams);
    memcpy(dst, mGameInfo, sizeof(mGameInfo));
    dst = (u8*)dst + sizeof(mGameInfo);
    memcpy(dst, mTeamStats, sizeof(mTeamStats));
    dst = (u8*)dst + sizeof(mTeamStats);
    memcpy(dst, &mPreviousTeamStats, sizeof(mPreviousTeamStats));
    return (u8*)dst + sizeof(mPreviousTeamStats);
}
template <>
void* Cup<10, 11>::SerializeData(void* dst) const
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
    dst = (u8*)dst + sizeof(mHumanTeams);
    memcpy(dst, mGameInfo, sizeof(mGameInfo));
    dst = (u8*)dst + sizeof(mGameInfo);
    memcpy(dst, mTeamStats, sizeof(mTeamStats));
    dst = (u8*)dst + sizeof(mTeamStats);
    memcpy(dst, &mPreviousTeamStats, sizeof(mPreviousTeamStats));
    return (u8*)dst + sizeof(mPreviousTeamStats);
}

template <>
void* Cup<4, 8>::DeserializeData(void* src)
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
    src = (u8*)src + sizeof(mHumanTeams);
    memcpy(mGameInfo, src, sizeof(mGameInfo));
    src = (u8*)src + sizeof(mGameInfo);
    memcpy(mTeamStats, src, sizeof(mTeamStats));
    src = (u8*)src + sizeof(mTeamStats);
    memcpy(&mPreviousTeamStats, src, sizeof(mPreviousTeamStats));
    return (u8*)src + sizeof(mPreviousTeamStats);
}
template <>
void* Cup<6, 12>::DeserializeData(void* src)
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
    src = (u8*)src + sizeof(mHumanTeams);
    memcpy(mGameInfo, src, sizeof(mGameInfo));
    src = (u8*)src + sizeof(mGameInfo);
    memcpy(mTeamStats, src, sizeof(mTeamStats));
    src = (u8*)src + sizeof(mTeamStats);
    memcpy(&mPreviousTeamStats, src, sizeof(mPreviousTeamStats));
    return (u8*)src + sizeof(mPreviousTeamStats);
}
template <>
void* Cup<10, 11>::DeserializeData(void* src)
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
    src = (u8*)src + sizeof(mHumanTeams);
    memcpy(mGameInfo, src, sizeof(mGameInfo));
    src = (u8*)src + sizeof(mGameInfo);
    memcpy(mTeamStats, src, sizeof(mTeamStats));
    src = (u8*)src + sizeof(mTeamStats);
    memcpy(&mPreviousTeamStats, src, sizeof(mPreviousTeamStats));
    return (u8*)src + sizeof(mPreviousTeamStats);
}

template <>
int Cup<4, 8>::GetSaveDataSize() const
{
    return 0x1A + sizeof(mGameInfo) + sizeof(mTeamStats) + sizeof(mPreviousTeamStats);
}
template <>
int Cup<6, 12>::GetSaveDataSize() const
{
    return 0x1A + sizeof(mGameInfo) + sizeof(mTeamStats) + sizeof(mPreviousTeamStats);
}
template <>
int Cup<10, 11>::GetSaveDataSize() const
{
    return 0x1A + sizeof(mGameInfo) + sizeof(mTeamStats) + sizeof(mPreviousTeamStats);
}

CupManager::~CupManager()
{
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

int CupManager::fn_8010AFA4() const
{
    return mCurrentCup->GetPreviousTeamStats()->mTeamIndex;
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

void CupManager::fn_8010C4DC()
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

void CupManager::fn_8010C52C(int mode)
{
    mCurrentMode = mode;
    fn_8010C4DC();
}

void CupManager::fn_8010C57C()
{
    mCupRecord.mUnidentified86A0.mValues[0] = 0;
    mCupRecord.mUnidentified86A0.mValues[1] = 0;
    mCupRecord.mUnidentified86A0.mValues[2] = 0;
    mCupRecord.mUnidentified86A6.mValues[0] = 0;
    mCupRecord.mUnidentified86A6.mValues[1] = 0;
    mCupRecord.mUnidentified86A6.mValues[2] = 0;
}

void CupManager::fn_8010C5A0()
{
    mCupRecord.mUnidentified86A6 = mCupRecord.mUnidentified86A0;
}

int CupManager::fn_8010D600() const
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

bool StrikerChallenge::IsCurrentChallengeWon() const
{
    bool won = false;
    eTeamSide side = HOME;
    eTeamSide opponent = side ? HOME : AWAY;
    const BasicGameInfo* info = GameInfoManager::Instance()->GetCurrentGameInfo();
    switch (mCondition)
    {
    case 0:
        if (info->GetFinalScore(side) > info->GetFinalScore(opponent))
        {
            won = true;
        }
        break;
    case 1:
        if (info->GetFinalScore(side) >= mWinParameter + info->GetFinalScore(opponent))
        {
            won = true;
        }
        break;
    case 2:
        if (g_pTeams[side]->m_nScore > g_pTeams[opponent]->m_nScore && g_pTeams[opponent]->m_nScore == 0)
        {
            won = true;
        }
        break;
    case 3:
        if (info->GetFinalScore(side) > info->GetFinalScore(opponent) && info->GetFinalScore(side) >= mWinParameter)
        {
            won = true;
        }
        break;
    }
    return won;
}

CupManager::CupManager()
    : mUnidentified869D(false)
    , mCurrentMode(-1)
    , mCurrentCup(0)
    , mShowCupPhasePopup(false)
    , unknown_0x8A28(-1)
    , unknown_0x8A38(0)
{
    mFireCupSeries.mRoundNumber = -6;
    mCrystalCupSeries.mRoundNumber = -6;
    mStrikerCupSeries.mRoundNumber = -6;
    mPreviousGameTeams[0] = -1;
    mPreviousGameTeams[1] = -1;
}

extern "C" void fn_8010EFDC(CupHistory* history, int index, OSCalendarTime* date,
    int captain, CupSidekicks* sidekicks, TeamStats* stats, CupRecord_8010EB90 records)
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
    CupRecord_8010C5C0 difference(
        records.mUnidentified86A0.mValues[0] - records.mUnidentified86A6.mValues[0],
        records.mUnidentified86A0.mValues[1] - records.mUnidentified86A6.mValues[1],
        records.mUnidentified86A0.mValues[2] - records.mUnidentified86A6.mValues[2]);
    history->mRecords[index][history->mWriteIndex[index]].mUnidentified2B = difference.mValues[0];
    history->mRecords[index][history->mWriteIndex[index]].mUnidentified32 = difference.mValues[1];
    history->mRecords[index][history->mWriteIndex[index]].mUnidentified39 = difference.mValues[2];
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
        goals = stats->mPlayerTotalStats.unknown_0x12;
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

void CupManager::fn_8010EB90(int index)
{
    CupSidekicks sidekicks = mCurrentCup->mUserSelectedSidekick;
    OSCalendarTime date;
    OSTicksToCalendarTime(OSGetTime(), &date);
    int captain = mCurrentCup->mUserSelectedTeam;
    TeamStats stats = GetTeamStats(captain);
    fn_8010EFDC(&mCupRecord.mHistory, index, &date, captain, &sidekicks, &stats, mCupRecord);
}

extern const int lbl_804DC918[6];
extern const int lbl_804DC930[7];
extern const int lbl_804DC950[8];
extern const int lbl_804DC970[7];
extern const int lbl_804DC990[6][4];

int CupManager::fn_8010B25C(bool final) const
{
    int mode = mCurrentMode;
    const int* stadiums = 0;
    int count = 0;
    if (mode == 0)
    {
        if (final)
        {
            return 7;
        }
        stadiums = lbl_804DC918;
        count = 6;
    }
    else if (mode == 1)
    {
        if (final)
        {
            return 3;
        }
        stadiums = lbl_804DC930;
        count = 7;
    }
    else if (mode == 2)
    {
        if (final)
        {
            return 9;
        }
        stadiums = lbl_804DC950;
        count = 8;
    }

    int index = nlRandom(count + 1, &nlDefaultSeed);
    if (index < count)
    {
        return stadiums[index];
    }
    return lbl_804DC970[nlRandom(7, &nlDefaultSeed)];
}

static inline bool IsInStadiumGroup(int stadium)
{
    for (int i = 0; i < 7; i++)
    {
        if (stadium == lbl_804DC970[i])
        {
            return true;
        }
    }
    return false;
}

void CupManager::fn_8010B348(int* stadiums)
{
    const int* choices = 0;
    int count = 0;
    int rounds = mCurrentCup->GetNumRegularRounds();
    int first = 0;
    if (mCurrentMode == 0)
    {
        choices = lbl_804DC918;
        count = 6;
    }
    else if (mCurrentMode == 1)
    {
        choices = lbl_804DC930;
        count = 7;
    }
    else if (mCurrentMode == 2)
    {
        choices = lbl_804DC950;
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
                    stadiums[i] = lbl_804DC970[nlRandom(7, &nlDefaultSeed)];
                    break;
                }
            }
        }
    }
}

struct CupMatchup
{
    int mHome;
    int mAway;
};

extern const CupMatchup lbl_804DC660[12];
extern const CupMatchup lbl_804DC6C0[30];
extern const CupMatchup lbl_804DC7B0[45];

void CupManager::fn_8010B578(int* teams, CupSidekicks* sidekicks)
{
    int numTeams = mCurrentCup->GetNumTeams();
    int numRounds = mCurrentCup->GetNumRegularRounds();
    int numGames = GetNumGamesPerRound(0, 0);
    mState = 15;
    BasicGameInfo* info;
    int home;
    int away;
    int stadiums[10];
    fn_8010B348(stadiums);
    mCurrentCup->mRoundNumber = 0;
    mCurrentCup->mGameNumber = 0;
    mCurrentCup->mRoundType = 0;
    mCurrentCup->Reset();
    for (int round = 0; round < numRounds; round++)
    {
        for (int game = 0; game < numGames; game++)
        {
            int index = round * numGames + game;
            switch (mCurrentCup->GetNumTeams())
            {
            case 4:
                home = lbl_804DC660[index].mHome;
                away = lbl_804DC660[index].mAway;
                break;
            case 6:
                home = lbl_804DC6C0[index].mHome;
                away = lbl_804DC6C0[index].mAway;
                break;
            case 10:
                home = lbl_804DC7B0[index].mHome;
                away = lbl_804DC7B0[index].mAway;
                break;
            }
            info = mCurrentCup->GetGameInfo(0, round, game);
            info->mTeamIndex[0] = teams[home];
            info->mTeamIndex[1] = teams[away];
            for (int i = 0; i < 3; i++)
            {
                info->SetSidekick(0, sidekicks[home].mValues[i], i);
                info->SetSidekick(1, sidekicks[away].mValues[i], i);
            }
            int stadium;
            if (mCurrentCup->IsHumanTeam(info->GetTeam(0)) || mCurrentCup->IsHumanTeam(info->GetTeam(1)))
            {
                stadium = stadiums[round];
            }
            else
            {
                stadium = fn_8010B25C(false);
            }
            info->mStadiumIndex = stadium;
        }
    }
    TeamStats* stats = mCurrentCup->GetTeamStats(0);
    for (int i = 0; i < numTeams; i++)
    {
        stats[i].Initialize((eTeamID)teams[i]);
        stats[i].SetSidekicks(sidekicks[i]);
    }
    fn_8010B918();
}

void CupManager::fn_8010B918()
{
    TeamStats* stats = mCurrentCup->GetTeamStats(0);
    int counts[6] = {};
    for (int i = 0; i < mCurrentCup->GetNumTeams(); i++)
    {
        if (stats[i].mTeamIndex != GetUserSelectedCupTeam())
        {
            int type;
            do
            {
                type = nlRandom(6, &nlDefaultSeed);
            } while (counts[type] >= lbl_804DC990[type][mCurrentMode]);
            stats[i].mUnidentified18 = type;
            counts[type]++;
        }
    }
}

void CupManager::fn_8010BA10()
{
    int indices[10];
    StatsTracker::Instance()->GetSortedTeamStats(mCurrentCup->GetTeamStats(0), mCurrentCup->GetNumTeams(), indices, mCurrentCup->GetNumTeams());
    bool human = false;
    u16 playoffTeams = fn_8010D600();
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
        info->mStadiumIndex = fn_8010B25C(false);
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

extern const int lbl_804DCA20[3];

void CupManager::fn_8010BCB8(bool overtime, int winningSide)
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
                stats->mUnidentified18 = lbl_804DCA20[mCurrentMode];
                for (unsigned int i = 0; i < 3; i++)
                {
                    BasicGameInfo* next = mCurrentCup->GetGameInfo(2, i, 0);
                    short captainSide = i == 1;
                    short userSide = i != 1;
                    next->mTeamIndex[captainSide] = captain;
                    next->mTeamIndex[userSide] = team;
                    for (int j = 0; j < 3; j++)
                    {
                        next->SetSidekick(captainSide, sidekicks.mValues[j], j);
                        next->SetSidekick(userSide, userSidekicks.mValues[j], j);
                    }
                    next->mStadiumIndex = fn_8010B25C(true);
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
            next->mStadiumIndex = fn_8010B25C(false);
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
            if (team != game->GetTeam(side))
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

inline void CupManager::IncreaseRoundNumber()
{
    int previousType = mCurrentCup->mRoundType;
    int nextType = -1;
    mCurrentCup->mRoundNumber = GetNextRoundNumber(&nextType);
    mCurrentCup->mRoundType = nextType;
    if (previousType != 1 && nextType == 1)
    {
        fn_8010BA10();
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

bool CupManager::fn_8010C280(int flags)
{
    int round = mCurrentCup->mRoundNumber;
    int pad = GameInfoManager::Instance()->mMainUserPadNumber;
    int stopForHuman = flags & 1;
    int simulate = flags & 16;
    int advanceGame = flags & 2;
    int advanceRound = flags & 4;
    int stopAfterRound = flags & 8;
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

void CupManager::fn_8010D838()
{
    int team = unknown_0x8A28;
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
    mUnidentified8684[0] = bowserJr;
    if (IsDiddyKongUnlocked())
    {
        do
        {
            diddy = nlRandom(12, &nlDefaultSeed);
        } while (diddy == team || diddy == bowserJr || diddy == petey);
    }
    mUnidentified8684[1] = diddy;
    if (IsPeteyUnlocked())
    {
        do
        {
            petey = nlRandom(12, &nlDefaultSeed);
        } while (petey == team || petey == bowserJr || petey == diddy);
    }
    mUnidentified8684[2] = petey;
}

void CupManager::fn_8010E8E0()
{
    int roundType = GetCurrentRoundType();
    int round = GetCurrentRoundNumber();
    int state = mState;
    if ((roundType == 1 && round == 0) || state == 16)
    {
        int statistic0 = 0;
        int statistic1 = 0;
        int team0 = CupManager::Instance()->fn_8010D9C4(&statistic0);
        int team1 = CupManager::Instance()->fn_8010DE2C(&statistic1);
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

void CupManager::fn_8010CAE8()
{
    IncreaseGameNumber(true);
    fn_8010C280(23);
}

void CupManager::fn_8010EA28()
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
    fn_8010C280(23);
}

extern const int lbl_804DC9F0[3][3];
extern "C" void fn_8010FED8();

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

void CupManager::fn_8010CBE4()
{
    int team = unknown_0x8A28;
    CupSidekicks userSidekicks = GetPendingCupSidekicks();
    fn_8010D838();
    unknown_0x8A28 = -1;
    eTeamID usedTeams[10];
    for (int i = 0; i < 10; i++)
    {
        usedTeams[i] = TEAM_INVALID;
    }
    for (int mode = 0; mode < 3; mode++)
    {
        fn_8010C52C(mode);
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
            int boss = mUnidentified8684[mode - 1];
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
        int excluded0 = mode == 0;
        int excluded1 = 2;
        eTeamID opponent = (eTeamID)mUnidentified8684[mode];
        if (mode == 2)
        {
            excluded1 = 1;
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
                if (mode != 2 && (candidate == mUnidentified8684[excluded0] || candidate == mUnidentified8684[excluded1]))
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
        fn_8010B578(teams, sidekicks);
    }
    fn_8010C52C(0);
    fn_8010C280(19);
}

void CupManager::RestartCupSeries()
{
    CupSidekicks userSidekicks = mCurrentCup->mUserSelectedSidekick;
    int team = mCurrentCup->mUserSelectedTeam;
    int originalMode = mCurrentMode;
    for (int mode = originalMode; mode < 3; mode++)
    {
        fn_8010C52C(mode);
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
        fn_8010B578(teams, sidekicks);
    }
    fn_8010C52C(originalMode);
    fn_8010C280(19);
}

void CupManager::fn_8010C5E0()
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
    if (mUnidentified869D == true)
    {
        skill = 5;
    }
    else if (GetCurrentRoundType() == 1)
    {
        skill = lbl_804DC9F0[GetCurrentMode()][GetCurrentRoundNumber()];
    }
    else
    {
        skill = GetTeamStats(opponent).mUnidentified18;
    }
    mCurrentCup->mCupSettings.SkillLevel = (GameplaySettings::eSkillLevel)skill;
    info->SetSidekick((short)side, sidekicks.mValues[0], 0);
    info->SetSidekick((short)side, sidekicks.mValues[1], 1);
    info->SetSidekick((short)side, sidekicks.mValues[2], 2);
    fn_8010FED8();
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

int CupManager::fn_8010D9C4(int* statistic)
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

int CupManager::fn_8010DE2C(int* statistic)
{
    int count = GetNumPlayingTeams();
    int index = 0;
    PlayerStats stats[10];
    for (int i = 0; i < count; i++)
    {
        stats[i] = GetTeamStatsByIndex(i).mPlayerTotalStats;
    }
    StatsTracker::Instance()->GetSortedStats(stats, count, &index, 1, STATS_0C, SORT_ASCENDING);
    *statistic = stats[index].unknown_0x12;
    return GetTeamStatsByIndex(index).mTeamIndex;
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

int CupManager::fn_8010E460()
{
    return GetTeamRank(GetUserSelectedCupTeam());
}

extern "C" bool fn_8010FEF0(unsigned int flags)
{
    return (flags & CupManager::Instance()->unknown_0x8A38) == 0;
}

extern bool lbl_806E0F98;

extern "C" bool fn_8010FD74()
{
    return lbl_806E0F98;
}

extern "C" void fn_8010FD7C(bool value)
{
    lbl_806E0F98 = value;
}

void RecordChallengeUnlock(ChallengeUnlockRecord* record, int flag);

inline bool ChallengeUnlockRecord::IsUnlocked(int flag) const
{
    int shift = nlLog2(0x100) + 1;
    return (mUnlockedChallenges & (flag >> shift)) != 0;
}

bool IsUnlockFlagSet(int flag)
{
    bool unlocked;
    if (flag <= 0x100)
    {
        unlocked = CupManager::Instance()->HasUnlockFlag(flag);
    }
    else
    {
        unlocked = g_pStrikerChallenge->mUnlocks.IsUnlocked(flag);
    }
    return unlocked;
}

void SetUnlockFlag(int flag)
{
    if (flag <= 0x100)
    {
        CupManager::Instance()->mCupRecord.mUnlockFlags |= flag;
        int index = nlLog2(flag);
        CupManager::Instance()->fn_8010EB90(index);
    }
    else if (!IsUnlockFlagSet(flag))
    {
        RecordChallengeUnlock(&g_pStrikerChallenge->mUnlocks, flag);
    }
}

extern "C" void fn_8010FED8()
{
    CupManager::Instance()->unknown_0x8A38 = CupManager::Instance()->GetUnlockFlags();
}

static inline bool IsUnlockOverrideEnabled(bool includeOnline)
{
    if (GetTweakBool("/user/media_build", false))
    {
        return false;
    }
    return lbl_806E0F98 || GetTweakBool("/user/unlock_all", false)
        || (includeOnline && GameInfoManager::Instance()->IsOnline());
}

bool IsBowserJrUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(1);
}

bool IsDiddyKongUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(2);
}

bool IsPeteyUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(4);
}

bool IsWastelandsUnlocked()
{
    return IsUnlockOverrideEnabled(true) || (IsUnlockFlagSet(8) && IsUnlockFlagSet(16));
}

bool IsDumpUnlocked()
{
    return IsUnlockOverrideEnabled(true) || (IsUnlockFlagSet(32) && IsUnlockFlagSet(64));
}

bool IsGalacticStadiumUnlocked()
{
    return IsUnlockOverrideEnabled(true) || (IsUnlockFlagSet(128) && IsUnlockFlagSet(256));
}

extern "C" bool fn_801102D8()
{
    return IsUnlockFlagSet(8) && IsUnlockFlagSet(16);
}

extern "C" bool fn_801102F8()
{
    return IsUnlockFlagSet(32) && IsUnlockFlagSet(64);
}

extern "C" bool fn_80110318()
{
    return IsUnlockFlagSet(128) && IsUnlockFlagSet(256);
}

bool IsStormshipUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(4);
}

bool IsCrystalCanyonUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(2);
}

bool IsLavaPitUnlocked()
{
    return IsUnlockOverrideEnabled(true) || IsUnlockFlagSet(1);
}

bool IsSecureEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x8000);
}

bool IsPowerEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x20000);
}

bool IsVoltageEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x4000);
}

bool IsTiltEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x400);
}

bool IsWhiteBallEnvironmentCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x100000);
}

bool IsPowerupCheatsUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x80000);
}

bool IsSuperPowerupsCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x1000);
}

bool IsDevastatingPlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x800);
}

bool IsSafePlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x2000);
}

bool IsSkillShotPlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x40000);
}

bool IsGlassJawPlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x200);
}

bool IsButterfingersPlayerCheatUnlocked()
{
    return IsUnlockOverrideEnabled(false) || IsUnlockFlagSet(0x10000);
}

extern "C" bool fn_80110CF0()
{
    return !(CupManager::Instance()->unknown_0x8A38 & 8)
        || !(CupManager::Instance()->unknown_0x8A38 & 16);
}

extern "C" bool fn_80110D18()
{
    return !(CupManager::Instance()->unknown_0x8A38 & 32)
        || !(CupManager::Instance()->unknown_0x8A38 & 64);
}

extern "C" bool fn_80110D40()
{
    return !(CupManager::Instance()->unknown_0x8A38 & 128)
        || !(CupManager::Instance()->unknown_0x8A38 & 256);
}

void RecordChallengeUnlock(ChallengeUnlockRecord* record, int flag)
{
    int shift = nlLog2(0x100) + 1;
    if (flag <= 0x100000)
    {
        int index = nlLog2(flag >> shift);
        OSCalendarTime date;
        OSTicksToCalendarTime(OSGetTime(), &date);
        if (date.year < 2000)
        {
            date.year = 2000;
        }
        record->mCompletionDates[index].mDay = date.mday - 1;
        record->mCompletionDates[index].mMonth = date.month;
        record->mCompletionDates[index].mYearOffset = date.year - 2000;
    }
    record->mUnlockedChallenges |= flag >> shift;
}
