#include "NL/nlSingleton.inl"
#include <dwc/dwc_nastime.h>
#include "Game/OnlinePlayer.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/Sys/debug.h"
#include "Game/DB/SaveLoad.h"

#include "Game/NetworkStatsManager.h"
#include "Game/FriendManager.h"

#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/main.h"
#include "Game/MatchSeries.h"
#include "NL/nlMemory.h"
#include "NL/nlstring_tmpl.h"
#include "Game/NetworkInput.h"

#include <string.h>

static int sLeaderboardJobs[5] = { 6, 7, 8, 9, 10 };

static NetworkStatsManager* sNetworkStatsManager;

static float sSecondsPerMinute = 60.0f;
static float sRankingRequestTimeout = 30.0f;

extern NetworkSeasonDate sNetworkSeasonDates[52];
extern int sMonthDays[12];
extern NetworkSeasonDateTable sNetworkSeasonDateTable;

int NetworkLeaderboardCategory::FindPlayer(int profileId) const
{
    for (int i = 0; i < mCount; ++i)
    {
        if (mPlayers[i].mProfileId == profileId)
        {
            return i;
        }
    }
    return -1;
}

void NetworkStatsManager::CreateInstance()
{
    sNetworkStatsManager = new (nlMalloc(sizeof(NetworkStatsManager), 8, false))
        NetworkStatsManager;
}

NetworkStatsManager* NetworkStatsManager::Instance()
{
    return sNetworkStatsManager;
}

void NetworkStatsManager::Reset(bool)
{
    mLeaderboardRequestComplete = false;
    mLeaderboardRequestSucceeded = false;
    mLeaderboardCategory = 2;
    mScoreRequestComplete = false;
    mScoreRequestSucceeded = false;
    mScoreCategory = 0;
    mOperation = 0;
    mRequestedCategory = 2;
    mSubmissionCategory = 0;
    mHasLocalStats[0] = false;
    mHasLocalStats[1] = false;
    mHasLocalStats[2] = false;

    mCurrentTime = 0.0f;
    mOperationStartTime = 0.0f;
    mUnidentifiedC410 = 0;
    mSaveState = 0;
    mStatsError = false;
    mSaveDataChanged = false;
    mGameResultReported = false;
    mDisconnectPending = false;
    mUnidentifiedC41C = 0;
    mDisconnectLossPending[0] = false;
    mUnidentifiedC420 = 0;
    mDisconnectLossPending[1] = false;
    mUnidentifiedC424 = 0;
    mDisconnectLossPending[2] = false;
    mJobReadIndex = 0;
    mJobCount = 0;

    for (int i = 0; i < 6; ++i)
    {
        mCategories[i].mAvailable = false;
        mCategories[i].mCount = 0;
        mCategories[i].mFirstRank = -1;
    }

    bool european = GetRegion() == 1;
    if (european)
    {
        int alternate = UsesEuropeanRankings() ? IsAlternateOnlineCountryGroup() : 0;
        mPersistentCategories[0] = alternate == 1 ? 3 : 0;
        mPersistentCategories[1] = alternate == 1 ? 4 : 1;
        mPersistentCategories[2] = 2;

        mCategories[0].mPersistentCategory = alternate == 1 ? 4 : 1;
        mCategories[0].mFilter = 0;
        mCategories[0].mResultType = 1;
        mCategories[1].mPersistentCategory = alternate == 1 ? 4 : 1;
        mCategories[1].mFilter = 2;
        mCategories[1].mResultType = 1;
        mCategories[2].mPersistentCategory = alternate == 1 ? 3 : 0;
        mCategories[2].mFilter = 0;
        mCategories[2].mResultType = 0;
        mCategories[3].mPersistentCategory = alternate == 1 ? 3 : 0;
        mCategories[3].mFilter = 2;
        mCategories[3].mResultType = 0;
        mCategories[4].mPersistentCategory = 2;
        mCategories[4].mFilter = 1;
        mCategories[4].mResultType = 2;
        mCategories[5].mPersistentCategory = 2;
        mCategories[5].mFilter = 1;
        mCategories[5].mResultType = 2;
    }
    else
    {
        mPersistentCategories[0] = 0;
        mPersistentCategories[1] = 1;
        mPersistentCategories[2] = 2;

        mCategories[0].mPersistentCategory = 1;
        mCategories[0].mFilter = 0;
        mCategories[0].mResultType = 1;
        mCategories[1].mPersistentCategory = 1;
        mCategories[1].mFilter = 2;
        mCategories[1].mResultType = 1;
        mCategories[2].mPersistentCategory = 0;
        mCategories[2].mFilter = 0;
        mCategories[2].mResultType = 0;
        mCategories[3].mPersistentCategory = 0;
        mCategories[3].mFilter = 2;
        mCategories[3].mResultType = 0;
        mCategories[4].mPersistentCategory = 0;
        mCategories[4].mFilter = 1;
        mCategories[4].mResultType = 0;
        mCategories[5].mPersistentCategory = 0;
        mCategories[5].mFilter = 1;
        mCategories[5].mResultType = 0;
    }
}

