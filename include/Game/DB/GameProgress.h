#ifndef GAME_DB_GAME_PROGRESS_H
#define GAME_DB_GAME_PROGRESS_H

#include "Game/DB/Cup.h"
#include "types.h"

struct CupHistoryRecord
{
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

class CupManager
{
public:
    virtual BasicGameInfo* GetGameInfo(int phase, int matchup);
    virtual bool HasGameBeenPlayed(int phase, int matchup);
    virtual int fn_8010F1E8() const;
    virtual BasicGameInfo* GetCurrentGameInfo();
    virtual s16 GetNextRoundNumber(int* roundType) const;
    virtual s16 GetPreviousRoundNumber(int* roundType) const;
    virtual int GetCurrentMode() const;
    virtual int fn_8010F1C8() const;
    virtual void fn_8010D94C();
    virtual s16 GetCurrentRoundNumber() const;
    virtual int GetCurrentRoundType() const;
    virtual int fn_8010F1D0() const;
    virtual ~CupManager();

    u16 GetNumPlayingTeams() const;
    TeamStats GetTeamStatsByIndex(u16 index) const;
    TeamStats* pGetTeamStatsByIndex(u16 index) const;
    BasicGameInfo* GetMatchupInfo(int phase, short round, int matchup) const;
    int GetUserSelectedCupTeam() const;
    int fn_80206B28(int index) const;
    bool fn_80206B3C() const;
    void fn_80206B48(bool value);

    /* 0x0004 */ Cup<4, 8> mFireCupSeries;
    /* 0x14F0 */ Cup<6, 12> mCrystalCupSeries;
    /* 0x41DC */ Cup<10, 11> mStrikerCupSeries;
    /* 0x8680 */ int mUnidentified8680;
    /* 0x8684 */ u8 unknown_0x8684[0x10];
    /* 0x8694 */ int mUnidentified8694[2];
    /* 0x869C */ bool mUnidentified869C;
    /* 0x869D */ u8 unknown_0x869D[0x37F];
    /* 0x8A1C */ int mCurrentMode;
    /* 0x8A20 */ BaseCup* mCurrentCup;
    /* 0x8A24 */ bool unknown_0x8A24;
    /* 0x8A25 */ u8 unknown_0x8A25[3];
    /* 0x8A28 */ int unknown_0x8A28;
    /* 0x8A2C */ int unknown_0x8A2C;
    /* 0x8A30 */ int unknown_0x8A30;
    /* 0x8A34 */ int unknown_0x8A34;
    /* 0x8A38 */ u32 unknown_0x8A38;
};

extern CupManager* g_pCupManager;

class StrikerChallenge
{
public:
    StrikerChallenge();
    virtual ~StrikerChallenge();

    int GetCaptain(int challenge) const;
    void SetCurrentChallenge(int challenge);
    void LoadSettings();
    bool IsUnlocked(int challenge) const;
    bool UnlockCurrentChallenge();
    const char* GetConfigPath(int challenge) const;
    const char* GetName() const;
    const char* GetTitle() const;
    const char* GetDifficulty() const;
    const char* GetDifficulty(int challenge) const;
    bool IsCurrentChallengeWon() const;
    void* SerializeData(void* dst) const;
    void* DeserializeData(void* src);

    /* 0x04 */ int mRemainingTime;
    /* 0x08 */ int mAIDifficulty;
    /* 0x0C */ int mCondition;
    /* 0x10 */ int mCaptain;
    /* 0x14 */ int mWinParameter;
    /* 0x18 */ int mHomeScore;
    /* 0x1C */ int mAwayScore;
    /* 0x20 */ int mHomeMissingSidekicks;
    /* 0x24 */ int mAwayMissingSidekicks;
    /* 0x28 */ bool mHomePowerupsEnabled;
    /* 0x29 */ bool mAwayPowerupsEnabled;
    /* 0x2A */ bool mHomeMegastrikeEnabled;
    /* 0x2B */ bool mAwayMegastrikeEnabled;
    /* 0x2C */ bool mHomeSkillshotEnabled;
    /* 0x2D */ bool mAwaySkillshotEnabled;
    /* 0x2E */ bool mStunnedHomeGoalies;
    /* 0x2F */ bool mStunnedAwayGoalies;
    /* 0x30 */ int mCustomPowerups;
    /* 0x34 */ int mCurrentChallenge;
    /* 0x38 */ u32 mCompletionDates[12];
    /* 0x68 */ u32 mUnlockedChallenges;
    /* 0x6C */ u8 mUnidentified6C;
    /* 0x6D */ s8 mHeadlineVariant;
    /* 0x6E */ u8 mPadding6E[2];
};

extern StrikerChallenge* g_pStrikerChallenge;

void SetUnlockFlag(u32 unlockFlag);
bool IsUnlockFlagSet(u32 unlockFlag);
void RecordChallengeUnlock(u32* completionData, u32 unlockFlag);

int fn_8011162C();

bool IsBowserJrUnlocked();
bool IsDiddyKongUnlocked();
bool IsPeteyUnlocked();

bool IsWastelandsUnlocked();
bool IsDumpUnlocked();
bool IsGalacticStadiumUnlocked();
bool IsStormshipUnlocked();
bool IsCrystalCanyonUnlocked();
bool IsLavaPitUnlocked();

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

#endif // GAME_DB_GAME_PROGRESS_H
