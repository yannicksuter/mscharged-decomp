#include <revolution/base/PPCArch.h>
#include <revolution/os/OSCache.h>

#include "Game/GL/GLFourTextureAddMeshWriter.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"
#include "Game/GL/glModelBuilder.h"
#include "Game/UnidentifiedStaticStorage.h"

GLFourTextureAddMeshWriter::GLFourTextureAddMeshWriter()
    : count(0)
    , model(0)
    , resource(0)
    , positions(0)
    , texcoords0(0)
    , texcoords1(0)
    , texcoords2(0)
    , texcoords3(0)
    , colours(0)
{
}

GLFourTextureAddMeshWriter::~GLFourTextureAddMeshWriter()
{
}

bool GLFourTextureAddMeshWriter::Begin(
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
    positions = positionData;
    glSetModelStream(streams, 0, positions, sizeof(float) * 3, 1);

    int texcoordCount = vertexCount * 2;
    float* texcoords0Data;
    if (texcoordCount == 0)
    {
        texcoords0Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            texcoords0Data = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            texcoords0Data = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    texcoords0 = texcoords0Data;
    glSetModelStream(streams + 1, 1, texcoords0, sizeof(float) * 2, 4);

    float* texcoords1Data;
    if (texcoordCount == 0)
    {
        texcoords1Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            texcoords1Data = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            texcoords1Data = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    texcoords1 = texcoords1Data;
    glSetModelStream(streams + 2, 2, texcoords1, sizeof(float) * 2, 4);

    float* texcoords2Data;
    if (texcoordCount == 0)
    {
        texcoords2Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            texcoords2Data = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            texcoords2Data = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    texcoords2 = texcoords2Data;
    glSetModelStream(streams + 3, 3, texcoords2, sizeof(float) * 2, 4);

    float* texcoords3Data;
    if (texcoordCount == 0)
    {
        texcoords3Data = 0;
    }
    else
    {
        if (allocator != 0)
        {
            texcoords3Data = (float*)glResourceAlloc(
                texcoordCount * sizeof(float), GLM_VertexData, allocator);
        }
        else
        {
            texcoords3Data = (float*)glFrameAlloc(
                texcoordCount * sizeof(float), GLM_VertexData);
        }
    }
    texcoords3 = texcoords3Data;
    glSetModelStream(streams + 4, 4, texcoords3, sizeof(float) * 2, 4);

    u32* coloursData;
    if (vertexCount == 0)
    {
        coloursData = 0;
    }
    else
    {
        if (allocator != 0)
        {
            coloursData = (u32*)glResourceAlloc(
                vertexCount * sizeof(u32), GLM_VertexData, allocator);
        }
        else
        {
            coloursData = (u32*)glFrameAlloc(vertexCount * sizeof(u32), GLM_VertexData);
        }
    }
    colours = coloursData;
    glSetModelStream(streams + 5, 5, colours, sizeof(u32), 3);

    return true;
}

bool GLFourTextureAddMeshWriter::End()
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
