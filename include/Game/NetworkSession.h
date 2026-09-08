#ifndef GAME_NETWORK_SESSION_H
#define GAME_NETWORK_SESSION_H

#include <dwc/dwci_error.h>

#include "Game/main.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkStats.h"
#include "NL/nlMemory.h"
#include "NL/plat/TransportSocket.h"
#include "types.h"
#include "Game/DetInput.h"

class NetMessageDraft;
struct RFLStoreData;
struct UnidentifiedDraftEntry;
class NetworkSocket_801246E4;
class NetworkTransport_8032CA4C;
class NetworkLobby;
class NetMessageGameStart;

struct RecordedGameConfig;

// Per-machine start payload consumed by the session base initializer.
struct UnidentifiedGameStartInfo
{
    /* 0x00 */ unsigned int mConfigSize;
    /* 0x04 */ RecordedGameConfig* mConfig;
    /* 0x08 */ unsigned int mSeed;
    /* 0x0C */ int mMachineCount;
    /* 0x10 */ int mMyMachineId;
    /* 0x14 */ int mPlayerCounts[4];
};

extern int gNetworkBuildNumberOverride;
unsigned int GetNetworkVersionWord();
void RestartSinglePlayerGame();
void StartSinglePlayerGame();
void PlaybackRecordedGame();

class UnidentifiedNetworkConnectionListener;
class UnidentifiedNetworkPeer;

class UnidentifiedNetworkPeerChannel
{
public:
    UnidentifiedNetworkPeerChannel();
    ~UnidentifiedNetworkPeerChannel() { }

    /* 0x000 */ UnidentifiedNetworkPeer* mPeer;
    /* 0x004 */ s8 mChannelIndex;
    /* 0x005 */ u8 mPadding005[3];
    /* 0x008 */ int mGlobalPadIndex;
    /* 0x00C */ bool mUnidentified00C;
    /* 0x00D */ u8 mPadding00D[3];
    /* 0x010 */ DetInput mInputs[4];
};

class UnidentifiedNetworkPeer
{
public:
    UnidentifiedNetworkPeer();

    /* 0x000 */ s8 mMachineId;
    /* 0x004 */ unsigned int mUnidentified004;
    /* 0x008 */ UnidentifiedNetworkPeerChannel mChannels[4];
}; // size: 0x908

// Data-only base establishing the peer storage that precedes every vtable
// pointer in the retail object.
class NetworkSessionData
{
public:
    ~NetworkSessionData();

    int GetNumMachines();
    UnidentifiedNetworkPeer* GetPeer(s8 machine);
    UnidentifiedNetworkPeer* GetLocalPeer();
    s8 GetLocalMachineId();
    void InitializeMachines(int machineCount, int playerCount);

    /* 0x0000 */ int mMachineCount;
    /* 0x0004 */ UnidentifiedNetworkPeer mPeers[4];
    /* 0x2424 */ int mLocalMachineId;
}; // size: 0x2428

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

class UnidentifiedNetworkConnectionListener
{
public:
    virtual void ListenerVirtual00(void* buffer, int size) = 0;
    virtual void ListenerVirtual04(int source, void* buffer, int size, bool reliable) = 0;
    virtual void ListenerVirtual08(unsigned int connection, u8* address) = 0;
    virtual void ListenerVirtual0C(unsigned int connection, int result) = 0;
    virtual void ListenerVirtual10(unsigned int connection, int reason) = 0;
    virtual void ListenerVirtual14() = 0;
    virtual void ListenerVirtual18() = 0;
};

// Interface presented by the reliable connection manager embedded in the
// direct socket layer.
class UnidentifiedTransportConnection;

