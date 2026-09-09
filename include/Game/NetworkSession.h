#include "Game/OnlinePlayer.h"
#ifndef GAME_NETWORK_SESSION_H
#define GAME_NETWORK_SESSION_H

#include "Game/LANLobbyListener.h"
#include "Game/main.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkSessionData.h"
#include "Game/NetworkGameConfig.h"
#include "Game/NetworkStats.h"
#include "NL/nlMemory.h"
#include "NL/plat/TransportSocket.h"
#include "NL/plat/ReliableSocket.h"
#include "types.h"
#include "Game/DetInput.h"

#include <dwc/dwc_main_fwd.h>

class NetMessageDraft;
struct NetworkDraftMachineInfo;
class NetworkSocket;
class LANLobby;
class NetworkLobby;
class NetMessageGameStart;

extern int g_BuildNumber;
extern int gNetworkBuildNumberOverride;

class NetworkConnectionListener;
class NetworkPeer;

class NetworkSessionControl
{
public:
    virtual void InitializeLAN() = 0;
    virtual void InitializeOnline() = 0;
    virtual void Shutdown() = 0;
    virtual int GetSessionMode() = 0;
    virtual int GetSessionState() = 0;
    virtual void SetSessionState(int) = 0;
};

class NetworkConnectionListener
{
public:
    virtual void ListenerVirtual00(void* buffer, int size) = 0;
    virtual void OnMessageReceived(int source, void* buffer, int size, bool reliable) = 0;
    virtual void OnConnectionRequest(u32 connection, u8* address) = 0;
    virtual void OnConnected(u32 connection, int result) = 0;
    virtual void OnConnectionClosed(u32 connection, int reason) = 0;
    virtual void ListenerVirtual14() = 0;
    virtual void ListenerVirtual18() = 0;
};

// Public operations supplied by the session's direct/broadcast socket.
class NetworkSocketInterface
{
public:
    virtual void Initialize(
        void* info, NetworkConnectionListener* listener) = 0;
    virtual void Shutdown() = 0;
    virtual void SetBroadcastEnabled(bool enabled) = 0;
    virtual void SendBroadcast(void* buffer, int size) = 0;
    virtual void SocketVirtual10(bool enabled) = 0;
    virtual bool SocketVirtual14() = 0;
    virtual bool Connect(void* connection, const u8* address, u16 port) = 0;
    virtual void AcceptConnection(u32 connection) = 0;
    virtual void RejectConnection(u32 connection) = 0;
    virtual void Disconnect(
        TransportConnection* connection, bool immediate) = 0;
    virtual void* FindConnection(const u8* address) = 0;
    virtual void Send(int aid, void* buffer, int size, bool reliable) = 0;
    virtual void Receive(void* buffer, int size) = 0;
    virtual void SocketVirtual34(u8 aid, void* buffer, int size) = 0;
    virtual void Update(float dt) = 0;
    virtual void DebugDraw(int a, int* b, bool c) = 0;
    virtual void SocketVirtual48() = 0;
    virtual u8* GetLocalAddress() = 0;
    virtual u16 GetLocalPort() = 0;
};

class NetworkLoginListener
{
public:
    virtual void OnLoginResult(int result) = 0;
    virtual void OnStatsResult(bool success) = 0;
};

struct TransportPlayerInfo
{
    TransportPlayerInfo()
    {
        mUnidentified0B = 0;
    }

    /* 0x00 */ char mName[11];
    /* 0x0B */ u8 mUnidentified0B;
    /* 0x0C */ u32 mUnidentified0C;
    /* 0x10 */ u16 mUnidentified10;
    /* 0x12 */ u16 mUnidentified12;
}; // size: 0x14


