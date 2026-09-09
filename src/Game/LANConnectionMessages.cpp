#include "Game/LANMessages.h"

NetMessageJoinRequest::NetMessageJoinRequest()
    : mUnidentified19(0)
{
}

void NetMessageJoinRequest::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified0C, sizeof(mUnidentified0C));
    serializer->Transfer(mUnidentified0E, sizeof(mUnidentified0E));
    serializer->Transfer(&mUnidentified19, sizeof(mUnidentified19));
    serializer->Transfer(mUnidentified1A, mUnidentified19);
}

NetMessageJoinResponse::NetMessageJoinResponse()
    : mUnidentified1A(0)
{
}

void NetMessageJoinResponse::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified0C, sizeof(mUnidentified0C));
    serializer->Transfer(&mUnidentified0E, sizeof(mUnidentified0E));
    serializer->Transfer(mUnidentified0F, sizeof(mUnidentified0F));
    serializer->Transfer(&mUnidentified1A, sizeof(mUnidentified1A));
    serializer->Transfer(mUnidentified1B, mUnidentified1A);
    serializer->Transfer(&mUnidentified23, sizeof(mUnidentified23));
    for (u8 index = 0; index < mUnidentified23; ++index)
    {
        LANPeerMessageInfo& info = mUnidentified24[index];
        serializer->Transfer(info.mUnidentified00, sizeof(info.mUnidentified00));
        serializer->Transfer(&info.mUnidentified04, sizeof(info.mUnidentified04));
        serializer->Transfer(info.mUnidentified06, sizeof(info.mUnidentified06));
        serializer->Transfer(&info.mUnidentified11, sizeof(info.mUnidentified11));
        serializer->Transfer(&info.mUnidentified12, sizeof(info.mUnidentified12));
        serializer->Transfer(info.mUnidentified13, info.mUnidentified12);
    }
}

NetMessageGamePeerAdded::NetMessageGamePeerAdded()
{
}

void NetMessageGamePeerAdded::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08.mUnidentified00, sizeof(mUnidentified08.mUnidentified00));
    serializer->Transfer(&mUnidentified08.mUnidentified04, sizeof(mUnidentified08.mUnidentified04));
    serializer->Transfer(mUnidentified08.mUnidentified06, sizeof(mUnidentified08.mUnidentified06));
    serializer->Transfer(&mUnidentified08.mUnidentified11, sizeof(mUnidentified08.mUnidentified11));
    serializer->Transfer(&mUnidentified08.mUnidentified12, sizeof(mUnidentified08.mUnidentified12));
    serializer->Transfer(mUnidentified08.mUnidentified13, mUnidentified08.mUnidentified12);
}

void NetMessageTransportType6::Serialize(NetworkMessageSerializer* serializer)
{
}

void NetMessageReadyToLaunchRequest::Serialize(NetworkMessageSerializer* serializer)
{
}

void NetMessageReadyToLaunchConfirm::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

void NetMessageClientConfirmedJoin::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

NetMessageTransportType6::~NetMessageTransportType6()
{
}

int NetMessageClientConfirmedJoin::GetType()
{
    return 12;
}

int NetMessageReadyToLaunchConfirm::GetType()
{
    return 11;
}

int NetMessageReadyToLaunchRequest::GetType()
{
    return 10;
}

int NetMessageTransportType6::GetType()
{
    return 6;
}

int NetMessageGamePeerAdded::GetType()
{
    return 7;
}

int NetMessageJoinResponse::GetType()
{
    return 5;
}

int NetMessageJoinRequest::GetType()
{
    return 4;
}
