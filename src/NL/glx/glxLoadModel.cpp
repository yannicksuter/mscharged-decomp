#include "NL/glx/glxLoadModel.h"

#include "Game/GL/GLInventory.h"
#include "Game/GL/ShaderSkinMesh.h"
#include "Game/SAnim.h"
#include "Game/SHierarchy.h"
#include "NL/gl/glLoadModel.h"
#include "NL/gl/glModel.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlRing.h"
#include "NL/platvmath.h"

#include <string.h>

extern "C"
{
    void* fn_802CC0A4(unsigned long size, int memoryType, void* allocator);
    void* fn_802CB7D0(void* resource);
    void fn_8036E438(glModelPacket* packet, bool permanent, void* allocator);
    void DCFlushRange(void* address, unsigned long size);
}

struct UnidentifiedLoadedModel
{
    glModel m_Model;
    unsigned char m_Unknown0C[0x28];
    glModel* m_pSourceModel;
};

class RLGReader_80369E5C : public RLGReader
{
public:
    RLGReader_80369E5C() { }

    virtual void fn_80369E5C(void* data, unsigned long size);
    virtual void fn_80369EC8(void* data, unsigned long size);
    virtual void fn_80369F34(nlChunk* chunk);
    virtual void fn_8036A138();
};

void RLGReader_80369E5C::fn_80369E5C(void* data, unsigned long size)
{
    m_Unknown1C = fn_802CC0A4(size, 3, m_pContext);
    memcpy(m_Unknown1C, data, size);
    DCFlushRange(m_Unknown1C, size);
}

void RLGReader_80369E5C::fn_80369EC8(void* data, unsigned long size)
{
    m_Unknown20 = fn_802CC0A4(size, 2, m_pContext);
    memcpy(m_Unknown20, data, size);
    DCFlushRange(m_Unknown20, size);
}

void RLGReader_80369E5C::fn_80369F34(nlChunk* chunk)
{
    fn_802CA870(chunk);
    void* data = chunk->GetData();
    unsigned long padding =
        (unsigned char*)chunk->GetAlignedData()
        - (unsigned char*)chunk->GetUnalignedData();
    DCFlushRange(data, chunk->GetSize() - padding);
}

static void fn_8036A038(glModelPacket* packet)
{
    unsigned char* boneIndices = 0;
    float* boneWeights = 0;
    unsigned long i;

    for (i = 0; i < packet->numStreams; ++i)
    {
        glModelStream* stream = &packet->streams[i];
        switch (stream->id)
        {
        case 7:
            boneIndices = (unsigned char*)stream->address;
            break;
        case 5:
            boneWeights = (float*)stream->address;
            break;
        }
    }

    for (i = 0; i < packet->numUniqueVertices; ++i)
    {
        float largestWeight = 0.0f;
        int largestIndex = 0;
        if (boneWeights[0] > largestWeight)
        {
            largestIndex = 0;
            largestWeight = boneWeights[0];
        }
        if (boneWeights[1] > largestWeight)
        {
            largestIndex = 1;
            largestWeight = boneWeights[1];
        }
        if (boneWeights[2] > largestWeight)
        {
            largestIndex = 2;
            largestWeight = boneWeights[2];
        }
        if (boneWeights[3] > largestWeight)
        {
            largestIndex = 3;
        }

        if (largestIndex != 0)
        {
            unsigned char firstIndex = boneIndices[0];
            float firstWeight = boneWeights[0];
            boneIndices[0] = boneIndices[largestIndex];
            boneWeights[0] = boneWeights[largestIndex];
            boneIndices[largestIndex] = firstIndex;
            boneWeights[largestIndex] = firstWeight;
        }

        boneIndices += 4;
        boneWeights += 4;
    }
}

static inline bool HasStream(
    const glModelPacket* packet, unsigned int streamID)
{
    for (unsigned long i = 0; i < packet->numStreams; ++i)
    {
        if (packet->streams[i].id == streamID)
        {
            return true;
        }
    }
    return false;
}

void RLGReader_80369E5C::fn_8036A138()
{
    for (unsigned long modelIndex = 0; modelIndex < m_nModels; ++modelIndex)
    {
        glModel* model = &m_pModels[modelIndex];
        for (unsigned long packetIndex = 0; packetIndex < model->numPackets;
            ++packetIndex)
        {
            glModelPacket* packet = &model->packets[packetIndex];
            packet->unknown10 = fn_802CB7D0(packet->unknown10);

            if (HasStream(packet, 7))
            {
                fn_8036A038(packet);
            }

            fn_8036E438(packet, true, m_pContext);
            ((UnidentifiedPacketResource*)packet->unknown10)->fn_Unknown3(packet);
        }

        UnidentifiedLoadedModel* loaded = (UnidentifiedLoadedModel*)
            m_pContext->m_pInventory->GetModel(model->unknown00);
        if (loaded != 0)
        {
            loaded->m_pSourceModel = model;
        }
    }
}

