#ifndef GAME_NETWORK_STATS_H
#define GAME_NETWORK_STATS_H

#include "NL/nlMemory.h"
#include "types.h"

#include <string.h>

struct NetworkStatsPlayer
{
    NetworkStatsPlayer()
    {
        mName[0] = 0;
        mProfileId = 0;
        memset(mData, 0, sizeof(mData));
    }

    void CopyFrom(const NetworkStatsPlayer& other);

    /* 0x00 */ int mProfileId;
    /* 0x04 */ u16 mName[11];
    /* 0x1A */ u8 mData[0x4C];
    /* 0x66 */ u8 mPadding66[2];
}; // size: 0x68

struct NetworkRankingMeta
{
    /* 0x00 */ u8 mMonth;
    /* 0x01 */ u8 mDay;
    /* 0x02 */ u16 mYear;
    /* 0x04 */ int mScore;
    /* 0x08 */ int mDisplayRank;
    /* 0x0C */ int mWins;
    /* 0x10 */ int mLosses;
    /* 0x14 */ int mUnidentified14;
}; // size: 0x18

struct NetworkScoreSubmission
{
    /* 0x00 */ u8 mMonth;
    /* 0x01 */ u8 mDay;
    /* 0x02 */ u16 mYear;
    /* 0x04 */ int mScore;
    /* 0x08 */ int mUnidentified08;
    /* 0x0C */ int mWins;
    /* 0x10 */ int mLosses;
    /* 0x14 */ u16 mPlayerId;
    /* 0x16 */ u8 mPadding16[2];
}; // size: 0x18

class NetworkStatsListener
{
public:
    virtual void StatsListenerVirtual00() = 0;
    virtual void OnLeaderboardResult(bool success, int category, int filter,
        int count, NetworkStatsPlayer* players, NetworkRankingMeta* metadata) = 0;
    virtual void OnSubmitScoreResult(bool success, int category) = 0;
    virtual void OnReportGameResult(bool success, int category) = 0;
};

class NetworkStatsInterface
{
public:
    virtual void SetListener(NetworkStatsListener* listener) = 0;
    virtual bool SubmitScore(
        int category, const NetworkRankingMeta* submission) = 0;
    virtual bool ReportGameResult(int category,
        const NetworkScoreSubmission* submission,
        const NetworkStatsPlayer* home,
        const NetworkStatsPlayer* away,
        bool reportHome,
        int homeScore,
        int awayScore,
        const NetworkScoreSubmission* fallback) = 0;
    virtual bool StatsVirtual0C() = 0;
    virtual bool GetLeaderboardStats(int category, int filter, int limit,
        NetworkStatsPlayer* players, NetworkRankingMeta* metadata) = 0;
    virtual void Update() = 0;
};

struct NetworkStatsSocket
{
    int mSocket;
};

class NetworkStatsReporter_8012CE20 : public NetworkStatsInterface
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkStatsReporter_8012CE20();

    void Reset();
    void Close();
    void ParseLeaderboardResponse(char* data, int size);

    virtual void SetListener(NetworkStatsListener* listener);
    virtual bool SubmitScore(
        int category, const NetworkRankingMeta* submission);
    virtual bool ReportGameResult(int category,
        const NetworkScoreSubmission* submission,
        const NetworkStatsPlayer* home,
        const NetworkStatsPlayer* away,
        bool reportHome,
        int homeScore,
        int awayScore,
        const NetworkScoreSubmission* fallback);
    virtual bool StatsVirtual0C();
    virtual bool GetLeaderboardStats(int category, int filter, int limit,
        NetworkStatsPlayer* players, NetworkRankingMeta* metadata);
    virtual void Update();

    /* 0x004 */ NetworkStatsListener* mListener;
    /* 0x008 */ NetworkStatsSocket mSocket;
    /* 0x00C */ int mState;
    /* 0x010 */ int mFilter;
    /* 0x014 */ int mLimit;
    /* 0x018 */ NetworkStatsPlayer* mLeaderboardPlayers;
    /* 0x01C */ NetworkRankingMeta* mLeaderboardMetadata;
    /* 0x020 */ NetworkStatsPlayer mHomePlayer;
    /* 0x088 */ NetworkStatsPlayer mAwayPlayer;
    /* 0x0F0 */ bool mReportHome;
    /* 0x0F1 */ u8 mPadding0F1[3];
    /* 0x0F4 */ int mHomeScore;
    /* 0x0F8 */ int mAwayScore;
    /* 0x0FC */ u32 mReportStartTime;
}; // size: 0x100

