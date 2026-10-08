#include "Game/NetworkMessages.h"

void NetMessagePauseRequest::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mPaused, sizeof(mPaused));
}

void NetMessagePauseResponse::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineMask, sizeof(mMachineMask));
}

int NetMessagePauseResponse::GetType()
{
    return NETMSG_PAUSE_RESPONSE;
}

int NetMessagePauseRequest::GetType()
{
    return NETMSG_PAUSE_REQUEST;
}

NetMessagePauseRequest::~NetMessagePauseRequest()
{
}
