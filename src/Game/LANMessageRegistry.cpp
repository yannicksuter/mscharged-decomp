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
    gNetworkMessageRegistry->RegisterFactory(NETMSG_FIND_GAME, &sFindGameFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_FOUND_GAME, &sFoundGameFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_JOIN_REQUEST, &sJoinRequestFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_JOIN_RESPONSE, &sJoinResponseFactory);
    gNetworkMessageRegistry->RegisterFactory(6, &sFactoryType6);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_GAME_PEER_ADDED, &sGamePeerAddedFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_READY_TO_LAUNCH_REQUEST, &sReadyToLaunchRequestFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_READY_TO_LAUNCH_CONFIRM, &sReadyToLaunchConfirmFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_CLIENT_CONFIRMED_JOIN, &sClientConfirmedJoinFactory);
}
