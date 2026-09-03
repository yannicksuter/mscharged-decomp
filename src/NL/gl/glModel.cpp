#include "NL/gl/glModel.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glMemory.h"

#include <string.h>

struct glModelPacketDataInfo
{
    /* 0x00 */ u8 unknown00[8];
    /* 0x08 */ u32 size;
}; // size: 0xC

extern "C"
{
    void fn_802C8284(unsigned long value);
    void fn_802C8288();
    void fn_8036E4C0(glModelPacket* packet, void* allocator);
}

void glModelSetMatrix(glModel* model, const nlMatrix4& value)
{
    unsigned long matrix = glAllocMatrix();
    if (matrix != 0xFFFFFFFF)
        glSetMatrix(matrix, value);

    unsigned long i = 0;
    unsigned long offset = 0;
    while (i < model->numPackets)
    {
        glModelPacket* packet = (glModelPacket*)((u8*)model->packets + offset);
        packet->matrix = matrix;
        ++i;
        offset += sizeof(glModelPacket);
    }
}

void glModelSetMatrix(glModel* model, unsigned long matrix)
{
    unsigned long offset;
    unsigned long i;
    i = 0;
    offset = 0;
    while (i < model->numPackets)
    {
        glModelPacket* packet = (glModelPacket*)((u8*)model->packets + offset);
        packet->matrix = matrix;
        ++i;
        offset += sizeof(glModelPacket);
    }
}

void glModelSetRasterState(glModel* model, unsigned long rasterState)
{
    unsigned long offset;
    unsigned long i;
    i = 0;
    offset = 0;
    while (i < model->numPackets)
    {
        glModelPacket* packet = (glModelPacket*)((u8*)model->packets + offset);
        packet->rasterState = rasterState;
        ++i;
        offset += sizeof(glModelPacket);
    }
}

void glModelGetMatrix(const glModel* model, nlMatrix4& matrix)
{
    glGetMatrix(model->packets->matrix, matrix);
}

glModel* glModelDupArrayNoStreams(
    const glModel* pModelArray, unsigned long nModels, bool bPermanent,
    void* pAllocator)
{
    glModel* result;
    glModelPacket* new_packets;
    void* data;
    void* source;
    unsigned long dataSize;
    glModelPacket* sourcePackets;
    glModel* dst_model;
    unsigned long packetOffset;
    unsigned long size;
    unsigned long packetSize;
    unsigned long j;
    glModelPacket* packet;
    glModelPacketDataInfo* info;
    unsigned long numPackets;
    int i;

    fn_802C8284(pModelArray->unknown00);

    if (bPermanent)
    {
        size = nModels * sizeof(glModel);
        result = (glModel*)glResourceAlloc(size, GLM_Header, pAllocator);
    }
    else
    {
        size = nModels * sizeof(glModel);
        result = (glModel*)glFrameAlloc(size, GLM_Header);
    }

    memcpy(result, pModelArray, size);

    dst_model = result;
    i = 0;
    while (i < (int)nModels)
    {
        numPackets = dst_model->numPackets;
        sourcePackets = dst_model->packets;
        if (bPermanent)
        {
            packetSize = numPackets * sizeof(glModelPacket);
            new_packets = (glModelPacket*)glResourceAlloc(
                packetSize, GLM_Header, pAllocator);
        }
        else
        {
            packetSize = numPackets * sizeof(glModelPacket);
            new_packets = (glModelPacket*)glFrameAlloc(
                packetSize, GLM_Header);
        }

        memcpy(new_packets, sourcePackets, packetSize);
        dst_model->packets = new_packets;

        j = 0;
        packetOffset = 0;
        while (j < dst_model->numPackets)
        {
            packet = (glModelPacket*)((u8*)dst_model->packets + packetOffset);
            info = (glModelPacketDataInfo*)packet->unknown10;
            source = packet->unknown20;
            dataSize = info->size;
            if (bPermanent)
            {
                data = glResourceAlloc(dataSize, GLM_Header, pAllocator);
            }
            else
            {
                data = glFrameAlloc(dataSize, GLM_Header);
            }

            memcpy(data, source, dataSize);
            packet->unknown20 = data;
            fn_8036E4C0(
                (glModelPacket*)((u8*)dst_model->packets + packetOffset),
                pAllocator);

            packetOffset += sizeof(glModelPacket);
            ++j;
        }

        ++dst_model;
        ++i;
    }

    fn_802C8288();
    return result;
}

glModel* glModelDupNoStreams(
    const glModel* pModel, bool bPermanent, void* pAllocator)
{
    return glModelDupArrayNoStreams(pModel, 1, bPermanent, pAllocator);
}
