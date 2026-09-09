#include "Game/NetworkMessageRegistry.h"
#include "Game/LANMessages.h"

static NetworkMessageFactory<NetMessageFindGame> sFactoryType2;
static NetworkMessageFactory<NetMessageFoundGame> sFactoryType3;
static NetworkMessageFactory<NetMessageJoinRequest> sFactoryType4;
static NetworkMessageFactory<NetMessageJoinResponse> sFactoryType5;
static NetworkMessageFactory<NetMessageTransportType6> sFactoryType6;
static NetworkMessageFactory<NetMessageGamePeerAdded> sFactoryType7;
static NetworkMessageFactory<NetMessageReadyToLaunchRequest> sFactoryType10;
static NetworkMessageFactory<NetMessageReadyToLaunchConfirm> sFactoryType11;
static NetworkMessageFactory<NetMessageClientConfirmedJoin> sFactoryType12;

void RegisterLANMessages()
{
    gNetworkMessageRegistry->RegisterFactory(2, &sFactoryType2);
    gNetworkMessageRegistry->RegisterFactory(3, &sFactoryType3);
    gNetworkMessageRegistry->RegisterFactory(4, &sFactoryType4);
    gNetworkMessageRegistry->RegisterFactory(5, &sFactoryType5);
    gNetworkMessageRegistry->RegisterFactory(6, &sFactoryType6);
    gNetworkMessageRegistry->RegisterFactory(7, &sFactoryType7);
    gNetworkMessageRegistry->RegisterFactory(10, &sFactoryType10);
    gNetworkMessageRegistry->RegisterFactory(11, &sFactoryType11);
    gNetworkMessageRegistry->RegisterFactory(12, &sFactoryType12);
}
