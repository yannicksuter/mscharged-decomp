#include <dwc/dwc_ranking.h>
#include <revolution/net/NETDigest.h>

#include "Game/NetworkStats.h"

#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/Sys/simpleparser.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/main.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"

#include <stdlib.h>
#include <string.h>

extern "C"
{
    int fn_8004F594(int channel, const char* format, ...);

    void fn_80374D68(NetworkStatsSocket* socket);
    bool fn_80374D74(NetworkStatsSocket* socket, bool stream);
    bool fn_80374DF0(NetworkStatsSocket* socket, u16 port);
    void fn_80374EA8(NetworkStatsSocket* socket);
    bool fn_80374EEC(NetworkStatsSocket* socket);
    void fn_80374F04(NetworkStatsSocket* socket, bool blocking);
    int fn_80374F84(
        NetworkStatsSocket* socket, const u8* address, u16 port);
    int fn_80374FF8(NetworkStatsSocket* socket, const void* data, int size);
    int fn_80375138(NetworkStatsSocket* socket, void* data, int size,
        u32* address, u16* port);

    int fn_8011C1B4();
}

extern int lbl_806E20E0;
extern u16 lbl_8058436C[];
extern u8 lbl_80584384[];

int g_nConnectToStatsAddress[4] = { 192, 168, 2, 188 };

static int g_nConnectToStatsPort = 80;
static float sReportSocketLifetime = 5000.0f;
static char sStatsSeparators[] = " \t\r\n:,";

NetworkStatsReporter_8012CE20::NetworkStatsReporter_8012CE20()
{
    fn_80374D68(&mSocket);
    Reset();
}

void NetworkStatsPlayer::CopyFrom(const NetworkStatsPlayer& other)
{
    int i = 0;
    for (; i < 10; ++i)
    {
        mName[i] = other.mName[i];
        if (other.mName[i] == 0)
        {
            break;
        }
    }
    mName[i] = 0;
    mProfileId = other.mProfileId;
    memcpy(mData, other.mData, sizeof(mData));
}

void NetworkStatsReporter_8012CE20::Reset()
{
    mListener = 0;
    mState = 0;
    mFilter = 0;
    mLimit = 0;
    mLeaderboardPlayers = 0;
    mLeaderboardMetadata = 0;
    mHomePlayer.mName[0] = 0;
    mHomePlayer.mProfileId = 0;
    memset(mHomePlayer.mData, 0, sizeof(mHomePlayer.mData));
    mAwayPlayer.mName[0] = 0;
    mAwayPlayer.mProfileId = 0;
    memset(mAwayPlayer.mData, 0, sizeof(mAwayPlayer.mData));
    mReportHome = false;
    mHomeScore = -1;
    mAwayScore = -1;
    mReportStartTime = 0;
}

void NetworkStatsReporter_8012CE20::Close()
{
    fn_80374EA8(&mSocket);
}

void NetworkStatsReporter_8012CE20::SetListener(
    NetworkStatsListener* listener)
{
    mListener = listener;
}

bool NetworkStatsReporter_8012CE20::ReportGameResult(int,
    const NetworkScoreSubmission*, const NetworkStatsPlayer* home,
    const NetworkStatsPlayer* away, bool reportHome, int homeScore,
    int awayScore, const NetworkScoreSubmission*)
{
    if (!reportHome)
    {
        fn_8004F594(16,
            "ReportGameResult returning true, but did not really report this game...only home team should do that for now\n");
        return true;
    }

    if (!fn_80374D74(&mSocket, true))
    {
        fn_8004F594(16, "Failed to open Stats TCP Socket");
    }
    else if (!fn_80374DF0(&mSocket, 1002))
    {
        fn_8004F594(16, "Failed to bind Stats TCP Socket");
    }
    else
    {
        fn_80374F04(&mSocket, false);
    }

    if (!fn_80374EEC(&mSocket))
    {
        fn_8004F594(
            16, "Failed to ReportGameResult, TCP Socket not open\n");
        return false;
    }

    u8 address[4];
    address[0] = (u8)g_nConnectToStatsAddress[0];
    address[1] = (u8)g_nConnectToStatsAddress[1];
    address[2] = (u8)g_nConnectToStatsAddress[2];
    address[3] = (u8)g_nConnectToStatsAddress[3];
    int result = fn_80374F84(
        &mSocket, address, (u16)g_nConnectToStatsPort);
    if (result != -26)
    {
        fn_8004F594(16,
            "Connect Result (ReportGameResult) to Stats Server %d\n",
            result);
        Close();
        return false;
    }

    fn_8004F594(16, "Connect (ReportGameResult) To Stats In Progress\n");
    mState = 3;
    mHomePlayer.CopyFrom(*home);
    mAwayPlayer.CopyFrom(*away);
    mReportHome = reportHome;
    mHomeScore = homeScore;
    mAwayScore = awayScore;
    return true;
}

