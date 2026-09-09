#ifndef GAME_MESSAGE_REGISTRY_H
#define GAME_MESSAGE_REGISTRY_H

#include "Game/NetworkMessage.h"

class MessageRegistry
{
public:
    MessageRegistry()
    {
        memset(mFactories, 0, sizeof(mFactories));
        memset(mReceivers, 0, sizeof(mReceivers));
    }

    int Serialize(NetworkMessage* message, u8* buffer,
        unsigned long size);
    void Dispatch(int source, u8* buffer, unsigned long size);
    void RegisterReceiver(u8 type, NetworkMessageReceiver* receiver);
    void UnregisterReceiver(u8 type);
    void RegisterFactory(u8 type, NetworkMessageFactoryBase* factory);

    /* 0x000 */ NetworkMessageFactoryBase* mFactories[256];
    /* 0x400 */ NetworkMessageReceiver* mReceivers[256];
}; // size: 0x800

extern MessageRegistry* gNetworkMessageRegistry;
void InitializeNetworkMessageRegistry();

#endif // GAME_MESSAGE_REGISTRY_H
