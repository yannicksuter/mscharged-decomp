#include "unclassified/tu_802A8A90.h"

#include "types.h"

extern "C" void* fn_802CC0A4(
    unsigned long size, int memoryType, void* allocator);
extern "C" void* fn_802CC0A8(unsigned long size, int memoryType);
extern "C" void fn_802D38A4(glModel* model, int count, int primitive,
    void* allocator, int numStreams, unsigned long format);
extern "C" void fn_802D39CC(glModelStream* streams, int stream,
    void* address, int stride, int type);
extern "C" void fn_8036E438(glModelPacket* packet, bool allocated);
extern "C" void DCStoreRangeNoSync(const void* address, u32 size);
extern "C" void PPCSync();

UnidentifiedMeshWriter_802A8A90::UnidentifiedMeshWriter_802A8A90()
{
    count = 0;
    model = 0;
    resource = 0;
    position = 0;
    colour = 0;
    texcoord = 0;
}

UnidentifiedMeshWriter_802A8A90::~UnidentifiedMeshWriter_802A8A90()
{
}

bool UnidentifiedMeshWriter_802A8A90::Begin(
    int vertexCount, int primitive, void* allocator)
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

    fn_802D38A4(
        model, vertexCount, primitive, allocator, 3, 0x386ECBDD);

    glModelStream* streams = model->packets->streams;
    int positionCount = vertexCount * 3;
    float* positionData;
    if (positionCount == 0)
    {
        positionData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            positionData = (float*)fn_802CC0A4(
                positionCount * sizeof(float), 3, allocator);
        }
        else
        {
            positionData = (float*)fn_802CC0A8(
                positionCount * sizeof(float), 3);
        }
    }
    position = positionData;
    fn_802D39CC(streams, 0, position,
        sizeof(float) * 3, 1);

    u32* colourData;
    if (vertexCount == 0)
    {
        colourData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            colourData = (u32*)fn_802CC0A4(
                vertexCount * sizeof(u32), 3, allocator);
        }
        else
        {
            colourData =
                (u32*)fn_802CC0A8(vertexCount * sizeof(u32), 3);
        }
    }
    colour = colourData;
    fn_802D39CC(
        streams + 1, 1, colour, sizeof(u32), 3);

    int texcoordCount = vertexCount * 2;
    float* texcoordData;
    if (texcoordCount == 0)
    {
        texcoordData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            texcoordData = (float*)fn_802CC0A4(
                texcoordCount * sizeof(float), 3, allocator);
        }
        else
        {
            texcoordData = (float*)fn_802CC0A8(
                texcoordCount * sizeof(float), 3);
        }
    }
    texcoord = texcoordData;
    fn_802D39CC(streams + 2, 2, texcoord,
        sizeof(float) * 2, 4);

    return true;
}

bool UnidentifiedMeshWriter_802A8A90::End()
{
    for (int i = 0; i < model->numPackets; ++i)
    {
        glModelPacket* packet = &model->packets[i];
        fn_8036E438(packet, resource != 0);
    }

    for (int i = 0; i < model->packets->numStreams; ++i)
    {
        glModelStream* stream = &model->packets->streams[i];
        DCStoreRangeNoSync(
            stream->address, count * stream->stride);
    }

    PPCSync();
    return true;
}
