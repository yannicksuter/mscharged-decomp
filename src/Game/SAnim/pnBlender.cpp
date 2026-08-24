#include "Game/SAnim/pnBlender.h"

SlotPool<cPN_Blender> cPN_Blender::m_BlenderSlotPool(16, 16);

cPN_Blender::cPN_Blender(
    cPoseNode* child0, cPoseNode* child1, float blendDuration)
    : cPoseNode(2)
{
    SetChild(0, child0);
    SetChild(1, child1);
    m_fBlendTime = 0.0f;
    m_fBlendDuration = blendDuration;
}

cPoseNode* cPN_Blender::Update(float dt)
{
    SetChild(0, GetChild(0)->Update(dt));
    SetChild(1, GetChild(1)->Update(dt));

    m_fBlendTime += dt / m_fBlendDuration;

    if (m_fBlendTime > 1.0f)
    {
        cPoseNode* oldChild1 = GetChild(1);
        SetChild(1, 0);
        delete this;
        return oldChild1;
    }

    return this;
}

void cPN_Blender::Evaluate(float weight, cPoseAccumulator* accumulator) const
{
    float blendFactor;
    float blendTime;
    blendTime = m_fBlendTime;
    blendFactor = blendTime * (blendTime * ((-2.0f * blendTime) + 3.0f));

    GetChild(0)->Evaluate(weight * (1.0f - blendFactor), accumulator);
    GetChild(1)->Evaluate(weight * blendFactor, accumulator);
}

void cPN_Blender::Evaluate(
    int nodeIndex, float weight, cPoseAccumulator* accumulator) const
{
    float blendFactor;
    float blendTime;
    blendTime = m_fBlendTime;
    blendFactor = blendTime * (blendTime * ((-2.0f * blendTime) + 3.0f));

    GetChild(0)->Evaluate(
        nodeIndex, weight * (1.0f - blendFactor), accumulator);
    GetChild(1)->Evaluate(nodeIndex, weight * blendFactor, accumulator);
}

void cPN_Blender::BlendRootTrans(
    nlVector3* outBase, float weight, float* scratch)
{
    float blendFactor;
    float blendTime;
    blendTime = m_fBlendTime;
    blendFactor = blendTime * (blendTime * ((-2.0f * blendTime) + 3.0f));

    GetChild(0)->BlendRootTrans(
        outBase, weight * (1.0f - blendFactor), scratch);
    GetChild(1)->BlendRootTrans(outBase, weight * blendFactor, scratch);
}

void cPN_Blender::BlendRootRot(u16* outRot, float weight, float* scratch)
{
    float blendFactor;
    float blendTime;
    blendTime = m_fBlendTime;
    blendFactor = blendTime * (blendTime * ((-2.0f * blendTime) + 3.0f));

    GetChild(0)->BlendRootRot(
        outRot, weight * (1.0f - blendFactor), scratch);
    GetChild(1)->BlendRootRot(outRot, weight * blendFactor, scratch);
}