bool NetworkStatsManager::UsesEuropeanRankings() const
{
    return GetRegion() == 1;
}

NetworkLeaderboardCategory* NetworkStatsManager::GetCategory(
    int category)
{
    return &mCategories[category];
}

bool NetworkStatsManager::RequestRankings(int category)
{
    g_pNetworkSession->GetStatsInterface()->SetListener(this);
    if (mOperation != 0)
    {
        return false;
    }

    mRequestedCategory = category;
    NetworkLeaderboardCategory& leaderboard = mCategories[category];
    leaderboard.mAvailable = false;
    if (g_pNetworkSession->GetStatsInterface()->GetLeaderboardStats(leaderboard.mPersistentCategory,
            leaderboard.mFilter,
            65,
            leaderboard.mPlayers,
            leaderboard.mMetadata))
    {
        mOperation = 3;
        mOperationStartTime = mCurrentTime;
        return true;
    }

    leaderboard.mCount = 0;
    leaderboard.mFirstRank = -1;
    tDebugPrintManager::Print(DC_NETWORK,
        "Initial failure GetLeaderboardStats cat %d filter %d\n",
        leaderboard.mPersistentCategory,
        leaderboard.mFilter);
    mOperation = 0;
    mStatsError = true;
    return false;
}

void NetworkStatsManager::StatsListenerVirtual00()
{
}

void NetworkStatsManager::ApplyLeaderboardToSave(
    NetworkLeaderboardCategory* leaderboard, bool updateProfile)
{
    if (g_pNetworkSessionBase->GetSessionMode() == 2)
    {
        GameInfoSaveSlot* slot = GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex);
        for (int i = 0; i < leaderboard->mCount; ++i)
        {
            if (slot->unknown_0x01C == leaderboard->mPlayers[i].mProfileId
                && leaderboard->mPlayers[i].mName[0] != 0)
            {
                leaderboard->mFirstRank = i;
                bool apply = false;
                if (updateProfile)
                {
                    if (leaderboard->mResultType == 2)
                    {
                        apply = true;
                    }
                    else
                    {
                        apply = leaderboard->mFilter == 0;
                    }
                }
                if (apply)
                {
                    mHasLocalStats[leaderboard->mResultType] = true;
                    mLocalStats[leaderboard->mResultType] = leaderboard->mMetadata[i];
                    if (leaderboard->mResultType == 0 && leaderboard->mFilter == 0)
                    {
                        NetworkRankingMeta& record = mLocalStats[leaderboard->mResultType];
                        int* pendingWins = GameInfoManager::GetInstance()->GetUnknown0xA98(gNetworkSaveSlotIndex);
                        int* pendingLosses = GameInfoManager::GetInstance()->GetUnknown0xA9C(gNetworkSaveSlotIndex);
                        int wins = *pendingWins;
                        int losses = *pendingLosses;
                        if (wins != record.mWins)
                        {
                            *pendingWins = record.mWins;
                            mSaveDataChanged = true;
                        }
                        if (losses != record.mLosses)
                        {
                            *pendingLosses = record.mLosses;
                            mSaveDataChanged = true;
                        }
                    }
                }
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < leaderboard->mCount; ++i)
        {
            if (nlStrICmp(gNetworkMiiNameWide, leaderboard->mPlayers[i].mName) == 0)
            {
                leaderboard->mFirstRank = i;
                if (leaderboard->mFilter == 0)
                {
                    mHasLocalStats[leaderboard->mPersistentCategory] = true;
                    mLocalStats[leaderboard->mPersistentCategory] = leaderboard->mMetadata[i];
                }
                break;
            }
        }
    }
}

