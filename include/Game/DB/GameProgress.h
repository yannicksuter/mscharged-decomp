#ifndef GAME_DB_CUP_MANAGER_H
#define GAME_DB_CUP_MANAGER_H

#include "Game/DB/Cup.h"
#include "Game/DB/CupInterface.h"
#include "Game/GameInfo.h"
#include "types.h"

struct OSCalendarTime;

struct CupHistoryRecord
{
    bool IsEmpty() const
    {
        bool emptyThrough04, emptyThrough07, emptyThrough0A, emptyThrough0D,
            emptyThrough12, emptyThrough16, emptyThrough20, emptyThrough2B,
            emptyThrough32, empty;
        emptyThrough04 = emptyThrough07 = emptyThrough0A = emptyThrough0D
            = emptyThrough12 = emptyThrough16 = emptyThrough20 = emptyThrough2B
            = emptyThrough32 = empty = false;

        if (mCaptain == 0 && mSidekick1 == 0)
            emptyThrough04 = true;
        if (emptyThrough04 && mSidekick2 == 0)
            emptyThrough07 = true;
        if (emptyThrough07 && mSidekick3 == 0)
            emptyThrough0A = true;
        if (emptyThrough0A && mDay == 0)
            emptyThrough0D = true;
        if (emptyThrough0D && mMonth == 0)
            emptyThrough12 = true;
        if (emptyThrough12 && mYearOffset == 0)
            emptyThrough16 = true;
        if (emptyThrough16 && mGoals == 0)
            emptyThrough20 = true;
        if (emptyThrough20 && mUnidentified2B == 0)
            emptyThrough2B = true;
        if (emptyThrough2B && mUnidentified32 == 0)
            emptyThrough32 = true;
        if (emptyThrough32 && mUnidentified39 == 0)
            empty = true;
        return empty;
    }

    unsigned int mCaptain : 4;
    unsigned int mSidekick1 : 3;
    unsigned int mSidekick2 : 3;
    unsigned int mSidekick3 : 3;
    unsigned int mDay : 5;
    unsigned int mMonth : 4;
    unsigned int mYearOffset : 10;
    unsigned int mGoals : 11;
    unsigned int mUnidentified2B : 7;
    unsigned int mUnidentified32 : 7;
    unsigned int mUnidentified39 : 7;
};

struct CupRecordCounters
{
    CupRecordCounters()
    {
        mValues[0] = 0;
        mValues[1] = 0;
        mValues[2] = 0;
    }

    CupRecordCounters(int value0, int value1, int value2)
    {
        mValues[0] = value0;
        mValues[1] = value1;
        mValues[2] = value2;
    }

    u16 mValues[3];
};

struct CupHistory
{
    CupHistoryRecord mRecords[9][12];
    u8 mWriteIndex[9];
};

struct CupProgressRecord
{
    CupProgressRecord()
        : mUnlockFlags(0)
    {
        memset(mHistory.mRecords, 0, sizeof(mHistory.mRecords));
        memset(mHistory.mWriteIndex, 0, sizeof(mHistory.mWriteIndex));
    }

    CupRecordCounters mCurrentRecord;
    CupRecordCounters mSavedRecord;
    u16 mUnlockFlags : 9;
    u16 mUnidentified86AD : 7;
    u8 unknown_0x86AE[2];
    CupHistory mHistory;
};

void AppendCupHistoryRecord(CupHistory* history, int index, OSCalendarTime* date,
    int captain, CupSidekicks* sidekicks, TeamStats* stats, CupProgressRecord records);

class CupManager : public CupInterface
{
public:
    CupManager();

    virtual BasicGameInfo* GetGameInfo(int phase, int matchup);
    virtual bool HasGameBeenPlayed(int phase, int matchup);
    virtual NetworkTournamentGame* GetTournamentGame(int phase, int matchup);
    virtual BasicGameInfo* GetCurrentGameInfo();
    virtual u16 GetNumGamesPerRound(int phase, int round) const;
    virtual u16 GetNumGames(int phase) const;
    virtual int GetCurrentMode() const;
    virtual int GetCupPersona() const;
    virtual bool IsCupWinningGame(int team) const;
    virtual s16 GetCurrentRoundNumber() const;
    virtual int GetCurrentRoundType() const;
    virtual u16 GetNumPlayoffRounds() const;
    virtual ~CupManager();

