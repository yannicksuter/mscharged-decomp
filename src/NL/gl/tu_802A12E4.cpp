#include <revolution/base/PPCArch.h>
#include <revolution/os/OSCache.h>

#include "NL/gl/tu_802A12E4.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"
#include "Game/GL/glModelBuilder.h"


State_802A12E4::State_802A12E4()
    : count(0)
    , model(0)
    , resource(0)
    , value_0C(0)
    , value_10(0)
    , value_14(0)
{
}

State_802A12E4::~State_802A12E4()
{
}

bool State_802A12E4::fn_802A1344(
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

    glCreateModel(
        model, vertexCount, primitive, allocator, 3, 0x257C2FF3);

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
    glSetModelStream(streams, 0, value_0C,
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
            value_10Data = (short*)glResourceAlloc(
                value_10Count * sizeof(short), GLM_VertexData, allocator);
        }
        else
        {
            value_10Data = (short*)glFrameAlloc(
                value_10Count * sizeof(short), GLM_VertexData);
        }
    }
    value_10 = value_10Data;
    glSetModelStream(streams + 1, 1, value_10,
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
            value_14Data = (u32*)glResourceAlloc(
                vertexCount * sizeof(u32), GLM_VertexData, allocator);
        }
        else
        {
            value_14Data =
                (u32*)glFrameAlloc(vertexCount * sizeof(u32), GLM_VertexData);
        }
    }
    value_14 = value_14Data;
    glSetModelStream(streams + 2, 2, value_14,
        sizeof(u32), 3);

    return true;
}

bool State_802A12E4::fn_802A14F0()
{
    for (u32 i = 0; i < model->numPackets; ++i)
    {
        glplatFinalizePacket(&model->packets[i], resource != 0, resource);
    }

    for (int i = 0; i < model->packets->numStreams; ++i)
    {
        glModelStream* stream = &model->packets->streams[i];
        DCStoreRangeNoSync(stream->address, count * stream->stride);
    }

    PPCSync();
    return true;
}
