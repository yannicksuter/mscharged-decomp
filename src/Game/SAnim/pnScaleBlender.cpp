#include "Game/SAnim/pnScaleBlender.h"

SlotPool<cPN_ScaleBlender> cPN_ScaleBlender::m_ScaleBlenderSlotPool(16, 16);

cPN_ScaleBlender::~cPN_ScaleBlender()
{
}

void cPN_ScaleBlender::BeginBlendIn(float duration)
{
    m_eScaleBlendMode = SCALE_BLEND_IN;
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

void cPN_ScaleBlender::BeginBlendOut(float duration)
{
    m_eScaleBlendMode = SCALE_BLEND_OUT;
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

cPoseNode* cPN_ScaleBlender::Update(float dt)
{
    if (GetChild(0))
    {
        SetChild(0, GetChild(0)->Update(dt));
    }
    if (GetChild(1))
    {
        SetChild(1, GetChild(1)->Update(dt));
    }

    if (GetChild(1))
    {
        switch (m_eScaleBlendMode)
        {
        case SCALE_BLEND_IN:
            m_fBlendTime += dt / m_fBlendDuration;
            if (m_fBlendTime > 1.0f)
            {
                m_fBlendTime = 1.0f;
            }
            break;
        case SCALE_BLEND_OUT:
            m_fBlendTime -= dt / m_fBlendDuration;
            if (m_fBlendTime <= 0.0f)
            {
                m_fBlendTime = 0.0f;
                if (GetChild(1) != 0)
                {
                    delete GetChild(1);
                    SetChild(1, 0);
                }
            }
            break;
        }
    }

    return this;
}

void cPN_ScaleBlender::Evaluate(
    float weight, cPoseAccumulator* accumulator) const
{
    if (GetChild(0) != 0)
    {
        GetChild(0)->Evaluate(weight, accumulator);
    }

    if (GetChild(1) != 0)
    {
        cPoseNode* scaleChild = GetChild(1);
        float blendTime = m_fBlendTime;
        float blendFactor = blendTime * (blendTime * ((-2.0f * blendTime) + 3.0f));
        weight *= blendFactor;
        if (weight > 0.0f)
        {
            for (int i = 0; i < accumulator->GetNumNodes(); ++i)
            {
                scaleChild->EvaluateScale(i, weight, accumulator);
            }
        }
    }
}

void cPN_ScaleBlender::Evaluate(
    int, float, cPoseAccumulator*) const
{
}

void cPN_ScaleBlender::BlendRootTrans(nlVector3*, float, float*)
{
}

void cPN_ScaleBlender::BlendRootRot(u16*, float, float*)
{
}
