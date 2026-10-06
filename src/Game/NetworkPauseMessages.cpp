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
    return 29;
}

int NetMessagePauseRequest::GetType()
{
    return 28;
}

NetMessagePauseRequest::~NetMessagePauseRequest()
{
}
