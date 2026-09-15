#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioSlider.h"
#include "Game/Sys/debug.h"
#include "NL/nlChunk.h"
#include "NL/nlDebugString.h"
#include "NL/nlMemory.h"
#include "types.h"

#include <NMWException.h>

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

char sSliderNameFormat[] = "Slider name: %s\n";
char sGlobalSliderValueFormat[] = "Global slider Value %f\n";
char sLocalSliderValueFormat[] = "Local slider value %f\n";

extern "C" void fn_802EF17C(AudioSliderSet* set);
extern "C" AudioSliderSet* fn_802EF198(
    AudioSliderSet* set, int destroy);

static inline void SetSliderValue_802EE964(
    AudioSlider* slider, float value, float duration)
{
    slider->Update(value, duration);
}

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

    table->globalSliders = new (8, false) AudioSlider[table->globalCount];

    AudioSliderDefinition* globalDefinition;
    AudioSlider* globalSlider;
    u32 globalIndex;
    u32 globalDefinitionOffset;
    u32 globalSliderOffset;
    globalIndex = 0;
    globalDefinitionOffset = 0;
    globalSliderOffset = 0;
    while (globalIndex < table->globalCount)
    {
        globalDefinition
            = (AudioSliderDefinition*)((u8*)table->globalDefinitionsCopy
                + globalDefinitionOffset);
        globalSlider
            = (AudioSlider*)((u8*)table->globalSliders + globalSliderOffset);
        globalSlider->definition = globalDefinition;
        float initialValue;
        float minimumValue;
        float maximumValue;
        maximumValue = globalDefinition->maximumValue;
        minimumValue = globalDefinition->minimumValue;
        initialValue = globalDefinition->initialValue;
        globalSlider->target = initialValue;
        globalSlider->value = initialValue;
        globalSlider->minimum = minimumValue;
        globalSlider->maximum = maximumValue;
        globalSlider->valid = true;
        globalIndex++;
        globalDefinitionOffset += sizeof(AudioSliderDefinition);
        globalSliderOffset += sizeof(AudioSlider);
    }

    table->localSets = (AudioSliderSet*)__construct_new_array(
        nlMalloc((0x50 * sizeof(AudioSliderSet)) + 0x10, 8, false),
        (ctor_dtor_ptr)fn_802EF17C,
        (ctor_dtor_ptr)fn_802EF198,
        sizeof(AudioSliderSet),
        0x50);

    AudioSliderSet* set;
    u32 setIndex;
    u32 setOffset;
    setIndex = 0;
    setOffset = 0;
    while (setIndex < 0x50)
    {
        set = (AudioSliderSet*)((u8*)table->localSets + setOffset);
        set->sliders = new (8, false) AudioSlider[table->localCount];

        AudioSliderDefinition* definition;
        AudioSlider* slider;
        u32 sliderIndex;
        u32 localDefinitionOffset;
        u32 localSliderOffset;
        sliderIndex = 0;
        localDefinitionOffset = 0;
        localSliderOffset = 0;
        while (sliderIndex < table->localCount)
        {
            definition
                = (AudioSliderDefinition*)((u8*)table->localDefinitions
                    + localDefinitionOffset);
            slider = (AudioSlider*)((u8*)set->sliders + localSliderOffset);
            slider->definition = definition;
            float initialValue;
            float minimumValue;
            float maximumValue;
            maximumValue = definition->maximumValue;
            minimumValue = definition->minimumValue;
            initialValue = definition->initialValue;
            slider->target = initialValue;
            slider->value = initialValue;
            slider->minimum = minimumValue;
            slider->maximum = maximumValue;
            slider->valid = true;
            sliderIndex++;
            localDefinitionOffset += sizeof(AudioSliderDefinition);
            localSliderOffset += sizeof(AudioSlider);
        }
        setIndex++;
        setOffset += sizeof(AudioSliderSet);
    }
    return table;
}

extern "C" AudioSlider* fn_802EED38(
    AudioSliderTable* table, u32 index, SliderOwner_802EE964* owner)
{
    AudioSliderDefinition* definition
        = table->globalDefinitions + table->localToGlobal[index];
    if (definition->kind == 2)
        return table->globalSliders + definition->index;
    return owner->localSet->sliders + definition->index;
}

extern "C" AudioSliderSet* fn_802EED88(
    AudioSliderTable* table, SliderOwner_802EE964* owner)
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
        SetSliderValue_802EE964(slider, 0.0f, 1.0f);
    }

    AudioSliderDefinition* definition
        = table->globalDefinitions + table->localToGlobal[0];
    AudioSlider* slider = set->sliders + definition->index;
    tDebugPrintManager::Print(
        DC_SOUND,
        sSliderNameFormat,
        nlLookupDebugString(
            g_pDebugStringTable, (unsigned long)slider->definition->name));
    slider->SetTarget((float)owner->valueSource->value, 0.0f);
    SetSliderValue_802EE964(slider, 0.0f, 1.0f);
    return set;
}

void AudioSliderTable::Update(float dt)
{
    for (u32 i = 0; i < globalCount; i++)
    {
        AudioSlider* slider = globalSliders + i;
        SetSliderValue_802EE964(slider, dt, 1.0f);
        if (slider->valid)
            tDebugPrintManager::Print(
                DC_SOUND, sGlobalSliderValueFormat, slider->value);
    }

    for (u32 setIndex = 0; setIndex < 0x50; setIndex++)
    {
        AudioSliderSet* set = localSets + setIndex;
        if (set->owner == 0)
            continue;
        for (u32 sliderIndex = 0; sliderIndex < localCount;
            sliderIndex++)
        {
            AudioSlider* slider = set->sliders + sliderIndex;
            SetSliderValue_802EE964(slider, dt, 1.0f);
            if (slider->valid)
                tDebugPrintManager::Print(
                    DC_SOUND, sLocalSliderValueFormat, slider->value);
        }
    }
}

AudioSlider::AudioSlider()
{
    definition = 0;
    target = 0.0f;
    value = 0.0f;
    minimum = 0.0f;
    maximum = 0.0f;
    valid = true;
}

extern "C" void fn_802EF17C(AudioSliderSet* set)
{
    set->field_00 = 0xFFFF;
    set->sliders = 0;
    set->owner = 0;
}

extern "C" AudioSliderSet* fn_802EF198(
    AudioSliderSet* set, int destroy)
{
    if (set != 0 && destroy > 0)
        operator delete(set);
    return set;
}

AudioSlider::~AudioSlider() { }
