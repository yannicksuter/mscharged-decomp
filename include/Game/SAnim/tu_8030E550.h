#ifndef GAME_SANIM_TU_8030E550_H
#define GAME_SANIM_TU_8030E550_H

#include "Game/PoseNode.h"
#include "NL/nlSlotPool.h"

enum BlendMode_8030E550
{
    BLEND_MODE_8030E550_IN = 0,
    BLEND_MODE_8030E550_OUT = 1,
};

// Pose-node type 4 overlays the second child's animated scale onto the first
// child's evaluated pose. Its original class name is not yet established.
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

    void BeginBlendIn(float duration);
    void BeginBlendOut(float duration);

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

    float m_fBlendTime;
    float m_fBlendDuration;
    BlendMode_8030E550 m_eBlendMode;

    static SlotPool<cPN_8030E550> mSlotPool;
};

#endif // GAME_SANIM_TU_8030E550_H