void NetworkStatsManager::CommitPendingOnlineTotals(
    NetworkRankingMeta*)
{
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    int* wins = gameInfo->GetUnknown0xA90(gNetworkSaveSlotIndex);
    int* losses = gameInfo->GetUnknown0xA94(gNetworkSaveSlotIndex);
    int* pendingWins = gameInfo->GetUnknown0xA98(gNetworkSaveSlotIndex);
    int* pendingLosses = gameInfo->GetUnknown0xA9C(gNetworkSaveSlotIndex);

    *wins += *pendingWins;
    if (*wins > 9999)
    {
        *wins = 9999;
    }
    *losses += *pendingLosses;
    if (*losses > 9999)
    {
        *losses = 9999;
    }
    *pendingWins = 0;
    *pendingLosses = 0;
    mSaveDataChanged = true;
}

void NetworkStatsManager::UpdateFriendRankingNames(
    NetworkLeaderboardCategory* leaderboard)
{
    if (leaderboard == 0)
    {
        return;
    }

    for (int i = 0; i < leaderboard->mCount; ++i)
    {
        if (leaderboard->mPlayers[i].mProfileId == 0)
        {
            leaderboard->mMetadata[i].mDisplayRank = -1;
        }
    }
}

void NetworkStatsManager::BuildFriendsLeaderboard(
    NetworkLeaderboardCategory* leaderboard)
{
    if (leaderboard == 0)
    {
        return;
    }

    int write = 0;
    for (int read = 0; read < leaderboard->mCount; ++read)
    {
        if (leaderboard->mPlayers[read].mProfileId == 0)
        {
            continue;
        }
        if (write != read)
        {
            leaderboard->mPlayers[write].CopyFrom(
                leaderboard->mPlayers[read]);
            leaderboard->mMetadata[write] = leaderboard->mMetadata[read];
        }
        leaderboard->mMetadata[write].mDisplayRank = write + 1;
        ++write;
    }
    leaderboard->mCount = write;
}

void NetworkStatsManager::OnLeaderboardResult(bool success,
    int category, int filter, int count, NetworkStatsPlayer*,
    NetworkRankingMeta*)
{
    mOperation = 0;
    mLeaderboardRequestComplete = true;
    mLeaderboardCategory = mRequestedCategory;
    NetworkLeaderboardCategory& leaderboard = mCategories[mRequestedCategory];
    if (!success)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Unavailable Leaderboard Stats cat %d filter %d\n",
            category,
            filter);
        mStatsError = true;
        mLeaderboardRequestSucceeded = false;
        leaderboard.mAvailable = false;
        leaderboard.mCount = 0;
        leaderboard.mFirstRank = -1;
        if (mRequestedCategory == 4)
        {
            mCategories[5].mAvailable = false;
            mCategories[5].mCount = 0;
            mCategories[5].mFirstRank = -1;
        }
        return;
    }

    tDebugPrintManager::Print(DC_NETWORK,
        "Sucessfully got leaderboard stats cat %d filter %d\n",
        category,
        filter);
    mLeaderboardRequestSucceeded = true;
    leaderboard.mAvailable = true;
    leaderboard.mCount = count;
    leaderboard.mFirstRank = -1;
    if (filter == 1)
    {
        UpdateFriendRankingNames(&leaderboard);
    }
    ApplyLeaderboardToSave(&leaderboard, true);
    if (mRequestedCategory == 4)
    {
        mCategories[5] = leaderboard;
        BuildFriendsLeaderboard(&mCategories[5]);
        ApplyLeaderboardToSave(&mCategories[5], false);
    }
}

bool NetworkStatsManager::PostResetMyPlayerStats(
    int category, bool useExistingStats)
{
    if (mOperation != 0)
    {
        return false;
    }

    mSubmissionCategory = category;
    if (!useExistingStats)
    {
        mLocalStats[category].mScore = 0;
        mLocalStats[category].mDisplayRank = 0;
        mLocalStats[category].mWins = 0;
        mLocalStats[category].mLosses = 0;
        mLocalStats[category].mUnidentified14 = 0;
        NetworkRankingIdentity* identity = reinterpret_cast<NetworkRankingIdentity*>(&mLocalStats[category]);
        identity->LoadLocal();
    }
    mHasLocalStats[category] = true;

    const NetworkRankingMeta* submission = useExistingStats ? &mLocalStats[category] : 0;
    NetworkRanking* ranking = g_pNetworkSession->GetRankingReporter();
    if (ranking->SubmitScore(mPersistentCategories[category], submission))
    {
        mOperation = 1;
        mOperationStartTime = mCurrentTime;
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK, "Initial failure PostResetMyPlayerStats cat %d\n", category);
        mOperation = 0;
        mStatsError = true;
        return false;
    }
    return true;
}

