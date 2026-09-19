#include <dwc/dwc_main.h>
#include <dwc/dwc_match.h>
#include <dwc/dwc_transport.h>
#include <revolution/os/OSThread.h>
#include "Game/NetworkMessageRegistry.h"

#include "Game/NetworkLobby.h"
#include "Game/FriendManager.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/GameInfo.h"
#include "Game/main.h"
#include "Game/NetworkSession.h"
#include "Game/Sys/debug.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"

#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

#include <string.h>

static int s_nTimeoutFindingMaxPlayersAcceptMin = 20;

static void MatchmakingCallback(
    DWCError error, BOOL cancelled, void* param);
static int EvaluateMatchmakingPlayer(int index, void* param);
static void ConnectionClosedCallback(
    int error, int isLocal, int isServer, u8 aid, int index, void* param);
static void UserReceiveCallback(u8 aid, u8* buffer, int size);
static void* MatchmakingThreadFunc(void* param);
static void NewClientCallback(int index, void* param);
static void FriendMatchCallback(DWCError error, BOOL cancelled, BOOL self,
    BOOL isServer, int index, void* param);

NetworkLobby::NetworkLobby()
{
    for (int i = 0; i < 4; ++i)
    {
        mReceiveBuffers[i] = nlMalloc(0x4000, 8, false);
    }

    mReceiverRegistered = false;
    Reset();
}

void NetworkLobby::RegisterMessageReceiver()
{
    gNetworkMessageRegistry->RegisterReceiver(0x16, static_cast<NetworkMessageReceiver*>(this));
    mReceiverRegistered = true;
    Reset();
}

void NetworkLobby::Reset()
{
    mTournamentMode = false;
    mFriendMatch = false;
    mHostingFriendMatch = false;
    mFriendHostInviting = false;
    mMatchFlags = 0;
    if (gOnlineTwoLocalPlayers)
    {
        mMatchFlags |= 2;
    }

    mVersionWord = GetNetworkVersionWord();
    mMatchmakingPoints = 0;
    mCountryMatchGroup = IsAlternateOnlineCountryGroup();
    mProfileId = 0;
    mMaxMatchmakingEntries = 0;
    mMinMatchmakingEntries = 0;
    mUnidentified030 = 0;
    mUnidentified034 = 0;
    mFriendProfileId = -1;
    mState = 0;
    mMatchFailed = false;
    mCancelRequested = false;
    mUnidentified052 = false;
    mElapsedTime = 0.0f;
    mConnectionDeadline = 0.0f;
    mMachineCount = 0;
    mGameStarted = 0;
    mUserMatchDataSize = 0;
    for (int i = 0; i < 4; ++i)
    {
        mMachineInfoReceived[i] = false;
    }
    mMatchmakingThreadRunning = false;
}

void NetworkLobby::UnregisterMessageReceiver()
{
    gNetworkMessageRegistry->UnregisterReceiver(0x16);
    Reset();
    mReceiverRegistered = false;
}

unsigned int NetworkLobby::GetMachineAid(int index)
{
    if (mMachineCount == 0)
    {
        return 0;
    }
    if (index < 0)
    {
        return 0;
    }
    if (mMachineCount <= index)
    {
        return 0;
    }
    if (index == DWC_GetMyAID())
    {
        return (unsigned int)-1;
    }
    return mPlayers[index].mConnection;
}

int NetworkLobby::MachineIdxFromConnection(unsigned int connection)
{
    if (mMachineCount == 0)
    {
        return -1;
    }
    if (connection == 0 || connection == (unsigned int)-2)
    {
        return -1;
    }
    if (connection == (unsigned int)-1)
    {
        return DWC_GetMyAID();
    }
    return *((u8*)connection + 0x27);
}

int NetworkLobby::GetTopology()
{
    return 0;
}

void NetworkLobby::SetTopology(int)
{
}

int NetworkLobby::GetMaxMachineCount()
{
    return 4;
}

void NetworkLobby::SetMaxMachineCount(int)
{
}

int NetworkLobby::GetMachineCount()
{
    return mMachineCount;
}

TransportPlayerInfo* NetworkLobby::GetPlayerInfo(int index)
{
    if (index >= 0 && index < mMachineCount)
    {
        return &mPlayers[index];
    }
    return 0;
}

int NetworkLobby::GetLocalMachineIndex()
{
    if (mMachineCount <= 0)
    {
        return -1;
    }
    return DWC_GetMyAID();
}

