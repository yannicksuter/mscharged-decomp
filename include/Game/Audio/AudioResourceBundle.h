#ifndef GAME_AUDIO_AUDIORESOURCEBUNDLE_H
#define GAME_AUDIO_AUDIORESOURCEBUNDLE_H

#include "types.h"

class nlChunk;
struct AudioSequenceDefinition;
struct SoundEventDefinition;
struct HitMarkerEventDefinition;
struct ParameterChangeEventDefinition;

struct AudioVoiceDefinition;

struct AudioCueEntry
{
    AudioVoiceDefinition* voice;
    float minimumValue;
    float weight;
    u32 selectionCount;
    u8 eligible;
    u8 pad_11[3];
};

struct AudioCueDefinition
{
    const char* name;
    u32 voiceCount;
    AudioCueEntry* voices;
    u8 useSlider;
    u8 pad_0D[3];
    s32 selectionMode;
    u32 sliderIndex;
    u32 selectedVoiceIndex;
    u32 activeCount;
    u32 maximumCount;
    void* field_24;
};

struct AudioVoiceDefinition
{
    const char* name;
    float volume;
    float pitch;
    u32 sliderIndex;
    u32 sequenceCount;
    AudioSequenceDefinition** sequences;
    u32 rpcGroupCount;
    u32* rpcGroupIndices;
    u32 dynamicRpcCount;
    u32 modifierCount;
    void** modifiers;
};

struct AudioResourceBundle
{
    u32 field_00;
    u32 field_04;
    u32 cueCount;
    AudioCueDefinition* cues;
    u32 voiceCount;
    AudioVoiceDefinition* voices;
    u32 sequenceCount;
    AudioSequenceDefinition* sequences;
    u32 soundEventCount;
    SoundEventDefinition* soundEvents;
    u32 hitMarkerEventCount;
    HitMarkerEventDefinition* hitMarkerEvents;
    u32 parameterEventCount;
    ParameterChangeEventDefinition* parameterEvents;
};

AudioResourceBundle* ParseAudioResourceBundle(nlChunk* chunk);
AudioVoiceDefinition* SelectAudioCueVoice(AudioCueDefinition* cue);
AudioVoiceDefinition* SelectAudioCueVoiceBySlider(
    AudioCueDefinition* cue, float value);
void InitializeAudioVoiceRpcModifiers(AudioVoiceDefinition* voice);

#endif // GAME_AUDIO_AUDIORESOURCEBUNDLE_H
