#ifndef GAME_NETWORK_SESSION_H
#define GAME_NETWORK_SESSION_H

#include "Game/main.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkStats.h"
#include "NL/nlMemory.h"
#include "types.h"

class NetMessageDraft;
struct UnidentifiedDraftEntry;
class NetworkSocket_801246E4;
class NetworkTransport_8032CA4C;
class NetworkLobby_80133634;
class NetMessageGameStart;

struct UnidentifiedGameConfig;

// Per-machine start payload consumed by the session base initializer.
struct UnidentifiedGameStartInfo
{
    /* 0x00 */ u8 mUnidentified00[0x4];
    /* 0x04 */ UnidentifiedGameConfig* mConfig;
    /* 0x08 */ u32 mSeed;
    /* 0x0C */ int mMachineCount;
    /* 0x10 */ int mMyMachineId;
    /* 0x14 */ int mPlayerCounts[4];
};

extern int g_BuildNumber;
extern int lbl_806E10F8;

class UnidentifiedNetworkConnectionListener;

class UnidentifiedNetworkPeerChannel
{
public:
    UnidentifiedNetworkPeerChannel();
    ~UnidentifiedNetworkPeerChannel() { }

    /* 0x000 */ u8 mData[0x240];
};

class UnidentifiedNetworkPeer
{
public:
    UnidentifiedNetworkPeer();

    /* 0x000 */ s8 mMachineId;
    /* 0x004 */ u32 mUnidentified004;
    /* 0x008 */ UnidentifiedNetworkPeerChannel mChannels[4];
}; // size: 0x908

// Data-only base establishing the peer storage that precedes every vtable
// pointer in the retail object.
class UnidentifiedNetworkSessionData
{
public:
    ~UnidentifiedNetworkSessionData();

    /* 0x0000 */ int mUnidentified0000;
    /* 0x0004 */ UnidentifiedNetworkPeer mPeers[4];
    /* 0x2424 */ int mUnidentified2424;
}; // size: 0x2428

class UnidentifiedNetworkOnlineInterface
{
public:
    virtual void OnlineVirtual00() = 0;
    virtual void OnlineVirtual04() = 0;
    virtual void OnlineVirtual08() = 0;
    virtual int OnlineVirtual0C() = 0;
    virtual int OnlineVirtual10() = 0;
    virtual void OnlineVirtual14(int) = 0;
};

class UnidentifiedNetworkConnectionListener
{
public:
    virtual void ListenerVirtual00(void* buffer, int size) = 0;
    virtual void ListenerVirtual04(int source, void* buffer, int size, bool reliable) = 0;
    virtual void ListenerVirtual08(u32 connection, u8* address) = 0;
    virtual void ListenerVirtual0C(u32 connection, int result) = 0;
    virtual void ListenerVirtual10(u32 connection, int reason) = 0;
    virtual void ListenerVirtual14() = 0;
    virtual void ListenerVirtual18() = 0;
};

// Interface presented by the reliable connection manager embedded in the
// direct socket layer.
class UnidentifiedReliableSocketCallback
{
public:
    virtual void OnConnectionAttempted(u32 connection, int result) = 0;
    virtual void OnConnectionClosed(u32 connection, int reason) = 0;
    virtual void ReliableCallbackVirtual08() = 0;
    virtual void ReliableCallbackVirtual0C(
        int source, void* buffer, int size, bool reliable) = 0;
    virtual void ReliableCallbackVirtual10() = 0;
    virtual void ReliableCallbackVirtual14(u32 connection, u8* address) = 0;
    virtual int SendDatagram(
        void* buffer, int size, const u8* address, u16 port) = 0;
};

// Public operations supplied by the session's direct/broadcast socket.
class UnidentifiedNetworkSocketInterface
{
public:
    virtual void Initialize(
        void* info, UnidentifiedNetworkConnectionListener* listener) = 0;
    virtual void Shutdown() = 0;
    virtual void SetBroadcastEnabled(bool enabled) = 0;
    virtual void SendBroadcast(void* buffer, int size) = 0;
    virtual void SocketVirtual10(bool enabled) = 0;
    virtual bool SocketVirtual14() = 0;
    virtual bool Connect(void* connection, const u8* address, u16 port) = 0;
    virtual void AcceptConnection(u32 connection) = 0;
    virtual void RejectConnection(u32 connection) = 0;
    virtual void SocketVirtual24(void* a, void* b) = 0;
    virtual void* FindConnection(const u8* address) = 0;
    virtual void Send(int aid, void* buffer, int size, bool reliable) = 0;
    virtual void Receive(void* buffer, int size) = 0;
    virtual void SocketVirtual34(u8 aid, void* buffer, int size) = 0;
    virtual void Update(float dt) = 0;
    virtual void SocketVirtual44(void* a, void* b, bool c) = 0;
    virtual void SocketVirtual48() = 0;
    virtual u8* GetLocalAddress() = 0;
    virtual u16 GetLocalPort() = 0;
};

