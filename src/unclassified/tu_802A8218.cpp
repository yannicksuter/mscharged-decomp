#include "Game/GL/GLMeshWriter.h"
#include "NL/nlMemory.h"

#include "types.h"

extern "C" void* fn_802CC0A4(
    unsigned long size, int memoryType, void* allocator);
extern "C" void* fn_802CC0A8(unsigned long size, int memoryType);
extern "C" void fn_802D38A4(glModel* model, int count, int primitive,
    void* allocator, int numStreams, unsigned long format);
extern "C" void fn_802D39CC(glModelStream* streams, int stream, void* address,
    int stride, int type);
extern "C" void fn_8036E438(glModelPacket* packet, bool allocated);
extern "C" void DCStoreRangeNoSync(const void* address, u32 size);
extern "C" void PPCSync();

GLMeshWriter::GLMeshWriter()
{
    count = 0;
    model = 0;
    resource = 0;
    position = 0;
    texcoord = 0;
    colour = 0;
}

GLMeshWriter::~GLMeshWriter()
{
}

bool GLMeshWriter::Begin(
    int numVerts, int prim, void* pResource)
{
    glModel* newModel;
    resource = pResource;
    count = numVerts;

    if (pResource != 0)
    {
        newModel = (glModel*)fn_802CC0A4(sizeof(glModel), 0, pResource);
    }
    else
    {
        newModel = (glModel*)fn_802CC0A8(sizeof(glModel), 0);
    }
    model = newModel;

    fn_802D38A4(
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
            positionData = fn_802CC0A4(positionCount * sizeof(float), 3, pResource);
        }
        else
        {
            positionData = fn_802CC0A8(positionCount * sizeof(float), 3);
        }
    }
    position = (float*)positionData;
    fn_802D39CC(streams, 0, position, sizeof(float) * 3, 1);

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
            texcoordData = (short*)fn_802CC0A4(
                texcoordCount * sizeof(short), 3, pResource);
        }
        else
        {
            texcoordData = (short*)fn_802CC0A8(texcoordCount * sizeof(short), 3);
        }
    }
    texcoord = texcoordData;
    fn_802D39CC(streams + 1, 1, texcoord, sizeof(short) * 2, 4);

    u32* colourData;
    if (numVerts == 0)
    {
        colourData = 0;
    }
    else
    {
        if (pResource != 0)
        {
            colourData = (u32*)fn_802CC0A4(
                numVerts * sizeof(u32), 3, pResource);
        }
        else
        {
            colourData = (u32*)fn_802CC0A8(numVerts * sizeof(u32), 3);
        }
    }
    colour = colourData;
    fn_802D39CC(streams + 2, 2, colour, sizeof(u32), 3);

    return true;
}

bool GLMeshWriter::End()
{
    u32 offset = 0;
    u32 i = 0;
    for (; i < model->numPackets; ++i)
    {
        glModelPacket* packet = (glModelPacket*)((u8*)model->packets + offset);
        fn_8036E438(packet, resource != 0);
        offset += sizeof(glModelPacket);
    }

    i = 0;
    offset = 0;
    for (; i < model->packets->numStreams; ++i)
    {
        glModelStream* stream = (glModelStream*)((u8*)model->packets->streams
                                                 + offset);
        DCStoreRangeNoSync(stream->address, count * stream->stride);
        offset += sizeof(glModelStream);
    }

    PPCSync();
    return true;
}
