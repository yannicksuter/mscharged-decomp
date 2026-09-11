#include <revolution/os/OSCache.h>

#include "Game/GL/ShaderSkinMesh.h"

#include "Game/SHierarchy.h"
#include "Game/GL/glModelBuilder.h"
#include "NL/gl/glMemory.h"
#include <string.h>
#include "Game/PoseAccumulator.h"
#include "NL/nlMemory.h"
#include "NL/platvmath.h"
#include "NL/nlDLRing.h"
#include "NL/nlString.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/glx/glxMatrix.h"

static const unsigned long SkinMatricesHash = nlStringLowerHash("SkinMatrices");

ShaderSkinMesh::~ShaderSkinMesh()
{
    nlDeleteRing(&boneMaps);
    if (boneMatrices != 0)
    {
        delete[] boneMatrices;
    }
    if (packetSkinData != 0)
    {
        delete[] packetSkinData;
    }
    if (poseMatrices != 0)
    {
        delete[] poseMatrices;
    }
    if (morphData != 0)
    {
        delete[] morphData;
    }
}

void ShaderSkinMesh::SetNumMorphPackets(unsigned long count)
{
    numPackets = count;
    morphData = (MorphDeltaList*)nlMalloc(
        count * numMorphs * sizeof(MorphDeltaList), 8, false);
}

void ShaderSkinMesh::SetMorphDeltas(unsigned long packetIndex,
    unsigned long morphIndex, unsigned long count, const MorphDelta* data)
{
    MorphDeltaList* entry = morphData == 0
                               ? 0
                               : morphData + (packetIndex * numMorphs + morphIndex);

    entry->numDeltas = count;
    entry->deltas = data;
}

void ShaderSkinMesh::InitializeSkinData()
{
    packetSkinData = new (8, false)
        PacketSkinData[pModel->numPackets];

    glModelPacket* pPacket = pModel->packets;
    BoneMapList* node = nlDLRingGetStart(boneMaps);
    rigidSkin = true;

    for (int i = 0; i < (int)pModel->numPackets; i++, pPacket++)
    {
        float (*pMatrices)[3][4] = (float (*)[3][4])node->m_pMatrices;
        for (unsigned long j = 0; j < node->m_nBones; j++)
        {
            glxCopyMatrix(*pMatrices, poseMatrices[node->m_pBoneIndices[j]]);
            pMatrices++;
        }

        glSetMaterialBufferParameter(pPacket, SkinMatricesHash,
            (unsigned long)node->m_pMatrices, node->m_nBones * sizeof(*pMatrices));
        PacketSkinData* data = &packetSkinData[i];
        if (data->numBones == 0)
        {
            BuildPacketSkinData(data, pPacket, node);
        }

        node = node->m_next;
    }
}

void ShaderSkinMesh::BuildPacketSkinData(
    PacketSkinData* data, glModelPacket* pPacket,
    BoneMapList* node)
{
    unsigned long numVertices = pPacket->numUniqueVertices;
    unsigned long numBones = node->m_nBones;
    SkinWeight* pairs =
        (SkinWeight*)nlMalloc(
            numVertices * 2 * sizeof(SkinWeight), 8, false);
    data->numVertices = numVertices;
    data->numBones = numBones;
    data->boneWeights = new (8, false)
        BoneSkinWeights[numBones];

    glModelStream* indexStream = glFindModelStream(pPacket, 7);
    glModelStream* weightStream = glFindModelStream(pPacket, 5);
    const unsigned char (*indices)[4] = (const unsigned char (*)[4])indexStream->address;
    const float (*weights)[4] = (const float (*)[4])weightStream->address;

    for (unsigned long i = 0; i < numBones; i++)
    {
        BoneSkinWeights* entry = &data->boneWeights[i];
        const unsigned char (*pIndices)[4] = indices;
        const float (*pWeights)[4] = weights;
        SkinWeight* pPair = pairs;
        int numPairs = 0;
        for (unsigned long j = 0; j < numVertices; j++)
        {
            if (0.0f != (*pWeights)[1])
            {
                rigidSkin = false;
            }
            for (unsigned long k = 0; k < 4; k++)
            {
                float weight = (*pWeights)[k];
                if (0.0f == weight)
                {
                    break;
                }
                if (i == (*pIndices)[k])
                {
                    pPair->vertexIndex = j;
                    pPair->vertexWeight = weight;
                    pPair++;
                    numPairs++;
                }
            }
            pIndices++;
            pWeights++;
        }
        if (numPairs > 0)
        {
            entry->numWeights = numPairs;
            entry->weights = (SkinWeight*)nlMalloc(
                numPairs * sizeof(SkinWeight), 8, false);
            memcpy(entry->weights, pairs,
                numPairs * sizeof(SkinWeight));
        }
    }
    delete[] pairs;
}

