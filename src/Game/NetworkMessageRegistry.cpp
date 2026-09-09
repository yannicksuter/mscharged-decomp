#include "Game/NetworkMessageRegistry.h"
#include "Game/Sys/debug.h"
#include "NL/nlMemory.h"


MessageRegistry* gNetworkMessageRegistry;

void InitializeNetworkMessageRegistry()
{
    gNetworkMessageRegistry = new (8, false) MessageRegistry;
}

int MessageRegistry::Serialize(
    NetworkMessage* message, u8* buffer, unsigned long size)
{
    NetworkMessageSerializer serializer(1, buffer, size);
    u8 type = message->GetType();
    memcpy(serializer.mPosition, &type, sizeof(type));
    serializer.mPosition += sizeof(type);
    message->Serialize(&serializer);
    return serializer.GetLength();
}

void MessageRegistry::Dispatch(
    int source, u8* buffer, unsigned long size)
{
    NetworkMessageSerializer serializer(0, buffer, size);
    u8 type = 0;
    memcpy(&type, serializer.mPosition, sizeof(type));
    serializer.mPosition += sizeof(type);

    NetworkMessageFactoryBase* factory = mFactories[type];
    if (factory == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "WARNING NO MESSAGE <FACTORY> FOR TYPE %d..DISCARDING MESSAGE\n",
            type);
        return;
    }

    NetworkMessage* message = factory->Create(&serializer);
    message->mSource = source;
    NetworkMessageReceiver* receiver = mReceivers[type];
    if (receiver != 0)
    {
        if (receiver->ProcessMessage(message))
        {
            delete message;
        }
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "WARNING NO MESSAGE HANDLER FOR TYPE %d..DISCARDING MESSAGE\n",
            type);
        delete message;
    }
}

void MessageRegistry::RegisterReceiver(
    u8 type, NetworkMessageReceiver* receiver)
{
    mReceivers[type] = receiver;
}

void MessageRegistry::UnregisterReceiver(u8 type)
{
    mReceivers[type] = 0;
}

void MessageRegistry::RegisterFactory(
    u8 type, NetworkMessageFactoryBase* factory)
{
    mFactories[type] = factory;
}
