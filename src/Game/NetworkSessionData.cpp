#include "Game/NetworkSessionData.h"


int NetworkSessionData::GetNumMachines()
{
    return mMachineCount;
}

NetworkPeer* NetworkSessionData::GetPeer(s8 machine)
{
    return &mPeers[machine];
}

NetworkPeer* NetworkSessionData::GetLocalPeer()
{
    return &mPeers[mLocalMachineId];
}

s8 NetworkSessionData::GetLocalMachineId()
{
    return mLocalMachineId;
}

void NetworkSessionData::InitializeMachines(int machineCount, int playerCount)
{
    mMachineCount = machineCount;
    mLocalMachineId = 0;

    for (int machine = 0; machine < machineCount; ++machine)
    {
        NetworkPeer* peer = &mPeers[machine];
        peer->mMachineId = machine;
        peer->mPlayerCount = playerCount;

        for (int player = 0; player < (int)peer->mPlayerCount; ++player)
        {
            (peer->GetNetworkPeerChannel(player))->Initialize(peer, (s8)player, player);
        }

        peer->ResetNetworkPeerInputs();
    }
}
