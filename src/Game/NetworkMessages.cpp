#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkMessages.h"

void NetMessageGameStart::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mRandomSeed, sizeof(mRandomSeed));
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mMachineCount, sizeof(mMachineCount));
    serializer->Transfer(&mHomeCharacters[0], sizeof(mHomeCharacters[0]));
    serializer->Transfer(&mHomeCharacters[1], sizeof(mHomeCharacters[1]));
    serializer->Transfer(&mHomeCharacters[2], sizeof(mHomeCharacters[2]));
    serializer->Transfer(&mHomeCharacters[3], sizeof(mHomeCharacters[3]));
    serializer->Transfer(&mAwayCharacters[0], sizeof(mAwayCharacters[0]));
    serializer->Transfer(&mAwayCharacters[1], sizeof(mAwayCharacters[1]));
    serializer->Transfer(&mAwayCharacters[2], sizeof(mAwayCharacters[2]));
    serializer->Transfer(&mAwayCharacters[3], sizeof(mAwayCharacters[3]));
    serializer->Transfer(&mStadium, sizeof(mStadium));
    serializer->Transfer(mMachineFlags, sizeof(mMachineFlags));
    serializer->Transfer(&mUnidentified1B, sizeof(mUnidentified1B));
    serializer->Transfer(&mUnidentified1C[0], sizeof(mUnidentified1C[0]));
    serializer->Transfer(&mUnidentified1C[1], sizeof(mUnidentified1C) - 1);
    serializer->Transfer(&mUnidentified20, sizeof(mUnidentified20));
    serializer->Transfer(&mUnidentified24, sizeof(mUnidentified24));
}

int NetMessageGameStart::GetType()
{
    return 13;
}

void NetworkMessageType17::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

void NetworkMessageType16::Serialize(NetworkMessageSerializer*)
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

static NetworkMessageFactory<NetMessageGameStart> sFactoryType13;
static NetworkMessageFactory<NetMessageLoadedGame> sFactoryType15;
static NetworkMessageFactory<NetworkMessageType16> sFactoryType16;
static NetworkMessageFactory<NetworkMessageType17> sFactoryType17;
static NetworkMessageFactory<NetMessageLoadedGameClient> sFactoryType18;
static NetworkMessageFactory<NetMessageLoadedGameEveryone> sFactoryType19;
static NetworkMessageFactory<NetMessageTournamentStart> sFactoryType20;
static NetworkMessageFactory<NetMessagePauseRequest> sFactoryType28;
static NetworkMessageFactory<NetMessagePauseResponse> sFactoryType29;
static NetworkMessageFactory<NetworkMessageType30> sFactoryType30;
static NetworkMessageFactory<NetworkMessageType31> sFactoryType31;
static NetworkMessageFactory<NetMessageTournamentGameUpdate> sFactoryType32;
static NetworkMessageFactory<NetMessageTournamentLoadingState> sFactoryType33;
static NetworkMessageFactory<NetMessageDraft> sFactoryType21;
static NetworkMessageFactory<NetMessageDraftMachineInfo> sFactoryType22;
static NetworkMessageFactory<NetMessageDraftPickedCaptain> sFactoryType23;
static NetworkMessageFactory<NetMessageDraftPickedSidekicks> sFactoryType24;
static NetworkMessageFactory<NetMessageSidesChanged> sSidesChangedFactory;
static NetworkMessageFactory<NetMessageCheckConnection> sFactoryType26;
static NetworkMessageFactory<NetMessageConnectionDecision> sFactoryType27;
static NetworkMessageFactory<NetMessageMegaBallPointer> sMegaBallPointerFactory;
static NetworkMessageFactory<NetworkMessageType35>
    sFactoryType35;

void RegisterNetworkMessages_801258A8()
{
    gNetworkMessageRegistry->RegisterFactory(13, &sFactoryType13);
    gNetworkMessageRegistry->RegisterFactory(15, &sFactoryType15);
    gNetworkMessageRegistry->RegisterFactory(16, &sFactoryType16);
    gNetworkMessageRegistry->RegisterFactory(17, &sFactoryType17);
    gNetworkMessageRegistry->RegisterFactory(18, &sFactoryType18);
    gNetworkMessageRegistry->RegisterFactory(19, &sFactoryType19);
    gNetworkMessageRegistry->RegisterFactory(20, &sFactoryType20);
    gNetworkMessageRegistry->RegisterFactory(21, &sFactoryType21);
    gNetworkMessageRegistry->RegisterFactory(22, &sFactoryType22);
    gNetworkMessageRegistry->RegisterFactory(23, &sFactoryType23);
    gNetworkMessageRegistry->RegisterFactory(24, &sFactoryType24);
    gNetworkMessageRegistry->RegisterFactory(25, &sSidesChangedFactory);
    gNetworkMessageRegistry->RegisterFactory(26, &sFactoryType26);
    gNetworkMessageRegistry->RegisterFactory(27, &sFactoryType27);
    gNetworkMessageRegistry->RegisterFactory(28, &sFactoryType28);
    gNetworkMessageRegistry->RegisterFactory(29, &sFactoryType29);
    gNetworkMessageRegistry->RegisterFactory(30, &sFactoryType30);
    gNetworkMessageRegistry->RegisterFactory(31, &sFactoryType31);
    gNetworkMessageRegistry->RegisterFactory(32, &sFactoryType32);
    gNetworkMessageRegistry->RegisterFactory(33, &sFactoryType33);
    gNetworkMessageRegistry->RegisterFactory(34, &sMegaBallPointerFactory);
    gNetworkMessageRegistry->RegisterFactory(35, &sFactoryType35);
}