class UnidentifiedReliableSocketCallback
{
public:
    virtual void OnConnectionAttempted(unsigned int connection, int result) = 0;
    virtual void OnConnectionClosed(unsigned int connection, int reason) = 0;
    virtual void ReliableCallbackVirtual08() = 0;
    virtual void ReliableCallbackVirtual0C(
        unsigned int connection, void* buffer, int size, bool reliable) = 0;
    virtual void ReliableCallbackVirtual10(
        unsigned int connection, void* buffer, int size) = 0;
    virtual void ReliableCallbackVirtual14(
        unsigned int connection, u8* address, int a, int b, int c) = 0;
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
    virtual void AcceptConnection(unsigned int connection) = 0;
    virtual void RejectConnection(unsigned int connection) = 0;
    virtual void SocketVirtual24(
        UnidentifiedTransportConnection* connection, bool immediate) = 0;
    virtual void* FindConnection(const u8* address) = 0;
    virtual void Send(int aid, void* buffer, int size, bool reliable) = 0;
    virtual void Receive(void* buffer, int size) = 0;
    virtual void SocketVirtual34(u8 aid, void* buffer, int size) = 0;
    virtual void Update(float dt) = 0;
    virtual void SocketVirtual44(int a, int* b, bool c) = 0;
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

struct UnidentifiedTransportPlayer
{
    UnidentifiedTransportPlayer()
    {
        mUnidentified0B = 0;
    }

    /* 0x00 */ char mName[11];
    /* 0x0B */ u8 mUnidentified0B;
    /* 0x0C */ unsigned int mUnidentified0C;
    /* 0x10 */ u16 mUnidentified10;
    /* 0x12 */ u16 mUnidentified12;
}; // size: 0x14

class UnidentifiedMachineRoster
{
public:
    virtual unsigned int GetMachineAid(int index);
    virtual int MachineIdxFromConnection(unsigned int connection);
    virtual int RosterVirtual08();
    virtual void RosterVirtual0C(int);
    virtual int GetMaxMachineCount();
    virtual void RosterVirtual14(int);
    virtual int GetMachineCount();
    virtual UnidentifiedTransportPlayer* GetPlayerInfo(int index);
    virtual int GetLocalMachineIndex();
    virtual UnidentifiedTransportPlayer* GetLocalPlayerInfo();
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
    virtual NetworkSocket_801246E4* GetDirectSocket();
    virtual UnidentifiedMachineRoster* GetMachineRoster();
    virtual NetworkTransport_8032CA4C* GetTransport();
    virtual void BaseVirtual3C(UnidentifiedGameStartInfo* info);
    virtual void BaseVirtual40();
    virtual void BaseVirtual44(NetMessageGameStart* message);
    virtual void BaseVirtual48(int reason);
    virtual int Send(s8 player, void* buffer, int size, bool reliable);
    virtual void DebugDraw();
};

struct UnidentifiedReliableSocketState
{
    UnidentifiedReliableSocketState();

    u8 mData[0xA1C];
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
    virtual void AcceptConnection(unsigned int connection);
    virtual void RejectConnection(unsigned int connection);
    virtual void SocketVirtual24(
        UnidentifiedTransportConnection* connection, bool immediate);
    virtual void* FindConnection(const u8* address);
    virtual void Send(int aid, void* buffer, int size, bool reliable);
    virtual void Receive(void* buffer, int size);
    virtual void SocketVirtual34(u8 aid, void* buffer, int size);
    virtual void Update(float dt);
    virtual void SocketVirtual44(int a, int* b, bool c);
    virtual void SocketVirtual48();
    virtual u8* GetLocalAddress();
    virtual u16 GetLocalPort();

