#include "Game/SAnim/tu_8030E550.h"

SlotPool<cPN_8030E550> cPN_8030E550::mSlotPool(16, 16);

cPN_8030E550::~cPN_8030E550()
{
}

void cPN_8030E550::BeginBlendIn(float duration)
{
    m_eBlendMode = BLEND_MODE_8030E550_IN;
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

void cPN_8030E550::BeginBlendOut(float duration)
{
    m_eBlendMode = BLEND_MODE_8030E550_OUT;
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

cPoseNode* cPN_8030E550::Update(float dt)
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
        switch (m_eBlendMode)
        {
        case BLEND_MODE_8030E550_IN:
            m_fBlendTime += dt / m_fBlendDuration;
            if (m_fBlendTime > 1.0f)
            {
                m_fBlendTime = 1.0f;
            }
            break;
        case BLEND_MODE_8030E550_OUT:
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

void cPN_8030E550::Evaluate(
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

void cPN_8030E550::Evaluate(
    int, float, cPoseAccumulator*) const
{
}

void cPN_8030E550::BlendRootTrans(nlVector3*, float, float*)
{
}

void cPN_8030E550::BlendRootRot(u16*, float, float*)
{
}
