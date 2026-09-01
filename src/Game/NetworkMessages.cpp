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

class UnidentifiedNetworkMessageFactory
{
public:
    virtual UnidentifiedNetworkMessage* Create(
        UnidentifiedMessageSerializer* serializer) = 0;
};

template <class T>
class NetworkMessageFactory : public UnidentifiedNetworkMessageFactory
{
public:
    virtual UnidentifiedNetworkMessage* Create(
        UnidentifiedMessageSerializer* serializer)
    {
        T* message = new T;
        message->Serialize(serializer);
        return message;
    }
};

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
static NetworkMessageFactory<NetworkMessageType25_8050B778> sFactoryType25;
static NetworkMessageFactory<NetMessageCheckConnection> sFactoryType26;
static NetworkMessageFactory<NetworkMessageType27_8050B750> sFactoryType27;
static NetworkMessageFactory<NetworkMessageType34_8050ADCC> sFactoryType34;
static NetworkMessageFactory<UnidentifiedNetworkMessage_80126D84>
    sFactoryType35;

extern void* lbl_806E2100;
extern "C" void fn_8032CA40(
    void* codec, int type, UnidentifiedNetworkMessageFactory* factory);

void RegisterNetworkMessages_801258A8()
{
    fn_8032CA40(lbl_806E2100, 13, &sFactoryType13);
    fn_8032CA40(lbl_806E2100, 15, &sFactoryType15);
    fn_8032CA40(lbl_806E2100, 16, &sFactoryType16);
    fn_8032CA40(lbl_806E2100, 17, &sFactoryType17);
    fn_8032CA40(lbl_806E2100, 18, &sFactoryType18);
    fn_8032CA40(lbl_806E2100, 19, &sFactoryType19);
    fn_8032CA40(lbl_806E2100, 20, &sFactoryType20);
    fn_8032CA40(lbl_806E2100, 21, &sFactoryType21);
    fn_8032CA40(lbl_806E2100, 22, &sFactoryType22);
    fn_8032CA40(lbl_806E2100, 23, &sFactoryType23);
    fn_8032CA40(lbl_806E2100, 24, &sFactoryType24);
    fn_8032CA40(lbl_806E2100, 25, &sFactoryType25);
    fn_8032CA40(lbl_806E2100, 26, &sFactoryType26);
    fn_8032CA40(lbl_806E2100, 27, &sFactoryType27);
    fn_8032CA40(lbl_806E2100, 28, &sFactoryType28);
    fn_8032CA40(lbl_806E2100, 29, &sFactoryType29);
    fn_8032CA40(lbl_806E2100, 30, &sFactoryType30);
    fn_8032CA40(lbl_806E2100, 31, &sFactoryType31);
    fn_8032CA40(lbl_806E2100, 32, &sFactoryType32);
    fn_8032CA40(lbl_806E2100, 33, &sFactoryType33);
    fn_8032CA40(lbl_806E2100, 34, &sFactoryType34);
    fn_8032CA40(lbl_806E2100, 35, &sFactoryType35);
}