TransportPlayerInfo* NetworkLobby::GetLocalPlayerInfo()
{
    int index = GetLocalMachineIndex();
    if (index == -1)
    {
        return 0;
    }
    return &mPlayers[index];
}

void NetworkLobby::SetUserMatchData(u8 size, const void* data)
{
    mUserMatchDataSize = size;
    memcpy(mUserMatchData, data, size);
}

void* NetworkLobby::GetUserMatchData(u8* size)
{
    *size = mUserMatchDataSize;
    return mUserMatchData;
}

int NetworkLobby::GetPlayerCount()
{
    if (DWC_GetMyAID() == 0)
    {
        if (mFriendMatch)
        {
            if (mState == 2 && AllMachineInfoReceived())
            {
                if (gOnlineFourMachineFriendLobby)
                {
                    if (mFriendHostInviting)
                    {
                        return mMachineCount;
                    }
                }
                else
                {
                    return mMachineCount;
                }
            }
        }
        else if (mState == 7)
        {
            return mMachineCount;
        }
    }
    return 0;
}

bool NetworkLobby::AllMachineInfoReceived()
{
    if (mMachineCount <= 0)
    {
        return false;
    }

    bool received = true;
    for (int i = 0; i < mMachineCount; ++i)
    {
        if (!mMachineInfoReceived[i])
        {
            received = false;
        }
    }
    return received;
}

bool NetworkLobby::AreAllConnectionsReady()
{
    if (mState == 2 && AllMachineInfoReceived())
    {
        return true;
    }
    return false;
}

void NetworkLobby::DebugDraw(int column, int* row)
{
    (void)column;
    (void)row;
}

void NetworkLobby::OnConnected(unsigned int connection, int result)
{
    if (result != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "ConnectionEstablished failed returned error %d\n", result);
        return;
    }

    for (int i = 0; i < mMachineCount; ++i)
    {
        if (mPlayers[i].mConnection == connection)
        {
            mPlayers[i].mConnectionState = 5;
            tDebugPrintManager::Print(DC_NETWORK, "Connection %d established I am %d\n", i, DWC_GetMyAID());
            return;
        }
    }

    tDebugPrintManager::Print(DC_NETWORK, "Unknown connection established from %d I am %d\n", *((u8*)connection + 0x27), DWC_GetMyAID());
}

int NetworkLobby::ShouldAcceptConnection(
    unsigned int connection, u8* address)
{
    u8 aid = address[3];
    u8 myAid = DWC_GetMyAID();
    if (aid < myAid)
    {
        mPlayers[aid].mConnection = connection;
        mPlayers[aid].mConnectionState = 5;
        tDebugPrintManager::Print(DC_NETWORK, "Accepting connection from %d to me %d\n", aid, myAid);
        return 1;
    }

    tDebugPrintManager::Print(DC_NETWORK, "Refusing attempt accept from %d I am %d\n", aid, myAid);
    return 0;
}

void NetworkLobby::OnConnectionClosed(
    unsigned int connection, int reason)
{
    u8 aid = *((u8*)connection + 0x27);
    tDebugPrintManager::Print(DC_NETWORK, "Connection Lost %d reason %d\n", aid, reason);
    if (aid >= mMachineCount)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Lost connection AID %d not in range 0 >= x < %d\n",
            aid,
            mMachineCount);
        return;
    }

    if (mPlayers[aid].mConnection == connection)
    {
        mPlayers[aid].mConnection = 0;
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "PeerInfoList[%d] connection %x does not match lost connection %x\n",
            aid,
            mPlayers[aid].mConnection,
            connection);
    }
}

void NetworkLobby::OnGameStarted()
{
    tDebugPrintManager::Print(DC_NETWORK, "DWCLobby GameStarted\n");
    mConnectionDeadline = 0.0f;
    g_pNetworkSession->GetDirectSocket()->SocketVirtual10(false);
}

void NetworkLobby::CloseConnections()
{
    NetworkSocket* socket = g_pNetworkSession->GetDirectSocket();
    for (int i = 0; i < mMachineCount; ++i)
    {
        unsigned int connection = mPlayers[i].mConnection;
        if (connection != (unsigned int)-1 && connection != 0)
        {
            socket->Disconnect(
                (TransportConnection*)connection, true);
        }
    }

    int result = DWC_CloseAllConnectionsHard();
    tDebugPrintManager::Print(DC_NETWORK, "DWC_CloseAllConnectionsHard returned %d\n", result);
    bool failed = mMatchmakingThreadRunning || mState != 0;
    if (failed)
    {
        mMatchFailed = true;
    }
}