    static CupManager* Instance();
    int GetGoalsAgainstLeader(int* statistic);
    int GetGoalsForLeader(int* statistic);
    s16 GetNextRoundNumber(int* roundType);
    int GetTeamRank(int team);
    TeamStats GetTeamStats(int team);
    u16 GetNumRegularRounds() const { return mCurrentCup->GetNumRegularRounds(); }
    u16 GetNumPlayingTeams() const;
    TeamStats GetTeamStatsByIndex(u16 index);
    TeamStats* pGetTeamStatsByIndex(u16 index) const;
    BasicGameInfo* GetMatchupInfo(int phase, short round, int matchup) const;
    eTeamID GetUserSelectedCupTeam() const;
    void SetSidekicks(GameRules sidekicks) { mPendingCupSidekicks = sidekicks; }
    int GetPendingCupTeam() const { return mPendingCupTeam; }
    int GetPreviousGameTeam(int index) const;
    bool ShouldShowCupPhasePopup() const;
    void SetShowCupPhasePopup(bool value);
    void RestoreCupRecord();
    void RestartCupSeries();
    void ShowRoundNews();
    void RecordCupUnlock(int index);
    int GetFinalOpponentTeam() const;
    void UpdateCurrentCup();
    void SetMode(int mode);
    void ResetCupRecord();
    void SaveCupRecord();
    int GetNumPlayoffTeams() const;
    int PickStadium(bool isLastRound) const;
    void PickRoundStadiums(int* stadiums);
    void SetupRoundRobinSchedule(int* lineup, CupSidekicks* sklineup);
    void AssignTeamSkillLevels();
    void SetupPlayoffSchedule();
    bool DetermineNextMatchups(int dnmflags);
    void AdvanceToNextUserGame();
    void PrepareCurrentGame();
    void StartCupSeries();
    void SetRoundResult(bool inOvertime, int winningSide);
    void IncreaseGameNumber(bool shouldIncreaseRound);
    void IncreaseRoundNumber();
    void SetUserSelectedCupTeam(int team);
    void SetUserSelectedCupSidekicks(CupSidekicks sidekicks);
    CupSidekicks GetUserSelectedCupSidekicks() const { return mCurrentCup->mUserSelectedSidekick; }
    CupSidekicks GetPendingCupSidekicks() const
    {
        return mPendingCupSidekicks;
    }
    void SelectFinalOpponents();
    int GetUserTeamRank();
    void AwardGoalTrophies();
    void ForfeitCurrentGame();
    u32 GetUnlockFlags() const { return mCupRecord.mUnlockFlags; }
    bool HasUnlockFlag(int flag) const { return (mCupRecord.mUnlockFlags & flag) != 0; }

    int GetSaveDataSize() const;
    int GetProgressDataSize() const
    {
        return (const char*)&mCurrentMode - (const char*)&mState + sizeof(mCurrentMode);
    }

    void* SerializeData(void* dst) const;
    void* DeserializeData(void* src);

    /* 0x0004 */ Cup<4, 8> mFireCupSeries;
    /* 0x14F0 */ Cup<6, 12> mCrystalCupSeries;
    /* 0x41DC */ Cup<10, 11> mStrikerCupSeries;
    /* 0x8680 */ int mState;
    /* 0x8684 */ int mFinalOpponentTeams[4];
    /* 0x8694 */ int mPreviousGameTeams[2];
    /* 0x869C */ bool mGameInProgress;
    /* 0x869D */ bool mForceHighestSkillLevel;
    /* 0x869E */ u8 unknown_0x869E[2];
    /* 0x86A0 */ CupProgressRecord mCupRecord;
    /* 0x8A1C */ int mCurrentMode;
    /* 0x8A20 */ BaseCup* mCurrentCup;
    /* 0x8A24 */ bool mShowCupPhasePopup;
    /* 0x8A25 */ u8 unknown_0x8A25[3];
    /* 0x8A28 */ int mPendingCupTeam;
    /* 0x8A2C */ GameRules mPendingCupSidekicks;
    /* 0x8A38 */ u32 mPreGameUnlockedState;
};

extern CupManager* g_pCupManager;
inline CupManager* CupManager::Instance() { return g_pCupManager; }

