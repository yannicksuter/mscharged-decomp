#include "Game/NetworkMessages.h"

void NetMessageGameStart::Serialize(UnidentifiedMessageSerializer* serializer)
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

void NetworkMessageType17_8050AC4C::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

void NetworkMessageType16_8050AC38::Serialize(UnidentifiedMessageSerializer*)
{
}

int NetworkMessageType16_8050AC38::GetType()
{
    return 16;
}

int NetworkMessageType17_8050AC4C::GetType()
{
    return 17;
}

static NetworkMessageFactory<NetMessageGameStart> sFactoryType13;
static NetworkMessageFactory<NetMessageLoadedGame> sFactoryType15;
static NetworkMessageFactory<NetworkMessageType16_8050AC38> sFactoryType16;
static NetworkMessageFactory<NetworkMessageType17_8050AC4C> sFactoryType17;
static NetworkMessageFactory<NetMessageLoadedGameClient> sFactoryType18;
static NetworkMessageFactory<NetMessageLoadedGameEveryone> sFactoryType19;
static NetworkMessageFactory<NetMessageTournamentStart> sFactoryType20;
static NetworkMessageFactory<NetMessagePauseRequest_8050AD7C> sFactoryType28;
static NetworkMessageFactory<NetMessagePauseResponse_8050AD68> sFactoryType29;
static NetworkMessageFactory<NetworkMessageType30_8050ADA4> sFactoryType30;
static NetworkMessageFactory<NetworkMessageType31_8050AD90> sFactoryType31;
static NetworkMessageFactory<NetMessageTournamentGameUpdate> sFactoryType32;
static NetworkMessageFactory<NetMessageTournamentLoadingState> sFactoryType33;
static NetworkMessageFactory<NetMessageDraft> sFactoryType21;
static NetworkMessageFactory<NetworkMessageType22_8050B7B4> sFactoryType22;
static NetworkMessageFactory<NetMessageDraftPickedCaptain> sFactoryType23;
static NetworkMessageFactory<NetMessageDraftPickedSidekicks> sFactoryType24;
static NetworkMessageFactory<NetMessageSidesChanged> sSidesChangedFactory;
static NetworkMessageFactory<NetMessageCheckConnection> sFactoryType26;
static NetworkMessageFactory<NetworkMessageType27_8050B750> sFactoryType27;
static NetworkMessageFactory<NetworkMessageType34_8050ADCC> sFactoryType34;
static NetworkMessageFactory<UnidentifiedNetworkMessage_80126D84>
    sFactoryType35;

void RegisterNetworkMessages_801258A8()
{
    lbl_806E2100->fn_8032CA40(13, &sFactoryType13);
    lbl_806E2100->fn_8032CA40(15, &sFactoryType15);
    lbl_806E2100->fn_8032CA40(16, &sFactoryType16);
    lbl_806E2100->fn_8032CA40(17, &sFactoryType17);
    lbl_806E2100->fn_8032CA40(18, &sFactoryType18);
    lbl_806E2100->fn_8032CA40(19, &sFactoryType19);
    lbl_806E2100->fn_8032CA40(20, &sFactoryType20);
    lbl_806E2100->fn_8032CA40(21, &sFactoryType21);
    lbl_806E2100->fn_8032CA40(22, &sFactoryType22);
    lbl_806E2100->fn_8032CA40(23, &sFactoryType23);
    lbl_806E2100->fn_8032CA40(24, &sFactoryType24);
    lbl_806E2100->fn_8032CA40(25, &sSidesChangedFactory);
    lbl_806E2100->fn_8032CA40(26, &sFactoryType26);
    lbl_806E2100->fn_8032CA40(27, &sFactoryType27);
    lbl_806E2100->fn_8032CA40(28, &sFactoryType28);
    lbl_806E2100->fn_8032CA40(29, &sFactoryType29);
    lbl_806E2100->fn_8032CA40(30, &sFactoryType30);
    lbl_806E2100->fn_8032CA40(31, &sFactoryType31);
    lbl_806E2100->fn_8032CA40(32, &sFactoryType32);
    lbl_806E2100->fn_8032CA40(33, &sFactoryType33);
    lbl_806E2100->fn_8032CA40(34, &sFactoryType34);
    lbl_806E2100->fn_8032CA40(35, &sFactoryType35);
}
