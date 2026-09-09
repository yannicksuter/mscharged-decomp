#include "Game/LANMessages.h"

NetMessageFindGame::NetMessageFindGame()
{
}

void NetMessageFindGame::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08, sizeof(mUnidentified08));
}

NetMessageFoundGame::NetMessageFoundGame()
{
}

void NetMessageFoundGame::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified10, sizeof(mUnidentified10));
    serializer->Transfer(mUnidentified14, sizeof(mUnidentified14));
    serializer->Transfer(&mUnidentified18, sizeof(mUnidentified18));
    serializer->Transfer(mUnidentified1A, 11);
}

int NetMessageFoundGame::GetType()
{
    return 3;
}

int NetMessageFindGame::GetType()
{
    return 2;
}
