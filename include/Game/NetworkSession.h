#ifndef GAME_NETWORK_SESSION_H
#define GAME_NETWORK_SESSION_H

#include "Game/main.h"
#include "NL/nlMemory.h"
#include "types.h"

class UnidentifiedNetworkMessage;
class NetMessageDraft;
struct UnidentifiedDraftEntry;
class NetworkSocket_801246E4;
class NetworkTransport_8032CA4C;
class NetworkLobby_80133634;
class NetworkStatsReporter_8012CE20;
class NetworkObject_8012D8F4;
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
    virtual void ListenerVirtual00(u32 a, void* b) = 0;
    virtual void ListenerVirtual04(int type, u32 a, void* b) = 0;
    virtual void ListenerVirtual08(u32 connection, u8* address) = 0;
    virtual void ListenerVirtual0C(u32 connection, int result) = 0;
    virtual void ListenerVirtual10(u32 connection, int reason) = 0;
    virtual void ListenerVirtual14() = 0;
    virtual void ListenerVirtual18() = 0;
};

class UnidentifiedNetworkMessageReceiver
{
public:
    virtual int ReceiverVirtual00(UnidentifiedNetworkMessage* message) = 0;
};

class UnidentifiedLoginListener
{
public:
    virtual void OnLoginResult(int result) = 0;
    virtual void OnStatsResult(bool success) = 0;
};

struct UnidentifiedTransportPlayer
{
    /* 0x00 */ char mName[12];
    /* 0x0C */ u32 mUnidentified0C;
    /* 0x10 */ u16 mUnidentified10;
    /* 0x12 */ u16 mUnidentified12;
};

class UnidentifiedMachineRoster
{
public:
    virtual u32 GetMachineAid(int index);
    virtual int MachineIdxFromConnection(u32 connection);
    virtual int RosterVirtual08();
    virtual void RosterVirtual0C();
    virtual void RosterVirtual10();
    virtual void RosterVirtual14();
    virtual int GetMachineCount();
    virtual UnidentifiedTransportPlayer* GetPlayerInfo(int index);
    virtual void RosterVirtual20();
    virtual void RosterVirtual24();
    virtual void RosterVirtual28();
    virtual void RosterVirtual2C();
    virtual void Update(float dt);
    virtual int GetPlayerCount();
    virtual void RosterVirtual38();
    virtual void OnConnected(u32 connection, int result);
    virtual int ShouldAcceptConnection(u32 connection, u8* address);
    virtual void OnConnectionClosed(u32 connection, int reason);
    virtual void RosterVirtual48();
    virtual void RosterVirtual4C(bool);
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

// Reliable UDP direct/broadcast socket layer owned by the session; its
// translation unit begins at 0x801246E4.
class NetworkSocket_801246E4
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkSocket_801246E4();

    virtual void SocketVirtual00(void* info, UnidentifiedNetworkConnectionListener* listener);
    virtual void SocketVirtual04();
    virtual void SocketVirtual08(bool);
    virtual void SocketVirtual0C();
    virtual void SocketVirtual10();
    virtual void SocketVirtual14();
    virtual void SocketVirtual18();
    virtual void SocketVirtual1C(u32 connection);
    virtual void SocketVirtual20(u32 connection);
    virtual void SocketVirtual24();
    virtual void SocketVirtual28();
    virtual void Send(int aid, void* buffer, int size, bool reliable);
    virtual void Receive(void* buffer, int size);
    virtual void SocketVirtual3C();
    virtual void Update(float dt);
    virtual void SocketVirtual44();
    virtual void SocketVirtual48();
    virtual void SocketVirtual4C();
    virtual void SocketVirtual50();

    /* 0x004 */ u8 mUnidentified004[0xFFC - 0x4];
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

class NetworkLobby_80133634 : public UnidentifiedMachineRoster
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkLobby_80133634();

    virtual void Update(float dt);

    /* 0x004 */ u8 mUnidentified004[0xE0 - 0x4];
    /* 0x0E0 */ int mUnidentified0E0;
    /* 0x0E4 */ u8 mUnidentified0E4[0x2F4 - 0xE4];
    /* 0x2F4 */ u8 mUnidentified2F4;
    /* 0x2F5 */ u8 mUnidentified2F5[0x4610 - 0x2F5];
}; // size: 0x4610

class NetworkStatsReporter_8012CE20
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkStatsReporter_8012CE20();

    virtual void ReporterVirtual00();
    virtual void ReporterVirtual04();
    virtual void ReporterVirtual08();
    virtual void ReporterVirtual0C();
    virtual void ReporterVirtual10();
    virtual void ReporterVirtual14();

    /* 0x004 */ u8 mUnidentified004[0x100 - 0x4];
}; // size: 0x100

class NetworkObject_8012D8F4
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkObject_8012D8F4();

    virtual void ObjectVirtual00();
    virtual void ObjectVirtual04();
    virtual void ObjectVirtual08();
    virtual void ObjectVirtual0C();
    virtual void ObjectVirtual10();
    virtual void ObjectVirtual14();

    /* 0x004 */ u8 mUnidentified004[0xA8 - 0x4];
}; // size: 0xA8

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
    void* fn_8012170C();
    NetworkStatsReporter_8012CE20* fn_80121738();
    NetworkObject_8012D8F4* fn_80121754();
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

    virtual void ListenerVirtual00(u32 a, void* b);
    virtual void ListenerVirtual04(int type, u32 a, void* b);
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
    /* 0x2460 */ NetworkObject_8012D8F4* mUnidentified2460;
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