class NetworkMachineRoster
{
public:
    virtual unsigned int GetMachineAid(int index);
    virtual int MachineIdxFromConnection(unsigned int connection);
    virtual int GetTopology();
    virtual void SetTopology(int);
    virtual int GetMaxMachineCount();
    virtual void SetMaxMachineCount(int);
    virtual int GetMachineCount();
    virtual TransportPlayerInfo* GetPlayerInfo(int index);
    virtual int GetLocalMachineIndex();
    virtual TransportPlayerInfo* GetLocalPlayerInfo();
    virtual void SetUserMatchData(u8 size, const void* data);
    virtual void* GetUserMatchData(u8* size);
    virtual void Update(float dt);
    virtual int GetPlayerCount();
    virtual void DebugDraw(int column, int* row);
    virtual void OnConnected(unsigned int connection, int result);
    virtual int ShouldAcceptConnection(unsigned int connection, u8* address);
    virtual void OnConnectionClosed(unsigned int connection, int reason);
    virtual void OnGameStarted();
    virtual void Shutdown(bool reset);
};

// NL-side session base: the non-overridden virtual at +0x50 retains its
// implementation at 0x80323B2C inside the NL networking translation unit.
class NetworkSessionBase : public NetworkSessionData,
                                       public NetworkSessionControl
{
public:
    NetworkSessionBase() { }
    ~NetworkSessionBase();

    virtual void Initialize(bool);
    virtual void Update();
    virtual NetworkSocket* GetDirectSocket();
    virtual NetworkMachineRoster* GetMachineRoster();
    virtual LANLobby* GetTransport();
    virtual void BaseVirtual3C(NetworkGameStartInfo* info);
    virtual void BaseVirtual40();
    virtual void BaseVirtual44(NetMessageGameStart* message);
    virtual void BaseVirtual48(int reason);
    virtual int Send(s8 player, void* buffer, int size, bool reliable);
    virtual void DebugDraw();
};


// Reliable UDP direct/broadcast socket layer owned by the session.
class NetworkSocket : public NetworkSocketInterface,
                              public ReliableSocketCallback
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkSocket();

    virtual void Initialize(
        void* info, NetworkConnectionListener* listener);
    virtual void Shutdown();
    virtual void SetBroadcastEnabled(bool enabled);
    virtual void SendBroadcast(void* buffer, int size);
    virtual void SocketVirtual10(bool enabled);
    virtual bool SocketVirtual14();
    virtual bool Connect(void* connection, const u8* address, u16 port);
    virtual void AcceptConnection(u32 connection);
    virtual void RejectConnection(u32 connection);
    virtual void Disconnect(
        TransportConnection* connection, bool immediate);
    virtual void* FindConnection(const u8* address);
    virtual void Send(int aid, void* buffer, int size, bool reliable);
    virtual void Receive(void* buffer, int size);
    virtual void SocketVirtual34(u8 aid, void* buffer, int size);
    virtual void Update(float dt);
    virtual void DebugDraw(int a, int* b, bool c);
    virtual void SocketVirtual48();
    virtual u8* GetLocalAddress();
    virtual u16 GetLocalPort();

    virtual void OnConnectionAttempted(u32 connection, int result);
    virtual void OnConnectionClosed(u32 connection, int reason);
    virtual void ReliableCallbackVirtual08();
    virtual void OnMessageReceived(
        u32 connection, void* buffer, int size, bool reliable);
    virtual void ReliableCallbackVirtual10(
        u32 connection, void* buffer, int size);
    virtual void OnConnectionRequest(
        u32 connection, u8* address, int a, int b, int c);
    virtual int SendDatagram(
        void* buffer, int size, const u8* address, u16 port);

    void ReceiveUnreliable(u8 aid, void* buffer, int size);

    static NetworkSocket* sInstance;

    /* 0x008 */ bool mInitialized;
    /* 0x009 */ bool mDirectMode;
    /* 0x00A */ u8 mPadding00A[2];
    /* 0x00C */ ReliableSocket mReliableSocket;
    /* 0xA28 */ bool mConnectionEnabled;
    /* 0xA29 */ u8 mPaddingA29[3];
    /* 0xA2C */ u32 mVersionWord;
    /* 0xA30 */ NetworkConnectionListener* mListener;
    /* 0xA34 */ TransportSocket mBroadcastSocket;
    /* 0xA38 */ TransportSocket mDirectSocket;
    /* 0xA3C */ u8 mPacketBuffer[0x5B9];
    /* 0xFF5 */ bool mHasLocalAddress;
    /* 0xFF6 */ u8 mPaddingFF6[2];
    /* 0xFF8 */ u8 mLocalAddress[4];
}; // size: 0xFFC

