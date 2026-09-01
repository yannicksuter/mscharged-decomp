#ifndef GAME_NETWORK_STATS_MANAGER_H
#define GAME_NETWORK_STATS_MANAGER_H

#include "Game/NetworkStats.h"
#include "types.h"

struct DWCDate;
struct DWCTime;

struct NetworkRankingRecord : NetworkRankingMeta
{
    NetworkRankingRecord()
    {
        mMonth = 1;
        mDay = 1;
        mYear = 2000;
        mScore = 0;
        mDisplayRank = 0;
        mWins = 0;
        mLosses = 0;
        mUnidentified14 = 0;
    }
}; // size: 0x18

struct NetworkLeaderboardCategory
{
    int FindPlayer(int profileId) const;

    /* 0x0000 */ int mPersistentCategory;
    /* 0x0004 */ int mFilter;
    /* 0x0008 */ int mResultType;
    /* 0x000C */ bool mAvailable;
    /* 0x000D */ u8 mPadding000D[3];
    /* 0x0010 */ int mCount;
    /* 0x0014 */ int mFirstRank;
    /* 0x0018 */ NetworkStatsPlayer mPlayers[65];
    /* 0x1A80 */ NetworkRankingRecord mMetadata[65];
}; // size: 0x2098

struct NetworkSeasonDate
{
    int mMonth;
    int mDay;
}; // size: 0x8

struct NetworkSeasonDateTable
{
    NetworkSeasonDateTable(int count, NetworkSeasonDate* dates)
        : mCount(count)
        , mDates(dates)
    {
    }

    int mCount;
    NetworkSeasonDate* mDates;
}; // size: 0x8

class NetworkStatsManager_8012F378 : public NetworkStatsListener
{
public:
    NetworkStatsManager_8012F378()
    {
        mCurrentJob = 0;
        mUnidentifiedC430 = false;
        mUnidentifiedC431 = false;
        mUnidentifiedC434 = 0;
        mUnidentifiedC438 = 0;
        mUnidentifiedC43C = 0;
        mJobs = mJobStorage;
        mJobReadIndex = 0;
        mJobCount = 0;
        mJobCapacity = 10;
        Reset(true);
    }

    static void CreateInstance();
    static NetworkStatsManager_8012F378* Instance();

    void Reset(bool initialize);
    bool UsesEuropeanRankings() const;
    NetworkLeaderboardCategory* GetCategory(int category);
    bool RequestRankings(int category);
    void ApplyLeaderboardToSave(
        NetworkLeaderboardCategory* leaderboard, bool updateProfile);
    void CommitPendingOnlineTotals(NetworkRankingMeta* record);
    void UpdateFriendRankingNames(NetworkLeaderboardCategory* leaderboard);
    void BuildFriendsLeaderboard(NetworkLeaderboardCategory* leaderboard);
    bool PostResetMyPlayerStats(int category, bool useExistingStats);
    void UpdateOnlineResultTotals(
        int result, bool home, int homeScore, int awayScore);
    bool ShouldRestoreDefaultDisconnectLoss();
    void ReportDefaultDisconnectLoss();
    void ReportGameResult(int result, const NetworkStatsPlayer* home,
        const NetworkStatsPlayer* away, bool reportHome, int homeScore,
        int awayScore, const NetworkScoreSubmission* fallback);
    void SubmitJob(int job);
    void RefreshSaveState_801314D0();
    void ClearGameResultReported();
    void ResetPregameDisconnectState();
    void MarkDisconnectPending();
    void PreGameRestoreDefaultDisconnectLoss();
    void RefreshFriendStats_80131B50();
    void BeginOnlineGame_80131DB4();
    void Update(float dt);
    void HandleDisconnect_8013243C(int result);
    void CalculateAndReportGameResult(int result);

    virtual void StatsListenerVirtual00();
    virtual void OnLeaderboardResult(bool success, int category, int filter,
        int count, NetworkStatsPlayer* players,
        NetworkRankingMeta* metadata);
    virtual void OnSubmitScoreResult(bool success, int category);
    virtual void OnReportGameResult(bool success, int category);

    /* 0x0000 */
    /* 0x0004 */ bool mLeaderboardRequestComplete;
    /* 0x0005 */ bool mLeaderboardRequestSucceeded;
    /* 0x0006 */ u8 mPadding0006[2];
    /* 0x0008 */ int mLeaderboardCategory;
    /* 0x000C */ bool mScoreRequestComplete;
    /* 0x000D */ bool mScoreRequestSucceeded;
    /* 0x000E */ u8 mPadding000E[2];
    /* 0x0010 */ int mScoreCategory;
    /* 0x0014 */ int mOperation;
    /* 0x0018 */ int mRequestedCategory;
    /* 0x001C */ int mSubmissionCategory;
    /* 0x0020 */ bool mHasLocalStats[3];
    /* 0x0023 */ u8 mPadding0023;
    /* 0x0024 */ NetworkRankingRecord mLocalStats[3];
    /* 0x006C */ NetworkLeaderboardCategory mCategories[6];
    /* 0xC3FC */ int mPersistentCategories[3];
    /* 0xC408 */ float mCurrentTime;
    /* 0xC40C */ float mOperationStartTime;
    /* 0xC410 */ int mUnidentifiedC410;
    /* 0xC414 */ int mSaveState;
    /* 0xC418 */ bool mStatsError;
    /* 0xC419 */ bool mSaveDataChanged;
    /* 0xC41A */ bool mGameResultReported;
    /* 0xC41B */ bool mDisconnectPending;
    /* 0xC41C */ int mUnidentifiedC41C;
    /* 0xC420 */ int mUnidentifiedC420;
    /* 0xC424 */ int mUnidentifiedC424;
    /* 0xC428 */ bool mDisconnectLossPending[3];
    /* 0xC42B */ u8 mPaddingC42B;
    /* 0xC42C */ int mCurrentJob;
    /* 0xC430 */ bool mUnidentifiedC430;
    /* 0xC431 */ bool mUnidentifiedC431;
    /* 0xC432 */ u8 mPaddingC432[2];
    /* 0xC434 */ int mUnidentifiedC434;
    /* 0xC438 */ int mUnidentifiedC438;
    /* 0xC43C */ int mUnidentifiedC43C;
    /* 0xC440 */ int* mJobs;
    /* 0xC444 */ u32 mJobReadIndex;
    /* 0xC448 */ u32 mJobCount;
    /* 0xC44C */ u32 mJobCapacity;
    /* 0xC450 */ int mJobStorage[10];
}; // size: 0xC478

int CalculateResultPoints_80130684(int result, bool home, int homeScore,
    int awayScore, bool* won, bool* tied, int* resultPoints,
    int* scorePoints, int* bonusPoints);
int GetLocalPlayingSide_801323F4();
bool IsNewNetworkSeason(const NetworkRankingMeta* previous);
bool IsNewNetworkDay(const NetworkRankingMeta* previous);
bool GetAdjustedNetworkDate(DWCDate* date, DWCTime* time);
int FindNetworkSeasonBoundary(
    const NetworkSeasonDateTable* dates, const NetworkSeasonDate* date);
int GetDaysUntilNextSeasonBoundary(
    const NetworkSeasonDateTable* dates, int index, int year);
int GetDaysSinceSeasonBoundary(const NetworkSeasonDateTable* dates, int index,
    const NetworkSeasonDate* date, int year);

extern int g_nAddHoursTime;
extern int g_nAddMinsTime;

#endif // GAME_NETWORK_STATS_MANAGER_H