void NetworkStatsManager::OnSubmitScoreResult(
    bool success, int category)
{
    mOperation = 0;
    mScoreRequestComplete = true;
    mScoreCategory = mSubmissionCategory;
    if ((int)success != 1)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "FinishedPostResetMyPlayerStats returned error cat %d\n",
            category);
        mScoreRequestSucceeded = false;
        mStatsError = true;
    }
    else
    {
        mScoreRequestSucceeded = true;
    }
}

int CalculateResultPoints_80130684(int result, bool home, int homeScore,
    int awayScore, bool* won, bool* tied, int* resultPoints,
    int* scorePoints, int* bonusPoints)
{
    *won = false;
    *tied = false;
    *resultPoints = 0;
    *scorePoints = 0;
    *bonusPoints = 0;

    if (home)
    {
        *scorePoints = homeScore > 10 ? 10 : homeScore;
        if (homeScore > awayScore)
        {
            *resultPoints = 10;
            *won = true;
        }
        else
        {
            *resultPoints = 1;
        }
    }
    else
    {
        *scorePoints = awayScore > 10 ? 10 : awayScore;
        if (awayScore > homeScore)
        {
            *resultPoints = 10;
            *won = true;
        }
        else
        {
            *resultPoints = 1;
        }
    }

    if (result == 3 || result == 4)
    {
        *resultPoints = 0;
        *won = false;
    }
    else if (result == 2)
    {
        *resultPoints = 0;
        *scorePoints = 0;
        *won = false;
    }
    return *resultPoints + *scorePoints + *bonusPoints;
}

void NetworkStatsManager::UpdateOnlineResultTotals(
    int result, bool home, int homeScore, int awayScore)
{
    if (IsOnlineRankedMatch())
    {
        return;
    }

    bool won;
    bool tied;
    int resultPoints;
    int scorePoints;
    int bonusPoints;
    CalculateResultPoints_80130684(result, home, homeScore, awayScore, &won, &tied, &resultPoints, &scorePoints, &bonusPoints);

    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    int* wins = gameInfo->GetUnknown0xAA0(gNetworkSaveSlotIndex);
    int* losses = gameInfo->GetUnknown0xAA4(gNetworkSaveSlotIndex);
    int* counter = won ? wins : losses;
    ++*counter;
    if (*counter > 9999)
    {
        *counter = 9999;
    }
    mSaveDataChanged = true;
}

bool NetworkStatsManager::ShouldRestoreDefaultDisconnectLoss()
{
    if (g_pNetworkSession->GetRankingReporter() != 0)
    {
        int count = UsesEuropeanRankings() ? 3 : 2;
        for (int i = 0; i < count; ++i)
        {
            if (mDisconnectLossPending[i])
            {
                return true;
            }
        }
    }
    return false;
}

void NetworkStatsManager::ReportDefaultDisconnectLoss()
{
    if (mGameResultReported)
    {
        return;
    }
    if (IsNewNetworkDay(&mLocalStats[0]))
    {
        tDebugPrintManager::Print(DC_NETWORK, "Skipping default disconnect loss new day\n");
        return;
    }
    if (IsNewNetworkSeason(&mLocalStats[1]))
    {
        tDebugPrintManager::Print(DC_NETWORK, "Skipping default disconnect loss new season\n");
        return;
    }

    tDebugPrintManager::Print(DC_NETWORK, "Returning Default Disconnect Loss\n");
    for (int i = 0; i < 3; ++i)
    {
        if (mDisconnectLossPending[i])
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "ReportDefaultDisconnectLoss: pers cat %d oldPoints %d new points %d New W:L %d:%d OneBasedRegion:%d\n",
                mPersistentCategories[i],
                mLocalStats[i].mScore,
                mLocalStats[i].mScore,
                mLocalStats[i].mWins,
                mLocalStats[i].mLosses,
                GetRegion() + 1);
        }
    }
}

