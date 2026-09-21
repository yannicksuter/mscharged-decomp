#ifndef GAME_NETWORK_PEER_H
#define GAME_NETWORK_PEER_H

#include "Game/DetInput.h"
#include "types.h"

class cGlobalPad;
class NetworkPeer;
struct PackedDetInput;

class NetworkPeerChannel
{
public:
    NetworkPeerChannel();

    void Initialize(NetworkPeer* peer, s8 channelIndex, int globalPadIndex);
    DetInput* GetNetworkPeerChannelInput();
    s8 GetNetworkPeerChannelId();
    cGlobalPad* GetLocalChannelPad();
    void CaptureNetworkPeerChannelInput();
    void ApplyNetworkPeerChannelInput(PackedDetInput* record, u16 tick, u8 connected);
    void PackNetworkPeerChannelInput(PackedDetInput* record);
    u16 GetNetworkPeerChannelRemapAngle();
    u8 GetNetworkPeerChannelConnectionStatus();
    ~NetworkPeerChannel() { }

    /* 0x000 */ NetworkPeer* mPeer;
    /* 0x004 */ s8 mChannelIndex;
    /* 0x005 */ u8 mPadding005[3];
    /* 0x008 */ int mGlobalPadIndex;
    /* 0x00C */ bool mUnidentified00C;
    /* 0x00D */ u8 mPadding00D[3];
    /* 0x010 */ DetInput mInput0;
    /* 0x09C */ DetInput mInput1;
    /* 0x128 */ DetInput mInput2;
    /* 0x1B4 */ DetInput mInput3;
};

class NetworkPeer
{
public:
    NetworkPeer();

    NetworkPeerChannel* GetNetworkPeerChannel(int channel);
    s8 GetNetworkPeerMachineId();
    void ResetNetworkPeerInputs();

    /* 0x000 */ s8 mMachineId;
    /* 0x004 */ u32 mPlayerCount;
    /* 0x008 */ NetworkPeerChannel mChannels[4];
}; // size: 0x908

s8 GetNetworkPlayerId(s8 player, s8 machine);

#endif // GAME_NETWORK_PEER_H
