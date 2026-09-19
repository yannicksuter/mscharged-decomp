#ifndef GAME_GL_SHADER_SKIN_MESH_H
#define GAME_GL_SHADER_SKIN_MESH_H

#include "NL/nlMath.h"
#include "NL/gl/glModel.h"
#include "types.h"

class cSHierarchy;
class cPoseAccumulator;
struct glModel;
struct glModelPacket;

extern int gMorphOverrideID;
extern float gMorphOverrideWeight;
extern unsigned char gMorphOverrideEnabled;

struct MorphWeight
{
    /* 0x00 */ unsigned long morphID;
    /* 0x04 */ float morphWeight;
};

struct SkinWeight
{
    /* 0x00 */ unsigned long vertexIndex;
    /* 0x04 */ float vertexWeight;
};

struct BoneSkinWeights
{
    BoneSkinWeights();
    ~BoneSkinWeights();

    /* 0x00 */ unsigned long numWeights;
    /* 0x04 */ SkinWeight* weights;
};

struct PacketSkinData
{
    PacketSkinData();
    ~PacketSkinData();

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

class GLSkinMesh
{
public:
    GLSkinMesh()
        : pModel(0)
        , hierarchySignature(0)
        , m_Unknown0C(0)
        , morphWeights(0)
        , numMorphs(0)
        , numActiveMorphs(0)
        , morphWeightsChanged(true)
    {
    }

    virtual ~GLSkinMesh();
    virtual void SetModel(glModel* model);
    virtual glModel* GetModel() { return pModel; }
    virtual void Pose(cPoseAccumulator* pPoseAccumulator) = 0;
    virtual void PrepareToRender() = 0;
    virtual void GetPoseMatrix(nlMatrix4* matrix, int nodeIndex) = 0;

    unsigned long GetNumPackets() { return GetModel()->numPackets; }
    int GetModelIndex() const { return m_Unknown0C; }

    void SetNumMorphs(unsigned long count);
    void SetMorphID(unsigned long index, unsigned long id);
    void UpdateMorphWeights(cPoseAccumulator* pPoseAccumulator);
    void ApplyMorphOverride();

    /* 0x04 */ glModel* pModel;
    /* 0x08 */ unsigned long hierarchySignature;
    /* 0x0C */ int m_Unknown0C;
    /* 0x10 */ MorphWeight* morphWeights;
    /* 0x14 */ unsigned long numMorphs;
    /* 0x18 */ unsigned long numActiveMorphs;
    /* 0x1C */ bool morphWeightsChanged;
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
    virtual glModel* GetModel();
    virtual void Pose(cPoseAccumulator* pPoseAccumulator);
    virtual void PrepareToRender();
    virtual void GetPoseMatrix(nlMatrix4* matrix, int nodeIndex);

    void SetNumMorphPackets(unsigned long count);
    void SetMorphDeltas(unsigned long packetIndex, unsigned long morphIndex,
        unsigned long count, const MorphDelta* data);
    void InitializeSkinData();
    void SetHierarchy(cSHierarchy* hierarchy);
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
