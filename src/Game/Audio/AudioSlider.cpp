#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioSlider.h"
#include "Game/Sys/debug.h"
#include "NL/nlChunk.h"
#include "NL/nlDebugString.h"
#include "types.h"

struct SliderOwnerValue_802EE964
{
    u8 pad_00[0x1C];
    u32 value;
};

struct SliderOwner_802EE964
{
    u8 pad_00[0x20];
    AudioSliderSet* localSet;
    u8 pad_24[8];
    SliderOwnerValue_802EE964* valueSource;
};

AudioSliderTable* ParseAudioSliderTable(nlChunk* outer)
{
    nlChunk* header = (nlChunk*)outer->GetData();
    AudioSliderTable* table = (AudioSliderTable*)header->GetData();

    nlChunk* definitions = header->GetNextChunk();
    table->globalDefinitions = (AudioSliderDefinition*)definitions->GetData();
    table->globalDefinitionsCopy = table->globalDefinitions;
    table->localDefinitions = table->globalDefinitions + table->globalCount;

    nlChunk* indices = definitions->GetNextChunk();
    table->localToGlobal = (u32*)indices->GetData();

    table->CreateGlobalSliders();
    table->CreateLocalSets();
    return table;
}

extern "C" AudioSlider* fn_802EED38(AudioSliderTable* table, u32 index, SliderOwner_802EE964* owner)
{
    AudioSliderDefinition* definition = table->globalDefinitions + table->localToGlobal[index];
    if (definition->kind == 2)
        return table->globalSliders + definition->index;
    return owner->localSet->sliders + definition->index;
}

extern "C" AudioSliderSet* fn_802EED88(AudioSliderTable* table, SliderOwner_802EE964* owner)
{
    AudioSliderSet* set = table->localSets;
    u32 setIndex;
    for (setIndex = 0; setIndex < 0x50; setIndex++, set++)
    {
        if (set->owner == 0)
            break;
    }
    if (setIndex >= 0x50)
        DumpAudioMemory();

    set->owner = owner;
    for (u32 i = 0; i < table->localCount; i++)
    {
        AudioSlider* slider = set->sliders + i;
        slider->SetTarget(slider->definition->initialValue, 0.0f);
        slider->Update(0.0f, 1.0f);
    }

    AudioSliderDefinition* definition = table->globalDefinitions + table->localToGlobal[0];
    AudioSlider* slider = set->sliders + definition->index;
    tDebugPrintManager::Print(DC_SOUND, "Slider name: %s\n", nlLookupDebugString(g_pDebugStringTable, (unsigned long)slider->definition->name));
    slider->SetTarget((float)owner->valueSource->value, 0.0f);
    slider->Update(0.0f, 1.0f);
    return set;
}

void AudioSliderTable::Update(float dt)
{
    for (u32 i = 0; i < globalCount; i++)
    {
        AudioSlider* slider = globalSliders + i;
        slider->Update(dt, 1.0f);
        if (slider->valid)
            tDebugPrintManager::Print(DC_SOUND, "Global slider Value %f\n", slider->value);
    }

    for (u32 setIndex = 0; setIndex < 0x50; setIndex++)
    {
        AudioSliderSet* set = localSets + setIndex;
        if (set->owner == 0)
            continue;
        for (u32 sliderIndex = 0; sliderIndex < localCount; sliderIndex++)
        {
            AudioSlider* slider = set->sliders + sliderIndex;
            slider->Update(dt, 1.0f);
            if (slider->valid)
                tDebugPrintManager::Print(DC_SOUND, "Local slider value %f\n", slider->value);
        }
    }
}
