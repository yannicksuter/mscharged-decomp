#ifndef GAME_AUDIO_AUDIO_SLIDER_H
#define GAME_AUDIO_AUDIO_SLIDER_H

#include "Game/Audio/Transition.h"
#include "types.h"

class nlChunk;

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
    AudioSlider();
    ~AudioSlider();

    AudioSliderDefinition* definition;
};

struct AudioSliderSet
{
    u32 field_00;
    AudioSlider* sliders;
    void* owner;
};

struct AudioSliderTable
{
    void Update(float dt);

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

#endif // GAME_AUDIO_AUDIO_SLIDER_H
