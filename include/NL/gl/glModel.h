#ifndef NL_GL_MODEL_H
#define NL_GL_MODEL_H

#include "types.h"

struct DisplayList;

struct glModelStream
{
    /* 0x00 */ void* address;
    /* 0x04 */ u8 unknown04;
    /* 0x05 */ u8 stride;
    /* 0x06 */ u8 id;
    /* 0x07 */ u8 unknown07;
}; // size: 0x8

struct glModelPacket
{
    /* 0x00 */ u16* indexBuffer;
    /* 0x04 */ u32 numVertices;
    /* 0x08 */ u16 numUniqueVertices;
    /* 0x0A */ char primType;
    /* 0x0B */ u8 numStreams;
    /* 0x0C */ glModelStream* streams;
    /* 0x10 */ u8 unknown10[0x14];
    /* 0x24 */ DisplayList* displayList;
    /* 0x28 */ u8 unknown28[8];
}; // size: 0x30

struct glModel
{
    /* 0x00 */ u32 unknown00;
    /* 0x04 */ u32 numPackets;
    /* 0x08 */ glModelPacket* packets;
}; // size: 0xC

#endif // NL_GL_MODEL_H
