#include "NL/nlSlotPool.h"
#include "NL/plat/TransportPacket.h"

SlotPool<TransportPacket> gTransportPacketPool(400, 0);

void* TransportPacket::operator new(
    unsigned long, unsigned int, bool)
{
    return gTransportPacketPool.Allocate();
}

void TransportPacket::operator delete(void* packet)
{
    gTransportPacketPool.Free((TransportPacket*)packet);
}

void TransportPacket::SerializeReliable(NetworkMessageSerializer* serializer)
{
    u8 value = 0;
    if (serializer->mDirection == 1)
    {
        value = mType;
        if (mSize > 0xFF)
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
        if ((mType = value) == 1)
        {
            mType = 0;
        }
    }

    serializer->Transfer(&mSequence,
        sizeof(mSequence));
    if (value == 1)
    {
        serializer->Transfer(&mSize, sizeof(mSize));
    }
    else if (serializer->mDirection == 1)
    {
        u8 size = mSize;
        memcpy(serializer->mPosition, &size, sizeof(size));
        serializer->mPosition += sizeof(size);
    }
    else
    {
        u8 size = 0;
        memcpy(&size, serializer->mPosition, sizeof(size));
        serializer->mPosition += sizeof(size);
        mSize = size;
    }

    serializer->Transfer(mPayload, mSize);
}

int TransportPacket::GetReliableSize() const
{
    u16 packetSize = mSize;
    unsigned int size = packetSize + 4;
    if (packetSize > 0xFF)
    {
        size = packetSize + 5;
    }
    return size;
}

void TransportPacket::SerializeUnreliable(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mSize, sizeof(mSize));
    serializer->Transfer(mPayload, mSize);
}

int TransportPacket::GetUnreliableSize() const
{
    return mSize + 2;
}
