#ifndef GAME_NETWORK_STATS_MANAGER_H
#define GAME_NETWORK_STATS_MANAGER_H

#include "Game/NetworkStats.h"
#include "Game/NetworkSeasonCalendar.h"
#include "NL/CircularQueue.h"
#include "types.h"

enum NetworkPersistentCategory
{
    NETWORK_PERSISTENT_SEASON = 0,
    NETWORK_PERSISTENT_STRIKER_OF_DAY = 1,
    NETWORK_PERSISTENT_FRIENDS = 2,
    NETWORK_PERSISTENT_ALTERNATE_SEASON = 3,
    NETWORK_PERSISTENT_ALTERNATE_STRIKER_OF_DAY = 4
};

struct NetworkLeaderboardCategory
{
    int FindPlayer(int profileId) const;
    int GetCount() const { return mCount; }

    /* 0x0000 */ NetworkPersistentCategory mPersistentCategory;
    /* 0x0004 */ int mFilter;
    /* 0x0008 */ int mLocalStatsCategory;
    /* 0x000C */ bool mAvailable;
    /* 0x000D */ u8 mPadding000D[3];
    /* 0x0010 */ int mCount;
    /* 0x0014 */ int mLocalPlayerIndex;
    /* 0x0018 */ NetworkStatsPlayer mPlayers[65];
    /* 0x1A80 */ NetworkRankingMeta mMetadata[65];
}; // size: 0x2098

class NetworkStatsManager : public NetworkStatsListener
{
public:
    NetworkStatsManager()
        : mLastTotalPoints(0)
        , mLastGameWon(false)
        , mLastGameTied(false)
        , mLastResultPoints(0)
        , mLastGoalPoints(0)
        , mLastBonusPoints(0)
    {
        Reset(true);
    }

    static void CreateInstance();
    static NetworkStatsManager* Instance();

    void Reset(bool initialize);
    bool UsesEuropeanRankings() const;
    NetworkLeaderboardCategory* GetCategory(int category);
    bool RequestRankings(int category);
    void ApplyLeaderboardToSave(
        NetworkLeaderboardCategory* leaderboard, bool updateProfile);
    void CommitPendingOnlineTotals(NetworkRankingMeta* record);
    void UpdateFriendRankingNames(NetworkLeaderboardCategory* leaderboard);
    void BuildFriendsLeaderboard();
    bool PostResetMyPlayerStats(int category, bool useExistingStats);
    void UpdateOnlineResultTotals(
        int result, bool home, int homeScore, int awayScore);
    bool ShouldRestoreDefaultDisconnectLoss();
    void ReportDefaultDisconnectLoss();
    void ReportGameResult(int result, NetworkStatsPlayer* home,
        NetworkStatsPlayer* away, bool reportHome, int homeScore,
        int awayScore, const NetworkScoreSubmission* fallback);
    void SubmitJob(int job);
    void RefreshFriendCount();
    void ClearGameResultReported();
    void ResetPregameDisconnectState();
    void MarkDisconnectPending();
    void PreGameRestoreDefaultDisconnectLoss();
    bool RefreshRankings();
    void BeginOnlineGame();
    void Update(float dt);
    void ReportDisconnect(int result);
    void CalculateAndReportGameResult(int result);

    NetworkRankingMeta* GetLocalStats(int category)
    {
        return mHasLocalStats[category] ? &mLocalStats[category] : 0;
    }

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
    /* 0x0024 */ NetworkRankingMeta mLocalStats[3];
    /* 0x006C */ NetworkLeaderboardCategory mCategories[6];
    /* 0xC3FC */ NetworkPersistentCategory mPersistentCategories[3];
    /* 0xC408 */ float mCurrentTime;
    /* 0xC40C */ float mOperationStartTime;
    /* 0xC410 */ int mNextLeaderboardJobIndex;
    /* 0xC414 */ int mCachedFriendCount;
    /* 0xC418 */ bool mStatsError;
    /* 0xC419 */ bool mSaveDataChanged;
    /* 0xC41A */ bool mGameResultReported;
    /* 0xC41B */ bool mDisconnectPending;
    /* 0xC41C */ int mDisconnectPointsLost[3];
    /* 0xC428 */ bool mDisconnectLossPending[3];
    /* 0xC42B */ u8 mPaddingC42B;
    /* 0xC42C */ int mLastTotalPoints;
    /* 0xC430 */ bool mLastGameWon;
    /* 0xC431 */ bool mLastGameTied;
    /* 0xC432 */ u8 mPaddingC432[2];
    /* 0xC434 */ int mLastResultPoints;
    /* 0xC438 */ int mLastGoalPoints;
    /* 0xC43C */ int mLastBonusPoints;
    /* 0xC440 */ StaticCircularQueue<int, 10> mJobs;
}; // size: 0xC478

int CalculateNetworkResultPoints(int result, bool home, int homeScore,
    int awayScore, bool* won, bool* tied, int* resultPoints,
    int* goalPoints, int* bonusPoints);
int GetLocalNetworkPlayingSide();
bool IsNewNetworkSeason(const NetworkRankingMeta* previous);
bool IsNewNetworkDay(const NetworkRankingMeta* previous);
#endif // GAME_NETWORK_STATS_MANAGER_H