bool NetworkStatsReporter_8012CE20::SubmitScore(
    int, const NetworkRankingMeta*)
{
    return false;
}

bool NetworkStatsReporter_8012CE20::StatsVirtual0C()
{
    return false;
}

bool NetworkStatsReporter_8012CE20::GetLeaderboardStats(int category,
    int filter, int limit, NetworkStatsPlayer* players,
    NetworkRankingMeta* metadata)
{
    if (!fn_80374D74(&mSocket, true))
    {
        fn_8004F594(16, "Failed to open Stats TCP Socket");
    }
    else if (!fn_80374DF0(&mSocket, 1002))
    {
        fn_8004F594(16, "Failed to bind Stats TCP Socket");
    }
    else
    {
        fn_80374F04(&mSocket, false);
    }

    if (!fn_80374EEC(&mSocket))
    {
        fn_8004F594(
            16, "Failed to Get Leaderboard stats, TCP Socket not open\n");
        if (mListener != 0)
        {
            mListener->OnLeaderboardResult(
                false, category, filter, 0, 0, 0);
        }
        return false;
    }

    u8 address[4];
    address[0] = (u8)g_nConnectToStatsAddress[0];
    address[1] = (u8)g_nConnectToStatsAddress[1];
    address[2] = (u8)g_nConnectToStatsAddress[2];
    address[3] = (u8)g_nConnectToStatsAddress[3];
    int result = fn_80374F84(
        &mSocket, address, (u16)g_nConnectToStatsPort);
    if (result != -26)
    {
        fn_8004F594(16, "Connect Result to Stats Server %d\n", result);
        if (mListener != 0)
        {
            mListener->OnLeaderboardResult(
                false, category, filter, 0, 0, 0);
        }
        Close();
        return false;
    }

    fn_8004F594(16, "Connect To Stats In Progress\n");
    mState = 1;
    mFilter = filter;
    mLimit = limit;
    mLeaderboardPlayers = players;
    mLeaderboardMetadata = metadata;
    return true;
}

void NetworkStatsReporter_8012CE20::ParseLeaderboardResponse(
    char* data, int size)
{
    SimpleParser parser;
    parser.StartParsing(data, size, sStatsSeparators);

    int row = 0;
    while (row < mLimit)
    {
        NetworkStatsPlayer& player = mLeaderboardPlayers[row];
        NetworkRankingMeta& metadata = mLeaderboardMetadata[row];
        char* token = parser.NextTokenOnLine(true);
        if (token == 0)
        {
            break;
        }
        metadata.mUnidentified14 = fn_8011C1B4();
        metadata.mDisplayRank = atoi(token);

        token = parser.NextTokenOnLine(true);
        if (token == 0)
        {
            break;
        }
        nlStrToWcs(token, player.mName, 11);
        player.mProfileId = 0;

        token = parser.NextTokenOnLine(true);
        if (token == 0)
        {
            break;
        }
        metadata.mScore = atoi(token);

        token = parser.NextTokenOnLine(true);
        // The simple HTTP backend retains this column only as a delimiter.
        token = parser.NextTokenOnLine(true);
        if (token != 0)
        {
            metadata.mWins = atoi(token);
        }
        token = parser.NextTokenOnLine(true);
        if (token != 0)
        {
            metadata.mLosses = atoi(token);
        }
        ++row;
        if (!parser.AdvanceLine())
        {
            break;
        }
    }

    if (mListener != 0)
    {
        mListener->OnLeaderboardResult(true, 0, mFilter, row,
            mLeaderboardPlayers, mLeaderboardMetadata);
    }
}

