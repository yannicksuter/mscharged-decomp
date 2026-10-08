#ifndef GAME_NETWORK_MESSAGE_H
#define GAME_NETWORK_MESSAGE_H

#include "Game/NetworkMessageSerializer.h"

enum eNetworkMessageType
{
    NETMSG_INPUT = 0,
    NETMSG_INPUT_BUNDLE = 1,
    NETMSG_FIND_GAME = 2,
    NETMSG_FOUND_GAME = 3,
    NETMSG_JOIN_REQUEST = 4,
    NETMSG_JOIN_RESPONSE = 5,
    NETMSG_GAME_PEER_ADDED = 7,
    NETMSG_ALL_INPUTS = 8,
    NETMSG_ALL_INPUTS_BUNDLE = 9,
    NETMSG_READY_TO_LAUNCH_REQUEST = 10,
    NETMSG_READY_TO_LAUNCH_CONFIRM = 11,
    NETMSG_CLIENT_CONFIRMED_JOIN = 12,
    NETMSG_GAME_START = 13,
    NETMSG_LOADED_GAME = 15,
    NETMSG_LOADED_GAME_CLIENT = 18,
    NETMSG_LOADED_GAME_EVERYONE = 19,
    NETMSG_TOURNAMENT_START = 20,
    NETMSG_DRAFT = 21,
    NETMSG_DRAFT_MACHINE_INFO = 22,
    NETMSG_DRAFT_PICKED_CAPTAIN = 23,
    NETMSG_DRAFT_PICKED_SIDEKICKS = 24,
    NETMSG_SIDES_CHANGED = 25,
    NETMSG_CHECK_CONNECTION = 26,
    NETMSG_CONNECTION_DECISION = 27,
    NETMSG_PAUSE_REQUEST = 28,
    NETMSG_PAUSE_RESPONSE = 29,
    NETMSG_SKIP_NIS = 30,
    NETMSG_SKIP_NIS_CLIENT = 31,
    NETMSG_TOURNAMENT_GAME_UPDATE = 32,
    NETMSG_TOURNAMENT_LOADING_STATE = 33,
    NETMSG_MEGA_BALL_POINTER = 34,
    NETMSG_MEGA_STRIKE_METER = 35,
};

class NetworkMessage
{
public:
    void* operator new(unsigned long size)
    {
        return operator new(size, 8, false);
    }
    void* operator new(unsigned long size, unsigned int alignment, bool zeroMemory);
    void operator delete(void* p);

    NetworkMessage()
        : mSource(0)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer) = 0;
    virtual ~NetworkMessage() { }
    virtual int GetType() = 0;

    /* 0x04 */ u32 mSource;
};

class NetworkMessageFactoryBase
{
public:
    virtual NetworkMessage* Create(
        NetworkMessageSerializer* serializer) = 0;
};

template <class T>
class NetworkMessageFactory : public NetworkMessageFactoryBase
{
public:
    virtual NetworkMessage* Create(
        NetworkMessageSerializer* serializer)
    {
        T* message = new (8, false) T;
        message->Serialize(serializer);
        return message;
    }
};

class NetworkMessageReceiver
{
public:
    virtual int ProcessMessage(NetworkMessage* message) = 0;
};

#endif // GAME_NETWORK_MESSAGE_H
