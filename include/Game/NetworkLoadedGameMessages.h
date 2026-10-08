#ifndef GAME_NETWORK_LOADED_GAME_MESSAGES_H
#define GAME_NETWORK_LOADED_GAME_MESSAGES_H

#include "Game/NetworkMessage.h"

// Payload-less notifications that coordinate game loading.
class NetMessageLoadedGame : public NetworkMessage
{
public:
    virtual int GetType() { return NETMSG_LOADED_GAME; }
    virtual void Serialize(NetworkMessageSerializer*) { }
};

class NetMessageLoadedGameClient : public NetworkMessage
{
public:
    virtual int GetType() { return NETMSG_LOADED_GAME_CLIENT; }
    virtual void Serialize(NetworkMessageSerializer*) { }
};

class NetMessageLoadedGameEveryone : public NetworkMessage
{
public:
    virtual int GetType() { return NETMSG_LOADED_GAME_EVERYONE; }
    virtual void Serialize(NetworkMessageSerializer*) { }
};

#endif // GAME_NETWORK_LOADED_GAME_MESSAGES_H