void NetworkStatsReporter_8012CE20::Update()
{
    if (mState == 1)
    {
        char request[256];
        nlSNPrintf(request, 255,
            "GET /OnlineRankingSimulator/Rankings.py?SimpleFormat=true\r\n\r\n");
        int result = fn_80374FF8(&mSocket, request, strlen(request));
        fn_8004F594(16, "Send Result to Stats Server %d\n", result);
        if (result > 0)
        {
            mState = 2;
        }
        else
        {
            if (mListener != 0)
            {
                mListener->OnLeaderboardResult(
                    false, 0, mFilter, 0, 0, 0);
            }
            mState = 0;
            Close();
        }
    }
    else if (mState == 2)
    {
        char response[1000];
        int result = fn_80375138(&mSocket, response, 999, 0, 0);
        if (result <= 0)
        {
            if (result != -6)
            {
                fn_8004F594(
                    16, "Received Stats String Error %d\n", result);
                if (mListener != 0)
                {
                    mListener->OnLeaderboardResult(
                        false, 0, mFilter, 0, 0, 0);
                }
                mState = 0;
                Close();
            }
        }
        else
        {
            fn_8004F594(16,
                "Received Stats Length = %d, Data follows:\n", result);
            response[result] = 0;
            fn_8004F594(16, response);
            ParseLeaderboardResponse(response, result);
            mState = 0;
            Close();
        }
    }
    else if (mState == 3)
    {
        char homeName[11] = { 0 };
        char awayName[11] = { 0 };
        char request[256];
        nlWcsToStr(mHomePlayer.mName, homeName, 11);
        nlWcsToStr(mAwayPlayer.mName, awayName, 11);
        nlSNPrintf(request, 255,
            "GET /OnlineRankingSimulator/Rankings.py?yourname=%s&opponentsname=%s&yourscore=%d&opponentsscore=%d HTTP/1.0\r\n\r\n",
            homeName, awayName, mHomeScore, mAwayScore);
        int result = fn_80374FF8(&mSocket, request, strlen(request));
        if (result > 0)
        {
            fn_8004F594(16,
                "Send ReportGameResult to Stats Server %d\n", result);
            mState = 4;
            mReportStartTime = nlGetTicker();
        }
        else
        {
            fn_8004F594(16,
                "Send Error ReportGameResult to Stats Server %d\n", result);
            mState = 0;
            Close();
        }
    }
    else if (mState == 4
        && nlGetTickerDifference(mReportStartTime, nlGetTicker())
            > sReportSocketLifetime)
    {
        fn_8004F594(16,
            "Waited, now closing socket that was used for ReportGameResult\n");
        mState = 0;
        Close();
    }
}

static u8 sRankingHmacKey[32] = {
    0x17, 0xB2, 0x40, 0x52, 0x5F, 0x89, 0xF0, 0xFF,
    0xE9, 0xB9, 0xB0, 0x17, 0x66, 0x06, 0x37, 0xA1,
    0x63, 0x70, 0x9E, 0xC9, 0xCF, 0x42, 0xD1, 0x89,
    0x91, 0x36, 0xC7, 0xBD, 0x2B, 0x58, 0xBC, 0x39,
};

NetworkRanking_8012D8F4::NetworkRanking_8012D8F4()
{
    mInitialized = false;
    Reset();
}

void NetworkRanking_8012D8F4::Reset()
{
    mRequestComplete = false;
    mOperation = 0;
    mListener = 0;
    mSubmission.mWins = 0;
    mSubmission.mLosses = 0;
    mSubmission.mUnidentified0C = 0;
    mSubmission.mName[0] = 0;
    memset(mSubmission.mData, 0, sizeof(mSubmission.mData));
    memset(mSubmission.mDigest, 0, sizeof(mSubmission.mDigest));
    mReportGame = false;
    mFilter = 0;
    mLimit = 0;
    mLeaderboardPlayers = 0;
    mLeaderboardMetadata = 0;
    mCategory = 0;
}

void NetworkRanking_8012D8F4::ShutdownRanking()
{
    if (mInitialized)
    {
        DWC_RnkShutdown();
        mInitialized = false;
    }
}

void NetworkRanking_8012D8F4::InitializeRanking()
{
    GameInfoSaveSlot* save =
        GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0);
    DWCRnkError result = DWC_RnkInitialize(
        "DbfBialvJznkQWYuOrRa0002282500000ef90000020042db60ecmschargedwii",
        (DWCUserData*)save);
    if (result == DWC_RNK_ERROR_INIT_ALREADYINITIALIZED)
    {
        fn_8004F594(16, "DWC_RnkInitialize already initialized\n");
    }
    if (result == DWC_RNK_SUCCESS
        || result == DWC_RNK_ERROR_INIT_ALREADYINITIALIZED)
    {
        fn_8004F594(16, "DWC_RnkInitialize succeeded.\n");
        mInitialized = true;
    }
    else
    {
        fn_8004F594(
            16, "DWC_RnkInitialize returned error %d\n", result);
    }
}

