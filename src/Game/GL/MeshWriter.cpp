#include <revolution/base/PPCArch.h>
#include <revolution/os/OSCache.h>

#include "Game/GL/MeshWriter.h"

#include "Game/GL/glModelBuilder.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"
#include "Game/UnidentifiedStaticStorage.h"

MeshWriter::MeshWriter()
    : count(0)
    , model(0)
    , resource(0)
    , position(0)
    , texcoord(0)
{
}

MeshWriter::~MeshWriter()
{
}

bool MeshWriter::Begin(int vertexCount, int primitive, void* allocator)
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

    glCreateModel(model, vertexCount, primitive, allocator, 2, 0xEE9D919D);

    glModelStream* streams = model->packets->streams;
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
            positionData = glResourceAlloc(positionCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            positionData = glFrameAlloc(positionCount * sizeof(float), GLM_VertexData);
        }
    }
    position = (float*)positionData;
    glSetModelStream(streams, 0, position, sizeof(float) * 3, 1);

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
            texcoordData = (short*)glResourceAlloc(texcoordCount * sizeof(short), GLM_VertexData, allocator);
        }
        else
        {
            texcoordData = (short*)glFrameAlloc(texcoordCount * sizeof(short), GLM_VertexData);
        }
    }
    texcoord = texcoordData;
    glSetModelStream(streams + 1, 1, texcoord, sizeof(short) * 2, 4);

    return true;
}

bool MeshWriter::End()
{
    for (int i = 0; i < model->numPackets; ++i)
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