struct NetworkRankingSubmission
{
    NetworkRankingSubmission()
        : mMonth(1)
        , mDay(1)
        , mYear(2000)
        , mWins(0)
        , mLosses(0)
        , mUnidentified0C(0)
    {
        mName[0] = 0;
        memset(mData, 0, sizeof(mData));
        memset(mDigest, 0, sizeof(mDigest));
    }

    /* 0x00 */ u8 mMonth;
    /* 0x01 */ u8 mDay;
    /* 0x02 */ u16 mYear;
    /* 0x04 */ int mWins;
    /* 0x08 */ int mLosses;
    /* 0x0C */ u16 mUnidentified0C;
    /* 0x0E */ u16 mName[11];
    /* 0x24 */ u8 mData[0x4C];
    /* 0x70 */ u8 mDigest[0x10];
}; // size: 0x80

struct NetworkRankingSortRow
{
    NetworkRankingSortRow();

    /* 0x00 */ NetworkStatsPlayer mPlayer;
    /* 0x68 */ NetworkRankingMeta mMetadata;
}; // size: 0x80

struct NetworkRankingIdentity
{
    void LoadLocal();

    /* 0x00 */ u8 mMonth;
    /* 0x01 */ u8 mDay;
    /* 0x02 */ u16 mYear;
}; // size: 0x4

class NetworkRanking_8012D8F4 : public NetworkStatsInterface
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkRanking_8012D8F4();

    void Reset();
    void ShutdownRanking();
    void InitializeRanking();
    void ProcessLeaderboardResults();
    void FilterCurrentSeason(int count);
    void SortLeaderboardResults(int count);
    static int CompareLeaderboardRows(const void* left, const void* right);
    static void AssignDisplayRanks(
        int count, NetworkRankingMeta* metadata, int firstRank);

    virtual void SetListener(NetworkStatsListener* listener);
    virtual bool SubmitScore(
        int category, const NetworkRankingMeta* submission);
    virtual bool ReportGameResult(int category,
        const NetworkScoreSubmission* submission,
        const NetworkStatsPlayer* home,
        const NetworkStatsPlayer* away,
        bool reportHome,
        int homeScore,
        int awayScore,
        const NetworkScoreSubmission* fallback);
    virtual bool StatsVirtual0C();
    virtual bool GetLeaderboardStats(int category, int filter, int limit,
        NetworkStatsPlayer* players, NetworkRankingMeta* metadata);
    virtual void Update();

    /* 0x004 */ bool mInitialized;
    /* 0x005 */ bool mRequestComplete;
    /* 0x006 */ u8 mPadding006[2];
    /* 0x008 */ int mOperation;
    /* 0x00C */ NetworkStatsListener* mListener;
    /* 0x010 */ NetworkRankingSubmission mSubmission;
    /* 0x090 */ bool mReportGame;
    /* 0x091 */ u8 mPadding091[3];
    /* 0x094 */ int mFilter;
    /* 0x098 */ int mLimit;
    /* 0x09C */ NetworkStatsPlayer* mLeaderboardPlayers;
    /* 0x0A0 */ NetworkRankingMeta* mLeaderboardMetadata;
    /* 0x0A4 */ int mCategory;
}; // size: 0xA8

#endif // GAME_NETWORK_STATS_H