struct ChallengeCompletionDate
{
    u32 mDay : 5;
    u32 mMonth : 4;
    u32 mYearOffset : 10;
    u32 mUnidentified : 13;
};

struct ChallengeUnlockRecord
{
    bool IsUnlocked(int flag) const;

    ChallengeCompletionDate mCompletionDates[12];
    u32 mUnlockedChallenges;
};

class StrikerChallenge
{
public:
    StrikerChallenge();
    virtual ~StrikerChallenge();

    int GetCaptain(int challenge) const;
    int GetCurrentCaptain() const;
    void SetCurrentChallenge(int challenge);
    void LoadSettings();
    bool IsUnlocked(int challenge) const;
    bool UnlockCurrentChallenge();
    const char* GetDifficulty(int challenge) const;
    const char* GetConfigPath() const;
    const char* GetName() const;
    const char* GetTitle() const;
    const char* GetTitle(int challenge) const;
    int GetCurrentChallenge() const;
    int GetRemainingTime() const;
    int GetScore(int side) const;
    bool IsCurrentChallengeWon() const;
    void* SerializeData(void* dst) const;
    void* DeserializeData(void* src);

    /* 0x04 */ int mRemainingTime;
    /* 0x08 */ int mAIDifficulty;
    /* 0x0C */ int mCondition;
    /* 0x10 */ int mCaptain;
    /* 0x14 */ int mWinParameter;
    /* 0x18 */ int mScore[2];
    /* 0x20 */ int mMissingSidekicks[2];
    /* 0x28 */ bool mHomePowerupsEnabled;
    /* 0x29 */ bool mAwayPowerupsEnabled;
    /* 0x2A */ bool mHomeMegastrikeEnabled;
    /* 0x2B */ bool mAwayMegastrikeEnabled;
    /* 0x2C */ bool mHomeSkillshotDisabled;
    /* 0x2D */ bool mAwaySkillshotDisabled;
    /* 0x2E */ bool mStunnedHomeGoalies;
    /* 0x2F */ bool mStunnedAwayGoalies;
    /* 0x30 */ int mCustomPowerups;
    /* 0x34 */ int mCurrentChallenge;
    /* 0x38 */ ChallengeUnlockRecord mUnlocks;
    /* 0x6C */ u8 mChallengeOffset;
    /* 0x6D */ s8 mHeadlineVariant;
    /* 0x6E */ u8 mPadding6E[2];
};

extern StrikerChallenge* g_pStrikerChallenge;
StrikerChallenge* GetStrikerChallenge();

void RecordChallengeUnlock(ChallengeUnlockRecord* record, int flag);

bool IsUnlockFlagSet(int flag);
void SetUnlockFlag(int flag);
bool GetUnlockAll();
void SetUnlockAll(bool value);

bool IsWastelandsUnlocked();
bool IsDumpUnlocked();
bool IsGalacticStadiumUnlocked();
bool IsStormshipUnlocked();
bool IsCrystalCanyonUnlocked();
bool IsLavaPitUnlocked();

bool IsBowserJrUnlocked();

bool IsDiddyKongUnlocked();

bool IsPeteyUnlocked();

bool IsSecureEnvironmentCheatUnlocked();

bool IsPowerEnvironmentCheatUnlocked();

bool IsVoltageEnvironmentCheatUnlocked();

bool IsTiltEnvironmentCheatUnlocked();

bool IsWhiteBallEnvironmentCheatUnlocked();

bool IsPowerupCheatsUnlocked();

bool IsSuperPowerupsCheatUnlocked();

bool IsDevastatingPlayerCheatUnlocked();

bool IsSafePlayerCheatUnlocked();

bool IsSkillShotPlayerCheatUnlocked();

bool IsGlassJawPlayerCheatUnlocked();

bool IsButterfingersPlayerCheatUnlocked();

// Shared functions and data from Game/DB/GameProgress.cpp.
void SavePreGameUnlockState();
bool WereUnlockFlagsClearBeforeGame(unsigned int flags);
bool HasWastelandsUnlockFlags();
bool HasDumpUnlockFlags();
bool HasGalacticStadiumUnlockFlags();
bool WasWastelandsLockedBeforeGame();
bool WasDumpLockedBeforeGame();
bool WasGalacticStadiumLockedBeforeGame();

#endif // GAME_DB_CUP_MANAGER_H
