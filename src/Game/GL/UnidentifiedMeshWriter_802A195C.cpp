#include <revolution/base/PPCArch.h>
#include <revolution/os/OSCache.h>

#include "Game/GL/UnidentifiedMeshWriter_802A195C.h"
#include "Game/GL/glModelBuilder.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"

UnidentifiedMeshWriter_802A195C::UnidentifiedMeshWriter_802A195C()
    : count(0)
    , model(0)
    , resource(0)
    , position(0)
    , texcoord(0)
{
}

UnidentifiedMeshWriter_802A195C::~UnidentifiedMeshWriter_802A195C()
{
}

bool UnidentifiedMeshWriter_802A195C::Begin(
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
        model, vertexCount, primitive, allocator, 2, 0xEC35CAAB);

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
    glSetModelStream(streams, 0, position,
        sizeof(float) * 3, 1);

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
            texcoordData = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            texcoordData = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    texcoord = texcoordData;
    glSetModelStream(streams + 1, 1, texcoord,
        sizeof(float) * 2, 4);

    return true;
}

bool UnidentifiedMeshWriter_802A195C::End()
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
