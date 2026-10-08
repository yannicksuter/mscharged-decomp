#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkMessages.h"

void NetworkMessageType17::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mPayload, sizeof(mPayload));
}

void NetworkMessageType16::Serialize(NetworkMessageSerializer*)
{
}

NetworkMessageType17::~NetworkMessageType17()
{
}

NetworkMessageType16::~NetworkMessageType16()
{
}

int NetworkMessageType16::GetType()
{
    return 16;
}

int NetworkMessageType17::GetType()
{
    return 17;
}

static NetworkMessageFactory<NetMessageGameStart> sGameStartFactory;
static NetworkMessageFactory<NetMessageLoadedGame> sLoadedGameFactory;
static NetworkMessageFactory<NetworkMessageType16> sFactoryType16;
static NetworkMessageFactory<NetworkMessageType17> sFactoryType17;
static NetworkMessageFactory<NetMessageLoadedGameClient> sLoadedGameClientFactory;
static NetworkMessageFactory<NetMessageLoadedGameEveryone> sLoadedGameEveryoneFactory;
static NetworkMessageFactory<NetMessageTournamentStart> sTournamentStartFactory;
static NetworkMessageFactory<NetMessagePauseRequest> sPauseRequestFactory;
static NetworkMessageFactory<NetMessagePauseResponse> sPauseResponseFactory;
static NetworkMessageFactory<NetMessageSkipNis> sSkipNisFactory;
static NetworkMessageFactory<NetMessageSkipNisClient> sSkipNisClientFactory;
static NetworkMessageFactory<NetMessageTournamentGameUpdate> sTournamentGameUpdateFactory;
static NetworkMessageFactory<NetMessageTournamentLoadingState> sTournamentLoadingStateFactory;
static NetworkMessageFactory<NetMessageDraft> sDraftFactory;
static NetworkMessageFactory<NetMessageDraftMachineInfo> sDraftMachineInfoFactory;
static NetworkMessageFactory<NetMessageDraftPickedCaptain> sDraftPickedCaptainFactory;
static NetworkMessageFactory<NetMessageDraftPickedSidekicks> sDraftPickedSidekicksFactory;
static NetworkMessageFactory<NetMessageSidesChanged> sSidesChangedFactory;
static NetworkMessageFactory<NetMessageCheckConnection> sCheckConnectionFactory;
static NetworkMessageFactory<NetMessageConnectionDecision> sConnectionDecisionFactory;
static NetworkMessageFactory<NetMessageMegaBallPointer> sMegaBallPointerFactory;
static NetworkMessageFactory<NetMessageMegaStrikeMeter>
    sMegaStrikeMeterFactory;

void RegisterNetworkMessageFactories()
{
    gNetworkMessageRegistry->RegisterFactory(NETMSG_GAME_START, &sGameStartFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_LOADED_GAME, &sLoadedGameFactory);
    gNetworkMessageRegistry->RegisterFactory(16, &sFactoryType16);
    gNetworkMessageRegistry->RegisterFactory(17, &sFactoryType17);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_LOADED_GAME_CLIENT, &sLoadedGameClientFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_LOADED_GAME_EVERYONE, &sLoadedGameEveryoneFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_TOURNAMENT_START, &sTournamentStartFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_DRAFT, &sDraftFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_DRAFT_MACHINE_INFO, &sDraftMachineInfoFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_DRAFT_PICKED_CAPTAIN, &sDraftPickedCaptainFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_DRAFT_PICKED_SIDEKICKS, &sDraftPickedSidekicksFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_SIDES_CHANGED, &sSidesChangedFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_CHECK_CONNECTION, &sCheckConnectionFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_CONNECTION_DECISION, &sConnectionDecisionFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_PAUSE_REQUEST, &sPauseRequestFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_PAUSE_RESPONSE, &sPauseResponseFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_SKIP_NIS, &sSkipNisFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_SKIP_NIS_CLIENT, &sSkipNisClientFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_TOURNAMENT_GAME_UPDATE, &sTournamentGameUpdateFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_TOURNAMENT_LOADING_STATE, &sTournamentLoadingStateFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_MEGA_BALL_POINTER, &sMegaBallPointerFactory);
    gNetworkMessageRegistry->RegisterFactory(NETMSG_MEGA_STRIKE_METER, &sMegaStrikeMeterFactory);
}