class UnidentifiedLoginListener
{
public:
    virtual void OnLoginResult(int result) = 0;
    virtual void OnStatsResult(bool success) = 0;
};

struct UnidentifiedTransportPlayer
{
    UnidentifiedTransportPlayer()
    {
        mName[11] = '\0';
    }

    /* 0x00 */ char mName[12];
    /* 0x0C */ u32 mUnidentified0C;
    /* 0x10 */ u16 mUnidentified10;
    /* 0x12 */ u16 mUnidentified12;
    /* 0x14 */ u32 mConnection;
    /* 0x18 */ int mConnectionState;
    /* 0x1C */ u8 mUnidentified1C[4];
}; // size: 0x20

class UnidentifiedMachineRoster
{
public:
    virtual u32 GetMachineAid(int index);
    virtual int MachineIdxFromConnection(u32 connection);
    virtual int RosterVirtual08();
    virtual void RosterVirtual0C();
    virtual int GetMaxMachineCount();
    virtual void RosterVirtual14();
    virtual int GetMachineCount();
    virtual UnidentifiedTransportPlayer* GetPlayerInfo(int index);
    virtual int GetLocalMachineIndex();
    virtual UnidentifiedTransportPlayer* GetLocalPlayerInfo();
    virtual void SetUserMatchData(u8 size, const void* data);
    virtual void* GetUserMatchData(u8* size);
    virtual void Update(float dt);
    virtual int GetPlayerCount();
    virtual void DebugDraw(int column, int* row);
    virtual void OnConnected(u32 connection, int result);
    virtual int ShouldAcceptConnection(u32 connection, u8* address);
    virtual void OnConnectionClosed(u32 connection, int reason);
    virtual void OnGameStarted();
    virtual void Shutdown(bool reset);
};

// NL-side session base: the non-overridden virtual at +0x50 retains its
// implementation at 0x80323B2C inside the NL networking translation unit.
class UnidentifiedNetworkSessionBase : public UnidentifiedNetworkSessionData,
                                       public UnidentifiedNetworkOnlineInterface
{
public:
    UnidentifiedNetworkSessionBase() { }
    ~UnidentifiedNetworkSessionBase();

    virtual void Initialize(bool);
    virtual void Update();
    virtual NetworkSocket_801246E4* GetDirectSocket();
    virtual UnidentifiedMachineRoster* GetMachineRoster();
    virtual NetworkTransport_8032CA4C* GetTransport();
    virtual void BaseVirtual3C(UnidentifiedGameStartInfo* info);
    virtual void BaseVirtual40();
    virtual void BaseVirtual44(NetMessageGameStart* message);
    virtual void BaseVirtual48(int reason);
    virtual int Send(s8 player, void* buffer, int size, bool reliable);
    virtual void BaseVirtual50();
};

struct UnidentifiedReliableSocketState
{
    u8 mData[0xA1C];
};

struct UnidentifiedDatagramSocket
{
    int mSocket;
};

