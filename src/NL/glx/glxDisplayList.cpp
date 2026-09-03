#include "NL/glx/glxDisplayList.h"

#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#define DISPLAY_LIST_HEADER 0xBA7EF00D

extern "C"
{
    void DCFlushRangeNoSync(void* address, unsigned long size);
    void PPCSync();

    extern const unsigned long lbl_80535588[];
}

static unsigned char opcodes[6] = {
    0x90,
    0x98,
    0xA0,
    0x80,
    0xA8,
    0xB0,
};

DisplayList* dlMakeDisplayList(
    const glModelPacket* packet, void* allocator, bool permanent)
{
    DisplayList* pList;
    unsigned long actualSize;
    unsigned long size;
    unsigned char* p;
    unsigned long i;
    unsigned long numStreams;
    unsigned char bStitch;
    unsigned char* stitchIndices;

    static const unsigned long skinMatricesHash = nlStringLowerHash("SkinMatrices");

    if (packet->indexBuffer == 0)
        return 0;

    numStreams = 0;
    bStitch = 0;
    stitchIndices = 0;
    i = 0;
    unsigned long packetStreams = packet->numStreams;
    for (unsigned long streamIndex = 0; streamIndex < packetStreams;
        ++streamIndex, i += sizeof(glModelStream))
    {
        unsigned char* stream = (unsigned char*)packet->streams + i;
        int streamID = stream[6];
        if ((unsigned long)(streamID - 1) <= 3)
        {
            ++numStreams;
        }
        else if (streamID == 7)
        {
            bStitch = 1;
            stitchIndices = *(unsigned char**)stream;
        }
    }

    if (numStreams == 0)
        return 0;

    actualSize = (numStreams << 1) * packet->numVertices + 3;
    if (bStitch)
        actualSize += packet->numVertices;

    unsigned long remainder = actualSize & 0x1F;
    size = actualSize + (remainder != 0) * (0x20 - remainder);

    if (permanent)
    {
        p = (unsigned char*)glResourceAlloc(
            size, GLM_VertexData, allocator);
    }
    else
    {
        p = (unsigned char*)glFrameAlloc(size, GLM_VertexData);
    }

    nlZeroMemory(p + actualSize, size - actualSize);

    p[0] = opcodes[(unsigned char)packet->primType];
    *(unsigned short*)(p + 1) = packet->numVertices;

    unsigned char* p8 = p + 3;
    unsigned char hasColor = bStitch;
    unsigned short* pInd = packet->indexBuffer;

    if (hasColor)
    {
        i = 0;
        while (i < packet->numVertices)
        {
            unsigned char stitchIndex = stitchIndices[*pInd * 4];
            *p8++ = (unsigned char)lbl_80535588[stitchIndex];

            for (unsigned long j = 0; j < numStreams; ++j)
            {
                *(unsigned short*)p8 = *pInd;
                p8 += 2;
            }

            ++i;
            ++pInd;
        }
    }
    else
    {
        i = 0;
        while (i < packet->numVertices)
        {
            for (unsigned long j = 0; j < numStreams; ++j)
            {
                *(unsigned short*)p8 = *pInd;
                p8 += 2;
            }

            ++i;
            ++pInd;
        }
    }

    if (permanent)
    {
        pList = (DisplayList*)glResourceAlloc(
            sizeof(DisplayList), GLM_Header, allocator);
    }
    else
    {
        pList = (DisplayList*)glFrameAlloc(
            sizeof(DisplayList), GLM_Header);
    }

    pList->magic = DISPLAY_LIST_HEADER;
    pList->list = p;
    pList->size = size;
    pList->numStreams = (unsigned short)numStreams;
    pList->hasColorStream = bStitch;

    DCFlushRangeNoSync(p, size);
    PPCSync();

    return pList;
}
