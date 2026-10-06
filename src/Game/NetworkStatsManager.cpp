#include "NL/nlSingleton.inl"
#include <dwc/dwc_account.h>
#include <dwc/dwc_nastime.h>
#include "Game/OnlinePlayer.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/Sys/debug.h"
#include "Game/DB/SaveLoad.h"

#include "Game/NetworkSeasonCalendar.h"
#include "Game/NetworkStatsManager.h"
#include "Game/FriendManager.h"

#include "Game/GameInfo.h"
#include "Game/NetTournManager.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkSync.h"
#include "Game/Team.h"
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


struct NetworkGameResultDetails
{
    int mPoints;
    bool mWon;
    bool mTied;
    u8 mPadding06[2];
    int mResultPoints;
    int mGoalPoints;
    int mBonusPoints;
}; // size: 0x14

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

static inline void SetRankingGroup(int& group, int value)
{
    group = value;
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
    mNextLeaderboardJobIndex = 0;
    mCachedFriendCount = 0;
    mStatsError = false;
    mSaveDataChanged = false;
    mGameResultReported = false;
    mDisconnectPending = false;
    mDisconnectPointsLost[0] = 0;
    mDisconnectLossPending[0] = false;
    mDisconnectPointsLost[1] = 0;
    mDisconnectLossPending[1] = false;
    mDisconnectPointsLost[2] = 0;
    mDisconnectLossPending[2] = false;
    mJobs.mHead = 0;
    mJobs.mCount = 0;

    for (int i = 0; i < 6; ++i)
    {
        mCategories[i].mAvailable = false;
        mCategories[i].mCount = 0;
        mCategories[i].mLocalPlayerIndex = -1;
    }

    bool european = GetRegion() == 1;
    if (european)
    {
        int alternate;
        if (UsesEuropeanRankings() == false)
            SetRankingGroup(alternate, 0);
        else
            SetRankingGroup(alternate, IsAlternateOnlineCountryGroup());
        mPersistentCategories[0] =
            alternate == 1 ? NETWORK_PERSISTENT_ALTERNATE_SEASON : NETWORK_PERSISTENT_SEASON;
        mPersistentCategories[1] =
            alternate == 1 ? NETWORK_PERSISTENT_ALTERNATE_STRIKER_OF_DAY : NETWORK_PERSISTENT_STRIKER_OF_DAY;
        mPersistentCategories[2] = NETWORK_PERSISTENT_FRIENDS;

        mCategories[0].mPersistentCategory =
            alternate == 1 ? NETWORK_PERSISTENT_ALTERNATE_STRIKER_OF_DAY : NETWORK_PERSISTENT_STRIKER_OF_DAY;
        mCategories[0].mFilter = 0;
        mCategories[0].mLocalStatsCategory = 1;
        mCategories[1].mPersistentCategory =
            alternate == 1 ? NETWORK_PERSISTENT_ALTERNATE_STRIKER_OF_DAY : NETWORK_PERSISTENT_STRIKER_OF_DAY;
        mCategories[1].mFilter = 2;
        mCategories[1].mLocalStatsCategory = 1;
        mCategories[2].mPersistentCategory =
            alternate == 1 ? NETWORK_PERSISTENT_ALTERNATE_SEASON : NETWORK_PERSISTENT_SEASON;
        mCategories[2].mFilter = 0;
        mCategories[2].mLocalStatsCategory = 0;
        mCategories[3].mPersistentCategory =
            alternate == 1 ? NETWORK_PERSISTENT_ALTERNATE_SEASON : NETWORK_PERSISTENT_SEASON;
        mCategories[3].mFilter = 2;
        mCategories[3].mLocalStatsCategory = 0;
        mCategories[4].mPersistentCategory = NETWORK_PERSISTENT_FRIENDS;
        mCategories[4].mFilter = 1;
        mCategories[4].mLocalStatsCategory = 2;
        mCategories[5].mPersistentCategory = NETWORK_PERSISTENT_FRIENDS;
        mCategories[5].mFilter = 1;
        mCategories[5].mLocalStatsCategory = 2;
    }
    else
    {
        mPersistentCategories[0] = NETWORK_PERSISTENT_SEASON;
        mPersistentCategories[1] = NETWORK_PERSISTENT_STRIKER_OF_DAY;
        mPersistentCategories[2] = NETWORK_PERSISTENT_FRIENDS;

        mCategories[0].mPersistentCategory = NETWORK_PERSISTENT_STRIKER_OF_DAY;
        mCategories[0].mFilter = 0;
        mCategories[0].mLocalStatsCategory = 1;
        mCategories[1].mPersistentCategory = NETWORK_PERSISTENT_STRIKER_OF_DAY;
        mCategories[1].mFilter = 2;
        mCategories[1].mLocalStatsCategory = 1;
        mCategories[2].mPersistentCategory = NETWORK_PERSISTENT_SEASON;
        mCategories[2].mFilter = 0;
        mCategories[2].mLocalStatsCategory = 0;
        mCategories[3].mPersistentCategory = NETWORK_PERSISTENT_SEASON;
        mCategories[3].mFilter = 2;
        mCategories[3].mLocalStatsCategory = 0;
        mCategories[4].mPersistentCategory = NETWORK_PERSISTENT_SEASON;
        mCategories[4].mFilter = 1;
        mCategories[4].mLocalStatsCategory = 0;
        mCategories[5].mPersistentCategory = NETWORK_PERSISTENT_SEASON;
        mCategories[5].mFilter = 1;
        mCategories[5].mLocalStatsCategory = 0;
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
    leaderboard.mLocalPlayerIndex = -1;
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
                leaderboard->mLocalPlayerIndex = i;
                bool apply = false;
                if (updateProfile)
                {
                    if (leaderboard->mLocalStatsCategory == 2)
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
                    mHasLocalStats[leaderboard->mLocalStatsCategory] = true;
                    mLocalStats[leaderboard->mLocalStatsCategory] = leaderboard->mMetadata[i];
                    if (leaderboard->mLocalStatsCategory == 0 && leaderboard->mFilter == 0)
                    {
                        NetworkRankingMeta& record = mLocalStats[leaderboard->mLocalStatsCategory];
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
                leaderboard->mLocalPlayerIndex = i;
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
    int* pendingWins = GameInfoManager::GetInstance()->GetUnknown0xA98(gNetworkSaveSlotIndex);
    int* pendingLosses = GameInfoManager::GetInstance()->GetUnknown0xA9C(gNetworkSaveSlotIndex);
    int* wins = GameInfoManager::GetInstance()->GetUnknown0xA90(gNetworkSaveSlotIndex);
    int* losses = GameInfoManager::GetInstance()->GetUnknown0xA94(gNetworkSaveSlotIndex);

    *wins += *pendingWins;
    *losses += *pendingLosses;
    if (*wins > 9999)
    {
        *wins = 9999;
    }
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
    int profileId;
    DWCAccFriendData* friendData;
    u16* name;
    u8* region;
    int i;
    int friendIndex;

    for (i = 0; i < leaderboard->mCount; ++i)
    {
        profileId = leaderboard->mPlayers[i].mProfileId;
        for (friendIndex = 0; friendIndex < 64; ++friendIndex)
        {
            friendData = reinterpret_cast<DWCAccFriendData*>(
                GameInfoManager::GetInstance()->GetUnknown0x40(
                    gNetworkSaveSlotIndex, friendIndex));
            name = GameInfoManager::GetInstance()->GetSavedFriendName(
                gNetworkSaveSlotIndex, friendIndex);
            region = static_cast<u8*>(
                GameInfoManager::GetInstance()->GetUnknown0xA40(
                    gNetworkSaveSlotIndex, friendIndex));
            if (friendData->gs_profile_id.id == profileId)
            {
                if (leaderboard->mPlayers[i].mName[0] != 0)
                {
                    nlStrNCpy(name, leaderboard->mPlayers[i].mName, 11);
                }
                *region = leaderboard->mMetadata[i].mOnlineRegion;
            }
        }
    }
}

void NetworkStatsManager::BuildFriendsLeaderboard()
{
    NetworkLeaderboardCategory& source = mCategories[4];
    NetworkLeaderboardCategory& friends = mCategories[5];
    friends.mAvailable = true;
    friends.mLocalPlayerIndex = -1;
    friends.mCount = 0;

    int write = 0;
    GameInfoSaveSlot* slot = GameInfoManager::GetInstance()->GetSaveSlot(
        gNetworkSaveSlotIndex);
    int localProfileId = slot->unknown_0x01C;

    for (int read = 0; read < source.mCount; ++read)
    {
        int profileId = source.mPlayers[read].mProfileId;
        if (profileId == localProfileId)
        {
            friends.mPlayers[write].CopyFrom(source.mPlayers[read]);
            friends.mMetadata[write] = source.mMetadata[read];
            ++write;
        }
        else
        {
            int friendIndex = 0;
            for (; friendIndex < 64; ++friendIndex)
            {
                DWCAccFriendData* friendData =
                    reinterpret_cast<DWCAccFriendData*>(
                        GameInfoManager::GetInstance()->GetUnknown0x40(
                            gNetworkSaveSlotIndex, friendIndex));
                if (friendData->gs_profile_id.id == profileId)
                {
                    int type = DWC_GetFriendDataType(friendData);
                    if (DWC_IsValidFriendData(friendData)
                        && type == DWC_FRIENDDATA_GS_PROFILE_ID)
                    {
                        friends.mPlayers[write].CopyFrom(
                            source.mPlayers[read]);
                        friends.mMetadata[write] = source.mMetadata[read];
                        ++write;
                    }
                    break;
                }
            }
        }
    }
    friends.mCount = write;
    NetworkRanking::AssignDisplayRanks(write, friends.mMetadata, 1);
}

struct LeaderboardResultView
{
    const bool& mSucceeded;
    int mCategory;
    int mFilter;
    const int& mCount;
};

static inline void FinishLeaderboardRequest(NetworkStatsManager* manager,
    const LeaderboardResultView& result)
{
    manager->mOperation = 0;
    manager->mLeaderboardRequestComplete = true;
    manager->mLeaderboardCategory = manager->mRequestedCategory;
    if ((int)result.mSucceeded != 1)
    {
        manager->mStatsError = true;
        manager->mLeaderboardRequestSucceeded = false;
        manager->mCategories[manager->mRequestedCategory].mAvailable = false;
        manager->mCategories[manager->mRequestedCategory].mCount = 0;
        manager->mCategories[manager->mRequestedCategory].mLocalPlayerIndex = -1;
        tDebugPrintManager::Print(DC_NETWORK,
            "Unavailable Leaderboard Stats cat %d filter %d\n",
            result.mCategory,
            result.mFilter);
        if (manager->mRequestedCategory == 4)
        {
            manager->mCategories[5].mAvailable = false;
            manager->mCategories[5].mCount = 0;
            manager->mCategories[5].mLocalPlayerIndex = -1;
        }
    }
    else
    {
        manager->mLeaderboardRequestSucceeded = true;
        tDebugPrintManager::Print(DC_NETWORK,
            "Sucessfully got leaderboard stats cat %d filter %d\n",
            result.mCategory,
            result.mFilter);
        manager->mCategories[manager->mRequestedCategory].mAvailable = true;
        manager->mCategories[manager->mRequestedCategory].mCount = result.mCount;
        manager->mCategories[manager->mRequestedCategory].mLocalPlayerIndex = -1;
        if (result.mFilter == 1)
        {
            GetRegion();
            manager->UpdateFriendRankingNames(&manager->mCategories[manager->mRequestedCategory]);
        }
        manager->ApplyLeaderboardToSave(&manager->mCategories[manager->mRequestedCategory], true);
        if (manager->mRequestedCategory == 4)
        {
            manager->BuildFriendsLeaderboard();
            manager->ApplyLeaderboardToSave(&manager->mCategories[5], false);
        }
    }
}

void NetworkStatsManager::OnLeaderboardResult(bool success,
    int category, int filter, int count, NetworkStatsPlayer*,
    NetworkRankingMeta*)
{
    const LeaderboardResultView result = { success, category, filter, count };
    FinishLeaderboardRequest(this, result);
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
        mLocalStats[category].mOnlineRegion = 0;
        mLocalStats[category].LoadLocal();
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

int CalculateNetworkResultPoints(int result, bool home, int homeScore,
    int awayScore, bool* won, bool* tied, int* resultPoints,
    int* goalPoints, int* bonusPoints)
{
    *won = false;
    *tied = false;
    *resultPoints = 0;
    *goalPoints = 0;
    *bonusPoints = 0;

    if (home)
    {
        *goalPoints = homeScore > 10 ? 10 : homeScore;
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
        *goalPoints = awayScore > 10 ? 10 : awayScore;
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
        *goalPoints = 0;
        *won = false;
    }
    return *resultPoints + *goalPoints + *bonusPoints;
}

void NetworkStatsManager::UpdateOnlineResultTotals(
    int result, bool home, int homeScore, int awayScore)
{
    if (g_pNetworkSessionBase->GetSessionMode() != 2)
    {
        return;
    }
    if (IsOnlineRankedMatch())
    {
        return;
    }

    NetworkGameResultDetails details;
    details.mPoints = 0;
    details.mWon = false;
    details.mTied = false;
    details.mResultPoints = 0;
    details.mGoalPoints = 0;
    details.mBonusPoints = 0;
    details.mPoints = CalculateNetworkResultPoints(result, home, homeScore,
        awayScore, &details.mWon, &details.mTied, &details.mResultPoints,
        &details.mGoalPoints, &details.mBonusPoints);

    int* wins = GameInfoManager::GetInstance()->GetUnknown0xAA0(gNetworkSaveSlotIndex);
    int* losses = GameInfoManager::GetInstance()->GetUnknown0xAA4(gNetworkSaveSlotIndex);
    if (details.mWon)
    {
        ++*wins;
        if (*wins > 9999)
        {
            *wins = 9999;
        }
    }
    else
    {
        ++*losses;
        if (*losses > 9999)
        {
            *losses = 9999;
        }
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
    if (g_pNetworkSession->GetRankingReporter() != 0)
    {
        int categoryCount = UsesEuropeanRankings() ? 3 : 2;
        NetworkRankingMeta* localStats = mLocalStats;
        for (int category = 0; category < categoryCount; ++category)
        {
            if (mLocalStats[category].mLosses >= 9999)
            {
                return;
            }
            if (category == 1)
            {
                if (IsNewNetworkDay(&mLocalStats[category]))
                {
                    tDebugPrintManager::Print(DC_NETWORK, "Skipping default disconnect loss new day\n");
                    return;
                }
            }
            else if (IsNewNetworkSeason(&localStats[category]))
            {
                tDebugPrintManager::Print(DC_NETWORK, "Skipping default disconnect loss new season\n");
                return;
            }
        }

        for (int category = 0; category < categoryCount; ++category)
        {
            int oldPoints = mLocalStats[category].mScore;
            int pointsLost = oldPoints < 5 ? oldPoints : 5;
            mDisconnectPointsLost[category] = pointsLost;
            mDisconnectLossPending[category] = true;
            mLocalStats[category].mScore -= pointsLost;
            ++mLocalStats[category].mLosses;
            mLocalStats[category].mOnlineRegion = GetOnlineRegion();

            tDebugPrintManager::Print(DC_NETWORK,
                "ReportDefaultDisconnectLoss: pers cat %d oldPoints %d new points %d New W:L %d:%d OneBasedRegion:%d\n",
                category,
                oldPoints,
                mLocalStats[category].mScore,
                mLocalStats[category].mWins,
                mLocalStats[category].mLosses,
                mLocalStats[category].mOnlineRegion);

            localStats[category].LoadLocal();
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
}

void NetworkStatsManager::ReportGameResult(int result,
    NetworkStatsPlayer* home, NetworkStatsPlayer* away,
    bool reportHome, int homeScore, int awayScore,
    const NetworkScoreSubmission* fallback)
{
    if (g_pNetworkSession->GetRankingReporter() != 0)
    {
        NetworkGameResultDetails details;
        details.mPoints = 0;
        details.mWon = false;
        details.mTied = false;
        details.mResultPoints = 0;
        details.mGoalPoints = 0;
        details.mBonusPoints = 0;

        if (fallback == 0)
        {
            if (!mGameResultReported)
            {
                details.mPoints = CalculateNetworkResultPoints(result, reportHome,
                    homeScore, awayScore, &details.mWon, &details.mTied, &details.mResultPoints,
                    &details.mGoalPoints, &details.mBonusPoints);
                mLastTotalPoints = details.mPoints;
                mLastGameWon = details.mWon;
                mLastGameTied = details.mTied;
                mLastResultPoints = details.mResultPoints;
                mLastGoalPoints = details.mGoalPoints;
                mLastBonusPoints = details.mBonusPoints;
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
            mLastTotalPoints = 0;
            mLastGameWon = false;
            mLastGameTied = false;
            mLastResultPoints = 0;
            mLastGoalPoints = 0;
            mLastBonusPoints = 0;
        }

        bool restoreDisconnectLoss = result == 0 ? IsCurrentSeriesComplete() : true;
        if (fallback != 0)
        {
            restoreDisconnectLoss = true;
        }

        int categoryCount = UsesEuropeanRankings() ? 3 : 2;
        for (int category = 0; category < categoryCount; ++category)
        {
            int oldPoints = mLocalStats[category].mScore;
            NetworkRankingMeta* localStats = &mLocalStats[category];
            int pointsScored = details.mPoints;
            bool startFresh = false;

            if (category == 1)
            {
                if (IsNewNetworkDay(&mLocalStats[category]))
                {
                    tDebugPrintManager::Print(DC_NETWORK, "New day starting score fresh\n");
                    startFresh = true;
                }
            }
            else if (IsNewNetworkSeason(localStats))
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
                    if (details.mWon)
                    {
                        ++mLocalStats[category].mWins;
                    }
                    else
                    {
                        ++mLocalStats[category].mLosses;
                    }
                }
                mDisconnectPointsLost[category] = 0;
                mDisconnectLossPending[category] = false;
            }
            else if (mDisconnectLossPending[category]
                && restoreDisconnectLoss)
            {
                pointsScored += mDisconnectPointsLost[category];
                mLocalStats[category].mScore += pointsScored;
                tDebugPrintManager::Print(DC_NETWORK, "Returning Default Disconnect Loss\n");
                mDisconnectPointsLost[category] = 0;
                mDisconnectLossPending[category] = false;
                if (result == 2)
                {
                    --mLocalStats[category].mLosses;
                }
                if (result == 3 || result == 4)
                {
                    --mLocalStats[category].mLosses;
                }
                else if (details.mWon)
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
                    if (details.mWon)
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
            mLocalStats[category].mOnlineRegion = GetOnlineRegion();

            tDebugPrintManager::Print(DC_NETWORK,
                "ReportGameResult: pers cat %d oldPoints %d + points scored %d = new points %d IWon: %d New W:L %d:%d OneBasedRegion:%d\n",
                category,
                oldPoints,
                pointsScored,
                mLocalStats[category].mScore,
                details.mWon,
                mLocalStats[category].mWins,
                mLocalStats[category].mLosses,
                mLocalStats[category].mOnlineRegion);

            localStats->LoadLocal();
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
    if (!mJobs.IsFull())
    {
        mJobs.Push(job);
        return;
    }
    tDebugPrintManager::Print(DC_NETWORK, "ERROR JobsQ full failed to submit %d\n", job);
}

void NetworkStatsManager::RefreshFriendCount()
{
    mCachedFriendCount = g_pFriendManager->CountBuddies();
}

void NetworkStatsManager::ClearGameResultReported()
{
    mGameResultReported = false;
}

void NetworkStatsManager::ResetPregameDisconnectState()
{
    mGameResultReported = false;
    mDisconnectPending = false;
    mDisconnectPointsLost[0] = 0;
    mDisconnectLossPending[0] = false;
    mDisconnectPointsLost[1] = 0;
    mDisconnectLossPending[1] = false;
    mDisconnectPointsLost[2] = 0;
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

bool NetworkStatsManager::RefreshRankings()
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
        if (count != mCachedFriendCount)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Num friends changed from %d to %d\n", mCachedFriendCount, count);
            SubmitJob(10);
            mNextLeaderboardJobIndex = -1;
            for (int i = 0; i < 5; ++i)
            {
                if (sLeaderboardJobs[i] != 10)
                {
                    mNextLeaderboardJobIndex = i;
                    break;
                }
            }
            mCachedFriendCount = count;
        }
        if (mJobs.GetCount() == 0 && mCurrentTime - mOperationStartTime >= sRankingRequestTimeout)
            PreGameRestoreDefaultDisconnectLoss();
        if (mJobs.GetCount() == 0 && mCurrentTime - mOperationStartTime >= sSecondsPerMinute)
        {
            SubmitJob(sLeaderboardJobs[mNextLeaderboardJobIndex]);
            ++mNextLeaderboardJobIndex;
            if (mNextLeaderboardJobIndex >= 5)
                mNextLeaderboardJobIndex = 0;
        }
    }
    return true;
}

static inline void PrepareOnlineGame(NetworkStatsManager* manager)
{
    manager->mGameResultReported = false;
    manager->mDisconnectPending = false;
    manager->mDisconnectPointsLost[0] = 0;
    manager->mDisconnectLossPending[0] = false;
    manager->mDisconnectPointsLost[1] = 0;
    manager->mDisconnectLossPending[1] = false;
    manager->mDisconnectPointsLost[2] = 0;
    manager->mDisconnectLossPending[2] = false;
    if (!IsOnlineRankedMatch())
    {
        return;
    }
    manager->SubmitJob(6);
    manager->SubmitJob(7);
    manager->SubmitJob(8);
    manager->SubmitJob(9);
    manager->SubmitJob(10);
}

void NetworkStatsManager::BeginOnlineGame()
{
    PrepareOnlineGame(this);
}

void NetworkStatsManager::Update(float dt)
{
    mCurrentTime += dt;
    if (mOperation != 0)
    {
        return;
    }
    if (g_pNetworkSession->mLoginStage != 14)
    {
        return;
    }
    if (mJobs.GetCount() == 0)
    {
        return;
    }

    int job = mJobs.Pop();
    switch (job)
    {
    case 0:
        if (g_pNetworkSession->GetRankingReporter()->ReportGameResult(
                mPersistentCategories[0], 0, 0, 0, false, 0, 0,
                reinterpret_cast<const NetworkScoreSubmission*>(&mLocalStats[0])))
        {
            mOperationStartTime = mCurrentTime;
            mOperation = 2;
            mSubmissionCategory = 0;
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Initial failure of ReportGameResultSeason\n");
            mOperation = 0;
            mStatsError = true;
        }
        break;
    case 1:
        if (g_pNetworkSession->GetRankingReporter()->ReportGameResult(
                mPersistentCategories[1], 0, 0, 0, false, 0, 0,
                reinterpret_cast<const NetworkScoreSubmission*>(&mLocalStats[1])))
        {
            mOperationStartTime = mCurrentTime;
            mOperation = 2;
            mSubmissionCategory = 1;
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Initial failure of ReportGameResultSOD\n");
            mOperation = 0;
            mStatsError = true;
        }
        break;
    case 2:
        if (g_pNetworkSession->GetRankingReporter()->ReportGameResult(
                mPersistentCategories[2], 0, 0, 0, false, 0, 0,
                reinterpret_cast<const NetworkScoreSubmission*>(&mLocalStats[2])))
        {
            mOperationStartTime = mCurrentTime;
            mOperation = 2;
            mSubmissionCategory = 2;
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Initial failure of ReportGameResultFriends\n");
            mOperation = 0;
            mStatsError = true;
        }
        break;
    case 3:
        Instance()->PostResetMyPlayerStats(0, false);
        break;
    case 4:
        Instance()->PostResetMyPlayerStats(1, false);
        break;
    case 5:
        Instance()->PostResetMyPlayerStats(2, false);
        break;
    case 6:
        if (!RequestRankings(0))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure to RequestRankings STRIKER_OF_DAY Nearby\n");
        }
        break;
    case 7:
        if (!RequestRankings(1))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure to RequestRankings STRIKER_OF_DAY TOP\n");
        }
        break;
    case 8:
        if (!RequestRankings(2))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure getting nearby season stats\n");
        }
        break;
    case 9:
        if (!RequestRankings(3))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure getting TOP season stats\n");
        }
        break;
    case 10:
        if (!RequestRankings(4))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Job initial failure to RequestRankings Season FRIENDS\n");
        }
        break;
    default:
        char message[30];
        nlSNPrintf(message, sizeof(message), "Bad eJob case %d\n", job);
        break;
    }

}

int GetLocalNetworkPlayingSide()
{
    s8 machine = g_pNetworkSessionBase->GetLocalMachineId();
    s8 player = GetNetworkPlayerId(0, machine);
    return GameInfoManager::GetInstance()->GetPlayingSide(player);
}

void NetworkStatsManager::ReportDisconnect(int result)
{
    if (!IsOnlineRankedMatch())
    {
        return;
    }
    if (mDisconnectPending)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "A disc error previously occured.  Exiting PreGameRestoreDefaultDisconnectLoss\n");
        PrepareOnlineGame(this);
        return;
    }

    ReportGameResult(result, 0, 0, false, 0, 0,
        reinterpret_cast<const NetworkScoreSubmission*>(1));
    PrepareOnlineGame(this);
}

