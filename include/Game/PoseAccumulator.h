#ifndef GAME_POSE_ACCUMULATOR_H
#define GAME_POSE_ACCUMULATOR_H

#include "NL/nlMath.h"
#include "types.h"

class cSHierarchy;
class nlMatrix4;
class nlQuaternion;
struct RotAccum;
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
    int GetNumNodes() const;
    void BlendRotIdentity(int nodeIndex, float weight);
    void BlendScaleIdentity(int nodeIndex, float weight);

    cSHierarchy* m_pHierarchy;
    nlMatrix4* m_pNodeMatrices;
    u32 m_Unknown08;
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
