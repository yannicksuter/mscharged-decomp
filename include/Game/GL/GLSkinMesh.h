#ifndef GAME_GL_GL_SKIN_MESH_H
#define GAME_GL_GL_SKIN_MESH_H

#include "NL/nlMath.h"

class cPoseAccumulator;
struct glModel;

struct GLMorphWeight
{
    /* 0x00 */ unsigned long morphID;
    /* 0x04 */ float morphWeight;
};

struct MorphDelta
{
    nlVector3 delta;
    int index;
};

class GLSkinMesh
{
public:
    GLSkinMesh()
        : pModel(0)
        , mHierarchyID(0)
        , m_Unknown0C(0)
        , mMorphWeights(0)
        , numMorphs(0)
        , mNumActiveMorphs(0)
        , mMorphWeightsChanged(true)
    {
    }

    virtual ~GLSkinMesh();
    virtual void SetModel(glModel* model);
    virtual glModel* GetModel() = 0;
    virtual void Pose(cPoseAccumulator* pPoseAccumulator) = 0;
    virtual void PrepareToRender() = 0;
    virtual void GetPoseMatrix(nlMatrix4* matrix, int nodeIndex) = 0;

    void SetNumMorphs(unsigned long count);
    void SetMorphID(unsigned long index, unsigned long id);
    void UpdateMorphWeights(cPoseAccumulator* pPoseAccumulator);
    void ApplyMorphOverride();

    /* 0x04 */ glModel* pModel;
    /* 0x08 */ unsigned long mHierarchyID;
    /* 0x0C */ int m_Unknown0C;
    /* 0x10 */ GLMorphWeight* mMorphWeights;
    /* 0x14 */ unsigned long numMorphs;
    /* 0x18 */ unsigned long mNumActiveMorphs;
    /* 0x1C */ bool mMorphWeightsChanged;
};

extern int gMorphOverrideID;
extern float gMorphOverrideWeight;
extern unsigned char gMorphOverrideEnabled;

#endif // GAME_GL_GL_SKIN_MESH_H
