#ifndef NL_GL_GLLOADMODEL_H
#define NL_GL_GLLOADMODEL_H

class GLResourcePool;
struct glModel;
class nlChunk;

class RLGReader
{
public:
    RLGReader() { }

    virtual void LoadSkinData(nlChunk* chunk);
    virtual void LoadPacketData(void* data, unsigned long size);
    virtual void LoadVertices(void* data, unsigned long size) = 0;
    virtual void LoadIndices(void* data, unsigned long size) = 0;
    virtual void LoadStreams(void* data, unsigned long size);
    virtual void LoadPackets(void* data, unsigned long size);
    virtual void LoadModels(void* data, unsigned long size);
    virtual void LoadMatrices(void* data, unsigned long size);
    virtual void LoadVertexAnim(nlChunk* chunk) = 0;
    virtual void HandleUnknownChunk(nlChunk* chunk);
    virtual void FinalizeModels() = 0;
    virtual void RegisterModels();

    void LoadVertexAnimData(nlChunk* chunk);
    void Read(void* data);

    /* 0x04 */ void* m_pPackets;
    /* 0x08 */ unsigned long m_numPacketEntries;
    /* 0x0C */ glModel* m_pModels;
    /* 0x10 */ unsigned long m_nModels;
    /* 0x14 */ void* m_pStreamData;
    /* 0x18 */ unsigned long m_numStreamEntries;
    /* 0x1C */ void* m_pVertexData;
    /* 0x20 */ void* m_pIndexData;
    /* 0x24 */ void* m_pParameterData;
    /* 0x28 */ void* m_pMatrices;
    /* 0x2C */ GLResourcePool* m_pResource;
};

void glSetIgnoreDuplicateModels(bool ignore);

#endif // NL_GL_GLLOADMODEL_H
