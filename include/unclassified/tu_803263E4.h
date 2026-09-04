#ifndef UNCLASSIFIED_TU_803263E4_H
#define UNCLASSIFIED_TU_803263E4_H

#include "Game/NetworkMessages.h"

struct UnidentifiedTransportPacket_803263E4
{
    /* 0x000 */ u32 mUnidentified00;
    /* 0x004 */ u8 mUnidentified04;
    /* 0x005 */ u8 mUnidentified05;
    /* 0x006 */ u16 mUnidentified06;
    /* 0x008 */ u16 mSize;
    /* 0x00A */ u8 mPayload[0x202];
}; // size: 0x20C

extern "C"
{
    void* fn_803263E4(unsigned long size, unsigned int alignment, bool fromEnd);
    void fn_8032644C(void* packet);
    void fn_80326464(UnidentifiedTransportPacket_803263E4* packet,
        UnidentifiedMessageSerializer* serializer);
    int fn_8032666C(const UnidentifiedTransportPacket_803263E4* packet);
    void fn_80326684(UnidentifiedTransportPacket_803263E4* packet,
        UnidentifiedMessageSerializer* serializer);
    int fn_80326754(const UnidentifiedTransportPacket_803263E4* packet);
}

#endif // UNCLASSIFIED_TU_803263E4_H
