#ifndef GAME_SANIM_PN_FEATHER_H
#define GAME_SANIM_PN_FEATHER_H

#include "Game/PoseNode.h"
#include "Game/SHierarchy.h"
#include "NL/nlSlotPool.h"

enum eFeatherBlendMode
{
    FEATHER_BLEND_IN = 0,
    FEATHER_BLEND_OUT = 1,
};

class cPN_Feather : public cPoseNode
{
public:
    cPN_Feather() { }
    cPN_Feather(
        cSHierarchy* hierarchy,
        void (*callback)(unsigned int, cPN_Feather*),
        unsigned int callbackParam);
    virtual ~cPN_Feather();
    virtual void Evaluate(
        int nodeIndex, float weight, cPoseAccumulator* accumulator) const;
    virtual void Evaluate(float weight, cPoseAccumulator* accumulator) const;
    virtual cPoseNode* Update(float dt);
    virtual int GetType()
    {
        return 1;
    }
    virtual void BlendRootTrans(nlVector3* outBase, float weight, float* scratch);
    virtual void BlendRootRot(u16* outRot, float weight, float* scratch);

    template <typename T>
    void Replay(T& frame)
    {
        Replayable<0>(frame, (cPoseNode&)*this);
        if (ReplayFrameTraits<T>::IsLoadFrame)
        {
            m_fBlendTime = 0.0f;
            m_pFeatherWeights = NULL;
        }
    }

    void ClearNodeWeights();
    void SetNodeWeight(int nodeIndex, float weight, float decayFactor);
    void SetChildFeatherWeight(int nodeIndex, float weight);
    void SetNodeWeight(int nodeIndex, float weight);
    void BeginBlendIn(float duration);
    void BeginBlendOut(float duration);

    static void* operator new(unsigned long)
    {
        cPN_Feather* feather = 0;
        m_FeatherSlotPool.Allocate(feather);
        return feather;
    }

    static void operator delete(void* pointer)
    {
        m_FeatherSlotPool.Free((cPN_Feather*)pointer);
    }

    float* m_pFeatherWeights;
    float m_fBlendTime;
    void (*m_fWeightTableCallback)(unsigned int, cPN_Feather*);
    unsigned int m_nCallbackParam1;
    float m_fBlendDuration;
    cSHierarchy* m_pBaseHierarchy;
    eFeatherBlendMode m_eFeatherBlendMode;

    static SlotPool<cPN_Feather> m_FeatherSlotPool;
};

#endif // GAME_SANIM_PN_FEATHER_H
