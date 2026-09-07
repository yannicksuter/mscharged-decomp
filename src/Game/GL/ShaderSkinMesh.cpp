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
#include "NL/gl/tu_802CC370.h"
#include "NL/glx/glxMatrix.h"

static const unsigned long lbl_806E2400 = nlStringLowerHash("SkinMatrices");
extern "C" void fn_8036F91C(ShaderSkinMesh*,
    UnidentifiedShaderSkinData_80370808*, glModelPacket*, BoneMapList*);

extern "C" void fn_80370194(ShaderSkinMesh*, glModel*);
extern "C" void fn_80370088(ShaderSkinMesh*, unsigned long, unsigned long);

ShaderSkinMesh::~ShaderSkinMesh()
{
    nlDeleteRing(&boneMaps);
    if (m_Unknown2C != 0)
    {
        delete[] m_Unknown2C;
    }
    if (m_Unknown24 != 0)
    {
        delete[] m_Unknown24;
    }
    if (m_Unknown30 != 0)
    {
        delete[] m_Unknown30;
    }
    if (m_Unknown3C != 0)
    {
        delete[] (unsigned long*)m_Unknown3C;
    }
}

void ShaderSkinMesh::fn_8036F768(unsigned long count)
{
    m_Unknown38 = count;
    m_Unknown3C = nlMalloc(count * numMorphs * 8, 8, false);
}

void ShaderSkinMesh::fn_8036F7B0(unsigned long firstIndex,
    unsigned long secondIndex, unsigned long count, const void* data)
{
    unsigned long* entry = m_Unknown3C == 0
                             ? 0
                             : (unsigned long*)m_Unknown3C
                                   + (firstIndex * numMorphs + secondIndex) * 2;

    entry[0] = count;
    entry[1] = (unsigned long)data;
}

void ShaderSkinMesh::fn_8036F7E4()
{
    m_Unknown24 = new (8, false)
        UnidentifiedShaderSkinData_80370808[pModel->numPackets];

    glModelPacket* pPacket = pModel->packets;
    BoneMapList* node = nlDLRingGetStart(boneMaps);
    m_Unknown45 = true;

    for (int i = 0; i < (int)pModel->numPackets; i++, pPacket++)
    {
        float (*pMatrices)[3][4] = (float (*)[3][4])node->m_pMatrices;
        for (unsigned long j = 0; j < node->m_nBones; j++)
        {
            glxCopyMatrix(*pMatrices, m_Unknown30[node->m_pBoneIndices[j]]);
            pMatrices++;
        }

        fn_802CC59C(pPacket, lbl_806E2400, (unsigned long)node->m_pMatrices,
            node->m_nBones * sizeof(*pMatrices));
        UnidentifiedShaderSkinData_80370808* data = &m_Unknown24[i];
        if (data->m_Unknown04 == 0)
        {
            fn_8036F91C(this, data, pPacket, node);
        }

        node = node->m_next;
    }
}

struct UnidentifiedShaderSkinWeight_8036F91C
{
    unsigned long vertexIndex;
    float vertexWeight;
};

