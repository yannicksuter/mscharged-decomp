#ifndef NL_GL_MODEL_H
#define NL_GL_MODEL_H

#include "types.h"

struct DisplayList;
class nlMatrix4;

struct glTextureBinding
{
    /* 0x00 */ u32 texture;
    /* 0x04 */ u16 textureIndex;
    /* 0x06 */ u8 flags;
    /* 0x07 */ u8 unknown07;

    void SetWrapS(unsigned char enabled)
    {
        flags = (flags & 2) | enabled;
    }

    void SetWrapT(unsigned char enabled)
    {
        flags = (flags & 1) | (enabled << 1);
    }
}; // size: 0x8

struct glModelStream
{
    /* 0x00 */ void* address;
    /* 0x04 */ u8 index;
    /* 0x05 */ u8 stride;
    /* 0x06 */ u8 id;
    /* 0x07 */ u8 unknown07;
}; // size: 0x8

struct glModelPacket;

struct glModelPacket
{
    /* 0x00 */ u16* indexBuffer;
    /* 0x04 */ u32 numVertices;
    /* 0x08 */ u16 numUniqueVertices;
    /* 0x0A */ char primType;
    /* 0x0B */ u8 numStreams;
    /* 0x0C */ glModelStream* streams;
    /* 0x10 */ void* materialProgram;
    /* 0x14 */ u32 unknown14;
    /* 0x18 */ u32 matrix;
    /* 0x1C */ u32 rasterState;
    /* 0x20 */ void* materialParameters;
    /* 0x24 */ DisplayList* displayList;
    /* 0x28 */ u32 skinnedVertices;
    /* 0x2C */ u32 skinnedNormals;
}; // size: 0x30

struct glModel
{
    /* 0x00 */ u32 id;
    /* 0x04 */ u32 numPackets;
    /* 0x08 */ glModelPacket* packets;
}; // size: 0xC

inline glModelStream* glModelPacketGetStream(glModelPacket* pPacket, int id)
{
    for (unsigned long i = 0; i < pPacket->numStreams; i++)
    {
        glModelStream* stream = &pPacket->streams[i];
        if (stream->id == id)
        {
            return stream;
        }
    }
    return 0;
}

inline glModelStream* glFindModelStream(glModelPacket* pPacket, int id)
{
    for (unsigned long i = 0; i < pPacket->numStreams; i++)
    {
        glModelStream* stream = &pPacket->streams[i];
        if (stream->id == id)
        {
            return stream;
        }
    }
    return 0;
}

void glModelSetMatrix(glModel* model, const nlMatrix4& matrix);
void glModelSetMatrix(glModel* model, unsigned long matrix);
void glModelSetRasterState(glModel* model, unsigned long rasterState);
void glModelGetMatrix(const glModel* model, nlMatrix4& matrix);
glModel* glModelDupArrayNoStreams(
    const glModel* pModelArray, unsigned long nModels, bool bPermanent,
    void* pAllocator);
glModel* glModelDupNoStreams(
    const glModel* pModel, bool bPermanent, void* pAllocator);

#endif // NL_GL_MODEL_H