struct LANGameInfo
{
    LANGameInfo() { }

    /* 0x00 */ char mUnidentified00[12];
    /* 0x0C */ u8 mUnidentified0C[4];
    /* 0x10 */ int mUnidentified10;
    /* 0x14 */ float mUnidentified14;
    /* 0x18 */ u16 mUnidentified18;
}; // size: 0x1C

struct NetworkTransportPeer : public TransportPlayerInfo
{
    /* 0x14 */ u8 mUnidentified14[4];
    /* 0x18 */ int mUnidentified18;
    /* 0x1C */ int mUnidentified1C;
    /* 0x20 */ u16 mUnidentified20;
    /* 0x22 */ u8 mUnidentified22;
}; // size: 0x24

struct NetworkTransportConnectionSlot
{
    TransportConnection* m_Connection;
    int mUnidentified04;
};


class NetMessageFoundGame;
class NetMessageJoinRequest;
class NetMessageJoinResponse;
class NetMessageGamePeerAdded;

class NetworkTransportInterface : public NetworkMachineRoster
{
public:
    virtual int CreateGame(int value) = 0;
    virtual int AbortCreateGame() = 0;
    virtual int JoinGame(LANGameInfo* game, int value) = 0;
    virtual int StartGame() = 0;
    virtual void EnumerateGames() = 0;
    virtual void SetLobbyListener(LANLobbyListener* listener) = 0;
    virtual void SetPlayerListener(LANLobbyPlayerListener* listener) = 0;
};

class LANLobby : public NetworkTransportInterface,
                                  public NetworkMessageReceiver
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    LANLobby();

    void Initialize();
    void Reset(bool initialize);
    void UnregisterMessageReceivers();
    int AbortCreateGame(int result);
    void DumpPeerInfo();
    void SendFoundGame(const void* data);
    void CompleteLaunch();
    void SendReadyToLaunchRequest();
    bool ArePeerConnectionsReady();
    void ProcessFoundGame(NetMessageFoundGame* message);
    void SendGamePeerAdded(int index);
    void SendJoinResponse(TransportConnection* connection, bool accepted);
    void ProcessJoinRequest(int index, NetMessageJoinRequest* message);
    void ProcessJoinResponse(int index, NetMessageJoinResponse* message);
    void ProcessGamePeerAdded(NetMessageGamePeerAdded* message);

    virtual unsigned int GetMachineAid(int index);
    virtual int MachineIdxFromConnection(unsigned int connection);
    virtual int GetTopology();
    virtual void SetTopology(int value);
    virtual int GetMaxMachineCount();
    virtual void SetMaxMachineCount(int value);
    virtual int GetMachineCount();
    virtual TransportPlayerInfo* GetPlayerInfo(int index);
    virtual int GetLocalMachineIndex();
    virtual TransportPlayerInfo* GetLocalPlayerInfo();
    virtual void SetUserMatchData(u8 size, const void* data);
    virtual void* GetUserMatchData(u8* size);
    virtual void Update(float dt);
    virtual int GetPlayerCount();
    virtual void DebugDraw(int column, int* row);
    virtual void OnConnected(unsigned int connection, int result);
    virtual int ShouldAcceptConnection(unsigned int connection, u8* address);
    virtual void OnConnectionClosed(unsigned int connection, int reason);
    virtual void OnGameStarted();
    virtual void Shutdown(bool reset);
    virtual int CreateGame(int value);
    virtual int AbortCreateGame();
    virtual int JoinGame(LANGameInfo* game, int value);
    virtual int StartGame();
    virtual void EnumerateGames();
    virtual void SetLobbyListener(LANLobbyListener* listener);
    virtual void SetPlayerListener(LANLobbyPlayerListener* listener);
    virtual int ProcessMessage(NetworkMessage* message);