void NetworkStatsManager::ReportGameResult(int result,
    const NetworkStatsPlayer* home, const NetworkStatsPlayer* away,
    bool reportHome, int homeScore, int awayScore,
    const NetworkScoreSubmission* fallback)
{
    if (g_pNetworkSession->GetRankingReporter() != 0)
    {
        int points = 0;
        bool won = false;
        bool tied = false;
        int resultPoints = 0;
        int scorePoints = 0;
        int bonusPoints = 0;

        if (fallback == 0)
        {
            if (!mGameResultReported)
            {
                points = CalculateResultPoints_80130684(result, reportHome,
                    homeScore, awayScore, &won, &tied, &resultPoints,
                    &scorePoints, &bonusPoints);
                mCurrentJob = points;
                mUnidentifiedC430 = won;
                mUnidentifiedC431 = tied;
                mUnidentifiedC434 = resultPoints;
                mUnidentifiedC438 = scorePoints;
                mUnidentifiedC43C = bonusPoints;
            }
            else if (result == 0)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Already did CalculateAndReportGameResult..ignoring\n");
                return;
            }
        }
        else
        {
            mCurrentJob = 0;
            mUnidentifiedC430 = false;
            mUnidentifiedC431 = false;
            mUnidentifiedC434 = 0;
            mUnidentifiedC438 = 0;
            mUnidentifiedC43C = 0;
        }

        bool restoreDisconnectLoss = result == 0 ? IsCurrentSeriesComplete() : true;
        if (fallback != 0)
        {
            restoreDisconnectLoss = true;
        }

        int categoryCount = UsesEuropeanRankings() ? 3 : 2;
        int* disconnectPoints = &mUnidentifiedC41C;
        for (int category = 0; category < categoryCount; ++category)
        {
            int oldPoints = mLocalStats[category].mScore;
            int pointsScored = points;
            bool startFresh = false;

            if (category == 1)
            {
                if (IsNewNetworkDay(&mLocalStats[category]))
                {
                    tDebugPrintManager::Print(DC_NETWORK, "New day starting score fresh\n");
                    startFresh = true;
                }
            }
            else if (IsNewNetworkSeason(&mLocalStats[category]))
            {
                tDebugPrintManager::Print(DC_NETWORK, "New season starting score fresh\n");
                startFresh = true;
            }

            if (startFresh)
            {
                mLocalStats[category].mScore = pointsScored;
                mLocalStats[category].mWins = 0;
                mLocalStats[category].mLosses = 0;
                if (result != 4 && result != 3 && result != 2)
                {
                    if (won)
                    {
                        ++mLocalStats[category].mWins;
                    }
                    else
                    {
                        ++mLocalStats[category].mLosses;
                    }
                }
                disconnectPoints[category] = 0;
                mDisconnectLossPending[category] = false;
            }
            else if (mDisconnectLossPending[category]
                && restoreDisconnectLoss)
            {
                mLocalStats[category].mScore +=
                    pointsScored + disconnectPoints[category];
                tDebugPrintManager::Print(DC_NETWORK, "Returning Default Disconnect Loss\n");
                disconnectPoints[category] = 0;
                mDisconnectLossPending[category] = false;
                if (result == 2)
                {
                    --mLocalStats[category].mLosses;
                }
                if (result == 3 || result == 4)
                {
                    --mLocalStats[category].mLosses;
                }
                else if (won)
                {
                    ++mLocalStats[category].mWins;
                    --mLocalStats[category].mLosses;
                }
            }
            else
            {
                mLocalStats[category].mScore += pointsScored;
                if (result != 4 && result != 3 && result != 2)
                {
                    if (won)
                    {
                        ++mLocalStats[category].mWins;
                    }
                    else
                    {
                        ++mLocalStats[category].mLosses;
                    }
                }
            }

            if (mLocalStats[category].mScore > 999999)
            {
                mLocalStats[category].mScore = 999999;
            }
            if (mLocalStats[category].mWins > 9999)
            {
                mLocalStats[category].mWins = 9999;
            }
            if (mLocalStats[category].mLosses > 9999)
            {
                mLocalStats[category].mLosses = 9999;
            }
            mLocalStats[category].mUnidentified14 = GetOnlineRegion();

            tDebugPrintManager::Print(DC_NETWORK,
                "ReportGameResult: pers cat %d oldPoints %d + points scored %d = new points %d IWon: %d New W:L %d:%d OneBasedRegion:%d\n",
                category,
                oldPoints,
                pointsScored,
                mLocalStats[category].mScore,
                won,
                mLocalStats[category].mWins,
                mLocalStats[category].mLosses,
                mLocalStats[category].mUnidentified14);

            NetworkRankingIdentity* identity =
                reinterpret_cast<NetworkRankingIdentity*>(
                    &mLocalStats[category]);
            identity->LoadLocal();
        }

        if (categoryCount == 2)
        {
            SubmitJob(0);
            SubmitJob(1);
        }
        else if (categoryCount == 3)
        {
            SubmitJob(0);
            SubmitJob(1);
            SubmitJob(2);
        }
    }
    else if (g_pNetworkSession->GetStatsReporter() != 0)
    {
        NetworkStatsReporter* stats =
            g_pNetworkSession->GetStatsReporter();
        stats->ReportGameResult(0,
            reinterpret_cast<const NetworkScoreSubmission*>(result), home,
            away, reportHome, homeScore, awayScore, 0);
    }
}

