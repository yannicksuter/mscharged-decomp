#ifndef GAME_NETWORK_LOADED_GAME_MESSAGES_H
#define GAME_NETWORK_LOADED_GAME_MESSAGES_H

#include "Game/NetworkMessage.h"

// Payload-less notifications that coordinate game loading.
class NetMessageLoadedGame : public NetworkMessage
{
public:
    virtual int GetType() { return 0xF; }
    virtual void Serialize(NetworkMessageSerializer*) { }
};

class NetMessageLoadedGameClient : public NetworkMessage
{
public:
    virtual int GetType() { return 0x12; }
    virtual void Serialize(NetworkMessageSerializer*) { }
};

class NetMessageLoadedGameEveryone : public NetworkMessage
{
public:
    virtual int GetType() { return 0x13; }
    virtual void Serialize(NetworkMessageSerializer*) { }
};

#endif // GAME_NETWORK_LOADED_GAME_MESSAGES_H
