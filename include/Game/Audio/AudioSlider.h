#ifndef GAME_AUDIO_AUDIO_SLIDER_H
#define GAME_AUDIO_AUDIO_SLIDER_H

#include "Game/Audio/Transition.h"
#include "NL/nlMemory.h"
#include "types.h"

class nlChunk;
class XSoundHandle;

struct AudioSliderDefinition
{
    u32 field_00;
    const char* name;
    float initialValue;
    float minimumValue;
    float maximumValue;
    u32 field_14;
    s32 kind;
    u32 index;
};

class AudioSlider : public Transition
{
public:
    AudioSlider()
    {
        definition = 0;
        Reset(0.0f, 0.0f, 0.0f);
    }

    void Initialize(AudioSliderDefinition* newDefinition)
    {
        definition = newDefinition;
        Reset(newDefinition->initialValue, newDefinition->minimumValue, newDefinition->maximumValue);
    }

    AudioSliderDefinition* definition;
};

struct AudioSliderSet
{
    AudioSliderSet()
    {
        field_00 = 0xFFFF;
        sliders = 0;
        owner = 0;
    }
    ~AudioSliderSet() { }

    u32 field_00;
    AudioSlider* sliders;
    void* owner;
};

struct AudioSliderTable
{
    void Update(float dt);

    void CreateGlobalSliders()
    {
        globalSliders = new (8, false) AudioSlider[globalCount];
        for (u32 i = 0; i < globalCount; i++)
            globalSliders[i].Initialize(&globalDefinitionsCopy[i]);
    }

    void CreateLocalSets()
    {
        localSets = new (8, false) AudioSliderSet[0x50];
        for (u32 i = 0; i < 0x50; i++)
        {
            AudioSliderSet* set = &localSets[i];
            set->sliders = new (8, false) AudioSlider[localCount];
            for (u32 j = 0; j < localCount; j++)
                set->sliders[j].Initialize(&localDefinitions[j]);
        }
    }

    u32 field_00;
    AudioSliderDefinition* globalDefinitions;
    u32 globalCount;
    AudioSliderDefinition* globalDefinitionsCopy;
    u32 localCount;
    AudioSliderDefinition* localDefinitions;
    u32* localToGlobal;
    AudioSlider* globalSliders;
    AudioSliderSet* localSets;
};

AudioSliderTable* ParseAudioSliderTable(nlChunk* chunk);
extern "C" AudioSlider* GetAudioSlider(
    AudioSliderTable* table, u32 index, XSoundHandle* owner);

#endif // GAME_AUDIO_AUDIO_SLIDER_H
