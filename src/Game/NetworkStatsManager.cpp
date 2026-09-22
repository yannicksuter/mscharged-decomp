#include "NL/nlSingleton.inl"
#include <dwc/dwc_account.h>
#include <dwc/dwc_nastime.h>
#include "Game/OnlinePlayer.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/Sys/debug.h"
#include "Game/DB/SaveLoad.h"

#include "Game/NetworkStatsManager.h"
#include "Game/FriendManager.h"

#include "Game/GameInfo.h"
#include "Game/NetTournManager.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkSync.h"
#include "Game/Team.h"
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

struct NetworkGameResultDetails
{
    int mPoints;
    bool mWon;
    bool mTied;
    u8 mPadding06[2];
    int mResultPoints;
    int mScorePoints;
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
    mUnidentifiedC41C[0] = 0;
    mDisconnectLossPending[0] = false;
    mUnidentifiedC41C[1] = 0;
    mDisconnectLossPending[1] = false;
    mUnidentifiedC41C[2] = 0;
    mDisconnectLossPending[2] = false;
    mJobs.mHead = 0;
    mJobs.mCount = 0;

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
        mPersistentCategories[0] =
            alternate == 1 ? NETWORK_PERSISTENT_CATEGORY_3 : NETWORK_PERSISTENT_CATEGORY_0;
        mPersistentCategories[1] =
            alternate == 1 ? NETWORK_PERSISTENT_CATEGORY_4 : NETWORK_PERSISTENT_CATEGORY_1;
        mPersistentCategories[2] = NETWORK_PERSISTENT_CATEGORY_2;

        mCategories[0].mPersistentCategory =
            alternate == 1 ? NETWORK_PERSISTENT_CATEGORY_4 : NETWORK_PERSISTENT_CATEGORY_1;
        mCategories[0].mFilter = 0;
        mCategories[0].mResultType = 1;
        mCategories[1].mPersistentCategory =
            alternate == 1 ? NETWORK_PERSISTENT_CATEGORY_4 : NETWORK_PERSISTENT_CATEGORY_1;
        mCategories[1].mFilter = 2;
        mCategories[1].mResultType = 1;
        mCategories[2].mPersistentCategory =
            alternate == 1 ? NETWORK_PERSISTENT_CATEGORY_3 : NETWORK_PERSISTENT_CATEGORY_0;
        mCategories[2].mFilter = 0;
        mCategories[2].mResultType = 0;
        mCategories[3].mPersistentCategory =
            alternate == 1 ? NETWORK_PERSISTENT_CATEGORY_3 : NETWORK_PERSISTENT_CATEGORY_0;
        mCategories[3].mFilter = 2;
        mCategories[3].mResultType = 0;
        mCategories[4].mPersistentCategory = NETWORK_PERSISTENT_CATEGORY_2;
        mCategories[4].mFilter = 1;
        mCategories[4].mResultType = 2;
        mCategories[5].mPersistentCategory = NETWORK_PERSISTENT_CATEGORY_2;
        mCategories[5].mFilter = 1;
        mCategories[5].mResultType = 2;
    }
    else
    {
        mPersistentCategories[0] = NETWORK_PERSISTENT_CATEGORY_0;
        mPersistentCategories[1] = NETWORK_PERSISTENT_CATEGORY_1;
        mPersistentCategories[2] = NETWORK_PERSISTENT_CATEGORY_2;

        mCategories[0].mPersistentCategory = NETWORK_PERSISTENT_CATEGORY_1;
        mCategories[0].mFilter = 0;
        mCategories[0].mResultType = 1;
        mCategories[1].mPersistentCategory = NETWORK_PERSISTENT_CATEGORY_1;
        mCategories[1].mFilter = 2;
        mCategories[1].mResultType = 1;
        mCategories[2].mPersistentCategory = NETWORK_PERSISTENT_CATEGORY_0;
        mCategories[2].mFilter = 0;
        mCategories[2].mResultType = 0;
        mCategories[3].mPersistentCategory = NETWORK_PERSISTENT_CATEGORY_0;
        mCategories[3].mFilter = 2;
        mCategories[3].mResultType = 0;
        mCategories[4].mPersistentCategory = NETWORK_PERSISTENT_CATEGORY_0;
        mCategories[4].mFilter = 1;
        mCategories[4].mResultType = 0;
        mCategories[5].mPersistentCategory = NETWORK_PERSISTENT_CATEGORY_0;
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
    for (int i = 0; i < leaderboard->mCount; ++i)
    {
        int profileId = leaderboard->mPlayers[i].mProfileId;
        for (int friendIndex = 0; friendIndex < 64; ++friendIndex)
        {
            DWCAccFriendData* friendData = reinterpret_cast<DWCAccFriendData*>(
                GameInfoManager::GetInstance()->GetUnknown0x40(
                    gNetworkSaveSlotIndex, friendIndex));
            u16* name = GameInfoManager::GetInstance()->GetSavedFriendName(
                gNetworkSaveSlotIndex, friendIndex);
            u8* region = static_cast<u8*>(
                GameInfoManager::GetInstance()->GetUnknown0xA40(
                    gNetworkSaveSlotIndex, friendIndex));
            if (friendData->gs_profile_id.id == profileId)
            {
                if (leaderboard->mPlayers[i].mName[0] != 0)
                {
                    nlStrNCpy(name, leaderboard->mPlayers[i].mName, 11);
                }
                *region = leaderboard->mMetadata[i].mUnidentified14;
            }
        }
    }
}

