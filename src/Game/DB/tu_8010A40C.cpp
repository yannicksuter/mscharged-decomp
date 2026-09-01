#include "Game/DB/tu_8010A40C.h"

struct StrikerChallengeDefinition
{
    /* 0x00 */ const char* mConfigPath;
    /* 0x04 */ int mCaptain;
    /* 0x08 */ const char* mName;
    /* 0x0C */ const char* mTitle;
    /* 0x10 */ const char* mDifficulty;
};

extern const StrikerChallengeDefinition lbl_804DCA30[22];
extern bool fn_8010FE54(u32);
extern void fn_8010FD84(u32);
extern void fn_80111560(u32* completionData, u32 unlockFlag);

void* BaseCup::SerializeData(void* dst) const
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
    return (u8*)dst + sizeof(mHumanTeams);
}

void* BaseCup::DeserializeData(void* src)
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
    return (u8*)src + sizeof(mHumanTeams);
}

int BaseCup::GetSaveDataSize() const
{
    return 0x1A;
}

u16 UnidentifiedCupManager::GetNumPlayingTeams() const
{
    return mCurrentCup->GetNumTeams();
}

BasicGameInfo* UnidentifiedCupManager::GetMatchupInfo(int phase, short round, int matchup) const
{
    return mCurrentCup->GetGameInfo(phase, round, matchup);
}

BasicGameInfo* UnidentifiedCupManager::GetCurrentGameInfo()
{
    return mCurrentCup->GetGameInfo(mCurrentCup->mRoundNumber, mCurrentCup->mGameNumber);
}

int UnidentifiedCupManager::GetUserSelectedCupTeam() const
{
    return mCurrentCup->mUserSelectedTeam;
}

s16 UnidentifiedCupManager::GetCurrentRoundNumber() const
{
    return mCurrentCup->mRoundNumber;
}

int UnidentifiedCupManager::GetCurrentRoundType() const
{
    return mCurrentCup->mRoundType;
}

int UnidentifiedCupManager::GetCurrentMode() const
{
    return mCurrentMode;
}

int UnidentifiedCupManager::fn_8010F1C8() const
{
    return 10;
}

int UnidentifiedCupManager::fn_8010F1E8() const
{
    return 0;
}

UnidentifiedStrikerChallenge::UnidentifiedStrikerChallenge()
{
    mRemainingTime = 0;
    mAIDifficulty = 1;
    mCondition = 0;
    mCaptain = 0;
    mWinParameter = 0;
    mCustomPowerups = 0;
    mCurrentChallenge = -1;
    mUnlockedChallenges = 0;
    memset(mCompletionDates, 0, sizeof(mCompletionDates));
    mUnidentified6C = false;
    mHeadlineVariant = -1;
    mHomeScore = 0;
    mAwayScore = 0;
    mHomeMissingSidekicks = 0;
    mAwayMissingSidekicks = 0;
    mHomePowerupsDisabled = true;
    mAwayPowerupsDisabled = true;
    mHomeMegastrikeDisabled = true;
    mAwayMegastrikeDisabled = true;
    mHomeSkillshotDisabled = true;
    mAwaySkillshotDisabled = true;
    mStunnedHomeGoalies = false;
    mStunnedAwayGoalies = false;
}

UnidentifiedStrikerChallenge::~UnidentifiedStrikerChallenge()
{
}

int UnidentifiedStrikerChallenge::GetCaptain(int challenge) const
{
    return lbl_804DCA30[challenge].mCaptain;
}

void UnidentifiedStrikerChallenge::SetCurrentChallenge(int challenge)
{
    mCurrentChallenge = challenge;
    mCaptain = lbl_804DCA30[challenge].mCaptain;
}