void NetworkStatsManager::OnReportGameResult(bool success, int category)
{
    mOperation = 0;
    if ((int)success != 1)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "FinishedReportGameResult returned error cat %d\n",
            category);
        mStatsError = true;
    }
}

void NetworkStatsManager::SubmitJob(int job)
{
    if (mJobCount < mJobCapacity)
    {
        mJobs[(mJobReadIndex + mJobCount++) % mJobCapacity] = job;
        return;
    }
    tDebugPrintManager::Print(DC_NETWORK, "ERROR JobsQ full failed to submit %d\n", job);
}

void NetworkStatsManager::RefreshSaveState_801314D0()
{
    mSaveState = g_pFriendManager->CountBuddies();
}

void NetworkStatsManager::ClearGameResultReported()
{
    mGameResultReported = false;
}

void NetworkStatsManager::ResetPregameDisconnectState()
{
    mGameResultReported = false;
    mDisconnectPending = false;
    mUnidentifiedC41C = 0;
    mDisconnectLossPending[0] = false;
    mUnidentifiedC420 = 0;
    mDisconnectLossPending[1] = false;
    mUnidentifiedC424 = 0;
    mDisconnectLossPending[2] = false;
    if (IsOnlineRankedMatch())
    {
        ReportDefaultDisconnectLoss();
    }
}

void NetworkStatsManager::MarkDisconnectPending()
{
    mDisconnectPending = true;
}

void NetworkStatsManager::PreGameRestoreDefaultDisconnectLoss()
{
    if (UsesEuropeanRankings())
    {
        NetworkRankingMeta* record = Instance()->GetLocalStats(2);
        if (record != 0 && IsNewNetworkSeason(record))
        {
            SubmitJob(5);
            SubmitJob(10);
        }
    }
    NetworkRankingMeta* record = Instance()->GetLocalStats(0);
    if (record != 0 && IsNewNetworkSeason(record))
    {
        CommitPendingOnlineTotals(record);
        SubmitJob(3);
        SubmitJob(8);
        SubmitJob(9);
        if (!UsesEuropeanRankings())
            SubmitJob(10);
    }
    record = Instance()->GetLocalStats(1);
    if (record != 0 && IsNewNetworkDay(record))
    {
        SubmitJob(4);
        SubmitJob(6);
        SubmitJob(7);
    }
}

bool NetworkStatsManager::RefreshFriendStats_80131B50()
{
    if (mOperation == 0 && g_pNetworkSession->mLoginStage == 14)
    {
        if (mStatsError)
            return false;
        if (mSaveDataChanged)
        {
            SaveLoad::StartSave(true);
            mSaveDataChanged = false;
        }
        int count = g_pFriendManager->CountBuddies();
        if (count != mSaveState)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Num friends changed from %d to %d\n", mSaveState, count);
            SubmitJob(10);
            mUnidentifiedC410 = -1;
            for (int i = 0; i < 5; ++i)
            {
                if (sLeaderboardJobs[i] != 10)
                {
                    mUnidentifiedC410 = i;
                    break;
                }
            }
            mSaveState = count;
        }
        if (mJobCount == 0 && mCurrentTime - mOperationStartTime >= sRankingRequestTimeout)
            PreGameRestoreDefaultDisconnectLoss();
        if (mJobCount == 0 && mCurrentTime - mOperationStartTime >= sSecondsPerMinute)
        {
            SubmitJob(sLeaderboardJobs[mUnidentifiedC410]);
            ++mUnidentifiedC410;
            if (mUnidentifiedC410 >= 5)
                mUnidentifiedC410 = 0;
        }
    }
    return true;
}

void NetworkStatsManager::BeginOnlineGame_80131DB4()
{
    mGameResultReported = false;
    mDisconnectPending = false;
    mDisconnectLossPending[0] = true;
    mDisconnectLossPending[1] = true;
    mDisconnectLossPending[2] = UsesEuropeanRankings();
}

