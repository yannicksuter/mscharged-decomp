#include "NL/gl/glLoadModel.h"

#include "Game/GL/GLInventory.h"
#include "Game/SAnim.h"
#include "NL/gl/glModel.h"
#include "NL/nlAVLTree.h"
#include "NL/nlFunction.h"
#include "NL/nlMemory.h"

#include <string.h>

extern "C"
{
    void* fn_802CC0A4(unsigned long size, int memoryType, void* allocator);
    void fn_802C8284(unsigned long resource);
    void fn_802C8288();
    void* fn_802D3D48(void* model, void* header, void* extraData);
    int nlPrintf(const char* format, ...);
}

struct LoadedModel_802CA870
{
    glModel m_Model;
    unsigned char m_Unknown0C[0x24];
    void* m_pVertexData;
    void* m_pMaterialList;
};

typedef nlAVLTree<unsigned long, void*, DefaultKeyCompare<unsigned long> >
    MaterialProgramTree;
typedef Function2<bool, unsigned long&, void*&> MaterialProgramCallback;

static bool glIgnoreDuplicateModels;
static MaterialProgramTree sMaterialPrograms;

void glSetIgnoreDuplicateModels(bool ignore)
{
    glIgnoreDuplicateModels = ignore;
}

void RLGReader::fn_802CA778(nlChunk* chunk)
{
    unsigned char* data = (unsigned char*)chunk->GetAlignedData();
    unsigned char* unalignedData = (unsigned char*)chunk->GetUnalignedData();
    unsigned long size = chunk->GetSize() - (data - unalignedData)
                       + sizeof(nlChunk);
    nlChunk* skinData = (nlChunk*)nlMalloc(size, 32, false);
    memcpy(skinData, chunk, size);
    m_pContext->m_pInventory->AddSkinData(
        m_pModels->unknown00, skinData);
}

void RLGReader::fn_802CA870(nlChunk* chunk)
{
    unsigned char* data = (unsigned char*)chunk->GetData();
    unsigned long* extraData = (unsigned long*)((unsigned char*)chunk->GetData() + 0x18);
    unsigned char* vertexData = (unsigned char*)(extraData + *(unsigned long*)(data + 0x14));

    LoadedModel_802CA870* model = (LoadedModel_802CA870*)nlMalloc(
        sizeof(LoadedModel_802CA870), 8, false);
    if (model != 0)
    {
        model = (LoadedModel_802CA870*)fn_802D3D48(model, data, extraData);
    }

    unsigned long vertexDataSize = *(unsigned long*)(data + 4)
                                 * *(unsigned long*)(data + 8) * *(unsigned long*)(data + 0x0C);
    model->m_pVertexData = fn_802CC0A4(vertexDataSize, 3, m_pContext);
    memcpy(model->m_pVertexData, vertexData, vertexDataSize);
    model->m_pMaterialList = m_pContext->m_pInventory->GetMaterialList(*(unsigned long*)data);
    m_pContext->m_pInventory->AddModel(*(unsigned long*)data, &model->m_Model);
}

void RLGReader::fn_802CAA00(void* data, unsigned long size)
{
    m_Unknown24 = fn_802CC0A4(size, 0, m_pContext);
    memcpy(m_Unknown24, data, size);
}

void RLGReader::fn_802CAA60(void* data, unsigned long size)
{
    m_Unknown18 = size >> 3;
    m_Unknown14 = fn_802CC0A4(size, 0, m_pContext);
    memcpy(m_Unknown14, data, m_Unknown18 << 3);
}

void RLGReader::fn_802CAAC0(void* data, unsigned long size)
{
    m_Unknown08 = size / sizeof(glModelPacket);
    m_Unknown04 = fn_802CC0A4(size, 0, m_pContext);
    memcpy(m_Unknown04, data, size);
}

void RLGReader::fn_802CAB34(void* data, unsigned long size)
{
    m_nModels = size / sizeof(glModel);
    fn_802C8284(*(unsigned long*)data);
    m_pModels = (glModel*)fn_802CC0A4(size, 0, m_pContext);
    fn_802C8288();
    memcpy(m_pModels, data, m_nModels * sizeof(glModel));
}

void RLGReader::fn_802CABBC(void* data, unsigned long size)
{
    size &= ~0x3F;
    m_Unknown28 = fn_802CC0A4(size, 1, m_pContext);
    memcpy(m_Unknown28, data, size);
}

void RLGReader::fn_802CAC1C()
{
    for (unsigned long i = 0; i < m_nModels; ++i)
    {
        glModel* model = &m_pModels[i];
        if (!glIgnoreDuplicateModels
            || m_pContext->m_pInventory->GetMaterialList(model->unknown00) == 0)
        {
            m_pContext->m_pInventory->AddMaterialList(
                model->unknown00, (GLMaterialList*)model);
        }
    }
}

void RLGReader::fn_802CACBC(nlChunk* chunk)
{
    nlPrintf("Unknown chunk id %d in RLGReader", chunk->GetID());
}

