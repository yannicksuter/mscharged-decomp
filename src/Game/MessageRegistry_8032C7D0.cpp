#include "Game/NetworkMessages.h"
#include "Game/Sys/debug.h"
#include "NL/nlMemory.h"


UnidentifiedMessageRegistry_8032C7D0* lbl_806E2100;

void fn_8032C7D0()
{
    lbl_806E2100 = new (8, false) UnidentifiedMessageRegistry_8032C7D0;
}

int UnidentifiedMessageRegistry_8032C7D0::fn_8032C830(
    UnidentifiedNetworkMessage* message, u8* buffer, unsigned long size)
{
    UnidentifiedMessageSerializer serializer(1, buffer, size);
    u8 type = message->GetType();
    memcpy(serializer.mPosition, &type, sizeof(type));
    serializer.mPosition += sizeof(type);
    message->Serialize(&serializer);
    return serializer.UnidentifiedGetLength();
}

void UnidentifiedMessageRegistry_8032C7D0::fn_8032C8CC(
    int source, u8* buffer, unsigned long size)
{
    UnidentifiedMessageSerializer serializer(0, buffer, size);
    u8 type = 0;
    memcpy(&type, serializer.mPosition, sizeof(type));
    serializer.mPosition += sizeof(type);

    UnidentifiedNetworkMessageFactory* factory = mUnidentified000[type];
    if (factory == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "WARNING NO MESSAGE <FACTORY> FOR TYPE %d..DISCARDING MESSAGE\n",
            type);
        return;
    }

    UnidentifiedNetworkMessage* message = factory->Create(&serializer);
    message->mUnidentified04 = source;
    UnidentifiedNetworkMessageReceiver* receiver = mUnidentified400[type];
    if (receiver != 0)
    {
        if (receiver->ReceiverVirtual00(message))
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

void UnidentifiedMessageRegistry_8032C7D0::fn_8032CA1C(
    u8 type, UnidentifiedNetworkMessageReceiver* receiver)
{
    mUnidentified400[type] = receiver;
}

void UnidentifiedMessageRegistry_8032C7D0::fn_8032CA2C(u8 type)
{
    mUnidentified400[type] = 0;
}

void UnidentifiedMessageRegistry_8032C7D0::fn_8032CA40(
    u8 type, UnidentifiedNetworkMessageFactory* factory)
{
    mUnidentified000[type] = factory;
}