void NetworkStatsManager::Update(float dt)
{
    mCurrentTime += dt;
    if (mOperation != 0 && mCurrentTime - mOperationStartTime > sRankingRequestTimeout)
    {
        mOperation = 0;
        mStatsError = true;
    }
    if (mOperation != 0 || mJobCount == 0)
    {
        return;
    }

    int job = mJobs[mJobReadIndex];
    mJobReadIndex = (mJobReadIndex + 1) % mJobCapacity;
    --mJobCount;
    switch (job)
    {
    case 0:
        PostResetMyPlayerStats(0, false);
        break;
    case 1:
        PostResetMyPlayerStats(1, false);
        break;
    case 2:
        PostResetMyPlayerStats(2, false);
        break;
    case 3:
        if (!RequestRankings(2))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure to RequestRankings STRIKER_OF_DAY Nearby\n");
        }
        break;
    case 4:
        if (!RequestRankings(3))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure to RequestRankings STRIKER_OF_DAY TOP\n");
        }
        break;
    case 5:
        if (!RequestRankings(0))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Job initial failure getting nearby season stats\n");
        }
        break;
    case 6:
        if (!RequestRankings(1))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure getting TOP season stats\n");
        }
        break;
    case 7:
        if (!RequestRankings(4))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure to RequestRankings Season FRIENDS\n");
        }
        break;
    case 8:
    case 9:
    case 10:
        RequestRankings(job - 5);
        break;
    default:
        tDebugPrintManager::Print(DC_NETWORK, "Bad eJob case %d\n", job);
        break;
    }

}

int GetLocalPlayingSide_801323F4()
{
    s8 machine = g_pNetworkSessionBase->GetLocalMachineId();
    s8 player = GetNetworkPlayerId(0, machine);
    return GameInfoManager::GetInstance()->GetPlayingSide(player);
}

void NetworkStatsManager::HandleDisconnect_8013243C(int result)
{
    if (!IsOnlineRankedMatch())
    {
        return;
    }
    if (mDisconnectPending)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "A disc error previously occured.  Exiting PreGameRestoreDefaultDisconnectLoss\n");
    }
    ResetPregameDisconnectState();
    for (int i = 0; i < 5; ++i)
    {
        SubmitJob(sLeaderboardJobs[i]);
    }
    mUnidentifiedC41C = result;
}

void NetworkStatsManager::CalculateAndReportGameResult(int result)
{
    if (mDisconnectPending)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "A disc error previously occured.  Exiting CalculateAndReportGameResult\n");
        return;
    }
    if (mGameResultReported)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Already did CalculateAndReportGameResult..ignoring\n");
        return;
    }

    tDebugPrintManager::Print(DC_NETWORK,
        "Reporting Online Game Results HOME %s %d vs AWAY %s %d I am home: %d AlreadyReported %d\n",
        "",
        0,
        "",
        0,
        GetLocalPlayingSide_801323F4() == 0,
        mGameResultReported);
    mUnidentifiedC41C = result;
    mGameResultReported = true;
}

bool IsNewNetworkSeason(const NetworkRankingMeta* previous)
{
    DWCDate date;
    DWCTime time;
    GetAdjustedNetworkDate(&date, &time);
    NetworkSeasonDate current = { date.month, date.mday };
    int currentYear = date.year;
    int currentSeason = FindNetworkSeasonBoundary(&sNetworkSeasonDateTable, current);
    NetworkSeasonDate old = { previous->mMonth, previous->mDay };
    int previousYear = previous->mYear;
    int previousSeason = FindNetworkSeasonBoundary(&sNetworkSeasonDateTable, old);
    if (currentYear != previousYear || currentSeason != previousSeason)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Detected new season old %d %d %d new %d %d %d\n",
            previousYear,
            old.mMonth,
            old.mDay,
            currentYear,
            current.mMonth,
            current.mDay);
        return true;
    }
    return false;
}

bool IsNewNetworkDay(const NetworkRankingMeta* previous)
{
    DWCDate date;
    DWCTime time;
    GetAdjustedNetworkDate(&date, &time);
    if (date.mday != previous->mDay || date.month != previous->mMonth || date.year != previous->mYear)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Detected Starting new day old %d %d %d new %d %d %d\n",
            previous->mYear,
            previous->mMonth,
            previous->mDay,
            date.year,
            date.month,
            date.mday);
        return true;
    }
    return false;
}

static int DaysInMonth(int month, int year)
{
    if (month == 2)
    {
        if (year % 4 == 0)
        {
            return 29;
        }
        return 28;
    }
    return sMonthDays[month - 1];
}