    virtual void OnConnectionAttempted(unsigned int connection, int result);
    virtual void OnConnectionClosed(unsigned int connection, int reason);
    virtual void ReliableCallbackVirtual08();
    virtual void ReliableCallbackVirtual0C(
        unsigned int connection, void* buffer, int size, bool reliable);
    virtual void ReliableCallbackVirtual10(
        unsigned int connection, void* buffer, int size);
    virtual void ReliableCallbackVirtual14(
        unsigned int connection, u8* address, int a, int b, int c);
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
    /* 0xA2C */ unsigned int mVersionWord;
    /* 0xA30 */ UnidentifiedNetworkConnectionListener* mListener;
    /* 0xA34 */ TransportSocket mBroadcastSocket;
    /* 0xA38 */ TransportSocket mDirectSocket;
    /* 0xA3C */ u8 mPacketBuffer[0x5B9];
    /* 0xFF5 */ bool mHasLocalAddress;
    /* 0xFF6 */ u8 mPaddingFF6[2];
    /* 0xFF8 */ u8 mLocalAddress[4];
}; // size: 0xFFC

struct UnidentifiedTransportGame_8032CA4C
{
    UnidentifiedTransportGame_8032CA4C() { }

    /* 0x00 */ char mUnidentified00[12];
    /* 0x0C */ u8 mUnidentified0C[4];
    /* 0x10 */ int mUnidentified10;
    /* 0x14 */ float mUnidentified14;
    /* 0x18 */ u16 mUnidentified18;
}; // size: 0x1C

struct UnidentifiedTransportPeer_8032CA4C : public UnidentifiedTransportPlayer
{
    /* 0x14 */ u8 mUnidentified14[4];
    /* 0x18 */ int mUnidentified18;
    /* 0x1C */ int mUnidentified1C;
    /* 0x20 */ u16 mUnidentified20;
    /* 0x22 */ u8 mUnidentified22;
}; // size: 0x24

struct UnidentifiedTransportPoolEntry_8032CA4C
{
    UnidentifiedTransportConnection* m_Connection;
    int mUnidentified04;
};

class UnidentifiedTransportListener_8032CA4C
{
public:
    virtual void UnidentifiedVirtual00(int result) = 0;
    virtual void UnidentifiedVirtual04(int result) = 0;
    virtual void UnidentifiedVirtual08(int result) = 0;
    virtual void UnidentifiedVirtual0C(UnidentifiedTransportGame_8032CA4C*) = 0;
    virtual void UnidentifiedVirtual10() = 0;
    virtual void UnidentifiedVirtual14(UnidentifiedTransportGame_8032CA4C*) = 0;
    virtual void UnidentifiedVirtual18() = 0;
};

class UnidentifiedTransportListener_8032D954
{
public:
    virtual void UnidentifiedVirtual00() = 0;
};

class NetworkMessageType3_805333C8;
class NetworkMessageType4_80533468;
class NetworkMessageType5_80533454;
class NetworkMessageType7_80533440;

class UnidentifiedTransportRoster_8032CA4C : public UnidentifiedMachineRoster
{
public:
    virtual int fn_8032CF60(int value) = 0;
    virtual int fn_8032D0EC() = 0;
    virtual int fn_8032D220(UnidentifiedTransportGame_8032CA4C* game, int value) = 0;
    virtual int fn_8032E84C() = 0;
    virtual void fn_8032EB0C() = 0;
    virtual void fn_8032D94C(UnidentifiedTransportListener_8032CA4C* listener) = 0;
    virtual void fn_8032D954(UnidentifiedTransportListener_8032D954* listener) = 0;
};

class NetworkTransport_8032CA4C : public UnidentifiedTransportRoster_8032CA4C,
                                  public UnidentifiedNetworkMessageReceiver
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkTransport_8032CA4C();

    void fn_8032CBC8();
    void fn_8032CBD0(bool initialize);
    void fn_8032CEAC();
    int fn_8032D0F4(int result);
    void fn_8032D5E0();
    void fn_8032E31C(const void* data);
    void fn_8032E890();
    void fn_8032E8C0();
    bool fn_8032EA20();
    void fn_8032EB88(NetworkMessageType3_805333C8* message);
    void fn_8032ED28(int index);
    void fn_8032EE7C(UnidentifiedTransportConnection* connection, bool accepted);
    void fn_8032F084(int index, NetworkMessageType4_80533468* message);
    void fn_8032F2AC(int index, NetworkMessageType5_80533454* message);
    void fn_8032F6B4(NetworkMessageType7_80533440* message);

