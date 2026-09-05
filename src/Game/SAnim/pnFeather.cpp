#include "Game/SAnim/pnFeather.h"

#include "NL/nlMemory.h"

SlotPool<cPN_Feather> cPN_Feather::m_FeatherSlotPool(16, 16);

cPN_Feather::cPN_Feather(
    cSHierarchy* hierarchy,
    void (*callback)(unsigned int, cPN_Feather*),
    unsigned int callbackParam)
    : cPoseNode(2)
{
    m_fBlendTime = 0.0f;
    m_fWeightTableCallback = callback;
    m_nCallbackParam1 = callbackParam;
    m_fBlendDuration = 0.0f;
    m_eFeatherBlendMode = FEATHER_BLEND_OUT;
    m_pBaseHierarchy = hierarchy;

    m_pFeatherWeights = (float*)nlMalloc(hierarchy->m_nNumNodes * sizeof(float), 8, false);

    ClearNodeWeights();

    SetChild(0, 0);
    SetChild(1, 0);
}

cPN_Feather::~cPN_Feather()
{
    delete[] m_pFeatherWeights;
}

void cPN_Feather::ClearNodeWeights()
{
    for (int i = 0; i < m_pBaseHierarchy->m_nNumNodes; ++i)
    {
        m_pFeatherWeights[i] = 0.0f;
    }
}

void cPN_Feather::SetNodeWeight(int nodeIndex, float weight, float decayFactor)
{
    SetChildFeatherWeight(nodeIndex, weight);

    while (nodeIndex != -1 && weight > 0.001f)
    {
        m_pFeatherWeights[nodeIndex] = weight;
        weight *= decayFactor;
        nodeIndex = m_pBaseHierarchy->GetParent(nodeIndex);
    }
}

void cPN_Feather::SetChildFeatherWeight(int nodeIndex, float weight)
{
    int childNodeIndex;
    for (int i = 0; i < m_pBaseHierarchy->GetNumChildren(nodeIndex); ++i)
    {
        childNodeIndex = m_pBaseHierarchy->GetChild(nodeIndex, i);
        m_pFeatherWeights[childNodeIndex] = weight;
        SetChildFeatherWeight(childNodeIndex, weight);
    }
}

void cPN_Feather::SetNodeWeight(int nodeIndex, float weight)
{
    m_pFeatherWeights[nodeIndex] = weight;
}

void cPN_Feather::BeginBlendIn(float duration)
{
    m_eFeatherBlendMode = FEATHER_BLEND_IN;
    if (duration > 0.0f)
    {
        m_fBlendTime = 0.0f;
        m_fBlendDuration = duration;
    }
    else
    {
        m_fBlendTime = 1.0f;
        m_fBlendDuration = 1.0f;
    }
}

void cPN_Feather::BeginBlendOut(float duration)
{
    m_eFeatherBlendMode = FEATHER_BLEND_OUT;
    if (duration > 0.0f)
    {
        m_fBlendDuration = duration;
        m_fBlendTime = 1.0f;
    }
    else
    {
        m_fBlendTime = 0.0f;
        m_fBlendDuration = 1.0f;
        if (GetChild(1) != 0)
        {
            delete GetChild(1);
            SetChild(1, 0);
        }
    }
}

cPoseNode* cPN_Feather::Update(float dt)
{
    if (GetChild(0))
    {
        SetChild(0, GetChild(0)->Update(dt));
    }
    if (GetChild(1))
    {
        SetChild(1, GetChild(1)->Update(dt));
    }

    if (m_fWeightTableCallback)
    {
        m_fWeightTableCallback(m_nCallbackParam1, this);
    }

    if (GetChild(1))
    {
        switch (m_eFeatherBlendMode)
        {
        case FEATHER_BLEND_IN:
            m_fBlendTime += dt / m_fBlendDuration;
            if (m_fBlendTime > 1.0f)
            {
                m_fBlendTime = 1.0f;
            }
            break;
        case FEATHER_BLEND_OUT:
            m_fBlendTime -= dt / m_fBlendDuration;
            if (m_fBlendTime <= 0.0f && GetChild(1) != 0)
            {
                delete GetChild(1);
                SetChild(1, 0);
            }
            break;
        }
    }

    return this;
}

void cPN_Feather::Evaluate(float weight, cPoseAccumulator* accumulator) const
{
    if (m_pFeatherWeights != 0 && GetChild(0) != 0 && GetChild(1) != 0)
    {
        accumulator->m_bUseObject = true;
        for (int i = 0; i < accumulator->GetNumNodes(); ++i)
        {
            Evaluate(i, weight, accumulator);
        }
        return;
    }

    if (GetChild(0) != 0)
    {
        GetChild(0)->Evaluate(weight, accumulator);
        return;
    }
    if (GetChild(1) != 0)
    {
        GetChild(1)->Evaluate(weight, accumulator);
    }
}

void cPN_Feather::Evaluate(
    int nodeIndex, float weight, cPoseAccumulator* accumulator) const
{
    float blendTime;
    float featherWeight;
    float baseNodeWeight;
    float featheredNodeWeight;

    if (GetChild(0) != 0 && GetChild(1) != 0)
    {
        blendTime = m_fBlendTime;
        featherWeight = m_pFeatherWeights[nodeIndex];
        if (blendTime < 1.0f)
        {
            featherWeight *= blendTime * (blendTime * ((-2.0f * blendTime) + 3.0f));
        }
        featheredNodeWeight = weight * featherWeight;
        baseNodeWeight = weight * (1.0f - featherWeight);
        GetChild(0)->Evaluate(nodeIndex, baseNodeWeight, accumulator);
        GetChild(1)->Evaluate(nodeIndex, featheredNodeWeight, accumulator);
        return;
    }

    if (GetChild(0) != 0)
    {
        GetChild(0)->Evaluate(nodeIndex, weight, accumulator);
        return;
    }
    if (GetChild(1) != 0)
    {
        GetChild(1)->Evaluate(nodeIndex, weight, accumulator);
    }
}

void cPN_Feather::BlendRootTrans(nlVector3*, float, float*)
{
}

void cPN_Feather::BlendRootRot(u16*, float, float*)
{
}
