#ifndef GAME_SANIM_H
#define GAME_SANIM_H

#include "NL/nlMath.h"
#include "types.h"

class cPoseAccumulator;
class cSAnim;

enum ePlayMode
{
    PM_CYCLIC = 0,
    PM_HOLD = 1,
    PM_PING_PONG = 2,
};

class cSAnimCallback
{
public:
    float m_fTime;
    unsigned int m_nParam;
    void (*m_pCallback)(cSAnim*, unsigned int);
    cSAnimCallback* m_pNext;
};

class cSAnim
{
public:
    void BlendRot(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void BlendScale(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void fn_80308E30(int accumulatorNode, int animNode, float time,
        float weight, cPoseAccumulator* accumulator) const;
    void BlendTrans(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void GetRootRot(float time, u16* rootRotation) const;
    void GetRootTrans(float time, nlVector3* rootTranslation) const;
    float GetMorphWeight(int channel, float time) const;

    cSAnimCallback* GetCallbackList() const
    {
        return m_pCallbackList;
    }

    float GetDuration() const
    {
        return (float)m_nNumKeys / 30.0f;
    }

    u32 m_Unknown00[2];
    unsigned int m_nNumKeys;
    unsigned int m_nNumNodes;
    unsigned int m_nNumMorphChannels;
    u8 m_Unknown14[0x38];
    cSAnimCallback* m_pCallbackList;
};

#endif // GAME_SANIM_H