void NetworkRanking_8012D8F4::SetListener(NetworkStatsListener* listener)
{
    mListener = listener;
}

bool NetworkRanking_8012D8F4::ReportGameResult(int category,
    const NetworkScoreSubmission*, const NetworkStatsPlayer*,
    const NetworkStatsPlayer*, bool, int, int,
    const NetworkScoreSubmission* fallback)
{
    mSubmission.mWins = fallback->mWins;
    mSubmission.mLosses = fallback->mLosses;
    mSubmission.mUnidentified0C = fallback->mPlayerId;
    int i = 0;
    for (; i < 10; ++i)
    {
        mSubmission.mName[i] = lbl_8058436C[i];
        if (lbl_8058436C[i] == 0)
        {
            break;
        }
    }
    mSubmission.mName[i] = 0;
    memcpy(mSubmission.mData, lbl_80584384, sizeof(mSubmission.mData));
    mSubmission.mMonth = fallback->mMonth;
    mSubmission.mDay = fallback->mDay;
    mSubmission.mYear = fallback->mYear;
    mCategory = category;
    mReportGame = false;

    NETHMACContext context;
    NETHMACInit(&context, NETGetMD5Interface(), sRankingHmacKey,
        sizeof(sRankingHmacKey));
    NETHMACUpdate(&context, &mSubmission,
        (u8*)mSubmission.mDigest - (u8*)&mSubmission);
    NETHMACGetDigest(&context, mSubmission.mDigest);

    DWCRnkRegion region = DWC_RNK_REGION_US;
    if (GetRegion() == 1)
    {
        region = DWC_RNK_REGION_EU;
    }
    else if (GetRegion() == 2)
    {
        region = DWC_RNK_REGION_JP;
    }
    DWCRnkError result = DWC_RnkPutScoreAsync(category, region,
        fallback->mScore, &mSubmission, sizeof(mSubmission));
    if (result == DWC_RNK_SUCCESS)
    {
        mOperation = 1;
        fn_8004F594(16,
            "DWC_RnkPutScoreAsync start processing okay cat %d\n",
            mCategory);
        return true;
    }
    fn_8004F594(16,
        "DWC_RnkPutScoreAsync cat %d returned error %d\n", mCategory,
        result);
    return false;
}

void NetworkRankingIdentity::LoadLocal()
{
    mMonth = 1;
    mDay = 1;
    mYear = 2000;
}

bool NetworkRanking_8012D8F4::SubmitScore(int category,
    const NetworkRankingMeta* submission)
{
    int i = 0;
    for (; i < 10; ++i)
    {
        mSubmission.mName[i] = lbl_8058436C[i];
        if (lbl_8058436C[i] == 0)
        {
            break;
        }
    }
    mSubmission.mName[i] = 0;
    memcpy(mSubmission.mData, lbl_80584384, sizeof(mSubmission.mData));

    if (submission == 0)
    {
        mSubmission.mWins = 0;
        mSubmission.mLosses = 0;
        mSubmission.mUnidentified0C = (u16)fn_8011C1B4();
        NetworkRankingIdentity identity;
        identity.LoadLocal();
        mSubmission.mMonth = identity.mMonth;
        mSubmission.mDay = identity.mDay;
        mSubmission.mYear = identity.mYear;
    }
    else
    {
        mSubmission.mWins = submission->mWins;
        mSubmission.mLosses = submission->mLosses;
        mSubmission.mUnidentified0C = (u16)submission->mUnidentified14;
        mSubmission.mMonth = submission->mMonth;
        mSubmission.mDay = submission->mDay;
        mSubmission.mYear = submission->mYear;
    }
    mCategory = category;
    mReportGame = true;

    NETHMACContext context;
    NETHMACInit(&context, NETGetMD5Interface(), sRankingHmacKey,
        sizeof(sRankingHmacKey));
    NETHMACUpdate(&context, &mSubmission,
        (u8*)mSubmission.mDigest - (u8*)&mSubmission);
    NETHMACGetDigest(&context, mSubmission.mDigest);

    DWCRnkRegion region = DWC_RNK_REGION_US;
    if (GetRegion() == 1)
    {
        region = DWC_RNK_REGION_EU;
    }
    else if (GetRegion() == 2)
    {
        region = DWC_RNK_REGION_JP;
    }
    int score = submission != 0 ? submission->mScore : 0;
    DWCRnkError result = DWC_RnkPutScoreAsync(
        category, region, score, &mSubmission, sizeof(mSubmission));
    if (result == DWC_RNK_SUCCESS)
    {
        mOperation = 1;
        fn_8004F594(
            16, "DWC_RnkPutScoreAsync start processing okay\n");
        return true;
    }
    fn_8004F594(
        16, "DWC_RnkPutScoreAsync returned error %d\n", result);
    return false;
}

