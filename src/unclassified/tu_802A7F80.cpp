#include "Game/GL/MeshWriter.h"

#include "types.h"

extern "C" void* fn_802CC0A4(unsigned long size, int memoryType, void* allocator);
extern "C" void* fn_802CC0A8(unsigned long size, int memoryType);
extern "C" void fn_802D38A4(glModel* model, int count, int primitive, void* allocator, int numStreams, unsigned long format);
extern "C" void fn_802D39CC(glModelStream* streams, int stream, void* address, int stride, int type);
extern "C" void fn_8036E438(glModelPacket* packet, bool allocated);
extern "C" void DCStoreRangeNoSync(const void* address, u32 size);
extern "C" void PPCSync();

MeshWriter::MeshWriter()
    : count(0)
    , model(0)
    , resource(0)
    , position(0)
    , texcoord(0)
{
}

MeshWriter::~MeshWriter()
{
}

bool MeshWriter::Begin(int vertexCount, int primitive, void* allocator)
{
    glModel* newModel;
    resource = allocator;
    count = vertexCount;

    if (allocator != 0)
    {
        newModel = (glModel*)fn_802CC0A4(sizeof(glModel), 0, allocator);
    }
    else
    {
        newModel = (glModel*)fn_802CC0A8(sizeof(glModel), 0);
    }
    model = newModel;

    fn_802D38A4(model, vertexCount, primitive, allocator, 2, 0xEE9D919D);

    glModelStream* streams = model->packets->streams;
    int positionCount = vertexCount * 3;
    void* positionData;
    if (positionCount == 0)
    {
        positionData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            positionData = fn_802CC0A4(positionCount * sizeof(float), 3, allocator);
        }
        else
        {
            positionData = fn_802CC0A8(positionCount * sizeof(float), 3);
        }
    }
    position = (float*)positionData;
    fn_802D39CC(streams, 0, position, sizeof(float) * 3, 1);

    int texcoordCount = vertexCount * 2;
    short* texcoordData;
    if (texcoordCount == 0)
    {
        texcoordData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            texcoordData = (short*)fn_802CC0A4(texcoordCount * sizeof(short), 3, allocator);
        }
        else
        {
            texcoordData = (short*)fn_802CC0A8(texcoordCount * sizeof(short), 3);
        }
    }
    texcoord = texcoordData;
    fn_802D39CC(streams + 1, 1, texcoord, sizeof(short) * 2, 4);

    return true;
}

bool MeshWriter::End()
{
    unsigned long packetOffset;
    int index;
    for (index = 0, packetOffset = 0; index < model->numPackets;
         packetOffset += sizeof(glModelPacket), ++index)
    {
        glModelPacket* packet = (glModelPacket*)((u8*)model->packets + packetOffset);
        fn_8036E438(packet, resource != 0);
    }

    for (index = 0, packetOffset = 0; index < model->packets->numStreams;
         packetOffset += sizeof(glModelStream), ++index)
    {
        glModelStream* stream = (glModelStream*)((u8*)model->packets->streams + packetOffset);
        DCStoreRangeNoSync(stream->address, count * stream->stride);
    }

    PPCSync();
    return true;
}
