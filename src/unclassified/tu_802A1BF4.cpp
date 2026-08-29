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

struct State_802A1BF4
{
    int count;
    glModel* model;
    void* resource;
    float* value_0C;
    short* value_10;
    u32* value_14;
};

extern "C" void fn_802A1BF4(State_802A1BF4* writer)
{
    writer->count = 0;
    writer->model = 0;
    writer->resource = 0;
    writer->value_0C = 0;
    writer->value_10 = 0;
    writer->value_14 = 0;
}

extern "C" void* fn_802A1C14(
    State_802A1BF4* writer, int shouldDelete)
{
    if (writer != 0 && shouldDelete > 0)
    {
        ::operator delete(writer);
    }
    return writer;
}

extern "C" bool fn_802A1C54(State_802A1BF4* writer,
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
    writer->value_0C = (float*)positionData;
    fn_802D39CC(streams, 0, writer->value_0C,
        sizeof(float) * 3, 1);

    int value_10Count = vertexCount * 2;
    short* value_10Data;
    if (value_10Count == 0)
    {
        value_10Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            value_10Data = (short*)fn_802CC0A4(
                value_10Count * sizeof(short), 3, allocator);
        }
        else
        {
            value_10Data = (short*)fn_802CC0A8(
                value_10Count * sizeof(short), 3);
        }
    }
    writer->value_10 = value_10Data;
    fn_802D39CC(streams + 1, 1, writer->value_10,
        sizeof(short) * 2, 4);

    u32* value_14Data;
    if (vertexCount == 0)
    {
        value_14Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            value_14Data = (u32*)fn_802CC0A4(
                vertexCount * sizeof(u32), 3, allocator);
        }
        else
        {
            value_14Data =
                (u32*)fn_802CC0A8(vertexCount * sizeof(u32), 3);
        }
    }
    writer->value_14 = value_14Data;
    fn_802D39CC(streams + 2, 2, writer->value_14,
        sizeof(u32), 3);

    return true;
}

extern "C" bool fn_802A1E00(State_802A1BF4* writer)
{
    int i = 0;
    for (; i < writer->model->numPackets; ++i)
    {
        glModelPacket* packet = writer->model->packets + i;
        fn_8036E438(packet, writer->resource != 0);
    }

    i = 0;
    for (; i < writer->model->packets->numStreams; ++i)
    {
        glModelStream* stream =
            writer->model->packets->streams + i;
        DCStoreRangeNoSync(
            stream->address, writer->count * stream->stride);
    }

    PPCSync();
    return true;
}
