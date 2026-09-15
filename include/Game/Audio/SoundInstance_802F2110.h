#ifndef GAME_AUDIO_SOUNDINSTANCE_802F2110_H
#define GAME_AUDIO_SOUNDINSTANCE_802F2110_H

#include "Game/Audio/Transition.h"
#include "NL/nlSlotPool.h"
#include "types.h"

class XSoundHandle;
class AudioSequenceInstance;
struct AudioSequenceDefinition;

struct RpcModifierDefinition
{
    u8 pad_00[0xC];
    s32 kind;
};

struct RpcRuntimeNode_802F2110
{
    RpcModifierDefinition* definition;
    float value;
    u8 valid;
    u8 pad_09[7];
};

struct RpcListEntry_802F2110
{
    RpcListEntry_802F2110* next;
    RpcListEntry_802F2110* previous;
    RpcRuntimeNode_802F2110* node;
};

struct VoiceDefinition_802F2110
{
    const char* name;
    float volume;
    float pitch;
    u32 sliderIndex;
    u32 voiceCount;
    AudioSequenceDefinition** voiceIds;
    u32 rpcGroupCount;
    u32* rpcGroupIndices;
    u8 pad_20[4];
    u32 modifierCount;
    RpcRuntimeNode_802F2110** modifiers;
};

struct SoundInstance_802F2110
{
    XSoundHandle* owner;
    VoiceDefinition_802F2110* definition;
    AudioSequenceInstance* voices;
    SlotPoolBase* entryPool;
    RpcListEntry_802F2110* rpcEntries;
    s32 state;
    float previousTime;
    float currentTime;
    RpcRuntimeNode_802F2110* activeRpc;
    float transitionTime;
    Transition volume;
    Transition pitch;
    float field_70;
    float releaseTime;
    SoundInstance_802F2110* nextInstance;
};

#endif // GAME_AUDIO_SOUNDINSTANCE_802F2110_H
