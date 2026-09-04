#include "Game/Effects/EffectsTemplate.h"

#include "Game/GL/GLInventory.h"
#include "Game/SAnim.h"
#include "NL/gl/glState.h"

extern GLInventory* lbl_806E1FFC;

unsigned int uSeed = 0x9184EB0C;

float Unidentified_802E0010::fn_802E0010(float value) const
{
    if (mUnidentified000 != 0)
    {
        unsigned long i = 0;
        for (; i < mUnidentified00C; ++i)
        {
            if (mUnidentified010[i].mUnidentified000 > value)
            {
                break;
            }
        }

        const UnidentifiedEffectsKey_802E0010& key
            = mUnidentified010[i - 1];
        float value2 = value * value;
        float quadratic = key.mUnidentified008 * value2;
        float value3 = value * value2;
        float result = key.mUnidentified004 * value3 + quadratic;
        result = key.mUnidentified00C * value + result;
        return key.mUnidentified010 + result;
    }

    return RandomizedValue(base, range);
}

float Unidentified_802E0010::fn_802E0108() const
{
    if (mUnidentified000 != 0)
    {
        float maximum = -10000000000.0f;
        for (unsigned long i = 0; i < mUnidentified00C; ++i)
        {
            const UnidentifiedEffectsKey_802E0010& key
                = mUnidentified010[i];
            float value2
                = key.mUnidentified000 * key.mUnidentified000;
            float value3 = key.mUnidentified000 * value2;
            float quadratic = value2 * key.mUnidentified008;
            float value = value3 * key.mUnidentified004 + quadratic;
            value = key.mUnidentified000 * key.mUnidentified00C + value;
            value = key.mUnidentified010 + value;
            if (value > maximum)
            {
                maximum = value;
            }
        }

        const UnidentifiedEffectsKey_802E0010& key
            = mUnidentified010[mUnidentified00C - 1];
        float value = key.mUnidentified004 + key.mUnidentified008
            + key.mUnidentified00C + key.mUnidentified010;
        if (value > maximum)
        {
            maximum = value;
        }
        return maximum;
    }

    return base + 0.5f * range;
}

EffectsTemplate* EffectsTemplate::fn_802E01C8(nlChunk* chunk)
{
    nlChunk* templateChunk = static_cast<nlChunk*>(chunk->GetData());
    EffectsTemplate* result
        = static_cast<EffectsTemplate*>(templateChunk->GetData());

    for (int i = 0; i < 8; ++i)
    {
        templateChunk = templateChunk->GetNextChunk();
        nlChunk* valueChunk
            = static_cast<nlChunk*>(templateChunk->GetData());
        Unidentified_802E0010* value
            = static_cast<Unidentified_802E0010*>(valueChunk->GetData());
        if (value->mUnidentified000 != 0)
        {
            nlChunk* keysChunk = valueChunk->GetNextChunk();
            value->mUnidentified010 = static_cast<UnidentifiedEffectsKey_802E0010*>(
                keysChunk->GetData());
        }
        result->mUnidentified058[i] = value;
    }

    if (result->m_hTexture == 0xFFFFFFFF)
    {
        result->m_hTexture = glGetTexture("global/white");
    }

    if (result->m_uModelID != 0xFFFFFFFF
        && lbl_806E1FFC->GetMaterialList(result->m_uModelID) == 0)
    {
        result->m_uModelID = 0xFFFFFFFF;
    }
    return result;
}

void EffectsTemplate::fn_802E04C8()
{
}

float EffectsTemplate::fn_802E04CC() const
{
    float result;
    switch (m_eEmitter)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    {
        float value4 = mUnidentified058[4]->fn_802E0108();
        float value1 = mUnidentified058[1]->fn_802E0108();
        result = value4 + value1 * mUnidentified058[2]->fn_802E0108();
        float duration = 0.5f * mUnidentified014 + mUnidentified010;
        result += duration * mUnidentified058[5]->fn_802E0108();
        break;
    }
    default:
        result = 5.0f;
        break;
    }
    return result;
}