bool NetworkRanking_8012D8F4::StatsVirtual0C()
{
    return false;
}

bool NetworkRanking_8012D8F4::GetLeaderboardStats(int category,
    int filter, int limit, NetworkStatsPlayer* players,
    NetworkRankingMeta* metadata)
{
    mFilter = filter;
    mLimit = limit;
    mLeaderboardPlayers = players;
    mLeaderboardMetadata = metadata;
    mCategory = category;

    DWCRnkGetParam parameter;
    memset(&parameter, 0, sizeof(parameter));
    DWCRnkGetMode mode = DWC_RNK_GET_MODE_TOPLIST;
    if (filter == 1)
    {
        mode = DWC_RNK_GET_MODE_FRIENDS;
        parameter.size = sizeof(parameter.friends);
        parameter.friends.sort = DWC_RNK_ORDER_DES;
        parameter.friends.limit = limit;
        for (int i = 0; i < DWC_RNK_FRIENDS_MAX; ++i)
        {
            int* entry = (int*)GameInfoManager::GetInstance()->GetUnknown0x40(
                lbl_806E20E0, i);
            parameter.friends.friends[i] = entry[1];
        }
    }
    else
    {
        if (limit > DWC_RNK_GET_MAX)
        {
            limit = DWC_RNK_GET_MAX;
            mLimit = limit;
        }
        parameter.size = sizeof(parameter.toplist);
        parameter.toplist.sort = DWC_RNK_ORDER_DES;
        parameter.toplist.limit = limit;
        parameter.toplist.since = 1;
    }

    DWCRnkRegion region = DWC_RNK_REGION_US;
    if (GetRegion() == 1)
    {
        region = DWC_RNK_REGION_EU;
    }
    else if (GetRegion() == 2)
    {
        region = DWC_RNK_REGION_JP;
    }
    DWCRnkError result =
        DWC_RnkGetScoreAsync(mode, category, region, &parameter);
    if (result == DWC_RNK_SUCCESS)
    {
        mOperation = 2;
        fn_8004F594(
            16, "DWC_RnkGetScoreAsync start processing okay.\n");
        return true;
    }
    fn_8004F594(
        16, "DWC_RnkGetScoreAsync returned error %d\n", result);
    return false;
}

