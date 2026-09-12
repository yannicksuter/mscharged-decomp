#include "Game/NetworkMessages.h"

void NetworkMessageType30::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

void NetworkMessageType31::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}
