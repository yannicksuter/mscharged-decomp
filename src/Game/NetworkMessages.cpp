#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkMessages.h"

void NetworkMessageType17::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
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
    gNetworkMessageRegistry->RegisterFactory(13, &sGameStartFactory);
    gNetworkMessageRegistry->RegisterFactory(15, &sLoadedGameFactory);
    gNetworkMessageRegistry->RegisterFactory(16, &sFactoryType16);
    gNetworkMessageRegistry->RegisterFactory(17, &sFactoryType17);
    gNetworkMessageRegistry->RegisterFactory(18, &sLoadedGameClientFactory);
    gNetworkMessageRegistry->RegisterFactory(19, &sLoadedGameEveryoneFactory);
    gNetworkMessageRegistry->RegisterFactory(20, &sTournamentStartFactory);
    gNetworkMessageRegistry->RegisterFactory(21, &sDraftFactory);
    gNetworkMessageRegistry->RegisterFactory(22, &sDraftMachineInfoFactory);
    gNetworkMessageRegistry->RegisterFactory(23, &sDraftPickedCaptainFactory);
    gNetworkMessageRegistry->RegisterFactory(24, &sDraftPickedSidekicksFactory);
    gNetworkMessageRegistry->RegisterFactory(25, &sSidesChangedFactory);
    gNetworkMessageRegistry->RegisterFactory(26, &sCheckConnectionFactory);
    gNetworkMessageRegistry->RegisterFactory(27, &sConnectionDecisionFactory);
    gNetworkMessageRegistry->RegisterFactory(28, &sPauseRequestFactory);
    gNetworkMessageRegistry->RegisterFactory(29, &sPauseResponseFactory);
    gNetworkMessageRegistry->RegisterFactory(30, &sSkipNisFactory);
    gNetworkMessageRegistry->RegisterFactory(31, &sSkipNisClientFactory);
    gNetworkMessageRegistry->RegisterFactory(32, &sTournamentGameUpdateFactory);
    gNetworkMessageRegistry->RegisterFactory(33, &sTournamentLoadingStateFactory);
    gNetworkMessageRegistry->RegisterFactory(34, &sMegaBallPointerFactory);
    gNetworkMessageRegistry->RegisterFactory(35, &sMegaStrikeMeterFactory);
}
