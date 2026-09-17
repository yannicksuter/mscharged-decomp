#ifndef GAME_NETWORK_LOBBY_H
#define GAME_NETWORK_LOBBY_H

#include "Game/NetworkSession.h"
#include <dwc/dwci_error.h>
#include <dwc/dwc_main_fwd.h>

struct NetworkLobbyPlayer : public TransportPlayerInfo
{
    /* 0x14 */ unsigned int mConnection;
    /* 0x18 */ u8 mUnidentified18[4];
    /* 0x1C */ int mConnectionState;
}; // size: 0x20

class NetworkLobby : public NetworkMachineRoster,
                     public NetworkMessageReceiver
{
public:
    void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }

    NetworkLobby();

    void RegisterMessageReceiver();
    void Reset();
    void UnregisterMessageReceiver();

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

    virtual int ProcessMessage(NetworkMessage* message);

    bool AreAllConnectionsReady();
    void CloseConnections();
    void CloseConnectionsAndReset();
    bool CanCancelMatchmaking();
    int GetConnectionCount() const { return DWC_GetNumConnectionHost(); }
    bool IsMatchmaking() const { return mMatchmakingThreadRunning || mState != 0; }
    void CancelMatchmaking();
    bool StartMatchmaking();
    void StartMatchmakingThread();
    void OnMatchmakingResult(DWCErrorType error, int cancelled, void* param);
    bool StartFriendServer();
    void StopFriendServer();
    void SetFriendHostInviting();
    bool ConnectToFriendServer(int profileId);
    void OnFriendMatchmakingResult(DWCErrorType error, int cancelled,
        int self, int isServer, int index, void* param);
    void UpdatePeerConnectionState(int aid);
    inline void UpdatePeerConnectionStates();
    void BuildLocalMachineInfo(NetworkDraftMachineInfo* info);
    inline void SendLocalMachineInfo(int aid);
    void MarkGameStarted();
    bool AllMachineInfoReceived();
    NetworkDraftMachineInfo* GetLocalMachineInfo();
    NetworkDraftMachineInfo* GetMachineInfo(int index);

    /* 0x0008 */ bool mReceiverRegistered;
    /* 0x0009 */ u8 mTournamentMode;
    /* 0x000A */ bool mFriendMatch;
    /* 0x000B */ bool mHostingFriendMatch;
    /* 0x000C */ bool mFriendHostInviting;
    /* 0x000D */ u8 mPadding00D[3];
    /* 0x0010 */ unsigned int mMatchFlags;
    /* 0x0014 */ unsigned int mVersionWord;
    /* 0x0018 */ int mMatchmakingPoints;
    /* 0x001C */ unsigned int mCountryMatchGroup;
    /* 0x0020 */ unsigned int mProfileId;
    /* 0x0024 */ int mMaxMatchmakingEntries;
    /* 0x0028 */ int mMinMatchmakingEntries;
    /* 0x002C */ unsigned int mPadding02C;
    /* 0x0030 */ unsigned int mUnidentified030;
    /* 0x0034 */ unsigned int mUnidentified034;
    /* 0x0038 */ int mFriendProfileId;
    /* 0x003C */ void* mReceiveBuffers[4];
    /* 0x004C */ int mState;
    /* 0x0050 */ bool mMatchFailed;
    /* 0x0051 */ bool mCancelRequested;
    /* 0x0052 */ bool mUnidentified052;
    /* 0x0053 */ u8 mPadding053;
    /* 0x0054 */ float mElapsedTime;
    /* 0x0058 */ float mConnectionDeadline;
    /* 0x005C */ NetworkLobbyPlayer mPlayers[4];
    /* 0x00DC */ int mMachineCount;
    /* 0x00E0 */ int mGameStarted;
    /* 0x00E4 */ u8 mUserMatchDataSize;
    /* 0x00E5 */ u8 mUserMatchData[8];
    /* 0x00ED */ bool mMachineInfoReceived[4];
    /* 0x00F1 */ u8 mPadding0F1[3];
    /* 0x00F4 */ NetworkDraftMachineInfo mMachineInfo[4];
    /* 0x02F4 */ bool mMatchmakingThreadRunning;
    /* 0x02F5 */ u8 mPadding2F5[3];
    /* 0x02F8 */ u8 mMatchmakingThread[0x318];
    /* 0x0610 */ u8 mMatchmakingThreadStack[0x4000];
}; // size: 0x4610

#endif // GAME_NETWORK_LOBBY_H