private:
    int GetConnectionIndex(TransportConnection* connection)
    {
        if (connection == 0)
            return -1;
        for (int index = 0; index < 8; ++index)
        {
            if (m_ConnectionPool[index].m_Connection == connection)
                return index;
        }
        return -1;
    }

    int GetFreeConnectionIndex()
    {
        for (int index = 0; index < 8; ++index)
        {
            if (m_ConnectionPool[index].mUnidentified04 == 0)
                return index;
        }
        return -1;
    }

public:
    /* 0x008 */ bool mInitialized;
    /* 0x00C */ int mTopology;
    /* 0x010 */ int mMaxMachineCount;
    /* 0x014 */ NetworkTransportConnectionSlot m_ConnectionPool[8];
    /* 0x054 */ LANLobbyListener* mListener;
    /* 0x058 */ LANLobbyPlayerListener* mRosterListener;
    /* 0x05C */ LANGameInfo* mFoundGames;
    /* 0x060 */ int mFoundGameCount;
    /* 0x064 */ char mLocalPlayerName[12];
    /* 0x070 */ int mLocalMachineIndex;
    /* 0x074 */ u8 mUserMatchDataSize;
    /* 0x075 */ u8 mUserMatchData[8];
    /* 0x07D */ bool mIsHost;
    /* 0x080 */ int mHostState;
    /* 0x084 */ int mGameType;
    /* 0x088 */ int mState;
    /* 0x08C */ unsigned int mLaunchRequestTicker;
    /* 0x090 */ int mJoinState;
    /* 0x094 */ bool mLaunchConfirmationPending;
    /* 0x095 */ bool mFindGameEnabled;
    /* 0x098 */ float mFindGameElapsedTime;
    /* 0x09C */ bool mAdvertiseGame;
    /* 0x0A0 */ NetworkTransportPeer mPeerInfoList[8];
    /* 0x1C0 */ int mPeerCount;
    /* 0x1C4 */ u8 mFindGameToken[8];
    /* 0x1CC */ NetworkSocket* mSocket;
}; // size: 0x1D0


