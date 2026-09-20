#include "NL/nlPrint.h"
#include "NL/gl/glLoadModel.h"
#include "NL/gl/glMaterialProgram.h"

#include "Game/GL/GLInventory.h"
#include "Game/GL/GLVertexAnim.h"
#include "Game/SAnim.h"
#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glMemory.h"
#include "NL/glx/glxLoadModel.h"
#include "NL/nlAVLTree.h"
#include "NL/nlFunction.h"
#include "NL/nlMemory.h"

#include <string.h>


typedef nlAVLTree<unsigned long, void*, DefaultKeyCompare<unsigned long> >
    MaterialProgramTree;

static bool glIgnoreDuplicateModels;
static MaterialProgramTree sMaterialPrograms;

void glSetIgnoreDuplicateModels(bool ignore)
{
    glIgnoreDuplicateModels = ignore;
}

void RLGReader::LoadSkinData(nlChunk* chunk)
{
    unsigned long size = chunk->GetDataSize() + sizeof(nlChunk);
    nlChunk* skinData = (nlChunk*)nlMalloc(size, 32, false);
    memcpy(skinData, chunk, size);
    m_pResource->m_inventory->AddSkinData(
        m_pModels->id, skinData);
}

void RLGReader::LoadVertexAnimData(nlChunk* chunk)
{
    unsigned char* data = (unsigned char*)chunk->GetData();
    unsigned long* extraData = (unsigned long*)((unsigned char*)chunk->GetData() + 0x18);
    unsigned char* vertexData = (unsigned char*)(extraData + *(unsigned long*)(data + 0x14));

    GLVertexAnim* model = new (nlMalloc(sizeof(GLVertexAnim), 8, false))
        GLVertexAnim(data, extraData);

    unsigned long vertexDataSize = *(unsigned long*)(data + 4)
                                 * *(unsigned long*)(data + 8) * *(unsigned long*)(data + 0x0C);
    model->m_pVertices = (u8*)glResourceAlloc(vertexDataSize, GLM_VertexData, m_pResource);
    memcpy(model->m_pVertices, vertexData, vertexDataSize);
    model->m_pModel = m_pResource->m_inventory->GetModel(
        *(unsigned long*)data);
    m_pResource->m_inventory->AddVertexAnim(
        *(unsigned long*)data, model);
}

void RLGReader::LoadPacketData(void* data, unsigned long size)
{
    m_pParameterData = glResourceAlloc(size, GLM_Header, m_pResource);
    memcpy(m_pParameterData, data, size);
}

void RLGReader::LoadStreams(void* data, unsigned long size)
{
    m_numStreamEntries = size >> 3;
    m_pStreamData = glResourceAlloc(size, GLM_Header, m_pResource);
    memcpy(m_pStreamData, data, m_numStreamEntries << 3);
}

void RLGReader::LoadPackets(void* data, unsigned long size)
{
    m_numPacketEntries = size / sizeof(glModelPacket);
    m_pPackets = glResourceAlloc(size, GLM_Header, m_pResource);
    memcpy(m_pPackets, data, size);
}

void RLGReader::LoadModels(void* data, unsigned long size)
{
    m_nModels = size / sizeof(glModel);
    glBeginResource(*(unsigned long*)data);
    m_pModels = (glModel*)glResourceAlloc(size, GLM_Header, m_pResource);
    glEndResource();
    memcpy(m_pModels, data, m_nModels * sizeof(glModel));
}

void RLGReader::LoadMatrices(void* data, unsigned long size)
{
    size &= ~0x3F;
    m_pMatrices = glResourceAlloc(size, GLM_Matrix, m_pResource);
    memcpy(m_pMatrices, data, size);
}

void RLGReader::RegisterModels()
{
    for (unsigned long i = 0; i < m_nModels; ++i)
    {
        glModel* model = &m_pModels[i];
        if (!glIgnoreDuplicateModels
            || m_pResource->m_inventory->GetModel(model->id) == 0)
        {
            m_pResource->m_inventory->AddModel(model->id, model);
        }
    }
}

void RLGReader::HandleUnknownChunk(nlChunk* chunk)
{
    nlPrintf("Unknown chunk id %d in RLGReader", chunk->GetID());
}

