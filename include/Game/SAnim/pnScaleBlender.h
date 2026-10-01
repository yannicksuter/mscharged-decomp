#ifndef GAME_SANIM_PN_SCALE_BLENDER_H
#define GAME_SANIM_PN_SCALE_BLENDER_H

#include "Game/PoseNode.h"
#include "NL/nlSlotPool.h"

enum eScaleBlendMode
{
    SCALE_BLEND_IN = 0,
    SCALE_BLEND_OUT = 1,
};

// Pose-node type 4 overlays the second child's animated scale onto the first
// child's evaluated pose: Evaluate runs child 0 at full weight, then drives
// child 1 through EvaluateScale on every node with the smoothstep blend
// factor, while BlendRootTrans/Rot stay empty. cPN_ScaleBlender is a
// reconstruction name; the original class name is not established.
class cPN_ScaleBlender : public cPoseNode
{
public:
    virtual ~cPN_ScaleBlender();
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

    template <typename T>
    void Replay(T& frame)
    {
        Replayable<0>(frame, (cPoseNode&)*this);
        Replayable<0>(frame, FloatCompressor<0, 1, 7>(m_fBlendTime));
    }

    void BeginBlendIn(float duration);
    void BeginBlendOut(float duration);

    static void* operator new(unsigned long)
    {
        cPN_ScaleBlender* node = 0;
        m_ScaleBlenderSlotPool.Allocate(node);
        return node;
    }

    static void operator delete(void* pointer)
    {
        m_ScaleBlenderSlotPool.Free((cPN_ScaleBlender*)pointer);
    }

    float m_fBlendTime;
    float m_fBlendDuration;
    eScaleBlendMode m_eScaleBlendMode;

    static SlotPool<cPN_ScaleBlender> m_ScaleBlenderSlotPool;
};

#endif // GAME_SANIM_PN_SCALE_BLENDER_H