bool GetAdjustedNetworkDate(DWCDate* date, DWCTime* time)
{
    bool valid = DWC_GetDateTime(date, time);
    if (!valid)
    {
        memset(time, 0, sizeof(*time));
        date->mday = 1;
        date->month = 0;
        date->year = 2000;
        date->wday = 0;
        date->yday = 0;
    }

    ++date->month;
    if (g_nAddHoursTime != 0 || g_nAddMinsTime != 0)
    {
        time->min += g_nAddMinsTime;
        if (time->min > 59)
        {
            time->min -= 60;
            ++time->hour;
        }
        else if (time->min < 0)
        {
            time->min += 60;
            --time->hour;
        }

        time->hour += g_nAddHoursTime;
        if (time->hour > 23)
        {
            time->hour -= 24;
            ++date->mday;
            if (date->mday > DaysInMonth(date->month, date->year))
            {
                date->mday -= DaysInMonth(date->month, date->year);
                if (++date->month > 12)
                {
                    date->month = 1;
                    ++date->year;
                }
            }
        }
        else if (time->hour < 0)
        {
            time->hour += 24;
            if (--date->mday < 1)
            {
                if (--date->month < 1)
                {
                    date->month = 12;
                    --date->year;
                }
                date->mday = DaysInMonth(date->month, date->year);
            }
        }
    }
    return true;
}

int FindNetworkSeasonBoundary(
    const NetworkSeasonDateTable* dates, NetworkSeasonDate date)
{
    int index = 0;
    for (; index < dates->mCount; ++index)
    {
        if (date.mDay == dates->mDates[index].mDay && date.mMonth == dates->mDates[index].mMonth)
        {
            return index;
        }
        if (dates->mDates[index].mMonth > date.mMonth
            || (date.mMonth == dates->mDates[index].mMonth && dates->mDates[index].mDay > date.mDay))
        {
            break;
        }
    }
    return index - 1;
}

static int DayOfYear(NetworkSeasonDate date, int year)
{
    int result = 0;
    for (int i = 1; i < date.mMonth; ++i)
    {
        result += DaysInMonth(i, year);
    }
    return result + date.mDay;
}

int GetDaysUntilNextSeasonBoundary(
    const NetworkSeasonDateTable* dates, int index, int year)
{
    NetworkSeasonDate current = dates->mDates[index];
    if (index == dates->mCount - 1)
    {
        const NetworkSeasonDate& next = dates->mDates[0];
        int currentDay = DayOfYear(current, year);
        int nextDay = DayOfYear(next, year + 1);
        int remaining = (year % 4 == 0) ? 366 : 365;
        return remaining - currentDay + nextDay;
    }
    const NetworkSeasonDate& next = dates->mDates[index + 1];
    return DayOfYear(next, year) - DayOfYear(current, year);
}

int GetDaysSinceSeasonBoundary(const NetworkSeasonDateTable* dates, int index,
    const NetworkSeasonDate* date, int year)
{
    const NetworkSeasonDate& boundary = dates->mDates[index];
    return DayOfYear(*date, year) - DayOfYear(boundary, year);
}

NetworkSeasonDate sNetworkSeasonDates[52] = {
    { 1, 1 }, { 1, 8 }, { 1, 15 }, { 1, 22 }, { 1, 29 }, { 2, 5 }, { 2, 12 }, { 2, 19 }, { 2, 26 }, { 3, 5 }, { 3, 12 }, { 3, 19 }, { 3, 26 }, { 4, 2 }, { 4, 9 }, { 4, 16 }, { 4, 23 }, { 4, 30 }, { 5, 7 }, { 5, 14 }, { 5, 21 }, { 5, 28 }, { 6, 4 }, { 6, 11 }, { 6, 18 }, { 6, 25 }, { 7, 2 }, { 7, 9 }, { 7, 16 }, { 7, 23 }, { 7, 30 }, { 8, 6 }, { 8, 13 }, { 8, 20 }, { 8, 27 }, { 9, 3 }, { 9, 10 }, { 9, 17 }, { 9, 24 }, { 10, 1 }, { 10, 8 }, { 10, 15 }, { 10, 22 }, { 10, 29 }, { 11, 5 }, { 11, 12 }, { 11, 19 }, { 11, 26 }, { 12, 3 }, { 12, 10 }, { 12, 17 }, { 12, 24 }
};

int sMonthDays[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

NetworkSeasonDateTable sNetworkSeasonDateTable(
    52, sNetworkSeasonDates);

int g_nAddHoursTime;
int g_nAddMinsTime;

static TweakIntBinding sAddHoursTimeTweak(
    "g_nAddHoursTime", "Network", &g_nAddHoursTime, true);
static TweakIntBinding sAddMinsTimeTweak(
    "g_nAddMinsTime", "Network", &g_nAddMinsTime, true);

