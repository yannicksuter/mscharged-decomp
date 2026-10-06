#include "Game/NetworkMessages.h"

void NetMessageSkipNis::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mByPassNumber, sizeof(mByPassNumber));
}

void NetMessageSkipNisClient::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mByPassNumber, sizeof(mByPassNumber));
}