// Reliable UDP direct/broadcast socket layer owned by the session.
class NetworkSocket_801246E4 : public UnidentifiedNetworkSocketInterface,
                              public UnidentifiedReliableSocketCallback
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkSocket_801246E4();

    virtual void Initialize(
        void* info, UnidentifiedNetworkConnectionListener* listener);
    virtual void Shutdown();
    virtual void SetBroadcastEnabled(bool enabled);
    virtual void SendBroadcast(void* buffer, int size);
    virtual void SocketVirtual10(bool enabled);
    virtual bool SocketVirtual14();
    virtual bool Connect(void* connection, const u8* address, u16 port);
    virtual void AcceptConnection(u32 connection);
    virtual void RejectConnection(u32 connection);
    virtual void SocketVirtual24(void* a, void* b);
    virtual void* FindConnection(const u8* address);
    virtual void Send(int aid, void* buffer, int size, bool reliable);
    virtual void Receive(void* buffer, int size);
    virtual void SocketVirtual34(u8 aid, void* buffer, int size);
    virtual void Update(float dt);
    virtual void SocketVirtual44(void* a, void* b, bool c);
    virtual void SocketVirtual48();
    virtual u8* GetLocalAddress();
    virtual u16 GetLocalPort();

    virtual void OnConnectionAttempted(u32 connection, int result);
    virtual void OnConnectionClosed(u32 connection, int reason);
    virtual void ReliableCallbackVirtual08();
    virtual void ReliableCallbackVirtual0C(
        int source, void* buffer, int size, bool reliable);
    virtual void ReliableCallbackVirtual10();
    virtual void ReliableCallbackVirtual14(u32 connection, u8* address);
    virtual int SendDatagram(
        void* buffer, int size, const u8* address, u16 port);

    void ReceiveUnreliable(u8 aid, void* buffer, int size);

    static NetworkSocket_801246E4* sInstance;

    /* 0x008 */ bool mInitialized;
    /* 0x009 */ bool mDirectMode;
    /* 0x00A */ u8 mPadding00A[2];
    /* 0x00C */ UnidentifiedReliableSocketState mReliableSocket;
    /* 0xA28 */ bool mConnectionEnabled;
    /* 0xA29 */ u8 mPaddingA29[3];
    /* 0xA2C */ u32 mVersionWord;
    /* 0xA30 */ UnidentifiedNetworkConnectionListener* mListener;
    /* 0xA34 */ UnidentifiedDatagramSocket mBroadcastSocket;
    /* 0xA38 */ UnidentifiedDatagramSocket mDirectSocket;
    /* 0xA3C */ u8 mPacketBuffer[0x5B9];
    /* 0xFF5 */ bool mHasLocalAddress;
    /* 0xFF6 */ u8 mPaddingFF6[2];
    /* 0xFF8 */ u8 mLocalAddress[4];
}; // size: 0xFFC

class NetworkTransport_8032CA4C : public UnidentifiedMachineRoster
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkTransport_8032CA4C();

    virtual UnidentifiedTransportPlayer* GetPlayerInfo(int index);
    virtual void Update(float dt);
    virtual int GetPlayerCount();

    /* 0x004 */ u8 mUnidentified004[0x88 - 0x4];
    /* 0x088 */ int mState;
    /* 0x08C */ u8 mUnidentified08C[0x1D0 - 0x8C];
}; // size: 0x1D0

