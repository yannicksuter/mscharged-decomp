#ifndef GAME_SANIM_PN_SINGLE_AXIS_BLENDER_H
#define GAME_SANIM_PN_SINGLE_AXIS_BLENDER_H

#include "Game/PoseNode.h"
#include "NL/nlSlotPool.h"

class cPN_SingleAxisBlender : public cPoseNode
{
public:
    cPN_SingleAxisBlender(int numChildren,
        void (*callback)(unsigned int, cPN_SingleAxisBlender*),
        unsigned int callbackParam, float weightSeek);
    virtual ~cPN_SingleAxisBlender()
    {
    }
    virtual void Evaluate(
        int nodeIndex, float weight, cPoseAccumulator* accumulator) const;
    virtual void Evaluate(float weight, cPoseAccumulator* accumulator) const;
    virtual cPoseNode* Update(float dt);
    virtual int GetType()
    {
        return 3;
    }
    virtual void BlendRootTrans(nlVector3* outBase, float weight, float* scratch);
    virtual void BlendRootRot(u16* outRot, float weight, float* scratch);

    static void* operator new(unsigned long)
    {
        cPN_SingleAxisBlender* blender = 0;
        m_SingleAxisBlenderSlotPool.Allocate(blender);
        return blender;
    }

    static void operator delete(void* pointer)
    {
        m_SingleAxisBlenderSlotPool.Free((cPN_SingleAxisBlender*)pointer);
    }

    float m_fSmoothedWeight;
    void (*m_fWeightCallback)(unsigned int, cPN_SingleAxisBlender*);
    unsigned int m_nCallbackParam;
    float m_fDesiredWeight;
    float m_fWeightSeek;

    static SlotPool<cPN_SingleAxisBlender> m_SingleAxisBlenderSlotPool;
};

#endif // GAME_SANIM_PN_SINGLE_AXIS_BLENDER_H