bool UnidentifiedStrikerChallenge::IsUnlocked(int challenge) const
{
    switch (challenge)
    {
    case 0:
        return fn_8010FE54(0x200);
    case 1:
        return fn_8010FE54(0x400);
    case 2:
        return fn_8010FE54(0x800);
    case 3:
        return fn_8010FE54(0x1000);
    case 4:
        return fn_8010FE54(0x2000);
    case 5:
        return fn_8010FE54(0x4000);
    case 6:
        return fn_8010FE54(0x8000);
    case 7:
        return fn_8010FE54(0x10000);
    case 8:
        return fn_8010FE54(0x20000);
    case 9:
        return fn_8010FE54(0x40000);
    case 10:
        return fn_8010FE54(0x80000);
    case 11:
        return fn_8010FE54(0x100000);
    case 12:
        return fn_8010FE54(0x200000);
    case 13:
        return fn_8010FE54(0x400000);
    case 14:
        return fn_8010FE54(0x800000);
    case 15:
        return fn_8010FE54(0x1000000);
    case 16:
        return fn_8010FE54(0x2000000);
    case 17:
        return fn_8010FE54(0x4000000);
    case 18:
        return fn_8010FE54(0x8000000);
    case 19:
        return fn_8010FE54(0x10000000);
    case 20:
        return fn_8010FE54(0x20000000);
    case 21:
        return fn_8010FE54(0x40000000);
    default:
        return false;
    }
}

bool UnidentifiedStrikerChallenge::UnlockCurrentChallenge()
{
    bool unlocked;
    u32 unlockFlag;
    switch (mCurrentChallenge)
    {
    case 0:
        unlockFlag = 0x200;
        break;
    case 1:
        unlockFlag = 0x400;
        break;
    case 2:
        unlockFlag = 0x800;
        break;
    case 3:
        unlockFlag = 0x1000;
        break;
    case 4:
        unlockFlag = 0x2000;
        break;
    case 5:
        unlockFlag = 0x4000;
        break;
    case 6:
        unlockFlag = 0x8000;
        break;
    case 7:
        unlockFlag = 0x10000;
        break;
    case 8:
        unlockFlag = 0x20000;
        break;
    case 9:
        unlockFlag = 0x40000;
        break;
    case 10:
        unlockFlag = 0x80000;
        break;
    case 11:
        unlockFlag = 0x100000;
        break;
    case 12:
        unlockFlag = 0x200000;
        break;
    case 13:
        unlockFlag = 0x400000;
        break;
    case 14:
        unlockFlag = 0x800000;
        break;
    case 15:
        unlockFlag = 0x1000000;
        break;
    case 16:
        unlockFlag = 0x2000000;
        break;
    case 17:
        unlockFlag = 0x4000000;
        break;
    case 18:
        unlockFlag = 0x8000000;
        break;
    case 19:
        unlockFlag = 0x10000000;
        break;
    case 20:
        unlockFlag = 0x20000000;
        break;
    case 21:
        unlockFlag = 0x40000000;
        break;
    default:
        return false;
    }

    unlocked = !fn_8010FE54(unlockFlag);
    if (unlocked)
    {
        fn_8010FD84(unlockFlag);
    }
    return unlocked;
}

const char* UnidentifiedStrikerChallenge::GetConfigPath(int challenge) const
{
    return lbl_804DCA30[challenge].mConfigPath;
}

const char* UnidentifiedStrikerChallenge::GetName() const
{
    return lbl_804DCA30[mCurrentChallenge].mName;
}

const char* UnidentifiedStrikerChallenge::GetTitle() const
{
    return lbl_804DCA30[mCurrentChallenge].mTitle;
}

const char* UnidentifiedStrikerChallenge::GetDifficulty() const
{
    return lbl_804DCA30[mCurrentChallenge].mDifficulty;
}

const char* UnidentifiedStrikerChallenge::GetDifficulty(int challenge) const
{
    return lbl_804DCA30[challenge].mDifficulty;
}

void* UnidentifiedStrikerChallenge::SerializeData(void* dst) const
{
    const u32 size = sizeof(mCompletionDates) + sizeof(mUnlockedChallenges);
    memcpy(dst, mCompletionDates, size);
    return (u8*)dst + size;
}

void* UnidentifiedStrikerChallenge::DeserializeData(void* src)
{
    const u32 size = sizeof(mCompletionDates) + sizeof(mUnlockedChallenges);
    memcpy(mCompletionDates, src, size);
    return (u8*)src + size;
}

int fn_8011162C()
{
    return 0;
}