void NetworkLobby::CloseConnectionsAndReset()
{
    g_pNetworkSession->GetDirectSocket()->SocketVirtual10(false);
    CloseConnections();
    Reset();
}

void NetworkLobby::Shutdown(bool reset)
{
    (void)reset;
    g_pNetworkSession->GetDirectSocket()->SocketVirtual10(false);
    CloseConnections();
    Reset();
}

static void MatchmakingCallback(
    DWCError error, BOOL cancelled, void* param)
{
    g_pNetworkSession->GetOnlineLobby()->OnMatchmakingResult(
        error, cancelled, param);
}

static int EvaluateMatchmakingPlayer(int index, void*)
{
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    int profileId = DWC_GetMatchIntValue(index, "PI", 0);
    int points = DWC_GetMatchIntValue(index, "PT", 0);
    int distance = points - lobby->mMatchmakingPoints;
    if (distance < 0)
    {
        distance = -distance;
    }
    if (distance > 999)
    {
        distance = 999;
    }
    int score = 1000 - distance;
    tDebugPrintManager::Print(DC_NETWORK, "Eval Player PID %d Value %d returned\n", profileId, score);
    return score;
}

static void ConnectionClosedCallback(
    int error, int isLocal, int isServer, u8 aid, int index, void* param)
{
    (void)param;

    g_pNetworkSession->GetOnlineLobby();
    tDebugPrintManager::Print(DC_NETWORK,
        "DWC Connection Closed error %d isLocal %d isServer %d aid %d index %d\n",
        error, isLocal, isServer, aid, index);

    u8 address[4];
    address[0] = 0;
    address[1] = 0;
    address[2] = 0;
    address[3] = aid;
    void* connection = g_pNetworkSession->GetDirectSocket()->FindConnection(address);
    if (connection != 0 && connection != (void*)-1)
    {
        g_pNetworkSession->GetDirectSocket()->Disconnect(
            (TransportConnection*)connection, true);
    }
}

static void UserReceiveCallback(u8 aid, u8* buffer, int size)
{
    g_pNetworkSession->GetOnlineLobby();
    g_pNetworkSession->GetDirectSocket()->ReceiveUnreliable(aid, buffer, size);
}

bool NetworkLobby::CanCancelMatchmaking()
{
    if (mMatchmakingThreadRunning)
    {
        return false;
    }
    if ((mState == 3 || mState == 1) && DWC_IsValidCancelMatching())
    {
        return true;
    }
    return false;
}

void NetworkLobby::CancelMatchmaking()
{
    tDebugPrintManager::Print(DC_NETWORK, "Cancelling DWC matchmaking\n");
    DWC_CancelMatching();
    mCancelRequested = false;
    g_pNetworkSession->GetDirectSocket()->SocketVirtual10(false);
    mState = 0;
}

