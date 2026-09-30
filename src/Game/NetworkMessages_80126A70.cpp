#include "Game/NetworkMessages.h"

void NetworkMessageType30::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mByPassNumber, sizeof(mByPassNumber));
}

void NetworkMessageType31::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mByPassNumber, sizeof(mByPassNumber));
}
