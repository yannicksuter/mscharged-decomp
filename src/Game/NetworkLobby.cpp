#include "Game/NetworkSession.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"

#include <string.h>

typedef void (*DWCMatchCallback)(int error, bool cancelled, void* param);
typedef void (*DWCFriendMatchCallback)(int error, bool cancelled, bool self,
    bool isServer, int index, void* param);
typedef void (*DWCServerBrowserCallback)(int result);
typedef int (*DWCMatchEvaluationCallback)(void* server);

struct DWCMatchOptMinComplete
{
    u8 valid;
    u8 minEntry;
    u8 padding[2];
    u32 timeout;
};

extern "C"
{
    int fn_8004F594(int channel, const char* format, ...);
    u32 fn_80124238();
    u32 fn_8011C1D0();
    int fn_8025BDA0();
    int fn_8032CA1C(
        void* registry, int type, UnidentifiedNetworkMessageReceiver* receiver);
    void fn_8032CA2C(void* registry, int type);
    int fn_8048FACC(void* server, const char* key, int defaultValue);
    void fn_8048F648(int key, int value);

    int OSCreateThread(void* thread, void* (*func)(void*), void* param,
        void* stack, u32 stackSize, int priority, u16 attributes);
    s32 OSResumeThread(void* thread);

    u8 DWC_GetMyAID();
    int DWC_GetNumConnectionHost();
    int DWC_SetConnectionClosedCallback(
        void (*callback)(int, int, int, u8, int, void*), void* param);
    int DWC_SetUserRecvCallback(void (*callback)(u8, u8*, int));
    int DWC_SetRecvBuffer(u8 aid, void* buffer, int size);
    int DWC_IsValidMatchCancel();
    int DWC_CancelMatch();
    int DWC_SetMatchingOption(int option, const void* value);
    u8 DWC_AddMatchKeyInt(u8 key, const char* name, const int* value);
    int DWC_CloseAllConnectionsHard();
    int DWC_ConnectToAnybodyAsync(u8 maxEntry, const char* filter,
        DWCMatchCallback callback, void* callbackParam,
        DWCMatchEvaluationCallback evaluationCallback, void* evaluationParam);
    int DWC_SetupGameServer(int maxEntry, DWCFriendMatchCallback callback,
        void* callbackParam, DWCServerBrowserCallback browserCallback,
        void* browserParam);
    int DWC_ConnectToGameServerAsync(int profileId,
        DWCFriendMatchCallback callback, void* callbackParam,
        DWCServerBrowserCallback browserCallback, void* browserParam);
    void DWC_ShutdownFriendsMatch();
}

extern void* lbl_806E2100;
extern u8 lbl_806E18D4;
extern u8 lbl_806E1009;

static int s_nTimeoutFindingMaxPlayersAcceptMin = 20;

static void MatchmakingCallback_801345D4(
    int error, bool cancelled, void* param);
static int CalculateMatchmakingPoints_8013462C(void* server);
static void ConnectionClosedCallback_801346D8(
    int error, int isLocal, int isServer, u8 aid, int index, void* param);
static void UserReceiveCallback_801347C0(u8 aid, u8* buffer, int size);
static void* MatchmakingThread_80134CBC(void* param);
static void ServerBrowserCallback_80134F20(int result);
static void FriendMatchCallback_80134F68(int error, bool cancelled, bool self,
    bool isServer, int index, void* param);

NetworkLobby_80133634::NetworkLobby_80133634()
{
    for (int i = 0; i < 4; ++i)
    {
        mReceiveBuffers[i] = nlMalloc(0x4000, 8, false);
    }

    mReceiverRegistered = false;
    Reset();
}

void NetworkLobby_80133634::RegisterMessageReceiver()
{
    fn_8032CA1C(lbl_806E2100, 0x16, static_cast<UnidentifiedNetworkMessageReceiver*>(this));
    mReceiverRegistered = true;
    Reset();
}