bool NetworkLobby::StartMatchmaking()
{
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback, 0);
    DWC_SetUserRecvCallback(UserReceiveCallback);

    mFriendMatch = false;
    mMaxMatchmakingEntries = gOnlineMaxMatchmakingEntries;
    mMinMatchmakingEntries = gOnlineMinMatchmakingEntries;
    mUnidentified034 = 0;
    mUnidentified030 = 0;
    mFriendProfileId = -1;
    mMatchFailed = false;

    DWCMatchOptMinComplete option;
    memset(&option, 0, sizeof(option));
    if (mMinMatchmakingEntries > 0)
    {
        option.valid = 1;
        option.minEntry = mMinMatchmakingEntries;
        option.timeout = s_nTimeoutFindingMaxPlayersAcceptMin * 1000;
    }
    else
    {
        option.valid = 0;
    }
    if (DWC_SetMatchingOption(DWC_MATCH_OPTION_MIN_COMPLETE, &option, sizeof(option)) != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to set Match Options.  Not matchmaking.\n");
        return false;
    }

    mMatchFlags = 0;
    if (mTournamentMode)
    {
        mMatchFlags |= 1;
    }
    if (gOnlineTwoLocalPlayers)
    {
        mMatchFlags |= 2;
    }

    int wins = GameInfoManager::Instance()->GetUnknown0xA90Total(gNetworkSaveSlotIndex);
    int losses = GameInfoManager::Instance()->GetUnknown0xA94Total(gNetworkSaveSlotIndex);
    float total = (float)wins + (float)losses;
    int points;
    if (total == 0.0f)
    {
        points = 0;
    }
    else
    {
        float ratio = (float)wins / total;
        float maxRatio = 1.0f - 1.0f / total;
        if (ratio > maxRatio)
        {
            ratio = maxRatio;
        }
        points = (int)(1000.0f * ratio);
        if (points < 0)
        {
            points = 0;
        }
        else if (points > 999)
        {
            points = 999;
        }
    }
    mMatchmakingPoints = points;
    tDebugPrintManager::Print(DC_NETWORK,
        "Matchmaking points calculated %d from W/L %d/%d\n", points, wins, losses);
    mCountryMatchGroup = IsAlternateOnlineCountryGroup();
    mProfileId = GameInfoManager::Instance()->GetSaveSlot(gNetworkSaveSlotIndex)->unknown_0x01C;

    DWC_AddMatchKeyInt(1, "GT", (int*)&mMatchFlags);
    DWC_AddMatchKeyInt(2, "PI", (int*)&mProfileId);
    DWC_AddMatchKeyInt(0, "AP", (int*)&mVersionWord);
    DWC_AddMatchKeyInt(3, "PT", &mMatchmakingPoints);
    DWC_AddMatchKeyInt(4, "CO", (int*)&mCountryMatchGroup);

    char filter[128];
    nlSNPrintf(filter, sizeof(filter), "AP = %d and GT = %d and CO = %d",
        mVersionWord, mMatchFlags, mCountryMatchGroup);
    for (int i = 0; i < gRejectedOpponentProfileIds.GetCount(); ++i)
    {
        char rejectedProfile[64];
        nlSNPrintf(rejectedProfile, sizeof(rejectedProfile), " and PI != %d",
            gRejectedOpponentProfileIds[i]);
        nlStrNCat(filter, filter, rejectedProfile, sizeof(filter));
    }

    bool started = DWC_ConnectToAnybodyAsync(mMaxMatchmakingEntries, filter,
        MatchmakingCallback, 0, EvaluateMatchmakingPlayer, 0);
    if (!started)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Initial failure of DWC_ConnectToAnybodyAsync\n");
        g_pNetworkSession->ReadAndClearDWCError();
        mState = 0;
        mMatchFailed = true;
        return false;
    }

    tDebugPrintManager::Print(DC_NETWORK,
        "Started DWC_ConnectToAnybodyAsync with filter %s\n", filter);
    mState = 3;
    g_pNetworkSession->GetDirectSocket()->SocketVirtual10(true);
    return true;
}

static void* MatchmakingThreadFunc(void* param)
{
    (void)param;
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    tDebugPrintManager::Print(DC_NETWORK, "Thread: StartMatchmaking\n");
    lobby->StartMatchmaking();
    tDebugPrintManager::Print(DC_NETWORK, "Thread: Finished StartMatchmaking\n");
    lobby->mMatchmakingThreadRunning = false;
    return 0;
}

void NetworkLobby::StartMatchmakingThread()
{
    mMatchmakingThreadRunning = true;
    bool created = OSCreateThread((OSThread*)mMatchmakingThread, MatchmakingThreadFunc, 0,
        mMatchmakingThreadStack + sizeof(mMatchmakingThreadStack),
        sizeof(mMatchmakingThreadStack), 14, OS_THREAD_ATTR_DETACH);
    tDebugPrintManager::Print(DC_NETWORK, "Created MatchmakingThreadFunc returned %d\n", created);
    int suspendCount = OSResumeThread((OSThread*)mMatchmakingThread);
    tDebugPrintManager::Print(DC_NETWORK, "Resumed MatchmakingThreadFunc returned %d\n", suspendCount);
}

