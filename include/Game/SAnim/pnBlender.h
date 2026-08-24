#ifndef GAME_SANIM_PN_BLENDER_H
#define GAME_SANIM_PN_BLENDER_H

#include "Game/PoseNode.h"
#include "NL/nlSlotPool.h"

class cPN_Blender : public cPoseNode
{
public:
    cPN_Blender(cPoseNode* child0, cPoseNode* child1, float blendDuration);
    virtual ~cPN_Blender()
    {
    }
    virtual void Evaluate(
        int nodeIndex, float weight, cPoseAccumulator* accumulator) const;
    virtual void Evaluate(float weight, cPoseAccumulator* accumulator) const;
    virtual cPoseNode* Update(float dt);
    virtual int GetType()
    {
        return 0;
    }
    virtual void BlendRootTrans(nlVector3* outBase, float weight, float* scratch);
    virtual void BlendRootRot(u16* outRot, float weight, float* scratch);

    static void* operator new(unsigned long)
    {
        cPN_Blender* blender = 0;
        m_BlenderSlotPool.Allocate(blender);
        return blender;
    }

    static void operator delete(void* pointer)
    {
        m_BlenderSlotPool.Free((cPN_Blender*)pointer);
    }

    float m_fBlendTime;
    float m_fBlendDuration;

    static SlotPool<cPN_Blender> m_BlenderSlotPool;
};

#endif // GAME_SANIM_PN_BLENDER_H
