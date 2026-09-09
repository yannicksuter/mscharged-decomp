#ifndef NL_PLAT_TRANSPORTPACKET_H
#define NL_PLAT_TRANSPORTPACKET_H

#include "Game/NetworkMessageSerializer.h"

struct TransportPacket
{
    static void* operator new(
        unsigned long size, unsigned int alignment, bool fromEnd);

    TransportPacket()
        : mSequence(0)
    {
    }

    void operator delete(void* packet);
    void SerializeReliable(NetworkMessageSerializer* serializer);
    int GetReliableSize() const;
    void SerializeUnreliable(NetworkMessageSerializer* serializer);
    int GetUnreliableSize() const;

    /* 0x000 */ u32 mLastSendTick;
    /* 0x004 */ u8 mType;
    /* 0x005 */ u8 mUnidentified05;
    /* 0x006 */ u16 mSequence;
    /* 0x008 */ u16 mSize;
    /* 0x00A */ u8 mPayload[0x202];
}; // size: 0x20C


#endif // NL_PLAT_TRANSPORTPACKET_H
