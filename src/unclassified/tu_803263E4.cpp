#include "NL/nlSlotPool.h"
#include "unclassified/tu_803263E4.h"

SlotPool<UnidentifiedTransportPacket_803263E4> lbl_805847F0(400, 0);

extern "C" void* fn_803263E4(unsigned long, unsigned int, bool)
{
    return lbl_805847F0.Allocate();
}

extern "C" void fn_8032644C(void* packet)
{
    lbl_805847F0.Free((UnidentifiedTransportPacket_803263E4*)packet);
}

extern "C" void fn_80326464(UnidentifiedTransportPacket_803263E4* packet,
    UnidentifiedMessageSerializer* serializer)
{
    u8 value = 0;
    if (serializer->mDirection == 1)
    {
        value = packet->mUnidentified04;
        if (packet->mSize > 0xFF)
        {
            value = 1;
        }
        memcpy(serializer->mPosition, &value, sizeof(value));
        serializer->mPosition += sizeof(value);
    }
    else
    {
        memcpy(&value, serializer->mPosition, sizeof(value));
        serializer->mPosition += sizeof(value);
        if ((packet->mUnidentified04 = value) == 1)
        {
            packet->mUnidentified04 = 0;
        }
    }

    serializer->Transfer(&packet->mUnidentified06,
        sizeof(packet->mUnidentified06));
    if (value == 1)
    {
        serializer->Transfer(&packet->mSize, sizeof(packet->mSize));
    }
    else if (serializer->mDirection == 1)
    {
        u8 size = packet->mSize;
        memcpy(serializer->mPosition, &size, sizeof(size));
        serializer->mPosition += sizeof(size);
    }
    else
    {
        u8 size = 0;
        memcpy(&size, serializer->mPosition, sizeof(size));
        serializer->mPosition += sizeof(size);
        packet->mSize = size;
    }

    serializer->Transfer(packet->mPayload, packet->mSize);
}

extern "C" int fn_8032666C(
    const UnidentifiedTransportPacket_803263E4* packet)
{
    u16 packetSize = packet->mSize;
    unsigned int size = packetSize + 4;
    if (packetSize > 0xFF)
    {
        size = packetSize + 5;
    }
    return size;
}

extern "C" void fn_80326684(UnidentifiedTransportPacket_803263E4* packet,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&packet->mSize, sizeof(packet->mSize));
    serializer->Transfer(packet->mPayload, packet->mSize);
}

extern "C" int fn_80326754(
    const UnidentifiedTransportPacket_803263E4* packet)
{
    return packet->mSize + 2;
}
