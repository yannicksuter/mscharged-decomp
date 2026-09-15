#include "Game/Audio/AudioCalculation.h"
#include "NL/nlChunk.h"
#include "NL/nlMemory.h"
#include "types.h"

#include <NMWException.h>

AudioCalculationTable* ParseAudioCalculationTable(nlChunk* outer)
{
    nlChunk* header = (nlChunk*)outer->GetData();
    AudioCalculationTable* table = (AudioCalculationTable*)header->GetData();
    u32 relocationDelta = (u32)table->definitions;

    nlChunk* definitions = header->GetNextChunk();
    table->definitions = (AudioCalculationDefinition*)definitions->GetData();
    relocationDelta -= (u32)table->definitions;
    u32 relocationOffset;
    u32 relocationIndex;
    relocationIndex = 0;
    relocationOffset = 0;
    while (relocationIndex < table->count)
    {
        AudioCalculationDefinition* definition
            = (AudioCalculationDefinition*)((u8*)table->definitions
                + relocationOffset);
        if (definition->field_0C != 0)
            definition->field_0C
                = (s32*)((u8*)definition->field_0C - relocationDelta);
        if (definition->field_10 != 0)
            definition->field_10
                = (s32*)((u8*)definition->field_10 - relocationDelta);
        if (definition->field_14 != 0)
            definition->field_14
                = (s32*)((u8*)definition->field_14 - relocationDelta);
        relocationOffset += sizeof(AudioCalculationDefinition);
        relocationIndex++;
    }

    table->sliders = new (8, false) AudioCalculationSlider[table->count];

    u32 i = 0;
    u32 definitionOffset = 0;
    u32 sliderOffset = 0;
    while (i < table->count)
    {
        AudioCalculationDefinition* definition
            = (AudioCalculationDefinition*)((u8*)table->definitions
                + definitionOffset);
        AudioCalculationSlider* slider
            = (AudioCalculationSlider*)((u8*)table->sliders + sliderOffset);
        slider->definition = definition;
        slider->SetTarget(definition->initialValue, 0.0f);
        ((AudioCalculationSlider*)((u8*)table->sliders + sliderOffset))
            ->SetTarget(
                ((AudioCalculationDefinition*)((u8*)table->definitions
                    + definitionOffset))
                    ->initialValue,
                0.0f);
        definitionOffset += sizeof(AudioCalculationDefinition);
        sliderOffset += sizeof(AudioCalculationSlider);
        i++;
    }
    return table;
}

void AudioCalculationTable::Update(float dt)
{
    AudioCalculationSlider* slider = sliders;
    AudioCalculationDefinition* definition = definitions;
    for (u32 i = 0; i < count;)
    {
        float multiplier = definition->field_0C != 0
                             ? sliders[*definition->field_0C].value
                             : 1.0f;
        slider->Update(dt, multiplier);
        i++;
        slider++;
        definition++;
    }
}

AudioCalculationSlider::AudioCalculationSlider()
{
    definition = 0;
    target = 0.0f;
    value = 0.0f;
    minimum = -96.0f;
    maximum = 6.0f;
    valid = true;
}

float AudioCalculationSlider::GetValue()
{
    float result = duration + value;
    result = (result >= -96.0f) ? result : -96.0f;
    result = (result <= 6.0f) ? result : 6.0f;
    return result;
}

AudioCalculationSlider::~AudioCalculationSlider() { }
