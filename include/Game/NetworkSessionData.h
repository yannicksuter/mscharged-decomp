#ifndef GAME_NETWORK_SESSION_DATA_H
#define GAME_NETWORK_SESSION_DATA_H

#include "Game/NetworkPeer.h"

// Data-only base establishing the peer storage that precedes every vtable
// pointer in the retail object.
class NetworkSessionData
{
public:
    ~NetworkSessionData();

    int GetNumMachines();
    NetworkPeer* GetPeer(s8 machine);
    NetworkPeer* GetLocalPeer();
    s8 GetLocalMachineId();
    void InitializeMachines(int machineCount, int playerCount);

    /* 0x0000 */ int mMachineCount;
    /* 0x0004 */ NetworkPeer mPeers[4];
    /* 0x2424 */ int mLocalMachineId;
}; // size: 0x2428

#endif // GAME_NETWORK_SESSION_DATA_H