class NetworkLobby_80133634 : public UnidentifiedMachineRoster,
                              public UnidentifiedNetworkMessageReceiver
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkLobby_80133634();

    void RegisterMessageReceiver();
    void Reset();
    void UnregisterMessageReceiver();

    virtual u32 GetMachineAid(int index);
    virtual int MachineIdxFromConnection(u32 connection);
    virtual int RosterVirtual08();
    virtual void RosterVirtual0C();
    virtual int GetMaxMachineCount();
    virtual void RosterVirtual14();
    virtual int GetMachineCount();
    virtual UnidentifiedTransportPlayer* GetPlayerInfo(int index);
    virtual int GetLocalMachineIndex();
    virtual UnidentifiedTransportPlayer* GetLocalPlayerInfo();
    virtual void SetUserMatchData(u8 size, const void* data);
    virtual void* GetUserMatchData(u8* size);
    virtual void Update(float dt);
    virtual int GetPlayerCount();
    virtual void DebugDraw(int column, int* row);
    virtual void OnConnected(u32 connection, int result);
    virtual int ShouldAcceptConnection(u32 connection, u8* address);
    virtual void OnConnectionClosed(u32 connection, int reason);
    virtual void OnGameStarted();
    virtual void Shutdown(bool reset);

    virtual int ReceiverVirtual00(UnidentifiedNetworkMessage* message);

    bool AreAllConnectionsReady();
    void CloseConnections();
    void CloseConnectionsAndReset();
    bool CanCancelMatchmaking();
    void CancelMatchmaking();
    bool StartMatchmaking();
    void StartMatchmakingThread();
    void OnMatchmakingResult_80134DBC(int error, bool cancelled, void* param);
    bool StartFriendServer();
    void StopFriendServer_801350E0();
    void SetFriendHostInviting_80135208();
    bool ConnectToFriendServer(int profileId);
    void OnFriendMatchmakingResult_8013535C(int error, bool cancelled,
        bool self, bool isServer, int index, void* param);
    void UpdatePeerConnectionState_801355EC(int aid);
    void BuildLocalMachineInfo(UnidentifiedDraftEntry* info);
    void MarkGameStarted();
    bool AllMachineInfoReceived();
    UnidentifiedDraftEntry* GetLocalMachineInfo();
    UnidentifiedDraftEntry* GetMachineInfo(int index);

    /* 0x0008 */ bool mReceiverRegistered;
    /* 0x0009 */ bool mUnidentified009;
    /* 0x000A */ bool mFriendMatch;
    /* 0x000B */ bool mHostingFriendMatch;
    /* 0x000C */ bool mFriendHostInviting;
    /* 0x000D */ u8 mPadding00D[3];
    /* 0x0010 */ u32 mMatchFlags;
    /* 0x0014 */ u32 mUnidentified014;
    /* 0x0018 */ int mMatchmakingPoints;
    /* 0x001C */ u32 mProfileId;
    /* 0x0020 */ u32 mUnidentified020;
    /* 0x0024 */ int mGameType;
    /* 0x0028 */ int mAnyPlayerMatch;
    /* 0x002C */ u32 mPadding02C;
    /* 0x0030 */ u32 mUnidentified030;
    /* 0x0034 */ u32 mUnidentified034;
    /* 0x0038 */ int mFriendProfileId;
    /* 0x003C */ void* mReceiveBuffers[4];
    /* 0x004C */ int mState;
    /* 0x0050 */ bool mMatchFailed;
    /* 0x0051 */ bool mCancelRequested;
    /* 0x0052 */ bool mUnidentified052;
    /* 0x0053 */ u8 mPadding053;
    /* 0x0054 */ float mElapsedTime;
    /* 0x0058 */ float mConnectionDeadline;
    /* 0x005C */ UnidentifiedTransportPlayer mPlayers[4];
    /* 0x00DC */ int mMachineCount;
    /* 0x00E0 */ int mGameStarted;
    /* 0x00E4 */ u8 mUserMatchDataSize;
    /* 0x00E5 */ u8 mUserMatchData[8];
    /* 0x00ED */ bool mMachineInfoReceived[4];
    /* 0x00F1 */ u8 mPadding0F1[3];
    /* 0x00F4 */ UnidentifiedDraftEntry mMachineInfo[4];
    /* 0x02F4 */ bool mMatchmakingThreadRunning;
    /* 0x02F5 */ u8 mPadding2F5[3];
    /* 0x02F8 */ u8 mMatchmakingThread[0x318];
    /* 0x0610 */ u8 mMatchmakingThreadStack[0x4000];
}; // size: 0x4610

