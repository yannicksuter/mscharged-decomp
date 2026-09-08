#include "Game/NetworkSession.h"

#include "unclassified/tu_80336B2C.h"

int NetworkSessionData::GetNumMachines()
{
    return mMachineCount;
}

UnidentifiedNetworkPeer* NetworkSessionData::GetPeer(s8 machine)
{
    return &mPeers[machine];
}

UnidentifiedNetworkPeer* NetworkSessionData::GetLocalPeer()
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
        UnidentifiedNetworkPeer* peer = &mPeers[machine];
        peer->mMachineId = machine;
        peer->mUnidentified004 = playerCount;

        for (int player = 0; player < (int)peer->mUnidentified004; ++player)
        {
            fn_80336D50(fn_80336B6C(peer, player), peer, (s8)player, player);
        }

        fn_80336BE0(peer);
    }
}
