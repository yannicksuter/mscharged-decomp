#include <revolution/base/PPCArch.h>
#include <revolution/os/OSCache.h>

#include "NL/gl/tu_802A0F04.h"
#include "NL/gl/glMemory.h"

extern "C" void fn_802D38A4(glModel* model, int count, int primitive,
    void* allocator, int numStreams, unsigned long format);
extern "C" void fn_802D39CC(glModelStream* streams, int stream,
    void* address, int stride, int type);
extern "C" void fn_8036E438(glModelPacket* packet, bool permanent, void* allocator);

State_802A0F04::State_802A0F04()
    : count(0)
    , model(0)
    , resource(0)
    , value_0C(0)
    , value_10(0)
    , value_14(0)
    , value_18(0)
    , value_1C(0)
    , value_20(0)
{
}

State_802A0F04::~State_802A0F04()
{
}

bool State_802A0F04::fn_802A0F70(
    int vertexCount, int primitive, void* allocator)
{
    glModel* newModel;
    resource = allocator;
    count = vertexCount;

    if (allocator != 0)
    {
        newModel = (glModel*)glResourceAlloc(sizeof(glModel), GLM_Header, allocator);
    }
    else
    {
        newModel = (glModel*)glFrameAlloc(sizeof(glModel), GLM_Header);
    }
    model = newModel;

    fn_802D38A4(
        model, vertexCount, primitive, allocator, 6, 0x2910966C);

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
            positionData = (float*)glResourceAlloc(
                positionCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            positionData = (float*)glFrameAlloc(
                positionCount * sizeof(float), GLM_VertexData);
        }
    }
    value_0C = positionData;
    fn_802D39CC(streams, 0, value_0C,
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
            value_10Data = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            value_10Data = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    value_10 = value_10Data;
    fn_802D39CC(streams + 1, 1, value_10,
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
            value_14Data = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            value_14Data = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    value_14 = value_14Data;
    fn_802D39CC(streams + 2, 2, value_14,
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
            value_18Data = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            value_18Data = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    value_18 = value_18Data;
    fn_802D39CC(streams + 3, 3, value_18,
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
            value_1CData = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            value_1CData = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    value_1C = value_1CData;
    fn_802D39CC(streams + 4, 4, value_1C,
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
            value_20Data = (u32*)glResourceAlloc(
                vertexCount * sizeof(u32), GLM_VertexData, allocator);
        }
        else
        {
            value_20Data =
                (u32*)glFrameAlloc(vertexCount * sizeof(u32), GLM_VertexData);
        }
    }
    value_20 = value_20Data;
    fn_802D39CC(streams + 5, 5, value_20,
        sizeof(u32), 3);

    return true;
}

bool State_802A0F04::fn_802A1200()
{
    for (u32 i = 0; i < model->numPackets; ++i)
    {
        fn_8036E438(&model->packets[i], resource != 0, resource);
    }

    for (int i = 0; i < model->packets->numStreams; ++i)
    {
        glModelStream* stream = &model->packets->streams[i];
        DCStoreRangeNoSync(stream->address, count * stream->stride);
    }

    PPCSync();
    return true;
}