    virtual unsigned int GetMachineAid(int index);
    virtual int MachineIdxFromConnection(unsigned int connection);
    virtual int RosterVirtual08();
    virtual void RosterVirtual0C(int value);
    virtual int GetMaxMachineCount();
    virtual void RosterVirtual14(int value);
    virtual int GetMachineCount();
    virtual UnidentifiedTransportPlayer* GetPlayerInfo(int index);
    virtual int GetLocalMachineIndex();
    virtual UnidentifiedTransportPlayer* GetLocalPlayerInfo();
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
    virtual int fn_8032CF60(int value);
    virtual int fn_8032D0EC();
    virtual int fn_8032D220(UnidentifiedTransportGame_8032CA4C* game, int value);
    virtual int fn_8032E84C();
    virtual void fn_8032EB0C();
    virtual void fn_8032D94C(UnidentifiedTransportListener_8032CA4C* listener);
    virtual void fn_8032D954(UnidentifiedTransportListener_8032D954* listener);
    virtual int ReceiverVirtual00(UnidentifiedNetworkMessage* message);

private:
    int UnidentifiedConnectionIndex(UnidentifiedTransportConnection* connection)
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

    int UnidentifiedFreeConnectionIndex()
    {
        for (int index = 0; index < 8; ++index)
        {
            if (m_ConnectionPool[index].mUnidentified04 == 0)
                return index;
        }
        return -1;
    }

public:
    /* 0x008 */ bool mUnidentified008;
    /* 0x00C */ int mUnidentified00C;
    /* 0x010 */ int mUnidentified010;
    /* 0x014 */ UnidentifiedTransportPoolEntry_8032CA4C m_ConnectionPool[8];
    /* 0x054 */ UnidentifiedTransportListener_8032CA4C* mUnidentified054;
    /* 0x058 */ UnidentifiedTransportListener_8032D954* mUnidentified058;
    /* 0x05C */ UnidentifiedTransportGame_8032CA4C* mUnidentified05C;
    /* 0x060 */ int mUnidentified060;
    /* 0x064 */ char mUnidentified064[12];
    /* 0x070 */ int mUnidentified070;
    /* 0x074 */ u8 mUnidentified074;
    /* 0x075 */ u8 mUnidentified075[8];
    /* 0x07D */ bool mUnidentified07D;
    /* 0x080 */ int mUnidentified080;
    /* 0x084 */ int mUnidentified084;
    /* 0x088 */ int mState;
    /* 0x08C */ unsigned int mUnidentified08C;
    /* 0x090 */ int mUnidentified090;
    /* 0x094 */ bool mUnidentified094;
    /* 0x095 */ bool mUnidentified095;
    /* 0x098 */ float mUnidentified098;
    /* 0x09C */ bool mUnidentified09C;
    /* 0x0A0 */ UnidentifiedTransportPeer_8032CA4C mUnidentified0A0[8];
    /* 0x1C0 */ int mUnidentified1C0;
    /* 0x1C4 */ u8 mUnidentified1C4[8];
    /* 0x1CC */ NetworkSocket_801246E4* mUnidentified1CC;
}; // size: 0x1D0

