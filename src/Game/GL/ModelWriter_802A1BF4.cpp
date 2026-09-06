#include <revolution/base/PPCArch.h>
#include <revolution/os/OSCache.h>

#include "Game/GL/ModelWriter_802A1BF4.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"
#include "Game/GL/tu_802D38A4.h"


ModelWriter_802A1BF4::ModelWriter_802A1BF4()
    : count(0)
    , model(0)
    , allocator(0)
    , position(0)
    , texcoord(0)
    , colour(0)
{
}

ModelWriter_802A1BF4::~ModelWriter_802A1BF4()
{
}

bool ModelWriter_802A1BF4::fn_802A1C54(
    int vertexCount, int primitive, void* allocator)
{
    glModel* newModel;
    this->allocator = allocator;
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
        model, vertexCount, primitive, allocator, 3, 0xCFB7215C);

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
    position = positionData;
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
            texcoordData = (short*)glResourceAlloc(
                texcoordCount * sizeof(short), GLM_VertexData, allocator);
        }
        else
        {
            texcoordData = (short*)glFrameAlloc(
                texcoordCount * sizeof(short), GLM_VertexData);
        }
    }
    texcoord = texcoordData;
    fn_802D39CC(streams + 1, 1, texcoord, sizeof(short) * 2, 4);

    u32* colourData;
    if (vertexCount == 0)
    {
        colourData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            colourData = (u32*)glResourceAlloc(
                vertexCount * sizeof(u32), GLM_VertexData, allocator);
        }
        else
        {
            colourData = (u32*)glFrameAlloc(vertexCount * sizeof(u32), GLM_VertexData);
        }
    }
    colour = colourData;
    fn_802D39CC(streams + 2, 2, colour, sizeof(u32), 3);

    return true;
}

bool ModelWriter_802A1BF4::fn_802A1E00()
{
    for (u32 i = 0; i < model->numPackets; ++i)
    {
        glplatFinalizePacket(&model->packets[i], allocator != 0, allocator);
    }

    for (int i = 0; i < model->packets->numStreams; ++i)
    {
        glModelStream* stream = &model->packets->streams[i];
        DCStoreRangeNoSync(stream->address, count * stream->stride);
    }

    PPCSync();
    return true;
}
