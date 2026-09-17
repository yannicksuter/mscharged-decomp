#ifndef GAME_AUDIO_AUDIO_CALCULATION_H
#define GAME_AUDIO_AUDIO_CALCULATION_H

#include "Game/Audio/Transition.h"
#include "NL/nlMemory.h"
#include "types.h"

class nlChunk;

struct AudioCalculationDefinition
{
    u8 pad_00[8];
    float initialValue;
    s32* field_0C;
    s32* field_10;
    s32* field_14;
};

class AudioCalculationSlider : public Transition
{
public:
    AudioCalculationSlider()
    {
        definition = 0;
        target = 0.0f;
        value = 0.0f;
        minimum = -96.0f;
        maximum = 6.0f;
        valid = true;
    }

    virtual float GetValue()
    {
        float result = duration + value;
        result = (result >= -96.0f) ? result : -96.0f;
        result = (result <= 6.0f) ? result : 6.0f;
        return result;
    }

    void Initialize(AudioCalculationDefinition* newDefinition)
    {
        definition = newDefinition;
        SetTarget(newDefinition->initialValue, 0.0f);
    }

    AudioCalculationDefinition* definition;
};

struct AudioCalculationTable
{
    void Update(float dt);

    void CreateSliders()
    {
        sliders = new (8, false) AudioCalculationSlider[count];
        for (u32 i = 0; i < count; i++)
        {
            sliders[i].Initialize(&definitions[i]);
            sliders[i].SetTarget(definitions[i].initialValue, 0.0f);
        }
    }

    u32 count;
    AudioCalculationDefinition* definitions;
    AudioCalculationSlider* sliders;
};

AudioCalculationTable* ParseAudioCalculationTable(nlChunk* chunk);

#endif // GAME_AUDIO_AUDIO_CALCULATION_H
