#ifndef GAME_AUDIO_CUEHANDLE_802F1758_H
#define GAME_AUDIO_CUEHANDLE_802F1758_H

#include "NL/nlDLListContainer.h"
#include "types.h"

class PlaybackBackend_802F2C3C;

struct SliderState_802F1758
{
    void** vtable;
    float value;
    u8 valid;
    u8 pad_09[0x1F];
};

struct LocalSliderSet_802F1758
{
    u32 field_00;
    SliderState_802F1758* sliders;
    void* owner;
};

struct CueDefinition_802F1758
{
    const char* name;
    u8 pad_04[8];
    u8 useSlider;
    u8 pad_0D[7];
    u32 sliderIndex;
    u8 pad_18[4];
    u32 activeCount;
    u32 maximumCount;
    void* field_24;
};

struct CueDefinitionTable_802F1758
{
    u8 pad_00[0xC];
    CueDefinition_802F1758* definitions;
};

struct CueResource_802F1758
{
    u8 pad_00[0xC];
    CueDefinitionTable_802F1758* cues;
};

struct CueOwner_802F1758
{
    u8 pad_00[0x1C];
    u32 referencesAndFlags;
};

struct RpcRuntimeNode_802F1758;
struct SoundInstance_802F1758;

extern "C" void fn_802F2A74(SoundInstance_802F1758*);

struct SoundInstance_802F1758
{
    ~SoundInstance_802F1758()
    {
        fn_802F2A74(this);
    }

    void* owner;
    void* definition;
    void* voices;
    DLListContainerBase<RpcRuntimeNode_802F1758*,
        BasicSlotPool<DLListEntry<RpcRuntimeNode_802F1758*> >&> rpcEntries;
    s32 state;
    float previousTime;
    float currentTime;
    void* activeRpc;
    float transitionTime;
    u8 pad_28[0x48];
    float field_70;
    float field_74;
    SoundInstance_802F1758* nextInstance;
};

struct CueHandle_802F1758
{
    void** vtable;
    CueResource_802F1758* resource;
    u32 field_08;
    s32 state;
    u8 callbackEnabled;
    u8 pad_11[3];
    CueOwner_802F1758* owner;
    float previousTime;
    float currentTime;
    LocalSliderSet_802F1758* localSliders;
    void (*callback)(void*, CueHandle_802F1758*, void*);
    void* callbackContext;
    CueDefinition_802F1758* definition;
    SoundInstance_802F1758* instance;
    SliderState_802F1758* slider;
    float sliderValue;
    union
    {
        u32 stateAndFlags;
        struct
        {
            u32 savedState : 16;
            u32 field_8000 : 1;
            u32 field_4000 : 1;
            u32 field_3FFF : 14;
        } bits;
    };
};

#endif
