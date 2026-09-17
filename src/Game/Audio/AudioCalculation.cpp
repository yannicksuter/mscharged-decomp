#include "Game/Audio/AudioCalculation.h"
#include "NL/nlChunk.h"
#include "types.h"

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

    table->CreateSliders();
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