void ShaderSkinMesh::SetHierarchy(cSHierarchy* hierarchy)
{
    hierarchySignature = hierarchy->GetHashID();
    numBones = hierarchy->GetNumNodes();

    poseMatrices = (nlMatrix4*)nlMalloc(
        numBones * sizeof(nlMatrix4), 8, false);
    for (int i = 0; i < hierarchy->GetNumNodes(); i++)
    {
        poseMatrices[i].SetIdentity();
    }

    boneMatrices = (nlMatrix4*)nlMalloc(
        numBones * sizeof(nlMatrix4), 8, false);
    for (int i = 0; i < hierarchy->GetNumNodes(); i++)
    {
        boneMatrices[i].SetIdentity();
    }
}

void ShaderSkinMesh::SetBoneMatrix(int nodeIndex, const nlMatrix4* matrix)
{
    boneMatrices[nodeIndex] = *matrix;
}

void ShaderSkinMesh::Pose(cPoseAccumulator* pPoseAccumulator)
{
    for (int i = 0; i < pPoseAccumulator->GetNumNodes(); i++)
    {
        nlMultMatrices(poseMatrices[i], boneMatrices[i],
            pPoseAccumulator->GetNodeMatrix(i));
    }

    GLSkinMesh::UpdateMorphWeights(pPoseAccumulator);
}

void ShaderSkinMesh::GetPoseMatrix(nlMatrix4* matrix, int nodeIndex)
{
    *matrix = poseMatrices[nodeIndex];
}

void ShaderSkinMesh::PrepareToRender()
{
    glModelPacket* pPacket = pModel->packets;
    BoneMapList* node = nlDLRingGetStart(boneMaps);
    for (int i = 0; i < (int)pModel->numPackets; i++, pPacket++)
    {
        float (*pMatrices)[3][4] = (float (*)[3][4])node->m_pMatrices;
        for (unsigned long j = 0; j < node->m_nBones; j++)
        {
            glxCopyMatrix(*pMatrices, poseMatrices[node->m_pBoneIndices[j]]);
            pMatrices++;
        }
        glSetMaterialBufferParameter(pPacket, SkinMatricesHash,
            (unsigned long)node->m_pMatrices, node->m_nBones * sizeof(*pMatrices));
        node = node->m_next;
    }

    if (numActiveMorphs != 0)
    {
        unsigned long count = 0;
        for (unsigned long i = 0; i < pModel->numPackets; i++)
        {
            count = count >= packetSkinData[i].numVertices
                        ? count : packetSkinData[i].numVertices;
        }
        morphBuffer = (nlVector3*)glFrameAlloc(
            count * sizeof(nlVector3), GLM_VertexData);
    }

    if (!rigidSkin && m_Unknown0C == 0)
    {
        SoftwareSkinModel(pModel);
        glModel* newModel = (glModel*)glFrameAlloc(sizeof(glModel), GLM_Header);
        softwareModel = newModel;
        memcpy(newModel, pModel, sizeof(glModel));
        unsigned long numPackets = newModel->numPackets;
        glModelPacket* pPackets = (glModelPacket*)glFrameAlloc(
            numPackets * sizeof(glModelPacket), GLM_Header);
        memcpy(pPackets, newModel->packets, numPackets * sizeof(glModelPacket));
        glSetModelPackets(newModel, pPackets, numPackets);

        for (unsigned long i = 0; i < newModel->numPackets; i++)
        {
            glModelPacket* pPacket = &newModel->packets[i];
            unsigned long numStreams = pPacket->numStreams;
            glModelStream* pStreams = (glModelStream*)glFrameAlloc(
                numStreams * sizeof(glModelStream), GLM_Header);
            memcpy(pStreams, pPacket->streams, numStreams * sizeof(glModelStream));
            glSetModelStream(pStreams, pStreams->index,
                (void*)pPacket->skinnedVertices, pStreams->stride, pStreams->id);
            if (pPacket->skinnedNormals != 0)
            {
                glModelStream* pStream = pStreams + 1;
                for (unsigned long j = 1; j < pPacket->numStreams; j++)
                {
                    if (pStream->id == 2)
                    {
                        glSetModelStream(pStream, pStream->index,
                            (void*)pPacket->skinnedNormals, sizeof(nlVector3), 2);
                        break;
                    }
                }
            }
            glSetPacketStreams(pPacket, pStreams, numStreams);
        }
    }
    else
    {
        for (unsigned long i = 0; i < pModel->numPackets; i++)
        {
            pModel->packets[i].skinnedVertices = 0;
        }
    }
}