glModel* glplatEndLoadModel(
    void* data, unsigned long size, unsigned long* pNumModels, void* context)
{
    RLGReader_80369E5C reader;
    reader.m_pContext = (UnidentifiedLoadContext*)context;
    reader.fn_802CADC4(data);
    if (pNumModels != 0)
    {
        *pNumModels = reader.m_nModels;
    }
    return reader.m_pModels;
}

bool glplatBeginLoadModel(
    const char* filename, void (*callback)(void*, unsigned long, void*),
    void* userData)
{
    return nlLoadEntireFileAsync(filename, callback, userData, 32, AllocateEnd,
        0, 0, 0);
}

glModel* glplatLoadModel(
    const char* filename, unsigned long* pNumModels, void* context)
{
    unsigned long size;
    void* data =
        nlLoadEntireFile(filename, &size, 32, AllocateStart, 0, 0, 0);

    RLGReader_80369E5C reader;
    reader.m_pContext = (UnidentifiedLoadContext*)context;
    reader.fn_802CADC4(data);
    delete[] (unsigned char*)data;

    if (pNumModels != 0)
    {
        *pNumModels = reader.m_nModels;
    }
    return reader.m_pModels;
}

GLSkinMesh* glx_MakeSkinMesh(
    nlChunk* outerChunk, GLMaterialList* materialList, cSHierarchy* hierarchy)
{
    unsigned long i;
    unsigned long count;
    unsigned long chunkSize;
    unsigned char* unalignedData;
    unsigned char* data;

    ShaderSkinMesh* mesh =
        new (nlMalloc(sizeof(ShaderSkinMesh), 8, false)) ShaderSkinMesh();

    mesh->SetMaterialList(materialList);
    mesh->fn_8036FB74(hierarchy);

    nlChunk* chunk = outerChunk->GetFirstChunk();
    nlChunk* chunkEnd = outerChunk->GetNextChunk();
    while (chunk != chunkEnd)
    {
        unsigned long chunkType = chunk->GetID();
        unsigned char* result =
            (unsigned char*)chunk->GetAlignedData();
        unalignedData = (unsigned char*)chunk->GetUnalignedData();
        chunkSize = chunk->GetSize() - (result - unalignedData);
        data = (unsigned char*)chunk->GetData();

        switch (chunkType)
        {
        case 0x1B009:
            break;
        case 0x1B00A:
        {
            count = chunkSize / 0x44;
            for (i = 0; i < count; ++i)
            {
                unsigned long boneID = *(unsigned long*)data;
                nlMatrix4 src;
                nlMatrix4 inv;
                memcpy(&src, data + 4, sizeof(nlMatrix4));
                data += 0x44;
                nlInvertMatrix(inv, src);
                int nodeIndex = hierarchy->GetNodeIndexByID(boneID);
                if (nodeIndex != -1)
                {
                    mesh->fn_8036FC4C(nodeIndex, &inv);
                }
            }
            break;
        }
        case 0x1B00B:
        {
            count = chunkSize >> 2;
            BoneMapList* node =
                new (nlMalloc(sizeof(BoneMapList), 8, false)) BoneMapList();
            node->m_nBones = count;
            node->m_pBoneIndices =
                (int*)nlMalloc(count * sizeof(int), 8, false);
            node->m_pMatrices =
                (nlMatrix4*)nlMalloc(count * sizeof(nlMatrix4), 8, false);

            for (i = 0; i < count; ++i)
            {
                unsigned long boneID = *(unsigned long*)data;
                data += sizeof(unsigned long);
                node->m_pBoneIndices[i] =
                    hierarchy->GetNodeIndexByID(boneID);
            }
            nlRingAddEnd(&mesh->boneMaps, node);
            break;
        }
        case 0x1B00C:
        {
            unsigned long numMorphs = *(unsigned long*)data;
            data += sizeof(unsigned long);
            unsigned long* morphIDs = (unsigned long*)data;
            data += numMorphs * sizeof(unsigned long);

            mesh->fn_802D407C(numMorphs);
            for (unsigned long i = 0; i < numMorphs; ++i)
            {
                mesh->fn_802D40F4(i, morphIDs[i]);
            }

            unsigned long elementSize = *(unsigned long*)data;
            unsigned long numVertices = *(unsigned long*)(data + 4);
            data += 8;
            mesh->fn_802D407C(numMorphs);
            mesh->fn_8036F768(numVertices);

            for (unsigned long vertex = 0; vertex < numVertices; ++vertex)
            {
                for (unsigned long morph = 0; morph < numMorphs; ++morph)
                {
                    unsigned long count = *(unsigned long*)data;
                    data += sizeof(unsigned long);
                    const void* values = data;
                    data += elementSize * count;
                    mesh->fn_8036F7B0(vertex, morph, count, values);
                }
            }
            break;
        }
        }

        unsigned long nextOffset;
        unsigned char* nextChunk = unalignedData + chunk->GetSize();
        nextOffset = (unsigned long)nextChunk & 3;
        chunk = (nlChunk*)(nextChunk
            + (nextOffset != 0) * (4 - nextOffset));
    }

    mesh->fn_8036F7E4();
    return mesh;
}
