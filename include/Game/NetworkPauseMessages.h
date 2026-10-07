#ifndef GAME_NETWORK_PAUSE_MESSAGES_H
#define GAME_NETWORK_PAUSE_MESSAGES_H

#include "Game/NetworkMessage.h"

class NetMessagePauseRequest : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessagePauseRequest();
    virtual int GetType();

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ u8 mPaused;
};

// "HOST sending Pause Response to all clients and myself".
class NetMessagePauseResponse : public NetworkMessage
{
public:
    NetMessagePauseResponse() { }
    NetMessagePauseResponse(u8 machineMask)
        : mMachineMask(machineMask)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessagePauseResponse() { }
    virtual int GetType();

    /* 0x08 */ u8 mMachineMask;
};

#endif // GAME_NETWORK_PAUSE_MESSAGES_H