void NetworkRanking_8012D8F4::ProcessLeaderboardResults()
{
    u32 rowCount = 0;
    DWCRnkError result = DWC_RnkResGetRowCount(&rowCount);
    if (result != DWC_RNK_SUCCESS)
    {
        fn_8004F594(16,
            "Error %d from DWC_RnkResGetRowCount cat %d filter %d\n",
            result, mCategory, mFilter);
        if (mListener != 0)
        {
            mListener->OnLeaderboardResult(
                false, mCategory, mFilter, 0, 0, 0);
        }
        return;
    }

    int retained = 0;
    for (u32 i = 0; i < rowCount; ++i)
    {
        DWCRnkData row;
        result = DWC_RnkResGetRow(&row, i);
        if (result != DWC_RNK_SUCCESS)
        {
            fn_8004F594(16,
                "Error %d calling DWC_RnkResGetRow %d\n", result, i);
            continue;
        }

        NetworkStatsPlayer& player = mLeaderboardPlayers[retained];
        NetworkRankingMeta& metadata = mLeaderboardMetadata[retained];
        player.mProfileId = row.pid;
        player.mName[0] = 0;

        bool valid = false;
        if (row.size == sizeof(NetworkRankingSubmission))
        {
            NetworkRankingSubmission* submission =
                (NetworkRankingSubmission*)row.userdata;
            u8 digest[16];
            NETHMACContext context;
            NETHMACInit(&context, NETGetMD5Interface(), sRankingHmacKey,
                sizeof(sRankingHmacKey));
            NETHMACUpdate(&context, submission,
                (u8*)submission->mDigest - (u8*)submission);
            NETHMACGetDigest(&context, digest);
            valid = memcmp(digest, submission->mDigest, sizeof(digest)) == 0;
            if (!valid)
            {
                fn_8004F594(
                    16, "Warning: Binary Data failed HMAC MD5 check\n");
            }
            else
            {
                int name = 0;
                for (; name < 10; ++name)
                {
                    player.mName[name] = submission->mName[name];
                    if (submission->mName[name] == 0)
                    {
                        break;
                    }
                }
                player.mName[name] = 0;
                memcpy(player.mData, submission->mData,
                    sizeof(player.mData));
                metadata.mMonth = submission->mMonth;
                metadata.mDay = submission->mDay;
                metadata.mYear = submission->mYear;
                metadata.mWins = submission->mWins;
                metadata.mLosses = submission->mLosses;
                metadata.mUnidentified14 = submission->mUnidentified0C;
            }
        }
        else
        {
            fn_8004F594(16,
                "Warning: Expected Binary Data Size %d Got Size %d\n",
                sizeof(NetworkRankingSubmission), row.size);
        }

        if (!valid)
        {
            metadata.mMonth = 1;
            metadata.mDay = 1;
            metadata.mYear = 2000;
            metadata.mWins = 0;
            metadata.mLosses = 0;
            metadata.mUnidentified14 = 0;
            memset(player.mData, 0, sizeof(player.mData));
        }
        metadata.mScore = row.score;
        metadata.mDisplayRank = row.order;
        ++retained;
    }

    if (mFilter == 1)
    {
        FilterCurrentSeason(retained);
    }
    SortLeaderboardResults(retained);
    if (mListener != 0)
    {
        mListener->OnLeaderboardResult(true, mCategory, mFilter, retained,
            mLeaderboardPlayers, mLeaderboardMetadata);
    }
}

void NetworkRanking_8012D8F4::FilterCurrentSeason(int count)
{
    NetworkRankingIdentity current;
    current.LoadLocal();
    for (int i = 0; i < count; ++i)
    {
        if (mLeaderboardMetadata[i].mYear != current.mYear)
        {
            mLeaderboardMetadata[i].mScore = 0;
            mLeaderboardMetadata[i].mDisplayRank = 0;
            mLeaderboardMetadata[i].mWins = 0;
            mLeaderboardMetadata[i].mLosses = 0;
            mLeaderboardMetadata[i].mUnidentified14 = 0;
        }
    }
}

int NetworkRanking_8012D8F4::CompareLeaderboardRows(
    const void* left, const void* right)
{
    const NetworkRankingSortRow* a = (const NetworkRankingSortRow*)left;
    const NetworkRankingSortRow* b = (const NetworkRankingSortRow*)right;
    int scoreDifference = a->mMetadata.mWins - b->mMetadata.mWins;
    if (scoreDifference > 0)
    {
        return -1;
    }
    if (scoreDifference < 0)
    {
        return 1;
    }

    GameInfoSaveSlot* save =
        GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0);
    int localProfile = *(int*)&save->unknown_0x000[0x1C];
    if (a->mPlayer.mProfileId == localProfile)
    {
        return -1;
    }
    if (b->mPlayer.mProfileId == localProfile)
    {
        return 1;
    }

    int i = 0;
    while (true)
    {
        u16 aChar = a->mPlayer.mName[i];
        u16 bChar = b->mPlayer.mName[i];
        if (aChar >= 'a' && aChar <= 'z')
        {
            aChar &= 0x5F;
        }
        if (bChar >= 'a' && bChar <= 'z')
        {
            bChar &= 0x5F;
        }
        if (aChar == 0 || bChar == 0 || aChar != bChar)
        {
            int difference = (int)aChar - (int)bChar;
            if (difference != 0)
            {
                return difference;
            }
            break;
        }
        ++i;
    }

    int differentialA = a->mMetadata.mWins - a->mMetadata.mLosses;
    int differentialB = b->mMetadata.mWins - b->mMetadata.mLosses;
    if (differentialA > differentialB)
    {
        return -1;
    }
    if (differentialA < differentialB)
    {
        return 1;
    }
    if (a->mMetadata.mWins > b->mMetadata.mWins)
    {
        return -1;
    }
    return a->mMetadata.mWins < b->mMetadata.mWins;
}