void NetworkStatsManager::BuildFriendsLeaderboard()
{
    NetworkLeaderboardCategory& source = mCategories[4];
    NetworkLeaderboardCategory& friends = mCategories[5];
    friends.mAvailable = true;
    friends.mFirstRank = -1;
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

void NetworkStatsManager::OnLeaderboardResult(bool success,
    int category, int filter, int count, NetworkStatsPlayer*,
    NetworkRankingMeta*)
{
    mOperation = 0;
    mLeaderboardRequestComplete = true;
    mLeaderboardCategory = mRequestedCategory;
    if ((int)success != 1)
    {
        mStatsError = true;
        mLeaderboardRequestSucceeded = false;
        mCategories[mRequestedCategory].mAvailable = false;
        mCategories[mRequestedCategory].mCount = 0;
        mCategories[mRequestedCategory].mFirstRank = -1;
        tDebugPrintManager::Print(DC_NETWORK,
            "Unavailable Leaderboard Stats cat %d filter %d\n",
            category,
            filter);
        if (mRequestedCategory == 4)
        {
            mCategories[5].mAvailable = false;
            mCategories[5].mCount = 0;
            mCategories[5].mFirstRank = -1;
        }
    }
    else
    {
        mLeaderboardRequestSucceeded = true;
        tDebugPrintManager::Print(DC_NETWORK,
            "Sucessfully got leaderboard stats cat %d filter %d\n",
            category,
            filter);
        mCategories[mRequestedCategory].mAvailable = true;
        mCategories[mRequestedCategory].mCount = count;
        mCategories[mRequestedCategory].mFirstRank = -1;
        if (filter == 1)
        {
            GetRegion();
            UpdateFriendRankingNames(&mCategories[mRequestedCategory]);
        }
        ApplyLeaderboardToSave(&mCategories[mRequestedCategory], true);
        if (mRequestedCategory == 4)
        {
            BuildFriendsLeaderboard();
            ApplyLeaderboardToSave(&mCategories[5], false);
        }
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
    details.mScorePoints = 0;
    details.mBonusPoints = 0;
    details.mPoints = CalculateResultPoints_80130684(result, home, homeScore,
        awayScore, &details.mWon, &details.mTied, &details.mResultPoints,
        &details.mScorePoints, &details.mBonusPoints);

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
            mUnidentifiedC41C[category] = pointsLost;
            mDisconnectLossPending[category] = true;
            mLocalStats[category].mScore -= pointsLost;
            ++mLocalStats[category].mLosses;
            mLocalStats[category].mUnidentified14 = GetOnlineRegion();

            tDebugPrintManager::Print(DC_NETWORK,
                "ReportDefaultDisconnectLoss: pers cat %d oldPoints %d new points %d New W:L %d:%d OneBasedRegion:%d\n",
                category,
                oldPoints,
                mLocalStats[category].mScore,
                mLocalStats[category].mWins,
                mLocalStats[category].mLosses,
                mLocalStats[category].mUnidentified14);

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
    const NetworkStatsPlayer* home, const NetworkStatsPlayer* away,
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
        details.mScorePoints = 0;
        details.mBonusPoints = 0;

        if (fallback == 0)
        {
            if (!mGameResultReported)
            {
                details.mPoints = CalculateResultPoints_80130684(result, reportHome,
                    homeScore, awayScore, &details.mWon, &details.mTied, &details.mResultPoints,
                    &details.mScorePoints, &details.mBonusPoints);
                mCurrentJob = details.mPoints;
                mUnidentifiedC430 = details.mWon;
                mUnidentifiedC431 = details.mTied;
                mUnidentifiedC434 = details.mResultPoints;
                mUnidentifiedC438 = details.mScorePoints;
                mUnidentifiedC43C = details.mBonusPoints;
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
        int* disconnectPoints = mUnidentifiedC41C;
        for (int category = 0; category < categoryCount; ++category)
        {
            int oldPoints = mLocalStats[category].mScore;
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
                    if (details.mWon)
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
            mLocalStats[category].mUnidentified14 = GetOnlineRegion();

            tDebugPrintManager::Print(DC_NETWORK,
                "ReportGameResult: pers cat %d oldPoints %d + points scored %d = new points %d IWon: %d New W:L %d:%d OneBasedRegion:%d\n",
                category,
                oldPoints,
                pointsScored,
                mLocalStats[category].mScore,
                details.mWon,
                mLocalStats[category].mWins,
                mLocalStats[category].mLosses,
                mLocalStats[category].mUnidentified14);

            mLocalStats[category].LoadLocal();
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
    mUnidentifiedC41C[0] = 0;
    mDisconnectLossPending[0] = false;
    mUnidentifiedC41C[1] = 0;
    mDisconnectLossPending[1] = false;
    mUnidentifiedC41C[2] = 0;
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
        if (mJobs.GetCount() == 0 && mCurrentTime - mOperationStartTime >= sRankingRequestTimeout)
            PreGameRestoreDefaultDisconnectLoss();
        if (mJobs.GetCount() == 0 && mCurrentTime - mOperationStartTime >= sSecondsPerMinute)
        {
            SubmitJob(sLeaderboardJobs[mUnidentifiedC410]);
            ++mUnidentifiedC410;
            if (mUnidentifiedC410 >= 5)
                mUnidentifiedC410 = 0;
        }
    }
    return true;
}

static inline void BeginOnlineGame(NetworkStatsManager* manager)
{
    manager->mGameResultReported = false;
    manager->mDisconnectPending = false;
    manager->mUnidentifiedC41C[0] = 0;
    manager->mDisconnectLossPending[0] = false;
    manager->mUnidentifiedC41C[1] = 0;
    manager->mDisconnectLossPending[1] = false;
    manager->mUnidentifiedC41C[2] = 0;
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

void NetworkStatsManager::BeginOnlineGame_80131DB4()
{
    BeginOnlineGame(this);
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
            mOperation = 2;
            mOperationStartTime = mCurrentTime;
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
            mOperation = 2;
            mOperationStartTime = mCurrentTime;
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
            mOperation = 2;
            mOperationStartTime = mCurrentTime;
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
        BeginOnlineGame(this);
        return;
    }

    ReportGameResult(result, 0, 0, false, 0, 0,
        reinterpret_cast<const NetworkScoreSubmission*>(1));
    BeginOnlineGame(this);
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
    int playingSide = GetLocalPlayingSide_801323F4();

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
    int count = dates->mCount;
    int index = 0;
    for (; index < count; ++index)
    {
        if (date.mDay == dates->mDates[index].mDay)
        {
            if (date.mMonth == dates->mDates[index].mMonth)
            {
                return index;
            }
        }
        if (dates->mDates[index].mMonth > date.mMonth)
        {
            break;
        }
        else if (date.mMonth == dates->mDates[index].mMonth)
        {
            if (dates->mDates[index].mDay > date.mDay)
            {
                break;
            }
        }
    }
    --index;
    return index;
}

static int DayOfYear(NetworkSeasonDate date, int year)
{
    int result = 0;
    for (int i = 1; i < date.mMonth; ++i)
    {
        result += DaysInMonth(i, year);
    }
    result += date.mDay - 1;
    return result;
}

int GetDaysUntilNextSeasonBoundary(
    const NetworkSeasonDateTable* dates, int index, int year)
{
    NetworkSeasonDate current = dates->GetDate(index);
    int currentDay;
    int nextDay;
    if (index == dates->mCount - 1)
    {
        const NetworkSeasonDate& next = dates->mDates[0];
        currentDay = DayOfYear(current, year);
        int nextYearDay = DayOfYear(next, year + 1);
        int remaining = (year % 4 == 0) ? 366 : 365;
        nextDay = nextYearDay + remaining;
    }
    else
    {
        currentDay = DayOfYear(current, year);
        const NetworkSeasonDate& next = dates->mDates[index + 1];
        nextDay = DayOfYear(next, year);
    }
    return nextDay - currentDay;
}

int GetDaysSinceSeasonBoundary(const NetworkSeasonDateTable* dates, int index,
    NetworkSeasonDate date, int year)
{
    const NetworkSeasonDate& boundary = dates->mDates[index];
    int boundaryDay = DayOfYear(boundary, year);
    return DayOfYear(date, year) - boundaryDay;
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