class NetworkSession : public NetworkSessionBase,
                                   public NetworkConnectionListener,
                                   public NetworkMessageReceiver
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkSession()
    {
        mUnidentified2464 = 0;
        mUnidentified2468 = 0;
        Initialize(true);
    }

    static void Create();

    void SendTournamentStartToEveryone();
    void SendGameStartToEveryone();
    void SendDraftToEveryone(int count, NetworkDraftMachineInfo* entries, bool, u8);
    void SendDraftToEveryone(NetMessageDraft* message);
    void SendSidesChangedToEveryone(NetworkMessage* message);
    void SendSidesChangedToHost(NetworkMessage* message);
    void SendCheckConnectionToEveryone();
    void SendConnectionDecisionToEveryone(NetworkMessage* message);
    void SendConnectionDecisionToHost(NetworkMessage* message);
    void ShutdownLAN();
    void StartLoginThread();
    bool IsLoginThreadComplete();
    bool RequiresDisconnectAfterError();
    void ReadAndClearDWCError();
    bool StartLogin();
    void DWCLoginCallback(int error, int profileID, void* param);
    bool RequestLoginRankings();
    void UpdateLogin();
    void ShutdownOnline();
    NetworkLobby* GetOnlineLobby();
    NetworkStatsInterface* GetStatsInterface();
    NetworkStatsReporter* GetStatsReporter();
    NetworkRanking* GetRankingReporter();
    void OnGameConnectionLost(u32 connection, int reason);
    void RematchGame();
    void OnPauseGame();
    void OnResumingGame();
    u8 GetPausedMachineMask();
    int IsLiveNetworkGame();
    int PollGameLoaded();
    void NotifyGameLoaded();
    void SetTournamentMode(u8 value);
    bool IsConnectedPeer(u32 connection);
    void PopupNetworkError(int overlay);
    void DisconnectOnlineMatch();


    virtual void Initialize(bool);
    virtual void Update();
    virtual NetworkSocket* GetDirectSocket();
    virtual NetworkMachineRoster* GetMachineRoster();
    virtual LANLobby* GetTransport();
    virtual void BaseVirtual3C(NetworkGameStartInfo* info);
    virtual void BaseVirtual40();
    virtual void BaseVirtual44(NetMessageGameStart* message);
    virtual void BaseVirtual48(int reason);
    virtual int Send(s8 player, void* buffer, int size, bool reliable);

    virtual void InitializeLAN();
    virtual void InitializeOnline();
    virtual void Shutdown();
    virtual int GetSessionMode();
    virtual int GetSessionState();
    virtual void SetSessionState(int);

    virtual void ListenerVirtual00(void* buffer, int size);
    virtual void OnMessageReceived(
        int source, void* buffer, int size, bool reliable);
    virtual void OnConnectionRequest(u32 connection, u8* address);
    virtual void OnConnected(u32 connection, int result);
    virtual void OnConnectionClosed(u32 connection, int reason);
    virtual void ListenerVirtual14();
    virtual void ListenerVirtual18();

    virtual int ProcessMessage(NetworkMessage* message);

    /* 0x2438 */ float mElapsedTime;
    /* 0x243C */ u32 mUpdateCount;
    /* 0x2440 */ u32 mLastTicker;
    /* 0x2444 */ int mSessionMode;
    /* 0x2448 */ int mSessionState;
    /* 0x244C */ int mGameEndReason;
    /* 0x2450 */ NetworkSocket* mDirectSocket;
    /* 0x2454 */ LANLobby* mTransport;
    /* 0x2458 */ NetworkLobby* mLobby;
    /* 0x245C */ NetworkStatsReporter* mStatsReporter;
    /* 0x2460 */ NetworkRanking* mRankingReporter;
    /* 0x2464 */ u32 mUnidentified2464;
    /* 0x2468 */ u32 mUnidentified2468;
    /* 0x246C */ u8 mPauseRequestMachineMask;
    /* 0x246D */ u8 mPausedMachineMask;
    /* 0x246E */ u8 mMachineLoadedGame[4];
    /* 0x2472 */ u8 mUnidentified2472;
    /* 0x2473 */ u8 mCupMode;
    /* 0x2474 */ u32 mUnidentified2474;
    /* 0x2478 */ u32 mUnidentified2478;
    /* 0x247C */ u32 mUnidentified247C;
    /* 0x2480 */ int mOverlayRequest;
    /* 0x2484 */ int mPoppedOverlay;
    /* 0x2488 */ long mDWCErrorCode;
    /* 0x248C */ DWCErrorType mDWCErrorType;
    /* 0x2490 */ int mDWCLastError;
    /* 0x2494 */ u8 mUnidentified2494;
    /* 0x2498 */ int mLoginStage;
    /* 0x249C */ NetworkLoginListener* mLoginListener;
    /* 0x24A0 */ float mLoginStartTime;
    /* 0x24A4 */ u8 mUnidentified24A4;
    /* 0x24A5 */ u8 mUnidentified24A5;
    /* 0x24A8 */ u32 mLoginThread[0x318 / 4];
    /* 0x27C0 */ u8 mLoginThreadStack[0x4000];
}; // size: 0x67C0

extern NetworkSessionBase* g_pNetworkSessionBase;
extern NetworkSession* g_pNetworkSession;


unsigned int GetNetworkVersionWord();

void RestartSinglePlayerGame();
void StartSinglePlayerGame();
void PlaybackRecordedGame();

extern bool gOnlineFourMachineFriendLobby;
extern u8 gOnlineTwoLocalPlayers;
extern int gOnlineLocalControllerIndices[2];

#endif // GAME_NETWORK_SESSION_H
