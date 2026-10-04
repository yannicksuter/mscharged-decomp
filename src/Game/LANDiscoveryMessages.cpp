#include "Game/LANMessages.h"

NetMessageFindGame::NetMessageFindGame()
{
}

void NetMessageFindGame::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mToken, sizeof(mToken));
}

NetMessageFoundGame::NetMessageFoundGame()
{
}

void NetMessageFoundGame::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mToken, sizeof(mToken));
    serializer->Transfer(&mGameType, sizeof(mGameType));
    serializer->Transfer(mAddress, sizeof(mAddress));
    serializer->Transfer(&mPort, sizeof(mPort));
    serializer->Transfer(mHostName, 11);
}

int NetMessageFoundGame::GetType()
{
    return 3;
}

int NetMessageFindGame::GetType()
{
    return 2;
}
