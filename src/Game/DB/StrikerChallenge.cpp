#include "Game/DB/GameProgress.h"
#include "Game/GameInfo.h"
#include "Game/Team.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/TweakRegistry.h"
#include "NL/nlPrint.h"
#include "NL/nlMath.h"
#include "revolution/os/OSTime_fwd.h"
#include <string.h>

struct StrikerChallengeDefinition
{
    /* 0x00 */ const char* mDifficulty;
    /* 0x04 */ int mCaptain;
    /* 0x08 */ const char* mConfigPath;
    /* 0x0C */ const char* mName;
    /* 0x10 */ const char* mTitle;
};

const StrikerChallengeDefinition gStrikerChallengeDefinitions[22] = {
    { "rookie", 1, "ini/strikerchallenges/tutorial1.ini", "CHARACTER_DIFF", "TUTORIAL_TITLE_CHARACTER_DIFF" },
    { "rookie", 0, "ini/strikerchallenges/tutorial2.ini", "BALL_CHARGE", "TUTORIAL_TITLE_BALL_CHARGE" },
    { "rookie", 0, "ini/strikerchallenges/tutorial3.ini", "MEGA_SAVES", "TUTORIAL_TITLE_MEGA_SAVES" },
    { "rookie", 0, "ini/strikerchallenges/tutorial4.ini", "MEGA_STRIKE", "TUTORIAL_TITLE_MEGA_STRIKE" },
    { "rookie", 0, "ini/strikerchallenges/tutorial5.ini", "SIDEKICK_SKILLSHOT", "TUTORIAL_TITLE_SIDEKICK_SKILLSHOT" },
    { "rookie", 0, "ini/strikerchallenges/tutorial6.ini", "SKILLSHOT_PART_TWO", "TUTORIAL_TITLE_SKILLSHOT_TWO" },
    { "rookie", 0, "ini/strikerchallenges/tutorial7.ini", "SUPER_MARIO", "TUTORIAL_TITLE_SUPER_MARIO" },
    { "rookie", 3, "ini/strikerchallenges/tutorial8.ini", "BRING_THUNDER", "TUTORIAL_TITLE_BRING_THUNDER" },
    { "rookie", 0, "ini/strikerchallenges/tutorial9.ini", "POWER_UPS", "TUTORIAL_TITLE_POWER_UPS" },
    { "rookie", 0, "ini/strikerchallenges/tutorial10.ini", "WIN_GAME", "TUTORIAL_TITLE_WIN_GAME" },
    { "rookie", 0, "ini/strikerchallenges/mario.ini", "MARIO", "CHALLENGE_TITLE_MARIO" },
    { "rookie", 4, "ini/strikerchallenges/luigi.ini", "LUIGI", "CHALLENGE_TITLE_LUIGI" },
    { "professional", 3, "ini/strikerchallenges/donkeykong.ini", "DONKEYKONG", "CHALLENGE_TITLE_DONKEY_KONG" },
    { "professional", 5, "ini/strikerchallenges/peach.ini", "PEACH", "CHALLENGE_TITLE_PEACH" },
    { "superstar", 2, "ini/strikerchallenges/daisy.ini", "DAISY", "CHALLENGE_TITLE_DAISY" },
    { "superstar", 7, "ini/strikerchallenges/wario.ini", "WARIO", "CHALLENGE_TITLE_WARIO" },
    { "superstar", 6, "ini/strikerchallenges/waluigi.ini", "WALUIGI", "CHALLENGE_TITLE_WALUIGI" },
    { "legend", 8, "ini/strikerchallenges/yoshi.ini", "YOSHI", "CHALLENGE_TITLE_YOSHI" },
    { "legend", 1, "ini/strikerchallenges/bowser.ini", "BOWSER", "CHALLENGE_TITLE_BOWSER" },
    { "legend", 11, "ini/strikerchallenges/petey.ini", "PETEY", "CHALLENGE_TITLE_PETEY" },
    { "megastriker", 9, "ini/strikerchallenges/bowserjr.ini", "BOWSERJR", "CHALLENGE_TITLE_BOWSER_JR" },
    { "megastriker", 10, "ini/strikerchallenges/diddykong.ini", "DIDDYKONG", "CHALLENGE_TITLE_DIDDY_KONG" },
};

StrikerChallenge* g_pStrikerChallenge;

