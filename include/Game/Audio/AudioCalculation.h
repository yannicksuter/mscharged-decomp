#ifndef GAME_AUDIO_AUDIO_CALCULATION_H
#define GAME_AUDIO_AUDIO_CALCULATION_H

#include "Game/Audio/Transition.h"
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
    AudioCalculationSlider();
    ~AudioCalculationSlider();

    virtual float GetValue();

    AudioCalculationDefinition* definition;
};

struct AudioCalculationTable
{
    void Update(float dt);

    u32 count;
    AudioCalculationDefinition* definitions;
    AudioCalculationSlider* sliders;
};

AudioCalculationTable* ParseAudioCalculationTable(nlChunk* chunk);

#endif // GAME_AUDIO_AUDIO_CALCULATION_H
