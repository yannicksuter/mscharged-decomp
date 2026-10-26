#ifndef GAME_GL_SHADER_SKIN_MESH_H
#define GAME_GL_SHADER_SKIN_MESH_H

#include "Game/GL/GLSkinMesh.h"

class cSHierarchy;
struct glModelPacket;

struct SkinWeight
{
    /* 0x00 */ unsigned long vertexIndex;
    /* 0x04 */ float vertexWeight;
};

struct BoneSkinWeights
{
    BoneSkinWeights()
        : numWeights(0)
        , weights(0)
    {
    }

    ~BoneSkinWeights() { delete[] weights; }

    /* 0x00 */ unsigned long numWeights;
    /* 0x04 */ SkinWeight* weights;
};

struct PacketSkinData
{
    PacketSkinData()
        : numVertices(0)
        , numBones(0)
        , boneWeights(0)
    {
    }

    ~PacketSkinData() { delete[] boneWeights; }

    /* 0x00 */ unsigned long numVertices;
    /* 0x04 */ unsigned long numBones;
    /* 0x08 */ BoneSkinWeights* boneWeights;
};

struct MorphDelta
{
    /* 0x00 */ nlVector3 delta;
    /* 0x0C */ int index;
};

struct MorphDeltaList
{
    /* 0x00 */ unsigned long numDeltas;
    /* 0x04 */ const MorphDelta* deltas;
};

struct BoneMapList
{
    BoneMapList()
        : m_next(0)
        , m_pBoneIndices(0)
        , m_pMatrices(0)
    {
    }

    ~BoneMapList()
    {
        if (m_pBoneIndices != 0)
        {
            delete[] m_pBoneIndices;
        }
        if (m_pMatrices != 0)
        {
            delete[] m_pMatrices;
        }
    }

    /* 0x00 */ BoneMapList* m_next;
    /* 0x04 */ unsigned long m_nBones;
    /* 0x08 */ int* m_pBoneIndices;
    /* 0x0C */ nlMatrix4* m_pMatrices;
};

class ShaderSkinMesh : public GLSkinMesh
{
public:
    ShaderSkinMesh()
        : boneMaps(0)
        , packetSkinData(0)
        , softwareModel(0)
        , numPackets(0)
        , morphData(0)
        , rigidSkin(false)
    {
    }

    virtual ~ShaderSkinMesh();
    virtual glModel* GetModel()
    {
        bool softwareSkinning = false;
        if (!rigidSkin && modelIndex == 0)
        {
            softwareSkinning = true;
        }
        if (softwareSkinning)
        {
            return softwareModel;
        }
        return pModel;
    }
    virtual void Pose(cPoseAccumulator* pPoseAccumulator);
    virtual void PrepareToRender();
    virtual void GetPoseMatrix(nlMatrix4* matrix, int nodeIndex);

    void SetNumMorphPackets(unsigned long count);
    void SetMorphDeltas(unsigned long packetIndex, unsigned long morphIndex,
        unsigned long count, const MorphDelta* data);
    void InitializeSkinData();
    void SetHierarchy(const cSHierarchy* hierarchy);
    void SetBoneMatrix(int nodeIndex, const nlMatrix4* matrix);

    /* 0x20 */ BoneMapList* boneMaps;
    /* 0x24 */ PacketSkinData* packetSkinData;
    /* 0x28 */ glModel* softwareModel;
    /* 0x2C */ nlMatrix4* boneMatrices;
    /* 0x30 */ nlMatrix4* poseMatrices;
    /* 0x34 */ unsigned long numBones;
    /* 0x38 */ unsigned long numPackets;
    /* 0x3C */ MorphDeltaList* morphData;
    /* 0x40 */ nlVector3* morphBuffer;
    /* 0x44 */ unsigned char m_Unknown44;
    /* 0x45 */ bool rigidSkin;

private:
    void CopyMatrices(BoneMapList* node);
    void BuildPacketSkinData(PacketSkinData* data, glModelPacket* pPacket,
        BoneMapList* node);
    void CreateMorphBuffer(unsigned long packetIndex, unsigned long count);
    void SoftwareSkinModel(glModel* model);
};

#endif // GAME_GL_SHADER_SKIN_MESH_H