static void FixupModelData(RLGReader* reader)
{
    glModelPacket* packets = (glModelPacket*)reader->m_Unknown04;
    glModel* model = reader->m_pModels;
    for (unsigned long modelIndex = 0; modelIndex < reader->m_nModels;
        ++modelIndex, ++model)
    {
        model->packets = packets;
        packets += model->numPackets;

        for (unsigned long packetIndex = 0; packetIndex < model->numPackets;
            ++packetIndex)
        {
            glModelPacket* packet = &model->packets[packetIndex];
            packet->unknown20 = (unsigned char*)reader->m_Unknown24
                              + (unsigned long)packet->unknown20;
            packet->streams = (glModelStream*)((unsigned char*)reader->m_Unknown14
                                               + (unsigned long)packet->streams);
            packet->indexBuffer = (unsigned short*)((unsigned char*)reader->m_Unknown20
                                                    + (unsigned long)packet->indexBuffer);
            packet->matrix = (unsigned long)reader->m_Unknown28
                           + packet->matrix * 64;

            for (int streamIndex = 0; streamIndex < packet->numStreams;
                ++streamIndex)
            {
                glModelStream* stream = &packet->streams[streamIndex];
                stream->address = (unsigned char*)reader->m_Unknown1C
                                + (unsigned long)stream->address;
            }
        }
    }
}

void RLGReader::fn_802CADC4(void* data)
{
    nlChunk* outerEnd = 0;
    nlChunk* chunk;
    nlChunk* outerChunk = (nlChunk*)data;

    if (outerChunk->GetID() == 0x8001B100)
    {
        nlChunk* header = outerChunk;
        outerChunk = header->GetFirstChunk();
        outerEnd = header->GetNextChunk();
    }

    while (outerChunk != outerEnd)
    {
        nlChunk* chunkEnd = outerChunk->GetNextChunk();
        chunk = outerChunk->GetFirstChunk();
        while (chunk != chunkEnd)
        {
            switch (chunk->GetID())
            {
            case 0x8001B008:
                fn_802CA778(chunk);
                break;
            case 0x1B016:
                fn_802CAA00(chunk->GetData(),
                    chunk->GetSize()
                        - ((unsigned char*)chunk->GetAlignedData()
                            - (unsigned char*)chunk->GetUnalignedData()));
                break;
            case 0x1B006:
                fn_80369E5C(chunk->GetData(),
                    chunk->GetSize()
                        - ((unsigned char*)chunk->GetAlignedData()
                            - (unsigned char*)chunk->GetUnalignedData()));
                break;
            case 0x1B007:
                fn_80369EC8(chunk->GetData(),
                    chunk->GetSize()
                        - ((unsigned char*)chunk->GetAlignedData()
                            - (unsigned char*)chunk->GetUnalignedData()));
                break;
            case 0x1B005:
                fn_802CAA60(chunk->GetData(),
                    chunk->GetSize()
                        - ((unsigned char*)chunk->GetAlignedData()
                            - (unsigned char*)chunk->GetUnalignedData()));
                break;
            case 0x1B004:
                fn_802CAAC0(chunk->GetData(),
                    chunk->GetSize()
                        - ((unsigned char*)chunk->GetAlignedData()
                            - (unsigned char*)chunk->GetUnalignedData()));
                break;
            case 0x1B002:
                fn_802CAB34(chunk->GetData(),
                    chunk->GetSize()
                        - ((unsigned char*)chunk->GetAlignedData()
                            - (unsigned char*)chunk->GetUnalignedData()));
                break;
            case 0x1B003:
                fn_802CABBC(chunk->GetData(),
                    chunk->GetSize()
                        - ((unsigned char*)chunk->GetAlignedData()
                            - (unsigned char*)chunk->GetUnalignedData()));
                break;
            case 0x8001B200:
            {
                nlChunk* subChunk = chunk->GetFirstChunk();
                while (subChunk != chunk->GetNextChunk())
                {
                    fn_80369F34(subChunk);
                    subChunk = subChunk->GetNextChunk();
                }
                break;
            }
            default:
                fn_802CACBC(chunk);
                break;
            }
            chunk = chunk->GetNextChunk();
        }

        FixupModelData(this);
        fn_8036A138();
        fn_802CAC1C();
        fn_802C8288();

        if (outerEnd == 0)
            return;

        m_pModels = 0;
        m_nModels = 0;
        outerChunk = outerChunk->GetNextChunk();
    }
}

extern "C" void fn_802CB790(void* program, unsigned long hash)
{
    sMaterialPrograms.Add(hash, program);
}

extern "C" void* fn_802CB7D0(unsigned long hash)
{
    void** program = 0;
    if (sMaterialPrograms.FindGet(hash, &program))
        return *program;
    return 0;
}

extern "C" void fn_802CB848(MaterialProgramCallback* callback)
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
