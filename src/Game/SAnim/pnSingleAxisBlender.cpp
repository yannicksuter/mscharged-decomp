#include "Game/SAnim/pnSingleAxisBlender.h"

SlotPool<cPN_SingleAxisBlender>
    cPN_SingleAxisBlender::m_SingleAxisBlenderSlotPool(16, 16);

cPN_SingleAxisBlender::cPN_SingleAxisBlender(int numChildren,
    void (*callback)(unsigned int, cPN_SingleAxisBlender*),
    unsigned int callbackParam, float weightSeek)
    : cPoseNode(numChildren)
{
    m_fSmoothedWeight = 0.0f;
    m_fWeightCallback = callback;
    m_nCallbackParam = callbackParam;
    m_fDesiredWeight = 0.0f;
    m_fWeightSeek = weightSeek;

    for (int i = 0; i < m_numChildren; ++i)
    {
        SetChild(i, 0);
    }

    if (m_fWeightCallback != 0)
    {
        m_fWeightCallback(m_nCallbackParam, this);
        m_fSmoothedWeight = m_fDesiredWeight;
    }
}

cPoseNode* cPN_SingleAxisBlender::Update(float dt)
{
    for (int i = 0; i < m_numChildren; ++i)
    {
        SetChild(i, GetChild(i)->Update(dt));
    }

    if (m_fWeightCallback != 0)
    {
        m_fWeightCallback(m_nCallbackParam, this);
        m_fSmoothedWeight = m_fSmoothedWeight
                          + m_fWeightSeek * (m_fDesiredWeight - m_fSmoothedWeight);
    }

    return this;
}

void cPN_SingleAxisBlender::Evaluate(
    int nodeIndex, float weight, cPoseAccumulator* accumulator) const
{
    if (m_fSmoothedWeight >= 1.0f)
    {
        GetChild(m_numChildren - 1)->Evaluate(nodeIndex, weight, accumulator);
        return;
    }

    float scaledWeight = m_fSmoothedWeight * (float)(m_numChildren - 1);
    int childIndex = (int)scaledWeight;
    float fraction = scaledWeight - (float)childIndex;

    GetChild(childIndex)->Evaluate(nodeIndex, weight * (1.0f - fraction), accumulator);
    GetChild(childIndex + 1)->Evaluate(nodeIndex, weight * fraction, accumulator);
}

void cPN_SingleAxisBlender::Evaluate(
    float weight, cPoseAccumulator* accumulator) const
{
    if (m_fSmoothedWeight >= 1.0f)
    {
        GetChild(m_numChildren - 1)->Evaluate(weight, accumulator);
        return;
    }

    float scaledWeight = m_fSmoothedWeight * (float)(m_numChildren - 1);
    int childIndex = (int)scaledWeight;
    float fraction = scaledWeight - (float)childIndex;

    GetChild(childIndex)->Evaluate(weight * (1.0f - fraction), accumulator);
    GetChild(childIndex + 1)->Evaluate(weight * fraction, accumulator);
}

void cPN_SingleAxisBlender::BlendRootTrans(
    nlVector3* outBase, float weight, float* scratch)
{
    if (m_fSmoothedWeight >= 1.0f)
    {
        GetChild(m_numChildren - 1)->BlendRootTrans(outBase, weight, scratch);
        return;
    }

    float scaledWeight = m_fSmoothedWeight * (float)(m_numChildren - 1);
    int childIndex = (int)scaledWeight;
    float fraction = scaledWeight - (float)childIndex;

    GetChild(childIndex)->BlendRootTrans(outBase, weight * (1.0f - fraction), scratch);
    GetChild(childIndex + 1)->BlendRootTrans(outBase, weight * fraction, scratch);
}

void cPN_SingleAxisBlender::BlendRootRot(
    u16* outRot, float weight, float* scratch)
{
    if (m_fSmoothedWeight >= 1.0f)
    {
        GetChild(m_numChildren - 1)->BlendRootRot(outRot, weight, scratch);
        return;
    }

    float scaledWeight = m_fSmoothedWeight * (float)(m_numChildren - 1);
    int childIndex = (int)scaledWeight;
    float fraction = scaledWeight - (float)childIndex;

    GetChild(childIndex)->BlendRootRot(outRot, weight * (1.0f - fraction), scratch);
    GetChild(childIndex + 1)->BlendRootRot(outRot, weight * fraction, scratch);
}
