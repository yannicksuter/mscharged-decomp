#ifndef UNCLASSIFIED_TU_8030E550_H
#define UNCLASSIFIED_TU_8030E550_H

#include "Game/PoseNode.h"
#include "NL/nlSlotPool.h"

enum BlendMode_8030E550
{
    BLEND_MODE_8030E550_IN = 0,
    BLEND_MODE_8030E550_OUT = 1,
};

class cPN_8030E550 : public cPoseNode
{
public:
    virtual ~cPN_8030E550();
    virtual void Evaluate(
        int nodeIndex, float weight, cPoseAccumulator* accumulator) const;
    virtual void Evaluate(float weight, cPoseAccumulator* accumulator) const;
    virtual cPoseNode* Update(float dt);
    virtual int GetType()
    {
        return 4;
    }
    virtual void BlendRootTrans(
        nlVector3* outBase, float weight, float* scratch);
    virtual void BlendRootRot(u16* outRot, float weight, float* scratch);

    static void* operator new(unsigned long)
    {
        cPN_8030E550* node = 0;
        mSlotPool.Allocate(node);
        return node;
    }

    static void operator delete(void* pointer)
    {
        mSlotPool.Free((cPN_8030E550*)pointer);
    }

    float mBlendTime;
    float mBlendDuration;
    BlendMode_8030E550 mBlendMode;

    static SlotPool<cPN_8030E550> mSlotPool;
};

#endif // UNCLASSIFIED_TU_8030E550_H