void NetworkLobby::OnMatchmakingResult(
    DWCError error, BOOL cancelled, void* param)
{
    (void)param;
    if (error == 0 && !cancelled)
    {
        mMachineCount = DWC_GetNumConnectionHost();
        for (int aid = 0; aid < mMachineCount; ++aid)
        {
            mPlayers[aid].mUnidentified0C = aid;
            mPlayers[aid].mConnection = 0;
            mPlayers[aid].mConnectionState = 0;
            if (aid != DWC_GetMyAID())
            {
                DWC_SetRecvBuffer(aid, mReceiveBuffers[aid], 0x4000);
            }
        }
        mState = 4;
        mConnectionDeadline = mElapsedTime + 5.0f;
    }
    else
    {
        mState = 0;
        mMatchFailed = true;
    }
}

static void NewClientCallback(int index, void*)
{
    g_pNetworkSession->GetOnlineLobby();
    tDebugPrintManager::Print(DC_NETWORK, "Friends New Client connecting %d\n", index);
}

static void FriendMatchCallback(DWCError error, BOOL cancelled, BOOL self,
    BOOL isServer, int index, void* param)
{
    g_pNetworkSession->GetOnlineLobby()->OnFriendMatchmakingResult(
        error, cancelled, self, isServer, index, param);
}

bool NetworkLobby::StartFriendServer()
{
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback, 0);
    DWC_SetUserRecvCallback(UserReceiveCallback);
    mFriendMatch = true;
    mHostingFriendMatch = true;
    mFriendHostInviting = false;
    mMatchFailed = false;

    int maxPlayers = gOnlineFourMachineFriendLobby ? 4 : 2;
    bool started = DWC_SetupGameServer(maxPlayers, FriendMatchCallback, 0, NewClientCallback, 0);
    if (!started)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Initial failure of DWC_SetupGameServer\n");
        g_pNetworkSession->ReadAndClearDWCError();
        mState = 0;
        mMatchFailed = true;
        return false;
    }

    tDebugPrintManager::Print(DC_NETWORK, "Started DWC_SetupGameServer\n");
    mState = 1;
    g_pNetworkSession->GetDirectSocket()->SocketVirtual10(true);
    return true;
}

void NetworkLobby::StopFriendServer()
{
    g_pNetworkSession->GetDirectSocket()->SocketVirtual10(false);
    DWC_ShutdownFriendsMatch();
    mState = 0;
    mFriendHostInviting = false;
}

void NetworkLobby::SetFriendHostInviting()
{
    DWC_StopSCMatchingAsync(0, 0);
    mFriendHostInviting = true;
}

bool NetworkLobby::ConnectToFriendServer(int profileId)
{
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback, 0);
    DWC_SetUserRecvCallback(UserReceiveCallback);
    mFriendMatch = true;
    mHostingFriendMatch = false;
    mFriendHostInviting = false;
    mMatchFailed = false;

    bool started = DWC_ConnectToGameServerAsync(profileId,
        FriendMatchCallback,
        0,
        NewClientCallback,
        0);
    if (!started)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Initial failure of DWC_ConnectToGameServerAsync\n");
        g_pNetworkSession->ReadAndClearDWCError();
        mState = 0;
        mMatchFailed = true;
        return false;
    }

    tDebugPrintManager::Print(DC_NETWORK, "Started DWC_ConnectToGameServerAsync\n");
    mState = 1;
    g_pNetworkSession->GetDirectSocket()->SocketVirtual10(true);
    return true;
}