StrikerChallenge::StrikerChallenge()
{
    mRemainingTime = 0;
    mAIDifficulty = 1;
    mCondition = CHALLENGE_WIN;
    mCaptain = 0;
    mWinParameter = 0;
    mCustomPowerups = POWERUP_CHEAT_NONE;
    mCurrentChallenge = STRIKER_CHALLENGE_NONE;
    mUnlocks.mUnlockedChallenges = 0;
    memset(mUnlocks.mCompletionDates, 0, sizeof(mUnlocks.mCompletionDates));
    mChallengeOffset = 0;
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
    mCondition = static_cast<eChallengeCondition>(GetTweakInt("challenge/condition", CHALLENGE_WIN));
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
    case STRIKER_CHALLENGE_MARIO:
        return IsUnlockFlagSet(0x200);
    case STRIKER_CHALLENGE_BOWSER:
        return IsUnlockFlagSet(0x400);
    case STRIKER_CHALLENGE_DAISY:
        return IsUnlockFlagSet(0x800);
    case STRIKER_CHALLENGE_DONKEY_KONG:
        return IsUnlockFlagSet(0x1000);
    case STRIKER_CHALLENGE_LUIGI:
        return IsUnlockFlagSet(0x2000);
    case STRIKER_CHALLENGE_PEACH:
        return IsUnlockFlagSet(0x4000);
    case STRIKER_CHALLENGE_WALUIGI:
        return IsUnlockFlagSet(0x8000);
    case STRIKER_CHALLENGE_WARIO:
        return IsUnlockFlagSet(0x10000);
    case STRIKER_CHALLENGE_YOSHI:
        return IsUnlockFlagSet(0x20000);
    case STRIKER_CHALLENGE_BOWSER_JR:
        return IsUnlockFlagSet(0x40000);
    case STRIKER_CHALLENGE_DIDDY_KONG:
        return IsUnlockFlagSet(0x80000);
    case STRIKER_CHALLENGE_PETEY:
        return IsUnlockFlagSet(0x100000);
    case TUTORIAL_CHARACTER_DIFFERENCES:
        return IsUnlockFlagSet(0x200000);
    case TUTORIAL_BALL_CHARGE:
        return IsUnlockFlagSet(0x400000);
    case TUTORIAL_MEGA_SAVES:
        return IsUnlockFlagSet(0x800000);
    case TUTORIAL_MEGA_STRIKE:
        return IsUnlockFlagSet(0x1000000);
    case TUTORIAL_SIDEKICK_SKILLSHOT:
        return IsUnlockFlagSet(0x2000000);
    case TUTORIAL_SKILLSHOT_PART_TWO:
        return IsUnlockFlagSet(0x4000000);
    case TUTORIAL_SUPER_MARIO:
        return IsUnlockFlagSet(0x8000000);
    case TUTORIAL_BRING_THUNDER:
        return IsUnlockFlagSet(0x10000000);
    case TUTORIAL_POWER_UPS:
        return IsUnlockFlagSet(0x20000000);
    case TUTORIAL_WIN_GAME:
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
    case STRIKER_CHALLENGE_MARIO:
        unlockFlag = 0x200;
        break;
    case STRIKER_CHALLENGE_BOWSER:
        unlockFlag = 0x400;
        break;
    case STRIKER_CHALLENGE_DAISY:
        unlockFlag = 0x800;
        break;
    case STRIKER_CHALLENGE_DONKEY_KONG:
        unlockFlag = 0x1000;
        break;
    case STRIKER_CHALLENGE_LUIGI:
        unlockFlag = 0x2000;
        break;
    case STRIKER_CHALLENGE_PEACH:
        unlockFlag = 0x4000;
        break;
    case STRIKER_CHALLENGE_WALUIGI:
        unlockFlag = 0x8000;
        break;
    case STRIKER_CHALLENGE_WARIO:
        unlockFlag = 0x10000;
        break;
    case STRIKER_CHALLENGE_YOSHI:
        unlockFlag = 0x20000;
        break;
    case STRIKER_CHALLENGE_BOWSER_JR:
        unlockFlag = 0x40000;
        break;
    case STRIKER_CHALLENGE_DIDDY_KONG:
        unlockFlag = 0x80000;
        break;
    case STRIKER_CHALLENGE_PETEY:
        unlockFlag = 0x100000;
        break;
    case TUTORIAL_CHARACTER_DIFFERENCES:
        unlockFlag = 0x200000;
        break;
    case TUTORIAL_BALL_CHARGE:
        unlockFlag = 0x400000;
        break;
    case TUTORIAL_MEGA_SAVES:
        unlockFlag = 0x800000;
        break;
    case TUTORIAL_MEGA_STRIKE:
        unlockFlag = 0x1000000;
        break;
    case TUTORIAL_SIDEKICK_SKILLSHOT:
        unlockFlag = 0x2000000;
        break;
    case TUTORIAL_SKILLSHOT_PART_TWO:
        unlockFlag = 0x4000000;
        break;
    case TUTORIAL_SUPER_MARIO:
        unlockFlag = 0x8000000;
        break;
    case TUTORIAL_BRING_THUNDER:
        unlockFlag = 0x10000000;
        break;
    case TUTORIAL_POWER_UPS:
        unlockFlag = 0x20000000;
        break;
    case TUTORIAL_WIN_GAME:
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

const char* StrikerChallenge::GetDifficulty(int challenge) const
{
    return gStrikerChallengeDefinitions[challenge].mDifficulty;
}

const char* StrikerChallenge::GetConfigPath() const
{
    return gStrikerChallengeDefinitions[mCurrentChallenge].mConfigPath;
}

const char* StrikerChallenge::GetName() const
{
    return gStrikerChallengeDefinitions[mCurrentChallenge].mName;
}

const char* StrikerChallenge::GetTitle() const
{
    return gStrikerChallengeDefinitions[mCurrentChallenge].mTitle;
}

const char* StrikerChallenge::GetTitle(int challenge) const
{
    return gStrikerChallengeDefinitions[challenge].mTitle;
}

bool StrikerChallenge::IsCurrentChallengeWon() const
{
    bool won = false;
    eTeamSide side = HOME;
    eTeamSide opponent = side ? HOME : AWAY;
    const BasicGameInfo* info = GameInfoManager::Instance()->GetCurrentGameInfo();
    switch (mCondition)
    {
    case CHALLENGE_WIN:
        if (info->GetFinalScore(side) > info->GetFinalScore(opponent))
        {
            won = true;
        }
        break;
    case CHALLENGE_WIN_BY_MARGIN:
        if (info->GetFinalScore(side) >= mWinParameter + info->GetFinalScore(opponent))
        {
            won = true;
        }
        break;
    case CHALLENGE_SHUTOUT:
        if (g_pTeams[side]->m_nScore > g_pTeams[opponent]->m_nScore && g_pTeams[opponent]->m_nScore == 0)
        {
            won = true;
        }
        break;
    case CHALLENGE_WIN_WITH_MINIMUM_GOALS:
        if (info->GetFinalScore(side) > info->GetFinalScore(opponent) && info->GetFinalScore(side) >= mWinParameter)
        {
            won = true;
        }
        break;
    }
    return won;
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
