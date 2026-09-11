#include "Game/Effects/EffectsTemplate.h"

#include "Game/GL/GLInventory.h"
#include "Game/Effects/EmissionManager.h"
#include "NL/nlChunk.h"
#include "NL/gl/glState.h"
#include "Game/UnidentifiedStaticStorage.h"

unsigned int uSeed = 0x9184EB0C;

float fxAnimatedRange::Evaluate(float value) const
{
    if (mUseCurve != 0)
    {
        unsigned long i = 0;
        for (; i < mNumKeys; ++i)
        {
            if (mKeys[i].mTime > value)
            {
                break;
            }
        }

        const fxCurveKey& key
            = mKeys[i - 1];
        float value2 = value * value;
        float quadratic = key.mQuadratic * value2;
        float value3 = value * value2;
        float result = key.mCubic * value3 + quadratic;
        result = key.mLinear * value + result;
        return key.mConstant + result;
    }

    return RandomizedValue(base, range);
}

float fxAnimatedRange::GetMaximum() const
{
    if (mUseCurve != 0)
    {
        float maximum = -10000000000.0f;
        for (unsigned long i = 0; i < mNumKeys; ++i)
        {
            const fxCurveKey& key
                = mKeys[i];
            float value2
                = key.mTime * key.mTime;
            float value3 = key.mTime * value2;
            float quadratic = value2 * key.mQuadratic;
            float value = value3 * key.mCubic + quadratic;
            value = key.mTime * key.mLinear + value;
            value = key.mConstant + value;
            if (value > maximum)
            {
                maximum = value;
            }
        }

        const fxCurveKey& key
            = mKeys[mNumKeys - 1];
        float value = key.mCubic + key.mQuadratic
            + key.mLinear + key.mConstant;
        if (value > maximum)
        {
            maximum = value;
        }
        return maximum;
    }

    return base + 0.5f * range;
}

EffectsTemplate* EffectsTemplate::LoadFromChunk(nlChunk* chunk)
{
    nlChunk* templateChunk = static_cast<nlChunk*>(chunk->GetData());
    EffectsTemplate* result
        = static_cast<EffectsTemplate*>(templateChunk->GetData());

    for (int i = 0; i < 8; ++i)
    {
        templateChunk = templateChunk->GetNextChunk();
        nlChunk* valueChunk
            = static_cast<nlChunk*>(templateChunk->GetData());
        fxAnimatedRange* value
            = static_cast<fxAnimatedRange*>(valueChunk->GetData());
        if (value->mUseCurve != 0)
        {
            nlChunk* keysChunk = valueChunk->GetNextChunk();
            value->mKeys = static_cast<fxCurveKey*>(
                keysChunk->GetData());
        }
        result->mProperties[i] = value;
    }

    if (result->m_hTexture == 0xFFFFFFFF)
    {
        result->m_hTexture = glGetTexture("global/white");
    }

    if (result->m_uModelID != 0xFFFFFFFF
        && gEffectsModelInventory->GetModel(result->m_uModelID) == 0)
    {
        result->m_uModelID = 0xFFFFFFFF;
    }
    return result;
}

void EffectsTemplate::Cleanup()
{
}

float EffectsTemplate::GetBoundingRadius() const
{
    float result;
    switch (m_eEmitter)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    {
        float value4 = mProperties[4]->GetMaximum();
        float value1 = mProperties[1]->GetMaximum();
        result = value4 + value1 * mProperties[2]->GetMaximum();
        float duration
            = 0.5f * m_rParticleLife.range + m_rParticleLife.base;
        result += duration * mProperties[5]->GetMaximum();
        break;
    }
    default:
        result = 5.0f;
        break;
    }
    return result;
}