void NetworkLobby::OnFriendMatchmakingResult(DWCError error,
    BOOL cancelled, BOOL self, BOOL isServer, int index, void* param)
{
    (void)param;
    tDebugPrintManager::Print(DC_NETWORK,
        "Friends Matching err:%d, cancel:%d self:%d isServer:%d index:%d\n",
        error, cancelled, self, isServer, index);

    if (cancelled && error == 0)
    {
        if (!self)
        {
            mCancelRequested = true;
        }
        return;
    }

    if (error == 0)
    {
        int connectionCount = GetConnectionCount();
        tDebugPrintManager::Print(DC_NETWORK,
            "Friends Matching Connected to anybody Number %d I am %d!\n",
            connectionCount, DWC_GetMyAID());

        bool acceptConnection = false;
        if (!mHostingFriendMatch)
        {
            acceptConnection = true;
        }
        else if (g_pFriendManager->mOwnStatus.mStatus
            == EFriendStatus_HostInvitingPlayer)
        {
            if (index == g_pFriendManager->mFriendStatusIndex)
            {
                acceptConnection = true;
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Friends Matching FriendHostIsInviting %d does not match index %d\n",
                    g_pFriendManager->mFriendStatusIndex, index);
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Friends Matching OwnFriendStatus %d not host inviting player\n",
                g_pFriendManager->mOwnStatus.mStatus);
        }

        if (acceptConnection)
        {
            int previousMachineCount = mMachineCount;
            while (mMachineCount < connectionCount)
            {
                mPlayers[mMachineCount].mUnidentified18[0] = mMachineCount;
                mPlayers[mMachineCount].mConnection = 0;
                mPlayers[mMachineCount].mName[0] = 0;
                mPlayers[mMachineCount].mUnidentified0B = 0;
                mPlayers[mMachineCount].mConnectionState = 0;
                if (mMachineCount != DWC_GetMyAID())
                {
                    DWC_SetRecvBuffer(mMachineCount,
                        mReceiveBuffers[mMachineCount], 0x4000);
                }
                ++mMachineCount;
            }

            if (previousMachineCount != mMachineCount)
            {
                g_pFriendManager->SetOwnStatusInitial(false);
            }
            mState = 2;
            mConnectionDeadline = mElapsedTime + 5.0f;
            tDebugPrintManager::Print(DC_NETWORK,
                "Friend Matchmaking Success Peers changed from %d to %d!\n",
                previousMachineCount, mMachineCount);
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Friend Matchmaking cancelling after the fact - after we got a connection\n");
            mUnidentified052 = true;
        }
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK, "Matching Error\n");
        g_pNetworkSession->ReadAndClearDWCError();
        g_pNetworkSession->GetDirectSocket()->SocketVirtual10(false);
        mState = 0;
        mMatchFailed = true;
    }
}

void NetworkLobby::UpdatePeerConnectionState(int aid)
{
    if (aid == DWC_GetMyAID())
    {
        mPlayers[aid].mConnectionState = 1;
    }
    else if (DWC_GetMyAID() < aid)
    {
        mPlayers[aid].mConnectionState = 2;

        u8 address[4];
        address[0] = 0;
        address[1] = 0;
        address[2] = 0;
        address[3] = aid;
        NetworkSocket* socket =
            g_pNetworkSessionBase->GetDirectSocket();
        if (socket->Connect(&mPlayers[aid].mConnection, address, 0))
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Attempting Reliable connection to AID %d...\n", aid);
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Initial Failure to make reliable connection with AID %d\n",
                aid);
            mPlayers[aid].mConnectionState = 4;
        }
    }
    else
    {
        mPlayers[aid].mConnectionState = 3;
    }
}

inline void NetworkLobby::UpdatePeerConnectionStates()
{
    for (int aid = 0; aid < GetConnectionCount(); ++aid)
    {
        UpdatePeerConnectionState(aid);
    }
}

void NetworkLobby::BuildLocalMachineInfo(
    NetworkDraftMachineInfo* info)
{
    NetworkRankingMeta* record = 0;
    if (IsOnlineRankedMatch())
    {
        record = NetworkStatsManager::Instance()->GetLocalStats(0);
    }
    else if (NetworkStatsManager::Instance()->UsesEuropeanRankings())
    {
        record = NetworkStatsManager::Instance()->GetLocalStats(2);
    }
    else
    {
        NetworkLeaderboardCategory* leaderboard =
            NetworkStatsManager::Instance()->GetCategory(4);
        if (leaderboard != 0)
        {
            int profileId = GameInfoManager::Instance()
                                ->GetSaveSlot(gNetworkSaveSlotIndex)
                                ->unknown_0x01C;
            int index = leaderboard->FindPlayer(profileId);
            if (index != -1)
            {
                record = &leaderboard->mMetadata[index];
            }
        }
    }

    if (record != 0)
    {
        info->mStats = *record;
    }
    else
    {
        info->mStats.Reset();
    }

    info->mProfileId = GameInfoManager::Instance()
                           ->GetSaveSlot(gNetworkSaveSlotIndex)
                           ->unknown_0x01C;

    nlStrNCpy(info->mName, gNetworkMiiNameWide, 11);
    memcpy(info->mMiiData, &gNetworkMiiData, sizeof(info->mMiiData));

    info->mMachineIndex = GetLocalMachineIndex();
    info->mGuestEnabled = HasOnlineTwoLocalPlayers();
}