void NetworkRanking_8012D8F4::AssignDisplayRanks(
    int count, NetworkRankingMeta* metadata, int firstRank)
{
    int previousScore = -1;
    int rank = firstRank;
    for (int i = 0; i < count; ++i)
    {
        if (previousScore != metadata[i].mScore)
        {
            rank = firstRank + i;
            previousScore = metadata[i].mScore;
        }
        metadata[i].mDisplayRank = rank;
    }
}

void NetworkRanking_8012D8F4::SortLeaderboardResults(int count)
{
    if (count > 1)
    {
        NetworkRankingSortRow* rows = new NetworkRankingSortRow[count];
        for (int i = 0; i < count; ++i)
        {
            rows[i].mPlayer.CopyFrom(mLeaderboardPlayers[i]);
            rows[i].mMetadata = mLeaderboardMetadata[i];
        }
        qsort(rows, count, sizeof(NetworkRankingSortRow),
            CompareLeaderboardRows);
        for (int i = 0; i < count; ++i)
        {
            mLeaderboardPlayers[i].CopyFrom(rows[i].mPlayer);
            mLeaderboardMetadata[i] = rows[i].mMetadata;
        }
        delete[] rows;
    }
    AssignDisplayRanks(count, mLeaderboardMetadata, 1);
}

NetworkRankingSortRow::NetworkRankingSortRow()
{
    mMetadata.mMonth = 1;
    mMetadata.mDay = 1;
    mMetadata.mYear = 2000;
    mMetadata.mScore = 0;
    mMetadata.mDisplayRank = 0;
    mMetadata.mWins = 0;
    mMetadata.mLosses = 0;
    mMetadata.mUnidentified14 = 0;
}

void NetworkRanking_8012D8F4::Update()
{
    if (!mInitialized || mRequestComplete || mOperation == 0)
    {
        return;
    }

    DWCRnkError result = DWC_RnkProcess();
    if (result == DWC_RNK_SUCCESS)
    {
        return;
    }

    if (result == DWC_RNK_IN_ERROR)
    {
        fn_8004F594(16, "DWC_RnkProcesss() returned error %d\n", result);
        mRequestComplete = true;
        if (mOperation == 1 && mListener != 0)
        {
            fn_8004F594(16, "Putting Score failed!\n");
            if (mReportGame)
            {
                mListener->OnSubmitScoreResult(false, mCategory);
            }
            else
            {
                mListener->OnReportGameResult(false, mCategory);
            }
        }
        else if (mOperation == 2 && mListener != 0)
        {
            fn_8004F594(16, "Getting score failed!\n");
            mListener->OnLeaderboardResult(
                false, mCategory, mFilter, 0, 0, 0);
        }
        mOperation = 0;
    }
    else if (result == DWC_RNK_ERROR_NOTCOMPLETED)
    {
        if (mOperation == 1 && mListener != 0)
        {
            fn_8004F594(16, "Putting Score succeeded!\n");
            if (mReportGame)
            {
                mListener->OnSubmitScoreResult(true, mCategory);
            }
            else
            {
                mListener->OnReportGameResult(true, mCategory);
            }
        }
        else if (mOperation == 2)
        {
            fn_8004F594(16, "Getting score succeeded!\n");
            ProcessLeaderboardResults();
        }
        mOperation = 0;
    }
    else
    {
        fn_8004F594(16,
            "Unexpected DWC_RnkProcesss returned %d\n", result);
    }
}

static TweakValueIntImpl_804FD898 sConnectToStatsAddress0Tweak(
    "g_nConnectToStatsAddress0", "Network/Stats",
    &g_nConnectToStatsAddress[0], true);
static TweakValueIntImpl_804FD898 sConnectToStatsAddress1Tweak(
    "g_nConnectToStatsAddress1", "Network/Stats",
    &g_nConnectToStatsAddress[1], true);
static TweakValueIntImpl_804FD898 sConnectToStatsAddress2Tweak(
    "g_nConnectToStatsAddress2", "Network/Stats",
    &g_nConnectToStatsAddress[2], true);
static TweakValueIntImpl_804FD898 sConnectToStatsAddress3Tweak(
    "g_nConnectToStatsAddress3", "Network/Stats",
    &g_nConnectToStatsAddress[3], true);

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
