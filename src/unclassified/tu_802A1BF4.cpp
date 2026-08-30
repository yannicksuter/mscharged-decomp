#include "Game/GL/ModelWriter_802A1BF4.h"

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

extern "C" void fn_802A1BF4(ModelWriter_802A1BF4* writer)
{
    writer->count = 0;
    writer->model = 0;
    writer->allocator = 0;
    writer->position = 0;
    writer->texcoord = 0;
    writer->colour = 0;
}

extern "C" void* fn_802A1C14(
    ModelWriter_802A1BF4* writer, int shouldDelete)
{
    if (writer != 0 && shouldDelete > 0)
    {
        ::operator delete(writer);
    }
    return writer;
}

extern "C" bool fn_802A1C54(ModelWriter_802A1BF4* writer,
    int vertexCount, int primitive, void* allocator)
{
    glModel* newModel;
    writer->allocator = allocator;
    writer->count = vertexCount;

    if (allocator != 0)
    {
        newModel = (glModel*)fn_802CC0A4(sizeof(glModel), 0, allocator);
    }
    else
    {
        newModel = (glModel*)fn_802CC0A8(sizeof(glModel), 0);
    }
    writer->model = newModel;

    fn_802D38A4(
        writer->model, vertexCount, primitive, allocator, 3, 0xCFB7215C);

    glModelStream* streams = writer->model->packets->streams;
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
            positionData = fn_802CC0A4(
                positionCount * sizeof(float), 3, allocator);
        }
        else
        {
            positionData = fn_802CC0A8(
                positionCount * sizeof(float), 3);
        }
    }
    writer->position = (float*)positionData;
    fn_802D39CC(streams, 0, writer->position, sizeof(float) * 3, 1);

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
            texcoordData = (short*)fn_802CC0A4(
                texcoordCount * sizeof(short), 3, allocator);
        }
        else
        {
            texcoordData = (short*)fn_802CC0A8(
                texcoordCount * sizeof(short), 3);
        }
    }
    writer->texcoord = texcoordData;
    fn_802D39CC(streams + 1, 1, writer->texcoord, sizeof(short) * 2, 4);

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
            colourData = (u32*)fn_802CC0A8(vertexCount * sizeof(u32), 3);
        }
    }
    writer->colour = colourData;
    fn_802D39CC(streams + 2, 2, writer->colour, sizeof(u32), 3);

    return true;
}

extern "C" bool fn_802A1E00(ModelWriter_802A1BF4* writer)
{
    int i = 0;
    for (; i < writer->model->numPackets; ++i)
    {
        glModelPacket* packet = writer->model->packets + i;
        fn_8036E438(packet, writer->allocator != 0);
    }

    i = 0;
    for (; i < writer->model->packets->numStreams; ++i)
    {
        glModelStream* stream = writer->model->packets->streams + i;
        DCStoreRangeNoSync(
            stream->address, writer->count * stream->stride);
    }

    PPCSync();
    return true;
}