class UnidentifiedNetworkSession : public UnidentifiedNetworkSessionBase,
                                   public UnidentifiedNetworkConnectionListener,
                                   public UnidentifiedNetworkMessageReceiver
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    UnidentifiedNetworkSession()
    {
        mUnidentified2464 = 0;
        mUnidentified2468 = 0;
        Initialize(true);
    }

    static void Create();

    void SendTournamentStartToEveryone();
    void SendGameStartToEveryone();
    void SendDraftToEveryone(int count, UnidentifiedDraftEntry* entries, bool, u8);
    void SendDraftToEveryone(NetMessageDraft* message);
    void SendSidesChangedToEveryone(UnidentifiedNetworkMessage* message);
    void SendSidesChangedToHost(UnidentifiedNetworkMessage* message);
    void SendCheckConnectionToEveryone();
    void SendConnectionDecisionToEveryone(UnidentifiedNetworkMessage* message);
    void SendConnectionDecisionToHost(UnidentifiedNetworkMessage* message);
    void fn_8011FE40();
    void fn_801202AC();
    bool fn_80120338();
    bool fn_80120368();
    void fn_801203C0();
    bool fn_80120440();
    void DWCLoginCallback(int error, int profileID, void* param);
    bool fn_80120738();
    void fn_80120838();
    void fn_801214BC();
    NetworkLobby_80133634* fn_801216F0();
    NetworkStatsInterface* fn_8012170C();
    NetworkStatsReporter_8012CE20* fn_80121738();
    NetworkRanking_8012D8F4* fn_80121754();
    void fn_80124038(u32 connection, int reason);
    void fn_80122C84();
    void fn_801239F8();
    void fn_801239FC();
    u8 fn_80123A00();
    int fn_80123314();
    int fn_80123360();
    void fn_80123A08();
    void fn_80123E44(u8 value);
    bool fn_80123E70(u32 connection);
    void fn_80123FBC(int overlay);
    void fn_801241C8();
    u32 GetVersionWord()
    {
        int channel = 10;
        switch (GetRegion())
        {
        case 0:
            channel = 10;
            break;
        case 1:
            channel = 14;
            break;
        case 2:
            channel = 15;
            break;
        }

        u32 low = (u16)g_BuildNumber;
        if (lbl_806E10F8 != 0)
        {
            low = (u16)lbl_806E10F8;
        }
        return (low | 0x1B030000) | ((u32)(channel & 0xFF) << 20);
    }

    virtual void Initialize(bool);
    virtual void Update();
    virtual NetworkSocket_801246E4* GetDirectSocket();
    virtual UnidentifiedMachineRoster* GetMachineRoster();
    virtual NetworkTransport_8032CA4C* GetTransport();
    virtual void BaseVirtual3C(UnidentifiedGameStartInfo* info);
    virtual void BaseVirtual40();
    virtual void BaseVirtual44(NetMessageGameStart* message);
    virtual void BaseVirtual48(int reason);
    virtual int Send(s8 player, void* buffer, int size, bool reliable);

    virtual void OnlineVirtual00();
    virtual void OnlineVirtual04();
    virtual void OnlineVirtual08();
    virtual int OnlineVirtual0C();
    virtual int OnlineVirtual10();
    virtual void OnlineVirtual14(int);

    virtual void ListenerVirtual00(void* buffer, int size);
    virtual void ListenerVirtual04(
        int source, void* buffer, int size, bool reliable);
    virtual void ListenerVirtual08(u32 connection, u8* address);
    virtual void ListenerVirtual0C(u32 connection, int result);
    virtual void ListenerVirtual10(u32 connection, int reason);
    virtual void ListenerVirtual14();
    virtual void ListenerVirtual18();

    virtual int ReceiverVirtual00(UnidentifiedNetworkMessage* message);

    /* 0x2438 */ float mElapsedTime;
    /* 0x243C */ u32 mUpdateCount;
    /* 0x2440 */ u32 mLastTicker;
    /* 0x2444 */ int mSessionMode;
    /* 0x2448 */ int mUnidentified2448;
    /* 0x244C */ int mUnidentified244C;
    /* 0x2450 */ NetworkSocket_801246E4* mDirectSocket;
    /* 0x2454 */ NetworkTransport_8032CA4C* mTransport;
    /* 0x2458 */ NetworkLobby_80133634* mLobby;
    /* 0x245C */ NetworkStatsReporter_8012CE20* mStatsReporter;
    /* 0x2460 */ NetworkRanking_8012D8F4* mRankingReporter;
    /* 0x2464 */ u32 mUnidentified2464;
    /* 0x2468 */ u32 mUnidentified2468;
    /* 0x246C */ u8 mUnidentified246C;
    /* 0x246D */ u8 mUnidentified246D;
    /* 0x246E */ u8 mUnidentified246E[4];
    /* 0x2472 */ u8 mUnidentified2472;
    /* 0x2473 */ u8 mUnidentified2473;
    /* 0x2474 */ u32 mUnidentified2474;
    /* 0x2478 */ u32 mUnidentified2478;
    /* 0x247C */ u32 mUnidentified247C;
    /* 0x2480 */ int mOverlayRequest;
    /* 0x2484 */ int mPoppedOverlay;
    /* 0x2488 */ int mDWCErrorCode;
    /* 0x248C */ int mDWCErrorType;
    /* 0x2490 */ int mDWCLastError;
    /* 0x2494 */ u8 mUnidentified2494;
    /* 0x2498 */ int mLoginStage;
    /* 0x249C */ UnidentifiedLoginListener* mLoginListener;
    /* 0x24A0 */ float mLoginStartTime;
    /* 0x24A4 */ u8 mUnidentified24A4;
    /* 0x24A5 */ u8 mUnidentified24A5;
    /* 0x24A8 */ u32 mLoginThread[0x318 / 4];
    /* 0x27C0 */ u8 mLoginThreadStack[0x4000];
}; // size: 0x67C0

extern UnidentifiedNetworkSession* lbl_806E20D8;
extern UnidentifiedNetworkSession* lbl_806E10EC;

extern "C" int fn_80338BF0(UnidentifiedNetworkSession* session);
extern "C" s8 fn_80338C20(UnidentifiedNetworkSession* session);

#endif // GAME_NETWORK_SESSION_H
