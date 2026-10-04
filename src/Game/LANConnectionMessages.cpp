#include "Game/LANMessages.h"

static inline void SerializeLANPeerMessageInfo(
    NetworkMessageSerializer* serializer, LANPeerMessageInfo& info)
{
    serializer->Transfer(info.mAddress, sizeof(info.mAddress));
    serializer->Transfer(&info.mPort, sizeof(info.mPort));
    serializer->Transfer(info.mName, sizeof(info.mName));
    serializer->Transfer(&info.mPeerIndex, sizeof(info.mPeerIndex));
    serializer->Transfer(&info.mUserMatchDataSize, sizeof(info.mUserMatchDataSize));
    serializer->Transfer(info.mUserMatchData, info.mUserMatchDataSize);
}

NetMessageJoinRequest::NetMessageJoinRequest()
    : mUserMatchDataSize(0)
{
}

void NetMessageJoinRequest::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mAddress, sizeof(mAddress));
    serializer->Transfer(&mPort, sizeof(mPort));
    serializer->Transfer(mName, sizeof(mName));
    serializer->Transfer(&mUserMatchDataSize, sizeof(mUserMatchDataSize));
    serializer->Transfer(mUserMatchData, mUserMatchDataSize);
}

NetMessageJoinResponse::NetMessageJoinResponse()
    : mUserMatchDataSize(0)
{
}

void NetMessageJoinResponse::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mAddress, sizeof(mAddress));
    serializer->Transfer(&mPort, sizeof(mPort));
    serializer->Transfer(&mAccepted, sizeof(mAccepted));
    serializer->Transfer(mName, sizeof(mName));
    serializer->Transfer(&mUserMatchDataSize, sizeof(mUserMatchDataSize));
    serializer->Transfer(mUserMatchData, mUserMatchDataSize);
    serializer->Transfer(&mPeerCount, sizeof(mPeerCount));
    for (u8 index = 0; index < mPeerCount; ++index)
    {
        SerializeLANPeerMessageInfo(
            serializer, mPeers[index]);
    }
}

NetMessageGamePeerAdded::NetMessageGamePeerAdded()
{
}

void NetMessageGamePeerAdded::Serialize(NetworkMessageSerializer* serializer)
{
    SerializeLANPeerMessageInfo(serializer, mPeer);
}

void NetMessageTransportType6::Serialize(NetworkMessageSerializer* serializer)
{
}

void NetMessageReadyToLaunchRequest::Serialize(NetworkMessageSerializer* serializer)
{
}

void NetMessageReadyToLaunchConfirm::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mConfirmed, sizeof(mConfirmed));
}

void NetMessageClientConfirmedJoin::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
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
