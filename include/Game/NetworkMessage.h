#ifndef GAME_NETWORK_MESSAGE_H
#define GAME_NETWORK_MESSAGE_H

#include "Game/NetworkMessageSerializer.h"

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

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetworkMessage() { }
    virtual int GetType();

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
        T* message = new T;
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
