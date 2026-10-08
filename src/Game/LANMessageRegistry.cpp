#include "Game/NetworkMessageRegistry.h"
#include "Game/LANMessages.h"

static NetworkMessageFactory<NetMessageFindGame> sFindGameFactory;
static NetworkMessageFactory<NetMessageFoundGame> sFoundGameFactory;
static NetworkMessageFactory<NetMessageJoinRequest> sJoinRequestFactory;
static NetworkMessageFactory<NetMessageJoinResponse> sJoinResponseFactory;
static NetworkMessageFactory<NetMessageTransportType6> sFactoryType6;
static NetworkMessageFactory<NetMessageGamePeerAdded> sGamePeerAddedFactory;
static NetworkMessageFactory<NetMessageReadyToLaunchRequest> sReadyToLaunchRequestFactory;
static NetworkMessageFactory<NetMessageReadyToLaunchConfirm> sReadyToLaunchConfirmFactory;
static NetworkMessageFactory<NetMessageClientConfirmedJoin> sClientConfirmedJoinFactory;

void RegisterLANMessages()
{
    gNetworkMessageRegistry->RegisterFactory(2, &sFindGameFactory);
    gNetworkMessageRegistry->RegisterFactory(3, &sFoundGameFactory);
    gNetworkMessageRegistry->RegisterFactory(4, &sJoinRequestFactory);
    gNetworkMessageRegistry->RegisterFactory(5, &sJoinResponseFactory);
    gNetworkMessageRegistry->RegisterFactory(6, &sFactoryType6);
    gNetworkMessageRegistry->RegisterFactory(7, &sGamePeerAddedFactory);
    gNetworkMessageRegistry->RegisterFactory(10, &sReadyToLaunchRequestFactory);
    gNetworkMessageRegistry->RegisterFactory(11, &sReadyToLaunchConfirmFactory);
    gNetworkMessageRegistry->RegisterFactory(12, &sClientConfirmedJoinFactory);
}