extern "C" void fn_8036F91C(ShaderSkinMesh* mesh,
    UnidentifiedShaderSkinData_80370808* data, glModelPacket* pPacket,
    BoneMapList* node)
{
    unsigned long numVertices = pPacket->numUniqueVertices;
    unsigned long numBones = node->m_nBones;
    UnidentifiedShaderSkinWeight_8036F91C* pairs =
        (UnidentifiedShaderSkinWeight_8036F91C*)nlMalloc(
            numVertices * 2 * sizeof(UnidentifiedShaderSkinWeight_8036F91C), 8, false);
    data->m_Unknown00 = numVertices;
    data->m_Unknown04 = numBones;
    data->m_Unknown08 = new (8, false)
        UnidentifiedShaderSkinEntry_80370868[numBones];

    glModelStream* indexStream = fn_8036F99C(pPacket, 7);
    glModelStream* weightStream = fn_8036F99C(pPacket, 5);
    const unsigned char (*indices)[4] = (const unsigned char (*)[4])indexStream->address;
    const float (*weights)[4] = (const float (*)[4])weightStream->address;

    for (unsigned long i = 0; i < numBones; i++)
    {
        UnidentifiedShaderSkinEntry_80370868* entry = &data->m_Unknown08[i];
        const unsigned char (*pIndices)[4] = indices;
        const float (*pWeights)[4] = weights;
        UnidentifiedShaderSkinWeight_8036F91C* pPair = pairs;
        int numPairs = 0;
        for (unsigned long j = 0; j < numVertices; j++)
        {
            if (0.0f != (*pWeights)[1])
            {
                mesh->m_Unknown45 = false;
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
            entry->m_Unknown00 = numPairs;
            entry->m_Unknown04 = (unsigned char*)nlMalloc(
                numPairs * sizeof(UnidentifiedShaderSkinWeight_8036F91C), 8, false);
            memcpy(entry->m_Unknown04, pairs,
                numPairs * sizeof(UnidentifiedShaderSkinWeight_8036F91C));
        }
    }
    delete[] pairs;
}

void ShaderSkinMesh::fn_8036FB74(cSHierarchy* hierarchy)
{
    m_Unknown08 = hierarchy->GetHashID();
    m_Unknown34 = hierarchy->GetNumNodes();

    m_Unknown30 = (nlMatrix4*)nlMalloc(
        m_Unknown34 * sizeof(nlMatrix4), 8, false);
    for (int i = 0; i < hierarchy->GetNumNodes(); i++)
    {
        m_Unknown30[i].SetIdentity();
    }

    m_Unknown2C = (nlMatrix4*)nlMalloc(
        m_Unknown34 * sizeof(nlMatrix4), 8, false);
    for (int i = 0; i < hierarchy->GetNumNodes(); i++)
    {
        m_Unknown2C[i].SetIdentity();
    }
}

void ShaderSkinMesh::fn_8036FC4C(int nodeIndex, const nlMatrix4* matrix)
{
    m_Unknown2C[nodeIndex] = *matrix;
}

void ShaderSkinMesh::Pose(cPoseAccumulator* pPoseAccumulator)
{
    for (int i = 0; i < pPoseAccumulator->GetNumNodes(); i++)
    {
        nlMultMatrices(m_Unknown30[i], m_Unknown2C[i],
            pPoseAccumulator->GetNodeMatrix(i));
    }

    GLSkinMesh::fn_802D4104(pPoseAccumulator);
}

void ShaderSkinMesh::fn_Unknown5(nlMatrix4* matrix, int nodeIndex)
{
    *matrix = m_Unknown30[nodeIndex];
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
            glxCopyMatrix(*pMatrices, m_Unknown30[node->m_pBoneIndices[j]]);
            pMatrices++;
        }
        fn_802CC59C(pPacket, lbl_806E2400, (unsigned long)node->m_pMatrices,
            node->m_nBones * sizeof(*pMatrices));
        node = node->m_next;
    }

    if (m_Unknown18 != 0)
    {
        unsigned long count = 0;
        for (unsigned long i = 0; i < pModel->numPackets; i++)
        {
            count = count >= m_Unknown24[i].m_Unknown00
                        ? count : m_Unknown24[i].m_Unknown00;
        }
        m_Unknown40 = glFrameAlloc(count * sizeof(nlVector3), GLM_VertexData);
    }

    if (!m_Unknown45 && m_Unknown0C == 0)
    {
        fn_80370194(this, pModel);
        glModel* newModel = (glModel*)glFrameAlloc(sizeof(glModel), GLM_Header);
        m_Unknown28 = newModel;
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
            glSetModelStream(pStreams, pStreams->unknown04,
                (void*)pPacket->unknown28, pStreams->stride, pStreams->id);
            if (pPacket->unknown2C != 0)
            {
                glModelStream* pStream = pStreams + 1;
                for (unsigned long j = 1; j < pPacket->numStreams; j++)
                {
                    if (pStream->id == 2)
                    {
                        glSetModelStream(pStream, pStream->unknown04,
                            (void*)pPacket->unknown2C, sizeof(nlVector3), 2);
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
            pModel->packets[i].unknown28 = 0;
        }
    }
}

struct MorphDelta
{
    nlVector3 delta;
    int index;
};

struct UnidentifiedShaderSkinMorph_80370088
{
    unsigned long m_Unknown00;
    const MorphDelta* m_Unknown04;
};

extern "C" void fn_80370088(ShaderSkinMesh* mesh, unsigned long firstIndex,
    unsigned long count)
{
    if (mesh->m_Unknown18 != 0)
    {
        nlZeroMemory(mesh->m_Unknown40, count * sizeof(nlVector3));
        for (unsigned long morphIndex = 0; morphIndex < mesh->numMorphs; morphIndex++)
        {
            float w = mesh->m_Unknown10[morphIndex].morphWeight;
            if (0.0f != w)
            {
                const UnidentifiedShaderSkinMorph_80370088* entry =
                    mesh->m_Unknown3C == 0 ? 0
                        : (UnidentifiedShaderSkinMorph_80370088*)mesh->m_Unknown3C
                            + (firstIndex * mesh->numMorphs + morphIndex);
                for (unsigned long i = 0; i < entry->m_Unknown00; i++)
                {
                    const MorphDelta* pCurrentMorph = &entry->m_Unknown04[i];
                    nlVector3* dst = &((nlVector3*)mesh->m_Unknown40)[pCurrentMorph->index];
                    nlVec3ScaleAdd(*dst, w, pCurrentMorph->delta, *dst);
                }
            }
        }
    }
}

// clang-format off
extern "C" void fn_80370194(ShaderSkinMesh* mesh, glModel* model)
{
    nlMatrix4 tempMatrices[32];
    BoneMapList* node = nlDLRingGetStart(mesh->boneMaps);
    glModelPacket* pPacket = model->packets;

    for (unsigned long packetIndex = 0; packetIndex < model->numPackets;
         packetIndex++, pPacket++)
    {
        UnidentifiedShaderSkinData_80370808* data = &mesh->m_Unknown24[packetIndex];
        unsigned long numVertices = data->m_Unknown00;
        unsigned long size = numVertices * sizeof(nlVector3);
        nlVector3* outVertices = (nlVector3*)glFrameAlloc(size, GLM_VertexData);
        nlZeroMemory(outVertices, size);
        pPacket->unknown28 = (unsigned long)outVertices;

        nlVector3* outNormals = 0;
        glModelStream* pStream = fn_8036F99C(pPacket, 2);
        const nlVector3* inNormals = 0;
        if (pStream != 0)
        {
            outNormals = (nlVector3*)glFrameAlloc(size, GLM_VertexData);
            nlZeroMemory(outNormals, size);
            inNormals = (const nlVector3*)pStream->address;
            pPacket->unknown2C = (unsigned long)outNormals;
        }

        fn_80370088(mesh, packetIndex, numVertices);
        const nlVector3* inVertices = (const nlVector3*)fn_8036F99C(pPacket, 1)->address;
        const float (*pMatrices)[3][4] =
            *(const float (**)[3][4])fn_802CC870(pPacket, lbl_806E2400);
        for (unsigned long i = 0; i < node->m_nBones; i++)
        {
            glxCopyMatrix(tempMatrices[i], *pMatrices);
            pMatrices++;
        }

        for (unsigned long matrixOffset = 0; matrixOffset < data->m_Unknown04; matrixOffset++)
        {
            UnidentifiedShaderSkinEntry_80370868* curr = &data->m_Unknown08[matrixOffset];
            register const nlMatrix4* pMatrix = &tempMatrices[matrixOffset];
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

            if (mesh->m_Unknown18 != 0)
            {
                for (unsigned long i = 0; i < curr->m_Unknown00; i++)
                {
                    const UnidentifiedShaderSkinWeight_8036F91C& pair =
                        ((const UnidentifiedShaderSkinWeight_8036F91C*)curr->m_Unknown04)[i];
                    float vertexWeight = pair.vertexWeight;
                    unsigned long index = pair.vertexIndex;
                    register const nlVector3& inVertex = inVertices[index];
                    register const nlVector3& morph = ((const nlVector3*)mesh->m_Unknown40)[index];
                    register nlVector3& outVertex = outVertices[index];
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
                }
            }
            else
            {
                for (unsigned long i = 0; i < curr->m_Unknown00; i++)
                {
                    const UnidentifiedShaderSkinWeight_8036F91C& pair =
                        ((const UnidentifiedShaderSkinWeight_8036F91C*)curr->m_Unknown04)[i];
                    float vertexWeight = pair.vertexWeight;
                    unsigned long index = pair.vertexIndex;
                    register const nlVector3& inVertex = inVertices[index];
                    register nlVector3& outVertex = outVertices[index];
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
                }
            }

            if (outNormals != 0)
            {
                for (unsigned long i = 0; i < curr->m_Unknown00; i++)
                {
                    const UnidentifiedShaderSkinWeight_8036F91C& pair =
                        ((const UnidentifiedShaderSkinWeight_8036F91C*)curr->m_Unknown04)[i];
                    float vertexWeight = pair.vertexWeight;
                    unsigned long index = pair.vertexIndex;
                    register const nlVector3& inNormal = inNormals[index];
                    register nlVector3& outNormal = outNormals[index];
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
// clang-format on

UnidentifiedShaderSkinData_80370808::~UnidentifiedShaderSkinData_80370808()
{
    delete[] m_Unknown08;
}

UnidentifiedShaderSkinEntry_80370868::~UnidentifiedShaderSkinEntry_80370868()
{
    delete[] m_Unknown04;
}

UnidentifiedShaderSkinData_80370808::UnidentifiedShaderSkinData_80370808()
    : m_Unknown00(0)
    , m_Unknown04(0)
    , m_Unknown08(0)
{
}

UnidentifiedShaderSkinEntry_80370868::UnidentifiedShaderSkinEntry_80370868()
    : m_Unknown00(0)
    , m_Unknown04(0)
{
}

glModel* ShaderSkinMesh::GetModel()
{
    bool unknown = false;
    if (!m_Unknown45 && m_Unknown0C == 0)
    {
        unknown = true;
    }
    if (unknown)
    {
        return (glModel*)m_Unknown28;
    }
    return pModel;
}