inline void NetworkLobby::SendLocalMachineInfo(int aid)
{
    unsigned int connection = mPlayers[aid].mConnection;
    if (connection == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "DWCLobby error no connection to peer %d\n", aid);
    }
    else
    {
        NetMessageDraftMachineInfo message;
        u8 buffer[0xFF];
        BuildLocalMachineInfo(&message.mEntry);
        NetworkSocket* socket =
            g_pNetworkSessionBase->GetDirectSocket();
        int size = gNetworkMessageRegistry->Serialize(
            &message, buffer, sizeof(buffer));
        socket->Send(connection, buffer, size, true);
    }
}

void NetworkLobby::Update(float dt)
{
    mElapsedTime += dt;
    if (mConnectionDeadline > 0.0f && mElapsedTime >= mConnectionDeadline
        && !mMatchFailed)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Error Matchmaking - Lobby Start game timed out!");
        mMatchFailed = true;
    }

    switch (mState)
    {
    case 3:
        mFriendProfileId =
            DWC_GetMOMinCompState((u64*)&mUnidentified030);
        break;
    case 2:
        for (int i = 0; i < mMachineCount; ++i)
        {
            switch (mPlayers[i].mConnectionState)
            {
            case 0:
                UpdatePeerConnectionState(i);
                break;
            case 1:
                BuildLocalMachineInfo(&mMachineInfo[i]);
                mMachineInfoReceived[i] = true;
                mPlayers[i].mConnectionState = 6;
                break;
            case 5:
            {
                SendLocalMachineInfo(i);
                mPlayers[i].mConnectionState = 7;
                break;
            }
            case 4:
                mMatchFailed = true;
                break;
            }
        }
        break;
    case 4:
        UpdatePeerConnectionStates();
        mState = 5;
        break;
    case 5:
    {
        bool connectionsReady = true;
        for (int i = 0; i < mMachineCount; ++i)
        {
            if (mPlayers[i].mConnectionState != 1
                && mPlayers[i].mConnectionState != 5)
            {
                connectionsReady = false;
            }
        }

        if (connectionsReady)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "DWCLobby finished establishing Rel Connections\n");
            if (GetLocalMachineIndex() == 0)
            {
                mState = 6;
                BuildLocalMachineInfo(&mMachineInfo[0]);
                mMachineInfoReceived[0] = true;
            }
            else
            {
                SendLocalMachineInfo(0);
                mState = 7;
            }
        }
        break;
    }
    case 6:
    {
        bool allMachineInfoReceived = true;
        for (int i = 0; i < mMachineCount; ++i)
        {
            if (!mMachineInfoReceived[i])
            {
                allMachineInfoReceived = false;
            }
        }
        if (allMachineInfoReceived)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "DWCLobby host finished waiting for all machine info\n");
            mState = 7;
        }
        break;
    }
    case 0:
    case 1:
    case 7:
    default:
        break;
    }
}

void NetworkLobby::MarkGameStarted()
{
    mGameStarted = 1;
}

NetworkDraftMachineInfo* NetworkLobby::GetLocalMachineInfo()
{
    return mMachineInfo;
}

NetworkDraftMachineInfo* NetworkLobby::GetMachineInfo(int index)
{
    if (!mMachineInfoReceived[index])
    {
        return 0;
    }
    return &mMachineInfo[index];
}

int NetworkLobby::ProcessMessage(
    NetworkMessage* message)
{
    s8 machine = MachineIdxFromConnection(message->mSource);
    if (machine < 0 || machine >= GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Discarded message type %d because from unknown connection %x\n",
            (u8)message->GetType(),
            message->mSource);
        return 1;
    }

    switch ((u8)message->GetType())
    {
    case 0x16:
    {
        NetMessageDraftMachineInfo* machineInfo = (NetMessageDraftMachineInfo*)message;
        int index = machineInfo->mEntry.mMachineIndex;
        if (index >= 0 && index < GetMachineCount())
        {
            mMachineInfo[index] = machineInfo->mEntry;
            mMachineInfoReceived[index] = true;
            tDebugPrintManager::Print(DC_NETWORK,
                "ReceivedDraftMachineInfo from index %d\n",
                index);
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignored ReceivedDraftMachineInfo because from index %d\n",
                index);
        }
        break;
    }
    }
    return 1;
}

static TweakIntBinding sTimeoutFindingMaxPlayersAcceptMinTweak(
    "s_nTimeoutFindingMaxPlayersAcceptMin", "Network/DWCLobby",
    &s_nTimeoutFindingMaxPlayersAcceptMin, true);