static void FixupModelData(RLGReader* reader)
{
    glModelPacket* packets = (glModelPacket*)reader->m_pPackets;
    unsigned char* pStreamData = (unsigned char*)reader->m_pStreamData;
    glModel* model = reader->m_pModels;
    for (unsigned long modelIndex = 0; modelIndex < reader->m_nModels;
        ++model, ++modelIndex)
    {
        model->packets = packets;
        packets += model->numPackets;

        for (unsigned long packetIndex = 0; packetIndex < model->numPackets;
            ++packetIndex)
        {
            glModelPacket* packet = &model->packets[packetIndex];
            packet->materialParameters = (unsigned char*)reader->m_pParameterData
                              + (unsigned long)packet->materialParameters;
            packet->streams = (glModelStream*)((unsigned long)packet->streams
                                               + (unsigned long)pStreamData);
            packet->indexBuffer = (unsigned short*)((unsigned long)packet->indexBuffer
                                                    + (unsigned long)reader->m_pIndexData);
            packet->matrix = (unsigned long)reader->m_pMatrices
                           + packet->matrix * 64;

            for (int streamIndex = 0; streamIndex < packet->numStreams;
                ++streamIndex)
            {
                glModelStream* stream = &packet->streams[streamIndex];
                stream->address = (unsigned char*)((unsigned long)stream->address
                                + (unsigned long)reader->m_pVertexData);
            }
        }
    }
}

void RLGReader::Read(void* data)
{
    nlChunk* outerEnd = 0;
    nlChunk* chunk;
    nlChunk* outerChunk = (nlChunk*)data;

    if (outerChunk->GetID() == 0x8001B100)
    {
        nlChunk* header = outerChunk;
        outerChunk = header->GetFirstChunk();
        outerEnd = header->GetLastChunk();
    }

    while (outerChunk != outerEnd)
    {
        nlChunk* chunkEnd = outerChunk->GetLastChunk();
        chunk = outerChunk->GetFirstChunk();
        while (chunk != chunkEnd)
        {
            switch (chunk->GetID())
            {
            case 0x8001B008:
                LoadSkinData(chunk);
                break;
            case 0x1B016:
                LoadPacketData(chunk->GetData(),
                    chunk->GetDataSize());
                break;
            case 0x1B006:
                LoadVertices(chunk->GetData(),
                    chunk->GetDataSize());
                break;
            case 0x1B007:
                LoadIndices(chunk->GetData(),
                    chunk->GetDataSize());
                break;
            case 0x1B005:
                LoadStreams(chunk->GetData(),
                    chunk->GetDataSize());
                break;
            case 0x1B004:
                LoadPackets(chunk->GetData(),
                    chunk->GetDataSize());
                break;
            case 0x1B003:
                LoadModels(chunk->GetData(),
                    chunk->GetDataSize());
                break;
            case 0x1B002:
                LoadMatrices(chunk->GetData(),
                    chunk->GetDataSize());
                break;
            case 0x8001B200:
            {
                nlChunk* subChunk = chunk->GetFirstChunk();
                while (subChunk != chunk->GetLastChunk())
                {
                    LoadVertexAnim(subChunk);
                    subChunk = subChunk->GetNextChunk();
                }
                break;
            }
            default:
                HandleUnknownChunk(chunk);
                break;
            }
            chunk = chunk->GetNextChunk();
        }

        FixupModelData(this);
        FinalizeModels();
        RegisterModels();
        glEndResource();

        if (outerEnd == 0)
            return;

        m_pModels = 0;
        m_nModels = 0;
        outerChunk = outerChunk->GetNextChunk();
    }
}

void glRegisterMaterialProgram(void* program, unsigned long hash)
{
    sMaterialPrograms.Add(hash, program);
}

void* glGetMaterialProgram(unsigned long hash)
{
    void** program = 0;
    if (sMaterialPrograms.FindGet(hash, &program))
        return *program;
    return 0;
}

void glForEachMaterialProgram(MaterialProgramCallback* callback)
{
    MaterialProgramTree::Entry* entry = sMaterialPrograms.m_Root;
    while (entry != 0)
    {
        if (!sMaterialPrograms.InorderWalk(
                (MaterialProgramTree::Entry*)entry->node.left, callback))
            return;
        if (!(*callback)(entry->key, entry->value))
            return;
        entry = (MaterialProgramTree::Entry*)entry->node.right;
    }
}
