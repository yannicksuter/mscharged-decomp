#include "NL/gl/glModel.h"

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

struct State_802A7C90
{
    int count;
    glModel* model;
    void* resource;
    short* position;
    short* texcoord;
    u32* colour;
};

extern "C" void fn_802A7C90(State_802A7C90* writer)
{
    writer->count = 0;
    writer->model = 0;
    writer->resource = 0;
    writer->position = 0;
    writer->texcoord = 0;
    writer->colour = 0;
}

extern "C" void* fn_802A7CB0(
    State_802A7C90* writer, int shouldDelete)
{
    if (writer != 0 && shouldDelete > 0)
    {
        ::operator delete(writer);
    }
    return writer;
}

extern "C" bool fn_802A7CF0(State_802A7C90* writer,
    int vertexCount, int primitive, void* allocator)
{
    glModel* newModel;
    writer->resource = allocator;
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
        writer->model, vertexCount, primitive, allocator, 3, 0x4ED6C66F);

    glModelStream* streams = writer->model->packets->streams;
    int positionCount = vertexCount * 3;
    short* positionData;
    if (positionCount == 0)
    {
        positionData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            positionData = (short*)fn_802CC0A4(
                positionCount * sizeof(short), 3, allocator);
        }
        else
        {
            positionData = (short*)fn_802CC0A8(
                positionCount * sizeof(short), 3);
        }
    }
    writer->position = positionData;
    fn_802D39CC(streams, 0, writer->position, sizeof(short) * 3, 1);

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
    fn_802D39CC(streams + 1, 1, writer->texcoord,
        sizeof(short) * 2, 4);

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
    writer->colour = colourData;
    fn_802D39CC(
        streams + 2, 2, writer->colour, sizeof(u32), 3);

    return true;
}

extern "C" bool fn_802A7E9C(State_802A7C90* writer)
{
    u32 offset = 0;
    u32 i = 0;
    for (; i < writer->model->numPackets; ++i)
    {
        glModelPacket* packet =
            (glModelPacket*)((u8*)writer->model->packets + offset);
        fn_8036E438(packet, writer->resource != 0);
        offset += sizeof(glModelPacket);
    }

    i = 0;
    offset = 0;
    for (; i < writer->model->packets->numStreams; ++i)
    {
        glModelStream* stream =
            (glModelStream*)((u8*)writer->model->packets->streams
                + offset);
        DCStoreRangeNoSync(
            stream->address, writer->count * stream->stride);
        offset += sizeof(glModelStream);
    }

    PPCSync();
    return true;
}
