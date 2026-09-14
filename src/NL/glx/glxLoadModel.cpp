#include <revolution/os/OSCache_fwd.h>

#include "NL/glx/glxLoadModel.h"

#include "Game/GL/GLInventory.h"
#include "Game/GL/GLVertexAnim.h"
#include "Game/GL/ShaderSkinMesh.h"
#include "Game/SAnim.h"
#include "Game/SHierarchy.h"
#include "NL/gl/glLoadModel.h"
#include "NL/gl/glMaterialProgram.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlRing.h"
#include "NL/platvmath.h"

#include <string.h>


class GLXRLGReader : public RLGReader
{
public:
    GLXRLGReader() { }

    virtual void LoadVertices(void* data, unsigned long size);
    virtual void LoadIndices(void* data, unsigned long size);
    virtual void LoadVertexAnim(nlChunk* chunk);
    virtual void FinalizeModels();
};

void GLXRLGReader::LoadVertices(void* data, unsigned long size)
{
    m_pVertexData = glResourceAlloc(size, GLM_VertexData, m_pResource);
    memcpy(m_pVertexData, data, size);
    DCFlushRange(m_pVertexData, size);
}

void GLXRLGReader::LoadIndices(void* data, unsigned long size)
{
    m_pIndexData = glResourceAlloc(size, GLM_IndexData, m_pResource);
    memcpy(m_pIndexData, data, size);
    DCFlushRange(m_pIndexData, size);
}

void GLXRLGReader::LoadVertexAnim(nlChunk* chunk)
{
    LoadVertexAnimData(chunk);
    void* data = chunk->GetData();
    DCFlushRange(data, chunk->GetDataSize());
}

static void MoveLargestBoneWeightFirst(glModelPacket* packet)
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

void GLXRLGReader::FinalizeModels()
{
    for (unsigned long modelIndex = 0; modelIndex < m_nModels; ++modelIndex)
    {
        glModel* model = &m_pModels[modelIndex];
        for (unsigned long packetIndex = 0; packetIndex < model->numPackets;
            ++packetIndex)
        {
            glModelPacket* packet = &model->packets[packetIndex];
            packet->materialProgram =
                glGetMaterialProgram((unsigned long)packet->materialProgram);

            if (HasStream(packet, 7))
            {
                MoveLargestBoneWeightFirst(packet);
            }

            glplatFinalizePacket(packet, true, m_pResource);
            ((GLMaterialProgram*)packet->materialProgram)->Prepare(packet);
        }

        GLVertexAnim* loaded = m_pResource->m_inventory->GetVertexAnim(model->id);
        if (loaded != 0)
        {
            loaded->m_pModel = model;
        }
    }
}

glModel* glplatEndLoadModel(
    void* data, unsigned long size, unsigned long* pNumModels, void* context)
{
    GLXRLGReader reader;
    reader.m_pResource = (GLResourcePool*)context;
    reader.Read(data);
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

    GLXRLGReader reader;
    reader.m_pResource = (GLResourcePool*)context;
    reader.Read(data);
    delete[] (unsigned char*)data;

    if (pNumModels != 0)
    {
        *pNumModels = reader.m_nModels;
    }
    return reader.m_pModels;
}

GLSkinMesh* glx_MakeSkinMesh(
    nlChunk* outerChunk, glModel* models, cSHierarchy* hierarchy)
{
    unsigned long i;
    unsigned long num;
    unsigned long chunkSize;
    char* pData;

    ShaderSkinMesh* mesh =
        new (nlMalloc(sizeof(ShaderSkinMesh), 8, false)) ShaderSkinMesh();

    mesh->SetModel(models);
    mesh->SetHierarchy(hierarchy);

    nlChunk* pChunk = outerChunk->GetFirstChunk();
    nlChunk* chunkEnd = outerChunk->GetLastChunk();
    while (pChunk != chunkEnd)
    {
        unsigned long chunkType = pChunk->GetID();
        chunkSize = pChunk->GetDataSize();
        pData = (char*)pChunk->GetData();

        switch (chunkType)
        {
        case 0x1B009:
            break;
        case 0x1B00A:
        {
            num = chunkSize / 0x44;
            for (i = 0; i < num; ++i)
            {
                unsigned long boneid = *(unsigned long*)pData;
                nlMatrix4 m0;
                nlMatrix4 m1;
                memcpy(&m0, pData + 4, sizeof(nlMatrix4));
                pData += 0x44;
                nlInvertMatrix(m1, m0);
                int nodeIndex = hierarchy->GetNodeIndexByID(boneid);
                if (nodeIndex != -1)
                {
                    mesh->SetBoneMatrix(nodeIndex, &m1);
                }
            }
            break;
        }
        case 0x1B00B:
        {
            num = chunkSize >> 2;
            BoneMapList* node =
                new (nlMalloc(sizeof(BoneMapList), 8, false)) BoneMapList();
            node->m_nBones = num;
            node->m_pBoneIndices =
                (int*)nlMalloc(num * sizeof(int), 8, false);
            node->m_pMatrices =
                (nlMatrix4*)nlMalloc(num * sizeof(nlMatrix4), 8, false);

            for (i = 0; i < num; ++i)
            {
                unsigned long boneid = *(unsigned long*)pData;
                pData += sizeof(unsigned long);
                node->m_pBoneIndices[i] =
                    hierarchy->GetNodeIndexByID(boneid);
            }
            nlRingAddEnd(&mesh->boneMaps, node);
            break;
        }
        case 0x1B00C:
        {
            unsigned long numMorphs = *(unsigned long*)pData;
            pData += sizeof(unsigned long);
            unsigned long* morphIDs = (unsigned long*)pData;
            pData += numMorphs * sizeof(unsigned long);

            mesh->SetNumMorphs(numMorphs);
            for (unsigned long i = 0; i < numMorphs; ++i)
            {
                mesh->SetMorphID(i, morphIDs[i]);
            }

            unsigned long elementSize = *(unsigned long*)pData;
            unsigned long numPackets = *(unsigned long*)(pData + 4);
            pData += 8;
            mesh->SetNumMorphs(numMorphs);
            mesh->SetNumMorphPackets(numPackets);

            for (unsigned long packetIndex = 0; packetIndex < numPackets; ++packetIndex)
            {
                for (unsigned long morph = 0; morph < numMorphs; ++morph)
                {
                    unsigned long count = *(unsigned long*)pData;
                    pData += sizeof(unsigned long);
                    const MorphDelta* values = (const MorphDelta*)pData;
                    pData += elementSize * count;
                    mesh->SetMorphDeltas(packetIndex, morph, count, values);
                }
            }
            break;
        }
        }

        pChunk = pChunk->GetNextChunk();
    }

    mesh->InitializeSkinData();
    return mesh;
}