class NetworkSession : public NetworkSessionBase,
                                   public UnidentifiedNetworkConnectionListener,
                                   public UnidentifiedNetworkMessageReceiver
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
    void SendDraftToEveryone(int count, UnidentifiedDraftEntry* entries, bool, u8);
    void SendDraftToEveryone(NetMessageDraft* message);
    void SendSidesChangedToEveryone(UnidentifiedNetworkMessage* message);
    void SendSidesChangedToHost(UnidentifiedNetworkMessage* message);
    void SendCheckConnectionToEveryone();
    void SendConnectionDecisionToEveryone(UnidentifiedNetworkMessage* message);
    void SendConnectionDecisionToHost(UnidentifiedNetworkMessage* message);
    void ShutdownLAN();
    void StartLoginThread();
    bool IsLoginThreadComplete();
    bool RequiresDisconnectAfterError();
    void ReadAndClearDWCError();
    bool fn_80120440();
    void DWCLoginCallback(int error, int profileID, void* param);
    bool RequestLoginRankings();
    void fn_80120838();
    void ShutdownOnline();
    NetworkLobby* GetOnlineLobby();
    NetworkStatsInterface* fn_8012170C();
    NetworkStatsReporter_8012CE20* fn_80121738();
    NetworkRanking_8012D8F4* fn_80121754();
    void fn_80124038(unsigned int connection, int reason);
    void RematchGame();
    void fn_801239F8();
    void fn_801239FC();
    u8 fn_80123A00();
    int fn_80123314();
    int fn_80123360();
    void fn_80123A08();
    void SetTournamentMode(u8 tournament);
    bool fn_80123E70(unsigned int connection);
    void fn_80123FBC(int overlay);
    void fn_801241C8();

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

    virtual void InitializeLAN();
    virtual void InitializeOnline();
    virtual void Shutdown();
    virtual int GetSessionMode();
    virtual int GetSessionState();
    virtual void SetSessionState(int);

    virtual void ListenerVirtual00(void* buffer, int size);
    virtual void ListenerVirtual04(
        int source, void* buffer, int size, bool reliable);
    virtual void ListenerVirtual08(unsigned int connection, u8* address);
    virtual void ListenerVirtual0C(unsigned int connection, int result);
    virtual void ListenerVirtual10(unsigned int connection, int reason);
    virtual void ListenerVirtual14();
    virtual void ListenerVirtual18();

    virtual int ReceiverVirtual00(UnidentifiedNetworkMessage* message);

    /* 0x2438 */ float mElapsedTime;
    /* 0x243C */ unsigned int mUpdateCount;
    /* 0x2440 */ unsigned int mLastTicker;
    /* 0x2444 */ int mSessionMode;
    /* 0x2448 */ int mSessionState;
    /* 0x244C */ int mUnidentified244C;
    /* 0x2450 */ NetworkSocket_801246E4* mDirectSocket;
    /* 0x2454 */ NetworkTransport_8032CA4C* mTransport;
    /* 0x2458 */ NetworkLobby* mLobby;
    /* 0x245C */ NetworkStatsReporter_8012CE20* mStatsReporter;
    /* 0x2460 */ NetworkRanking_8012D8F4* mRankingReporter;
    /* 0x2464 */ unsigned int mUnidentified2464;
    /* 0x2468 */ unsigned int mUnidentified2468;
    /* 0x246C */ u8 mUnidentified246C;
    /* 0x246D */ u8 mUnidentified246D;
    /* 0x246E */ u8 mUnidentified246E[4];
    /* 0x2472 */ u8 mUnidentified2472;
    /* 0x2473 */ u8 mTournamentMode;
    /* 0x2474 */ unsigned int mUnidentified2474;
    /* 0x2478 */ unsigned int mUnidentified2478;
    /* 0x247C */ unsigned int mUnidentified247C;
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
    /* 0x24A8 */ unsigned int mLoginThread[0x318 / 4];
    /* 0x27C0 */ u8 mLoginThreadStack[0x4000];
}; // size: 0x67C0

extern char gNetworkMiiName[12];
extern unsigned short gNetworkMiiNameWide[12];
extern RFLStoreData gNetworkMiiData;
extern u8 gNetworkMiiChanged;
extern int gNetworkSaveSlotIndex;
extern bool gOnlineFourMachineFriendLobby;

extern NetworkSessionBase* g_pNetworkSessionBase;
extern NetworkSession* g_pNetworkSession;

extern u8 gOnlineTwoLocalPlayers;
extern int gOnlineLocalControllerIndices[2];

#endif // GAME_NETWORK_SESSION_H