void ShaderSkinMesh::CreateMorphBuffer(unsigned long packetIndex,
    unsigned long count)
{
    if (numActiveMorphs != 0)
    {
        nlZeroMemory(morphBuffer, count * sizeof(nlVector3));
        for (unsigned long morphIndex = 0; morphIndex < numMorphs; morphIndex++)
        {
            float w = morphWeights[morphIndex].morphWeight;
            if (0.0f != w)
            {
                const MorphDeltaList* entry =
                    morphData == 0 ? 0
                        : morphData
                            + (packetIndex * numMorphs + morphIndex);
                for (unsigned long i = 0; i < entry->numDeltas; i++)
                {
                    const MorphDelta* pCurrentMorph = &entry->deltas[i];
                    nlVector3* dst = &morphBuffer[pCurrentMorph->index];
                    nlVec3ScaleAdd(*dst, w, pCurrentMorph->delta, *dst);
                }
            }
        }
    }
}

void ShaderSkinMesh::SoftwareSkinModel(glModel* model)
{
    nlMatrix4 tempMatrices[32];
    BoneMapList* node = nlDLRingGetStart(boneMaps);
    glModelPacket* pPacket = model->packets;

    for (unsigned long packetIndex = 0; packetIndex < model->numPackets;
         packetIndex++, pPacket++)
    {
        PacketSkinData* data = &packetSkinData[packetIndex];
        unsigned long numVertices = data->numVertices;
        unsigned long size = numVertices * sizeof(nlVector3);
        nlVector3* outVertices = (nlVector3*)glFrameAlloc(size, GLM_VertexData);
        nlZeroMemory(outVertices, size);
        pPacket->skinnedVertices = (unsigned long)outVertices;

        nlVector3* outNormals = 0;
        glModelStream* pStream = glFindModelStream(pPacket, 2);
        const nlVector3* inNormals = 0;
        if (pStream != 0)
        {
            outNormals = (nlVector3*)glFrameAlloc(size, GLM_VertexData);
            nlZeroMemory(outNormals, size);
            inNormals = (const nlVector3*)pStream->address;
            pPacket->skinnedNormals = (unsigned long)outNormals;
        }

        CreateMorphBuffer(packetIndex, numVertices);
        const nlVector3* inVertices = (const nlVector3*)glFindModelStream(pPacket, 1)->address;
        const float (*pMatrices)[3][4] =
            *(const float (**)[3][4])glGetMaterialParameterData(pPacket, SkinMatricesHash);
        for (unsigned long i = 0; i < node->m_nBones; i++)
        {
            glxCopyMatrix(tempMatrices[i], *pMatrices);
            pMatrices++;
        }

        for (unsigned long matrixOffset = 0; matrixOffset < data->numBones; matrixOffset++)
        {
            BoneSkinWeights* curr = &data->boneWeights[matrixOffset];
            register const nlMatrix4* pMatrix = &tempMatrices[matrixOffset];
            // clang-format off
            asm {
                psq_l f2, 0x0(pMatrix), 0, 0
                psq_l f3, 0x8(pMatrix), 0, 0
                psq_l f4, 0x10(pMatrix), 0, 0
                psq_l f5, 0x18(pMatrix), 0, 0
                psq_l f6, 0x20(pMatrix), 0, 0
                psq_l f7, 0x28(pMatrix), 0, 0
                psq_l f8, 0x30(pMatrix), 0, 0
                psq_l f9, 0x38(pMatrix), 0, 0
            }
            // clang-format on

            if (numActiveMorphs != 0)
            {
                for (unsigned long i = 0; i < curr->numWeights; i++)
                {
                    const SkinWeight& pair = curr->weights[i];
                    float vertexWeight = pair.vertexWeight;
                    unsigned long index = pair.vertexIndex;
                    register const nlVector3& inVertex = inVertices[index];
                    register const nlVector3& morph = morphBuffer[index];
                    register nlVector3& outVertex = outVertices[index];
                    // clang-format off
                    asm {
                        lfs f12, vertexWeight
                        psq_l f0, 0x0(inVertex), 0, 0
                        psq_l f10, 0x0(morph), 0, 0
                        psq_l f1, 0x8(inVertex), 1, 0
                        ps_add f0, f0, f10
                        psq_l f11, 0x8(morph), 1, 0
                        ps_add f1, f1, f11
                        ps_muls0 f10, f2, f0
                        ps_muls0 f11, f3, f0
                        ps_madds1 f10, f4, f0, f10
                        ps_madds1 f11, f5, f0, f11
                        psq_l f0, 0x0(outVertex), 0, 0
                        ps_madds0 f10, f6, f1, f10
                        ps_madds0 f11, f7, f1, f11
                        psq_l f1, 0x8(outVertex), 1, 0
                        ps_add f10, f8, f10
                        ps_add f11, f9, f11
                        ps_madds0 f10, f10, f12, f0
                        ps_madds0 f11, f11, f12, f1
                        psq_st f10, 0x0(outVertex), 0, 0
                        psq_st f11, 0x8(outVertex), 1, 0
                    }
                    // clang-format on
                }
            }
            else
            {
                for (unsigned long i = 0; i < curr->numWeights; i++)
                {
                    const SkinWeight& pair = curr->weights[i];
                    float vertexWeight = pair.vertexWeight;
                    unsigned long index = pair.vertexIndex;
                    register const nlVector3& inVertex = inVertices[index];
                    register nlVector3& outVertex = outVertices[index];
                    // clang-format off
                    asm {
                        lfs f12, vertexWeight
                        psq_l f0, 0x0(inVertex), 0, 0
                        psq_l f1, 0x8(inVertex), 1, 0
                        ps_muls0 f10, f2, f0
                        ps_muls0 f11, f3, f0
                        ps_madds1 f10, f4, f0, f10
                        ps_madds1 f11, f5, f0, f11
                        psq_l f0, 0x0(outVertex), 0, 0
                        ps_madds0 f10, f6, f1, f10
                        ps_madds0 f11, f7, f1, f11
                        psq_l f1, 0x8(outVertex), 1, 0
                        ps_add f10, f8, f10
                        ps_add f11, f9, f11
                        ps_madds0 f10, f10, f12, f0
                        ps_madds0 f11, f11, f12, f1
                        psq_st f10, 0x0(outVertex), 0, 0
                        psq_st f11, 0x8(outVertex), 1, 0
                    }
                    // clang-format on
                }
            }

            if (outNormals != 0)
            {
                for (unsigned long i = 0; i < curr->numWeights; i++)
                {
                    const SkinWeight& pair = curr->weights[i];
                    float vertexWeight = pair.vertexWeight;
                    unsigned long index = pair.vertexIndex;
                    register const nlVector3& inNormal = inNormals[index];
                    register nlVector3& outNormal = outNormals[index];
                    // clang-format off
                    asm {
                        lfs f12, vertexWeight
                        psq_l f0, 0x0(inNormal), 0, 0
                        psq_l f1, 0x8(inNormal), 1, 0
                        ps_muls0 f10, f2, f0
                        ps_muls0 f11, f3, f0
                        ps_madds1 f10, f4, f0, f10
                        ps_madds1 f11, f5, f0, f11
                        psq_l f0, 0x0(outNormal), 0, 0
                        ps_madds0 f10, f6, f1, f10
                        ps_madds0 f11, f7, f1, f11
                        psq_l f1, 0x8(outNormal), 1, 0
                        ps_madds0 f10, f10, f12, f0
                        ps_madds0 f11, f11, f12, f1
                        psq_st f10, 0x0(outNormal), 0, 0
                        psq_st f11, 0x8(outNormal), 1, 0
                    }
                    // clang-format on
                }
            }
        }

        DCFlushRange(outVertices, size);
        if (outNormals != 0)
        {
            DCFlushRange(outNormals, size);
        }
        node = node->m_next;
    }
}

PacketSkinData::~PacketSkinData()
{
    delete[] boneWeights;
}

BoneSkinWeights::~BoneSkinWeights()
{
    delete[] weights;
}

PacketSkinData::PacketSkinData()
    : numVertices(0)
    , numBones(0)
    , boneWeights(0)
{
}

BoneSkinWeights::BoneSkinWeights()
    : numWeights(0)
    , weights(0)
{
}

glModel* ShaderSkinMesh::GetModel()
{
    bool softwareSkinning = false;
    if (!rigidSkin && m_Unknown0C == 0)
    {
        softwareSkinning = true;
    }
    if (softwareSkinning)
    {
        return softwareModel;
    }
    return pModel;
}
