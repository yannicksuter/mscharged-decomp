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

struct State_802A195C
{
    int count;
    glModel* model;
    void* resource;
    float* value_0C;
    float* value_10;
};

extern "C" void fn_802A195C(State_802A195C* writer)
{
    writer->count = 0;
    writer->model = 0;
    writer->resource = 0;
    writer->value_0C = 0;
    writer->value_10 = 0;
}

extern "C" void* fn_802A1978(
    State_802A195C* writer, int shouldDelete)
{
    if (writer != 0 && shouldDelete > 0)
    {
        ::operator delete(writer);
    }
    return writer;
}

extern "C" bool fn_802A19B8(State_802A195C* writer,
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
        writer->model, vertexCount, primitive, allocator, 2, 0xEC35CAAB);

    glModelStream* streams = writer->model->packets->streams;
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
    writer->value_0C = positionData;
    fn_802D39CC(streams, 0, writer->value_0C,
        sizeof(float) * 3, 1);

    int texcoordCount = vertexCount * 2;
    float* value_10Data;
    if (texcoordCount == 0)
    {
        value_10Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            value_10Data = (float*)fn_802CC0A4(
                texcoordCount * sizeof(float), 3, allocator);
        }
        else
        {
            value_10Data = (float*)fn_802CC0A8(
                texcoordCount * sizeof(float), 3);
        }
    }
    writer->value_10 = value_10Data;
    fn_802D39CC(streams + 1, 1, writer->value_10,
        sizeof(float) * 2, 4);

    return true;
}

extern "C" bool fn_802A1B10(State_802A195C* writer)
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
