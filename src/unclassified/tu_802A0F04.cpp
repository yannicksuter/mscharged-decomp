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

struct State_802A0F04
{
    int count;
    glModel* model;
    void* resource;
    float* value_0C;
    float* value_10;
    float* value_14;
    float* value_18;
    float* value_1C;
    u32* value_20;
};

extern "C" void fn_802A0F04(State_802A0F04* writer)
{
    writer->count = 0;
    writer->model = 0;
    writer->resource = 0;
    writer->value_0C = 0;
    writer->value_10 = 0;
    writer->value_14 = 0;
    writer->value_18 = 0;
    writer->value_1C = 0;
    writer->value_20 = 0;
}

extern "C" void* fn_802A0F30(
    State_802A0F04* writer, int shouldDelete)
{
    if (writer != 0 && shouldDelete > 0)
    {
        ::operator delete(writer);
    }
    return writer;
}

extern "C" bool fn_802A0F70(State_802A0F04* writer,
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
        writer->model, vertexCount, primitive, allocator, 6, 0x2910966C);

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

    float* value_14Data;
    if (texcoordCount == 0)
    {
        value_14Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            value_14Data = (float*)fn_802CC0A4(
                texcoordCount * sizeof(float), 3, allocator);
        }
        else
        {
            value_14Data = (float*)fn_802CC0A8(
                texcoordCount * sizeof(float), 3);
        }
    }
    writer->value_14 = value_14Data;
    fn_802D39CC(streams + 2, 2, writer->value_14,
        sizeof(float) * 2, 4);

    float* value_18Data;
    if (texcoordCount == 0)
    {
        value_18Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            value_18Data = (float*)fn_802CC0A4(
                texcoordCount * sizeof(float), 3, allocator);
        }
        else
        {
            value_18Data = (float*)fn_802CC0A8(
                texcoordCount * sizeof(float), 3);
        }
    }
    writer->value_18 = value_18Data;
    fn_802D39CC(streams + 3, 3, writer->value_18,
        sizeof(float) * 2, 4);

    float* value_1CData;
    if (texcoordCount == 0)
    {
        value_1CData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            value_1CData = (float*)fn_802CC0A4(
                texcoordCount * sizeof(float), 3, allocator);
        }
        else
        {
            value_1CData = (float*)fn_802CC0A8(
                texcoordCount * sizeof(float), 3);
        }
    }
    writer->value_1C = value_1CData;
    fn_802D39CC(streams + 4, 4, writer->value_1C,
        sizeof(float) * 2, 4);

    u32* value_20Data;
    if (vertexCount == 0)
    {
        value_20Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            value_20Data = (u32*)fn_802CC0A4(
                vertexCount * sizeof(u32), 3, allocator);
        }
        else
        {
            value_20Data =
                (u32*)fn_802CC0A8(vertexCount * sizeof(u32), 3);
        }
    }
    writer->value_20 = value_20Data;
    fn_802D39CC(streams + 5, 5, writer->value_20,
        sizeof(u32), 3);

    return true;
}

extern "C" bool fn_802A1200(State_802A0F04* writer)
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
