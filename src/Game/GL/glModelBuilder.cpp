#include "NL/gl/glMaterialProgram.h"
#include "Game/GL/glModelBuilder.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxLoadModel.h"
#include "NL/gl/glLoadModel.h"

struct UnidentifiedMaterialProgram_802D38A4
{
    unsigned char unknown00[8];
    unsigned long dataSize;
};

void glCreateModel(glModel* pModel, int numVerts, int prim,
    void* pResource, int numStreams, unsigned long programHash)
{
    glModelPacket* pPacket;
    if (pResource != 0)
    {
        pPacket = (glModelPacket*)glResourceAlloc(
            sizeof(glModelPacket), GLM_Header, pResource);
    }
    else
    {
        pPacket = (glModelPacket*)glFrameAlloc(
            sizeof(glModelPacket), GLM_Header);
    }

    pModel->unknown00 = 0;
    pModel->numPackets = 1;
    pModel->packets = pPacket;

    pPacket->indexBuffer = 0;
    pPacket->numVertices = 0;
    pPacket->numUniqueVertices = numVerts;
    pPacket->primType = prim;
    pPacket->numStreams = numStreams;
    pPacket->matrix = glGetCurrentMatrix();
    pPacket->unknown10 = glGetMaterialProgram(programHash);

    glModelStream* pPktStreams;
    if (numStreams == 0)
    {
        pPktStreams = 0;
    }
    else if (pResource != 0)
    {
        pPktStreams = (glModelStream*)glResourceAlloc(
            numStreams * sizeof(glModelStream), GLM_Header, pResource);
    }
    else
    {
        pPktStreams = (glModelStream*)glFrameAlloc(
            numStreams * sizeof(glModelStream), GLM_Header);
    }
    pPacket->streams = pPktStreams;

    pPacket->rasterState = glGetCurrentRasterState();

    unsigned long dataSize =
        ((UnidentifiedMaterialProgram_802D38A4*)pPacket->unknown10)->dataSize;
    void* data;
    if (dataSize == 0)
    {
        data = 0;
    }
    else if (pResource != 0)
    {
        data = glResourceAlloc(dataSize, GLM_Header, pResource);
    }
    else
    {
        data = glFrameAlloc(dataSize, GLM_Header);
    }
    pPacket->unknown20 = data;
}

void glSetModelStream(glModelStream* pStream, int stream,
    void* address, int stride, int type)
{
    pStream->address = address;
    pStream->stride = stride;
    pStream->id = type;
    pStream->unknown07 = 0;
    pStream->unknown04 = stream;
}

void glSetModelPackets(
    glModel* pModel, glModelPacket* pPackets, unsigned long numPackets)
{
    pModel->packets = pPackets;
    pModel->numPackets = numPackets;
}

void glSetPacketStreams(
    glModelPacket* pPacket, glModelStream* pStreams, int numStreams)
{
    pPacket->streams = pStreams;
    pPacket->numStreams = numStreams;
}

void glSetStreamAddress(glModelStream* pStream, void* address)
{
    pStream->address = address;
}
