#ifndef GAME_POSE_ACCUMULATOR_H
#define GAME_POSE_ACCUMULATOR_H

#include "NL/nlMath.h"
#include "types.h"

class cSHierarchy;
class cPoseNode;
class cPoseAccumulator;
class nlMatrix4;
class nlQuaternion;
typedef void (*BuildNodeMatrixFn)(unsigned int, unsigned int,
    cPoseAccumulator*, unsigned int, int);

class cBuildNodeMatrixCallbackInfo
{
public:
    cBuildNodeMatrixCallbackInfo()
    {
        funcCallback = 0;
    }

    BuildNodeMatrixFn funcCallback;
    unsigned int nParam1;
    unsigned int nParam2;
};

struct RotAccum
{
    nlQuaternion q;
    float quatAccumulatedWeight;
    u16 rotAroundZ;
    float rotAroundZAccumulatedWeight;
    bool bIdentity;
};

struct ScaleAccum
{
    nlVector3 s;
    float fAccumulatedWeight;
    bool bIdentity;
};

struct TransAccum
{
    nlVector3 t;
    float fAccumulatedWeight;
    bool bIdentity;
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
    cPoseAccumulator(cSHierarchy* pSHierarchy, bool bStorePrevNodeMatrices);
    cPoseAccumulator(const cPoseAccumulator& other);
    ~cPoseAccumulator();
    cPoseAccumulator& operator=(const cPoseAccumulator& other);
    void InitAccumulators();
    void BuildNodeMatrices(const nlMatrix4& pWorldMatrix);
    void BlendRot(int nNode, const nlQuaternion* pRot, float fWeight,
        bool bMirror);
    void BlendRotAroundZ(int nNode, unsigned short rot, float fWeight);
    void BlendScale(int nNode, const nlVector3* pScale, float fWeight,
        bool bMirror);
    void BlendTrans(int nNode, const nlVector3* pTrans, float fWeight,
        bool bMirror);
    void BlendRotIdentity(int nNode, float fWeight);
    void BlendScaleIdentity(int nNode, float fWeight);
    void MultiplyScale(int nNode, const nlVector3* pScale, float fWeight);
    void BlendTransIdentity(int nNode, float fWeight);
    nlMatrix4& GetNodeMatrix(int nNode) const;
    nlMatrix4& GetNodeMatrixByHashID(unsigned int nHashID) const;
    s32 GetNumNodes() const;
    void Pose(const cPoseNode& pPoseTree, const nlMatrix4& pWorldMatrix);
    void SetBuildNodeMatrixCallback(int nNode,
        BuildNodeMatrixFn funcCallback, unsigned int nParam1,
        unsigned int nParam2);

    cSHierarchy* m_BaseSHierarchy;
    nlMatrix4* m_NodeMatrices;
    nlMatrix4* m_PrevNodeMatrices;
    nlQuaternion* m_pQuaternions;
    RotAccum* m_rot;
    ScaleAccum* m_scale;
    TransAccum* m_trans;
    cBuildNodeMatrixCallbackInfo* m_cb;
    MorphWeightAccum m_MorphWeights;
    u32 m_Unknown70;
    float m_Scale;
    bool m_bUseObject;
    u8 m_Padding[3];
};

#endif // GAME_POSE_ACCUMULATOR_H
