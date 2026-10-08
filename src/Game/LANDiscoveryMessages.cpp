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
    return NETMSG_FOUND_GAME;
}

int NetMessageFindGame::GetType()
{
    return NETMSG_FIND_GAME;
}