void NetworkStatsManager::CalculateAndReportGameResult(int result)
{
    if (mDisconnectPending)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "A disc error previously occured.  Exiting CalculateAndReportGameResult\n");
        return;
    }

    int homeScore;
    int awayScore;
    BasicGameInfo* gameInfo = GameInfoManager::GetInstance()->GetCurrentGameInfo();
    if (result == 0)
    {
        homeScore = gameInfo->GetFinalScore(0);
        awayScore = gameInfo->GetFinalScore(1);
    }
    else
    {
        homeScore = g_pTeams[0]->m_nScore;
        awayScore = g_pTeams[1]->m_nScore;
    }

    if (gNetworkSyncState->mTriggered)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Detected sync error.  Should end up with net 0 points\n");
        result = 2;
    }

    NetworkStatsPlayer home;
    NetworkStatsPlayer away;
    int playingSide = GetLocalNetworkPlayingSide();

    NetworkDraftTeam* homeTeam;
    NetworkDraftTeam* awayTeam;
    if (NetTournManager::Instance()->mState != 0)
    {
        int homeIndex = NetTournManager::Instance()->MachineIdxToTournamentIdx(0);
        int awayIndex = NetTournManager::Instance()->MachineIdxToTournamentIdx(1);
        homeTeam = NetworkDraft::Instance()->FindDraftTeamByPeerIndex(homeIndex);
        awayTeam = NetworkDraft::Instance()->FindDraftTeamByPeerIndex(awayIndex);
    }
    else
    {
        homeTeam = NetworkDraft::Instance()->GetDraftTeam(0);
        awayTeam = NetworkDraft::Instance()->GetDraftTeam(1);
    }

    char homeName[11] = { 0 };
    char awayName[11] = { 0 };
    nlWcsToStr(homeTeam->mPlayers[0].mName, homeName, 11);
    nlStrNCpy(home.mName, homeTeam->mPlayers[0].mName, 11);
    nlWcsToStr(awayTeam->mPlayers[0].mName, awayName, 11);
    nlStrNCpy(away.mName, awayTeam->mPlayers[0].mName, 11);

    tDebugPrintManager::Print(DC_NETWORK,
        "Reporting Online Game Results HOME %s %d vs AWAY %s %d I am home: %d AlreadyReported %d\n",
        homeName,
        homeScore,
        awayName,
        awayScore,
        playingSide == 0,
        mGameResultReported);

    if (result == 0 && !mGameResultReported)
    {
        UpdateOnlineResultTotals(result, playingSide == 0, homeScore, awayScore);
    }
    if (IsOnlineRankedMatch())
    {
        ReportGameResult(result, &home, &away, playingSide == 0,
            homeScore, awayScore, 0);
    }
    mGameResultReported = true;
}

bool IsNewNetworkSeason(const NetworkRankingMeta* previous)
{
    const NetworkSeasonDateTable* dates = &sNetworkSeasonDateTable;
    const NetworkRankingMeta* oldStats = previous;
    DWCDate date;
    DWCTime time;
    GetAdjustedNetworkDate(&date, &time);
    NetworkSeasonDate current;
    current.mMonth = date.month;
    current.mDay = date.mday;
    int currentYear = date.year;
    int currentSeason = FindNetworkSeasonBoundary(dates, current);
    NetworkSeasonDate old;
    old.mMonth = oldStats->mMonth;
    old.mDay = oldStats->mDay;
    int previousYear = oldStats->mYear;
    int previousSeason = FindNetworkSeasonBoundary(dates, old);
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
