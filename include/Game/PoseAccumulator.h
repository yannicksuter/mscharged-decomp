#ifndef GAME_POSE_ACCUMULATOR_H
#define GAME_POSE_ACCUMULATOR_H

#include "NL/nlMath.h"
#include "Game/Replay.h"
#include "Game/SHierarchy.h"
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
    template <typename T>
    void Replay(T& frame);
    void fn_801949E4(float scale);
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
    nlQuaternion& GetNodeQuaternion(int nNode) const
    {
        return m_pQuaternions[nNode];
    }
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

template <typename T>
inline void cPoseAccumulator::Replay(T& frame)
{
    for (unsigned int i = 0; i < m_Unknown70; i++)
    {
        const UnidentifiedQuaternionCompressor quaternion(m_pQuaternions[i]);
        frame.template Replayable<0>(quaternion);
    }
    for (unsigned int i = 0; i < m_Unknown70; i++)
    {
        if (!m_BaseSHierarchy->PreserveBoneLength(i))
        {
            Replayable<0>(frame, m_trans[i].bIdentity);
            if (!m_trans[i].bIdentity)
            {
                Replayable<0>(frame, FloatCompressor<-32, 32, 10>(m_trans[i].t.x));
                Replayable<0>(frame, FloatCompressor<-32, 32, 10>(m_trans[i].t.y));
                Replayable<0>(frame, FloatCompressor<-32, 32, 10>(m_trans[i].t.z));
            }
        }
    }
    for (unsigned int i = 0; i < m_Unknown70; i++)
    {
        Replayable<0>(frame, m_scale[i].bIdentity);
        if (!m_trans[i].bIdentity)
        {
            Replayable<0>(frame, FloatCompressor<0, 32, 11>(m_scale[i].s.x));
            Replayable<0>(frame, FloatCompressor<0, 32, 11>(m_scale[i].s.y));
            Replayable<0>(frame, FloatCompressor<0, 32, 11>(m_scale[i].s.z));
        }
    }
}

#endif // GAME_POSE_ACCUMULATOR_H
