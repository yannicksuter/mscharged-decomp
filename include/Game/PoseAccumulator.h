#ifndef GAME_POSE_ACCUMULATOR_H
#define GAME_POSE_ACCUMULATOR_H

#include "NL/nlMath.h"
#include "types.h"

class cSHierarchy;
class cPoseNode;
class cPoseAccumulator;
class nlMatrix4;
class nlQuaternion;
struct RotAccum;
typedef void (*BuildNodeMatrixFn)(unsigned int, unsigned int,
    cPoseAccumulator*, unsigned int, int);
struct VectorAccum
{
    nlVector3 t;
    float accumulatedWeight;
    bool identity;
    u8 padding[3];
};

struct MorphWeightAccum
{
    float& operator[](int index)
    {
        return mData[index];
    }

    float mData[20];
};

class cPoseAccumulator
{
public:
    void InitAccumulators(float scale, float damage);
    int GetNumNodes() const;
    nlMatrix4& GetNodeMatrix(int nodeIndex) const;
    void BlendRot(int nodeIndex, const nlQuaternion* rotation, float weight,
        bool additive);
    void BlendRotAroundZ(int nodeIndex, unsigned short rotation, float weight);
    void BlendScale(int nodeIndex, const nlVector3* scale, float weight,
        bool additive);
    void BlendTrans(int nodeIndex, const nlVector3* translation, float weight,
        bool mirror);
    void BlendRotIdentity(int nodeIndex, float weight);
    void BlendScaleIdentity(int nodeIndex, float weight);
    void MultiplyScale(int nodeIndex, const nlVector3* scale, float weight);
    void BlendTransIdentity(int nodeIndex, float weight);
    void Pose(const cPoseNode& poseNode, const nlMatrix4& matrix);
    void SetBuildNodeMatrixCallback(int nodeIndex,
        BuildNodeMatrixFn funcCallback, unsigned int param1,
        unsigned int param2);

    cSHierarchy* m_pHierarchy;
    nlMatrix4* m_pNodeMatrices;
    nlMatrix4* m_PrevNodeMatrices;
    nlQuaternion* m_pQuaternions;
    RotAccum* m_pRotations;
    VectorAccum* m_pScales;
    VectorAccum* m_pTranslations;
    u32 m_Unknown1C;
    MorphWeightAccum m_MorphWeights;
    u32 m_Unknown70;
    float m_Scale;
    bool m_bUseObject;
    u8 m_Padding[3];
};

#endif // GAME_POSE_ACCUMULATOR_H
