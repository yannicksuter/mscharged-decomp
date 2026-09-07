#include <revolution/base/PPCArch.h>
#include <revolution/os/OSCache.h>

#include "Game/GL/GLTexturedColourMeshWriter.h"
#include "NL/nlMemory.h"

#include "Game/GL/glModelBuilder.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"

GLTexturedColourMeshWriter::GLTexturedColourMeshWriter()
{
    count = 0;
    model = 0;
    resource = 0;
    position = 0;
    texcoord = 0;
    colour = 0;
}

GLTexturedColourMeshWriter::~GLTexturedColourMeshWriter()
{
}

bool GLTexturedColourMeshWriter::Begin(
    int numVerts, int prim, void* pResource)
{
    glModel* newModel;
    resource = pResource;
    count = numVerts;

    if (pResource != 0)
    {
        newModel = (glModel*)glResourceAlloc(sizeof(glModel), GLM_Header, pResource);
    }
    else
    {
        newModel = (glModel*)glFrameAlloc(sizeof(glModel), GLM_Header);
    }
    model = newModel;

    glCreateModel(
        model, numVerts, prim, pResource, 3, 0xD3E572DA);

    glModelStream* streams = model->packets->streams;
    int positionCount = numVerts * 3;
    void* positionData;
    if (positionCount == 0)
    {
        positionData = 0;
    }
    else
    {
        if (pResource != 0)
        {
            positionData = glResourceAlloc(positionCount * sizeof(float), GLM_VertexData, pResource);
        }
        else
        {
            positionData = glFrameAlloc(positionCount * sizeof(float), GLM_VertexData);
        }
    }
    position = (float*)positionData;
    glSetModelStream(streams, 0, position, sizeof(float) * 3, 1);

    int texcoordCount = numVerts * 2;
    short* texcoordData;
    if (texcoordCount == 0)
    {
        texcoordData = 0;
    }
    else
    {
        if (pResource != 0)
        {
            texcoordData = (short*)glResourceAlloc(
                texcoordCount * sizeof(short), GLM_VertexData, pResource);
        }
        else
        {
            texcoordData = (short*)glFrameAlloc(texcoordCount * sizeof(short), GLM_VertexData);
        }
    }
    texcoord = texcoordData;
    glSetModelStream(streams + 1, 1, texcoord, sizeof(short) * 2, 4);

    u32* colourData;
    if (numVerts == 0)
    {
        colourData = 0;
    }
    else
    {
        if (pResource != 0)
        {
            colourData = (u32*)glResourceAlloc(
                numVerts * sizeof(u32), GLM_VertexData, pResource);
        }
        else
        {
            colourData = (u32*)glFrameAlloc(numVerts * sizeof(u32), GLM_VertexData);
        }
    }
    colour = colourData;
    glSetModelStream(streams + 2, 2, colour, sizeof(u32), 3);

    return true;
}

bool GLTexturedColourMeshWriter::End()
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
