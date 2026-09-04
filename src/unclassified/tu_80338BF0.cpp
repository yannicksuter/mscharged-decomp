#include "Game/NetworkSession.h"

#include "unclassified/tu_80336B2C.h"

extern "C" int fn_80338BF0(UnidentifiedNetworkSessionData* session)
{
    return session->mUnidentified0000;
}

extern "C" UnidentifiedNetworkPeer* fn_80338BF8(
    UnidentifiedNetworkSessionData* session, s8 machine)
{
    return &session->mPeers[machine];
}

extern "C" UnidentifiedNetworkPeer* fn_80338C0C(
    UnidentifiedNetworkSessionData* session)
{
    return &session->mPeers[session->mUnidentified2424];
}

extern "C" s8 fn_80338C20(UnidentifiedNetworkSessionData* session)
{
    return session->mUnidentified2424;
}

extern "C" void fn_80338C2C(UnidentifiedNetworkSessionData* session,
    int machineCount, int playerCount)
{
    session->mUnidentified0000 = machineCount;
    session->mUnidentified2424 = 0;

    for (int machine = 0; machine < machineCount; ++machine)
    {
        UnidentifiedNetworkPeer* peer = &session->mPeers[machine];
        peer->mMachineId = machine;
        peer->mUnidentified004 = playerCount;

        for (int player = 0; player < (int)peer->mUnidentified004; ++player)
        {
            fn_80336D50(fn_80336B6C(peer, player), peer, (s8)player, player);
        }

        fn_80336BE0(peer);
    }
}