void NetworkLobby_80133634::Reset()
{
    mUnidentified009 = false;
    mFriendMatch = false;
    mHostingFriendMatch = false;
    mFriendHostInviting = false;
    mMatchFlags = 0;
    if (lbl_806E18D4)
    {
        mMatchFlags |= 2;
    }

    mUnidentified014 = fn_80124238();
    mMatchmakingPoints = 0;
    mProfileId = fn_8011C1D0();
    mUnidentified020 = 0;
    mGameType = 0;
    mAnyPlayerMatch = 0;
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

void NetworkLobby_80133634::UnregisterMessageReceiver()
{
    fn_8032CA2C(lbl_806E2100, 0x16);
    Reset();
    mReceiverRegistered = false;
}

u32 NetworkLobby_80133634::GetMachineAid(int index)
{
    if (mMachineCount == 0 || index < 0 || index >= mMachineCount)
    {
        return 0;
    }
    if (index == DWC_GetMyAID())
    {
        return (u32)-1;
    }
    return mPlayers[index].mConnection;
}

int NetworkLobby_80133634::MachineIdxFromConnection(u32 connection)
{
    if (mMachineCount == 0 || connection == 0 || connection == (u32)-2)
    {
        return -1;
    }
    if (connection == (u32)-1)
    {
        return DWC_GetMyAID();
    }
    return *((u8*)connection + 0x27);
}

int NetworkLobby_80133634::RosterVirtual08()
{
    return 0;
}

void NetworkLobby_80133634::RosterVirtual0C()
{
}

int NetworkLobby_80133634::GetMaxMachineCount()
{
    return 4;
}

void NetworkLobby_80133634::RosterVirtual14()
{
}

int NetworkLobby_80133634::GetMachineCount()
{
    return mMachineCount;
}

UnidentifiedTransportPlayer* NetworkLobby_80133634::GetPlayerInfo(int index)
{
    if (index < 0 || index >= mMachineCount)
    {
        return 0;
    }
    return &mPlayers[index];
}

int NetworkLobby_80133634::GetLocalMachineIndex()
{
    if (mMachineCount <= 0)
    {
        return -1;
    }
    return DWC_GetMyAID();
}

UnidentifiedTransportPlayer* NetworkLobby_80133634::GetLocalPlayerInfo()
{
    int index = GetLocalMachineIndex();
    if (index == -1)
    {
        return 0;
    }
    return &mPlayers[index];
}

void NetworkLobby_80133634::SetUserMatchData(u8 size, const void* data)
{
    mUserMatchDataSize = size;
    memcpy(mUserMatchData, data, size);
}

void* NetworkLobby_80133634::GetUserMatchData(u8* size)
{
    *size = mUserMatchDataSize;
    return mUserMatchData;
}

int NetworkLobby_80133634::GetPlayerCount()
{
    if (DWC_GetMyAID() == 0)
    {
        if (mFriendMatch)
        {
            if (mState == 2 && AllMachineInfoReceived())
            {
                if (!lbl_806E1009 || mFriendHostInviting)
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

bool NetworkLobby_80133634::AreAllConnectionsReady()
{
    if (mState != 2 || mMachineCount <= 0)
    {
        return false;
    }

    bool ready = true;
    for (int i = 0; i < mMachineCount; ++i)
    {
        if (!mMachineInfoReceived[i])
        {
            ready = false;
        }
    }
    return ready;
}

void NetworkLobby_80133634::DebugDraw(int column, int* row)
{
    (void)column;
    (void)row;
}

void NetworkLobby_80133634::OnConnected(u32 connection, int result)
{
    if (result != 0)
    {
        fn_8004F594(
            0x10, "ConnectionEstablished failed returned error %d\n", result);
        return;
    }

    for (int i = 0; i < mMachineCount; ++i)
    {
        if (mPlayers[i].mConnection == connection)
        {
            mPlayers[i].mConnectionState = 5;
            fn_8004F594(0x10, "Connection %d established I am %d\n", i, DWC_GetMyAID());
            return;
        }
    }

    fn_8004F594(0x10, "Unknown connection established from %d I am %d\n", *((u8*)connection + 0x27), DWC_GetMyAID());
}

int NetworkLobby_80133634::ShouldAcceptConnection(
    u32 connection, u8* address)
{
    u8 aid = address[3];
    u8 myAid = DWC_GetMyAID();
    if (aid < myAid)
    {
        mPlayers[aid].mConnection = connection;
        mPlayers[aid].mConnectionState = 5;
        fn_8004F594(
            0x10, "Accepting connection from %d to me %d\n", aid, myAid);
        return 1;
    }

    fn_8004F594(
        0x10, "Refusing attempt accept from %d I am %d\n", aid, myAid);
    return 0;
}

void NetworkLobby_80133634::OnConnectionClosed(
    u32 connection, int reason)
{
    u8 aid = *((u8*)connection + 0x27);
    fn_8004F594(0x10, "Connection Lost %d reason %d\n", aid, reason);
    if (aid >= mMachineCount)
    {
        fn_8004F594(0x10,
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
        fn_8004F594(0x10,
            "PeerInfoList[%d] connection %x does not match lost connection %x\n",
            aid,
            mPlayers[aid].mConnection,
            connection);
    }
}

void NetworkLobby_80133634::OnGameStarted()
{
    fn_8004F594(0x10, "DWCLobby GameStarted\n");
    mConnectionDeadline = 0.0f;
    lbl_806E10EC->GetDirectSocket()->SocketVirtual10(false);
}

void NetworkLobby_80133634::CloseConnections()
{
    NetworkSocket_801246E4* socket = lbl_806E10EC->GetDirectSocket();
    for (int i = 0; i < mMachineCount; ++i)
    {
        u32 connection = mPlayers[i].mConnection;
        if (connection != (u32)-1 && connection != 0)
        {
            socket->SocketVirtual24((void*)connection, (void*)1);
        }
    }

    int result = DWC_CloseAllConnectionsHard();
    fn_8004F594(
        0x10, "DWC_CloseAllConnectionsHard returned %d\n", result);
    if (mMatchmakingThreadRunning || mState != 0)
    {
        mMatchFailed = true;
    }
}

void NetworkLobby_80133634::CloseConnectionsAndReset()
{
    lbl_806E10EC->GetDirectSocket()->SocketVirtual10(false);
    CloseConnections();
    Reset();
}

void NetworkLobby_80133634::Shutdown(bool reset)
{
    (void)reset;
    lbl_806E10EC->GetDirectSocket()->SocketVirtual10(false);
    CloseConnections();
    Reset();
}

static void MatchmakingCallback_801345D4(
    int error, bool cancelled, void* param)
{
    lbl_806E10EC->fn_801216F0()->OnMatchmakingResult_80134DBC(
        error, cancelled, param);
}

static int CalculateMatchmakingPoints_8013462C(void* server)
{
    NetworkLobby_80133634* lobby = lbl_806E10EC->fn_801216F0();
    int profileId = fn_8048FACC(server, "PI", 0);
    int points = fn_8048FACC(server, "PT", 0);
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
    fn_8004F594(
        0x10, "Matchmaking profile %d evaluation %d\n", profileId, score);
    return score;
}

static void ConnectionClosedCallback_801346D8(
    int error, int isLocal, int isServer, u8 aid, int index, void* param)
{
    (void)error;
    (void)isLocal;
    (void)isServer;
    (void)index;
    (void)param;

    u8 address[4] = { 0, 0, 0, aid };
    NetworkSocket_801246E4* socket = lbl_806E10EC->GetDirectSocket();
    void* connection = socket->FindConnection(address);
    if (connection != 0 && connection != (void*)-1)
    {
        socket->SocketVirtual24(connection, (void*)1);
    }
}

static void UserReceiveCallback_801347C0(u8 aid, u8* buffer, int size)
{
    lbl_806E10EC->fn_801216F0();
    lbl_806E10EC->GetDirectSocket()->ReceiveUnreliable(aid, buffer, size);
}

bool NetworkLobby_80133634::CanCancelMatchmaking()
{
    if (mMatchmakingThreadRunning)
    {
        return false;
    }
    if ((mState == 3 || mState == 1) && DWC_IsValidMatchCancel())
    {
        return true;
    }
    return false;
}

void NetworkLobby_80133634::CancelMatchmaking()
{
    fn_8004F594(0x10, "Cancelling DWC matchmaking\n");
    DWC_CancelMatch();
    mCancelRequested = false;
    lbl_806E10EC->GetDirectSocket()->SocketVirtual10(false);
    mState = 0;
}

bool NetworkLobby_80133634::StartMatchmaking()
{
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback_801346D8, 0);
    DWC_SetUserRecvCallback(UserReceiveCallback_801347C0);

    mFriendMatch = false;
    mGameType = mUnidentified014;
    mAnyPlayerMatch = mProfileId;
    mUnidentified030 = 0;
    mUnidentified034 = 0;
    mFriendProfileId = -1;

    DWCMatchOptMinComplete option = {
        1, 2, { 0, 0 }, s_nTimeoutFindingMaxPlayersAcceptMin
    };
    if (DWC_SetMatchingOption(0, &option) != 0)
    {
        mMatchFailed = true;
        return false;
    }

    DWC_AddMatchKeyInt(1, "GT", (int*)&mMatchFlags);
    DWC_AddMatchKeyInt(2, "PI", (int*)&mUnidentified020);
    DWC_AddMatchKeyInt(0, "AP", (int*)&mUnidentified014);
    DWC_AddMatchKeyInt(3, "PT", &mMatchmakingPoints);
    DWC_AddMatchKeyInt(4, "CO", (int*)&mProfileId);

    if (!DWC_ConnectToAnybodyAsync(4, 0, MatchmakingCallback_801345D4, 0, CalculateMatchmakingPoints_8013462C, 0))
    {
        mState = 0;
        mMatchFailed = true;
        return false;
    }

    mState = 1;
    lbl_806E10EC->GetDirectSocket()->SocketVirtual10(true);
    return true;
}

static void* MatchmakingThread_80134CBC(void* param)
{
    (void)param;
    NetworkLobby_80133634* lobby = lbl_806E10EC->fn_801216F0();
    fn_8004F594(0x10, "Matchmaking thread begin\n");
    lobby->StartMatchmaking();
    fn_8004F594(0x10, "Matchmaking thread end\n");
    lobby->mMatchmakingThreadRunning = false;
    return 0;
}

void NetworkLobby_80133634::StartMatchmakingThread()
{
    mMatchmakingThreadRunning = true;
    OSCreateThread(mMatchmakingThread, MatchmakingThread_80134CBC, 0, mMatchmakingThreadStack + sizeof(mMatchmakingThreadStack), sizeof(mMatchmakingThreadStack), 14, 1);
    OSResumeThread(mMatchmakingThread);
}

void NetworkLobby_80133634::OnMatchmakingResult_80134DBC(
    int error, bool cancelled, void* param)
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

static void ServerBrowserCallback_80134F20(int result)
{
    lbl_806E10EC->fn_801216F0();
    fn_8004F594(0x10, "Friend server browser result %d\n", result);
}

static void FriendMatchCallback_80134F68(int error, bool cancelled, bool self,
    bool isServer, int index, void* param)
{
    lbl_806E10EC->fn_801216F0()->OnFriendMatchmakingResult_8013535C(
        error, cancelled, self, isServer, index, param);
}

bool NetworkLobby_80133634::StartFriendServer()
{
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback_801346D8, 0);
    DWC_SetUserRecvCallback(UserReceiveCallback_801347C0);
    mFriendMatch = true;
    mHostingFriendMatch = true;
    mFriendHostInviting = false;
    mMatchFailed = false;

    int maxPlayers = lbl_806E1009 ? 4 : 2;
    if (!DWC_SetupGameServer(maxPlayers, FriendMatchCallback_80134F68, 0, ServerBrowserCallback_80134F20, 0))
    {
        mState = 0;
        mMatchFailed = true;
        return false;
    }

    mState = 1;
    lbl_806E10EC->GetDirectSocket()->SocketVirtual10(true);
    return true;
}

void NetworkLobby_80133634::StopFriendServer_801350E0()
{
    lbl_806E10EC->GetDirectSocket()->SocketVirtual10(false);
    DWC_ShutdownFriendsMatch();
    mState = 0;
    mFriendHostInviting = false;
}

void NetworkLobby_80133634::SetFriendHostInviting_80135208()
{
    fn_8048F648(0, 0);
    mFriendHostInviting = true;
}

bool NetworkLobby_80133634::ConnectToFriendServer(int profileId)
{
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback_801346D8, 0);
    DWC_SetUserRecvCallback(UserReceiveCallback_801347C0);
    mFriendMatch = true;
    mHostingFriendMatch = false;
    mFriendHostInviting = false;
    mMatchFailed = false;

    if (!DWC_ConnectToGameServerAsync(profileId,
            FriendMatchCallback_80134F68,
            0,
            ServerBrowserCallback_80134F20,
            0))
    {
        mState = 0;
        mMatchFailed = true;
        return false;
    }

    mState = 1;
    lbl_806E10EC->GetDirectSocket()->SocketVirtual10(true);
    return true;
}

void NetworkLobby_80133634::OnFriendMatchmakingResult_8013535C(int error,
    bool cancelled, bool self, bool isServer, int index, void* param)
{
    (void)self;
    (void)isServer;
    (void)param;
    if (error == 0 && !cancelled)
    {
        if (index >= mMachineCount)
        {
            mMachineCount = index + 1;
        }
        mState = 2;
        mConnectionDeadline = mElapsedTime + 5.0f;
    }
    else
    {
        mState = 0;
        mMatchFailed = true;
    }
}

void NetworkLobby_80133634::UpdatePeerConnectionState_801355EC(int aid)
{
    if (aid == DWC_GetMyAID())
    {
        mPlayers[aid].mConnectionState = 1;
    }
    else if (DWC_GetMyAID() < aid)
    {
        mPlayers[aid].mConnectionState = 2;
    }
    else
    {
        mPlayers[aid].mConnectionState = 3;
    }
}

void NetworkLobby_80133634::BuildLocalMachineInfo(
    UnidentifiedDraftEntry* info)
{
    info->mIndex = GetLocalMachineIndex();
    info->mUnidentified7F = fn_8025BDA0();
}

void NetworkLobby_80133634::Update(float dt)
{
    mElapsedTime += dt;
    if (mConnectionDeadline > 0.0f && mElapsedTime >= mConnectionDeadline
        && !mMatchFailed)
    {
        mMatchFailed = true;
    }

    if (mCancelRequested && CanCancelMatchmaking())
    {
        CancelMatchmaking();
    }
}

void NetworkLobby_80133634::MarkGameStarted()
{
    mGameStarted = 1;
}

bool NetworkLobby_80133634::AllMachineInfoReceived()
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

UnidentifiedDraftEntry* NetworkLobby_80133634::GetLocalMachineInfo()
{
    return mMachineInfo;
}

UnidentifiedDraftEntry* NetworkLobby_80133634::GetMachineInfo(int index)
{
    if (!mMachineInfoReceived[index])
    {
        return 0;
    }
    return &mMachineInfo[index];
}

int NetworkLobby_80133634::ReceiverVirtual00(
    UnidentifiedNetworkMessage* message)
{
    int machine = MachineIdxFromConnection(message->mUnidentified04);
    if (machine < 0 || machine >= GetMachineCount())
    {
        fn_8004F594(0x10,
            "Discarded message type %d because from unknown connection %x\n",
            message->GetType(),
            message->mUnidentified04);
        return 1;
    }

    if (message->GetType() == 0x16)
    {
        NetworkMessageType22_8050B7B4* machineInfo = (NetworkMessageType22_8050B7B4*)message;
        int index = (s8)machineInfo->mEntry.mIndex;
        if (index >= 0 && index < GetMachineCount())
        {
            mMachineInfo[index] = machineInfo->mEntry;
            mMachineInfoReceived[index] = true;
            fn_8004F594(0x10,
                "ReceivedDraftMachineInfo from index %d\n",
                index);
        }
        else
        {
            fn_8004F594(0x10,
                "Ignored ReceivedDraftMachineInfo because from index %d\n",
                index);
        }
    }
    return 1;
}

static TweakValueIntImpl_804FD898 sTimeoutFindingMaxPlayersAcceptMinTweak(
    "s_nTimeoutFindingMaxPlayersAcceptMin", "Network/DWCLobby",
    &s_nTimeoutFindingMaxPlayersAcceptMin);

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
