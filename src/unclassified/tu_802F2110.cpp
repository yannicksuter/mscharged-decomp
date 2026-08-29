#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

struct Transition_802F2110
{
    void** vtable;
    float value;
    u8 valid;
    u8 pad_09[3];
    float target;
    float elapsed;
    float duration;
    float minimum;
    float maximum;
    u8 enabled;
    u8 pad_21[3];
};

struct VoiceDefinition_802F2110
{
    const char* name;
    float volume;
    float pitch;
    u32 sliderIndex;
    u32 voiceCount;
    u32* voiceIds;
    u32 rpcGroupCount;
    u32* rpcGroupIndices;
};

struct VoiceNode_802F2110
{
    VoiceNode_802F2110* next;
    u8 pad_04[0x18];
};

struct RpcRuntimeNode_802F2110
{
    void* definition;
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

struct RpcDefinition_802F2110
{
    u8 pad_00[8];
    u32 kind;
    u8 pad_0C[0x18];
};

struct RpcGroup_802F2110
{
    u8 pad_00[0x14];
    u32 definitionCount;
    RpcDefinition_802F2110* definitions;
};

struct RpcController_802F2110
{
    u32 groupCount;
    RpcGroup_802F2110* groups;
};

struct CueHandle_802F2110;

struct SoundInstance_802F2110
{
    CueHandle_802F2110* owner;
    VoiceDefinition_802F2110* definition;
    VoiceNode_802F2110* voices;
    SlotPoolBase* entryPool;
    RpcListEntry_802F2110* rpcEntries;
    s32 state;
    float previousTime;
    float currentTime;
    RpcRuntimeNode_802F2110* activeRpc;
    float transitionTime;
    Transition_802F2110 volume;
    Transition_802F2110 pitch;
    float field_70;
    float releaseTime;
    SoundInstance_802F2110* nextInstance;
};

struct CueHandle_802F2110
{
    void** vtable;
    void* resource;
    u8 pad_08[4];
    s32 state;
    u8 pad_10[0x10];
    void* localSliders;
};

struct AudioResources_802F2110
{
    u8 pad_00[0xC];
    void* sliderTable;
    void* calculationTable;
    u8 pad_14[0x3C];
    RpcController_802F2110* rpcController;
};

struct AudioSystem_802F2110
{
    u8 pad_00[0xCC];
    AudioResources_802F2110* resources;
};

extern void* lbl_8052F3C8[];
extern SlotPoolBase lbl_8057FA10;
extern SlotPoolBase lbl_8057FB78;
extern AudioSystem_802F2110* lbl_806E201C;

extern "C" void* fn_802F0394(RpcController_802F2110*, RpcDefinition_802F2110*, SoundInstance_802F2110*);
extern "C" void fn_802F04D4(RpcController_802F2110*, SoundInstance_802F2110*);
extern "C" void* fn_802F1A70(CueHandle_802F2110*, u32);
extern "C" VoiceNode_802F2110* fn_802F3E20(VoiceNode_802F2110*, SoundInstance_802F2110*, u32);
extern "C" void fn_802F3ECC(VoiceNode_802F2110*, int);
extern "C" void fn_802F3F6C(VoiceNode_802F2110*);
extern "C" void fn_802F4070(VoiceNode_802F2110*);
extern "C" int fn_802F437C(VoiceNode_802F2110*, float);
extern "C" void fn_802F4174(VoiceNode_802F2110*);
extern "C" void fn_802F4278(VoiceNode_802F2110*);
extern "C" void fn_802F4518(VoiceNode_802F2110*);
extern "C" void fn_802F4640(VoiceNode_802F2110*, u32, void*);

static inline void SetTransition_802F2110(Transition_802F2110* transition,
    float target, float duration)
{
    typedef void (*SetFunc)(Transition_802F2110*, float, float);
    ((SetFunc)transition->vtable[3])(transition, target, duration);
}

static inline VoiceNode_802F2110* AllocateVoice_802F2110()
{
    VoiceNode_802F2110* voice = 0;
    if (lbl_8057FB78.m_FreeList == 0)
        SlotPoolBase::BaseAddNewBlock(&lbl_8057FB78, sizeof(VoiceNode_802F2110));
    if (lbl_8057FB78.m_FreeList != 0)
    {
        voice = (VoiceNode_802F2110*)lbl_8057FB78.m_FreeList;
        lbl_8057FB78.m_FreeList = lbl_8057FB78.m_FreeList->next;
    }
    return voice;
}

static inline RpcListEntry_802F2110* AllocateEntry_802F2110(SlotPoolBase* pool)
{
    RpcListEntry_802F2110* entry = 0;
    if (pool->m_FreeList == 0)
        SlotPoolBase::BaseAddNewBlock(pool, sizeof(RpcListEntry_802F2110));
    if (pool->m_FreeList != 0)
    {
        entry = (RpcListEntry_802F2110*)pool->m_FreeList;
        pool->m_FreeList = pool->m_FreeList->next;
    }
    if (entry != 0)
    {
        entry->next = 0;
        entry->previous = 0;
        entry->node = 0;
    }
    return entry;
}

static inline void AppendEntry_802F2110(SoundInstance_802F2110* instance,
    RpcListEntry_802F2110* entry)
{
    if (instance->rpcEntries == 0)
    {
        instance->rpcEntries = entry;
        entry->next = entry;
        entry->previous = entry;
    }
    else
    {
        RpcListEntry_802F2110* tail = instance->rpcEntries->previous;
        tail->next = entry;
        entry->next = instance->rpcEntries;
        entry->previous = tail;
        instance->rpcEntries->previous = entry;
        instance->rpcEntries = entry;
    }
}

static inline void DestroyVoices_802F2110(SoundInstance_802F2110* instance)
{
    VoiceNode_802F2110* voice = instance->voices;
    while (voice != 0)
    {
        VoiceNode_802F2110* next = voice->next;
        fn_802F3ECC(voice, 1);
        voice = next;
    }
    instance->voices = 0;
}

extern "C" bool fn_802F2110()
{
    return true;
}

extern "C" SlotPoolBase* fn_802F2118(SlotPoolBase* pool, int destroy)
{
    if (pool != 0)
    {
        fn_802B467C(pool);
        SlotPoolBase::BaseFreeBlocks(pool, 0x40);
        pool->~SlotPoolBase();
        if (destroy > 0)
            operator delete(pool);
    }
    return pool;
}

extern "C" SoundInstance_802F2110* fn_802F2188(SoundInstance_802F2110* instance,
    CueHandle_802F2110* owner, VoiceDefinition_802F2110* definition)
{
    instance->volume.value = 0.0f;
    instance->volume.valid = true;
    instance->volume.target = 0.0f;
    instance->volume.minimum = 0.0f;
    instance->volume.maximum = 1.0f;
    instance->owner = owner;
    instance->definition = definition;
    instance->voices = 0;
    instance->rpcEntries = 0;
    instance->entryPool = &lbl_8057FA10;
    instance->state = 0;
    instance->previousTime = -1.0f;
    instance->currentTime = 0.0f;
    instance->activeRpc = 0;
    instance->transitionTime = 0.0f;
    instance->volume.vtable = lbl_8052F3C8;
    instance->volume.elapsed = -1.0f;
    instance->volume.duration = 1.0f;
    instance->volume.enabled = true;
    instance->pitch.vtable = lbl_8052F3C8;
    instance->pitch.elapsed = -1.0f;
    instance->pitch.duration = 1.0f;
    instance->pitch.enabled = true;
    instance->field_70 = 0.0f;
    instance->releaseTime = -1.0f;
    instance->nextInstance = 0;

    instance->volume.target = 0.0f;
    instance->volume.value = 0.0f;
    instance->volume.minimum = -96.0f;
    instance->volume.maximum = 6.0f;
    instance->volume.valid = true;
    instance->pitch.target = 0.0f;
    instance->pitch.value = 0.0f;
    instance->pitch.minimum = -12.0f;
    instance->pitch.maximum = 12.0f;
    instance->pitch.valid = true;

    VoiceNode_802F2110* previous = 0;
    for (u32 i = 0; i < definition->voiceCount; i++)
    {
        VoiceNode_802F2110* voice = AllocateVoice_802F2110();
        if (voice != 0)
            voice = fn_802F3E20(voice, instance, definition->voiceIds[i]);
        if (previous == 0)
            instance->voices = voice;
        else
            previous->next = voice;
        previous = voice;
    }
    return instance;
}

extern "C" void fn_802F2320(SoundInstance_802F2110* instance, float)
{
    SetTransition_802F2110(&instance->volume, 0.0f, 1.0f);
    SetTransition_802F2110(&instance->pitch, 0.0f, 1.0f);
    if (instance->voices != 0)
        fn_802F3F6C(instance->voices);
    instance->state = 4;
}

extern "C" void fn_802F2398(SoundInstance_802F2110* instance)
{
    float value = instance->definition->volume;
    if (value < instance->volume.minimum)
        value = instance->volume.minimum;
    else if (value > instance->volume.maximum)
        value = instance->volume.maximum;
    instance->volume.target = value;
    instance->volume.elapsed = 0.0f;

    value = instance->definition->pitch;
    if (value < instance->pitch.minimum)
        value = instance->pitch.minimum;
    else if (value > instance->pitch.maximum)
        value = instance->pitch.maximum;
    instance->pitch.target = value;
    instance->pitch.elapsed = 0.0f;

    RpcController_802F2110* controller = lbl_806E201C->resources->rpcController;
    for (u32 groupIndex = 0; groupIndex < instance->definition->rpcGroupCount;
        groupIndex++)
    {
        RpcGroup_802F2110* group = controller->groups
                                 + instance->definition->rpcGroupIndices[groupIndex];
        for (u32 definitionIndex = 0;
            definitionIndex < group->definitionCount;
            definitionIndex++)
        {
            RpcDefinition_802F2110* definition = group->definitions + definitionIndex;
            RpcRuntimeNode_802F2110* node = (RpcRuntimeNode_802F2110*)fn_802F0394(
                controller, definition, instance);
            instance->activeRpc = definition->kind == 2 ? node : 0;
            RpcListEntry_802F2110* entry = AllocateEntry_802F2110(instance->entryPool);
            entry->node = node;
            AppendEntry_802F2110(instance, entry);
        }
    }

    if (instance->voices != 0)
    {
        fn_802F4070(instance->voices);
        instance->state = 2;
    }
    else
        instance->state = 3;
}

extern "C" void fn_802F2594(SoundInstance_802F2110* instance, bool saveDuration,
    float target, float duration)
{
    if (target < instance->volume.minimum)
        instance->volume.target = instance->volume.minimum;
    else if (target > instance->volume.maximum)
        instance->volume.target = instance->volume.maximum;
    else
        instance->volume.target = target;
    instance->volume.elapsed = duration;
    if (saveDuration)
        instance->releaseTime = duration;
}

extern "C" void fn_802F25D4(SoundInstance_802F2110* instance, void* preserve)
{
    if (instance->state != 4 || preserve != 0 || instance->activeRpc == 0)
    {
        instance->activeRpc = 0;
        if (instance->voices != 0)
            fn_802F4518(instance->voices);
    }
    instance->state = 7;
}

extern "C" void fn_802F2640(SoundInstance_802F2110* instance)
{
    fn_802F4174(instance->voices);
}

extern "C" void fn_802F2648(SoundInstance_802F2110* instance)
{
    fn_802F4278(instance->voices);
}

extern "C" void fn_802F2650(SoundInstance_802F2110* instance,
    u32 value, void* output)
{
    for (VoiceNode_802F2110* voice = instance->voices;
        voice != 0;
        voice = voice->next)
    {
        fn_802F4640(voice, value, output);
    }
}

extern "C" void fn_802F26B0(SoundInstance_802F2110* instance, float dt)
{
    if (instance->state == 4 || instance->state == 7)
    {
        SetTransition_802F2110(&instance->volume, dt, 1.0f);
        SetTransition_802F2110(&instance->pitch, dt, 1.0f);
        instance->previousTime = instance->currentTime;
        instance->currentTime += dt;
    }

    if (instance->activeRpc != 0 && instance->state == 7)
    {
        instance->transitionTime += dt;
        Transition_802F2110* slider = (Transition_802F2110*)fn_802F1A70(instance->owner, 2);
        float value = instance->transitionTime;
        if (value < slider->minimum)
            slider->target = slider->minimum;
        else if (value > slider->maximum)
            slider->target = slider->maximum;
        else
            slider->target = value;
        slider->elapsed = 0.0f;
        if (instance->activeRpc->value < -94.0f)
        {
            if (instance->voices != 0)
                fn_802F4518(instance->voices);
            instance->activeRpc = 0;
        }
    }

    int voiceState = instance->voices != 0
                       ? fn_802F437C(instance->voices, dt)
                       : 8;
    switch (instance->state)
    {
    case 2:
        if (voiceState == 3)
            instance->state = 3;
        break;
    case 4:
    case 7:
        if (voiceState == 8)
        {
            fn_802F04D4(lbl_806E201C->resources->rpcController, instance);
            instance->rpcEntries = 0;
            DestroyVoices_802F2110(instance);
            instance->state = 8;
        }
        break;
    case 6:
        instance->state = 8;
        break;
    default:
        break;
    }

    if (instance->releaseTime >= 0.0f)
    {
        instance->releaseTime -= dt;
        if (instance->releaseTime < 0.0f)
        {
            if (instance->state != 4 || instance->activeRpc == 0)
            {
                instance->activeRpc = 0;
                if (instance->voices != 0)
                    fn_802F4518(instance->voices);
            }
            instance->state = 7;
        }
    }
}

extern "C" float fn_802F29F8(SoundInstance_802F2110* instance)
{
    void* table = lbl_806E201C->resources->calculationTable;
    u8* entry = (u8*)*(void**)((u8*)table + 8)
              + instance->definition->sliderIndex * 0x28;
    typedef float (*CalculateFunc)(void*, void*, AudioResources_802F2110*);
    float value = ((CalculateFunc)(*(void***)entry)[2])(
        entry, table, lbl_806E201C->resources);
    return instance->field_70 + instance->volume.value + value;
}

extern "C" float fn_802F2A6C(SoundInstance_802F2110* instance)
{
    return instance->pitch.value;
}

extern "C" void fn_802F2A74(SoundInstance_802F2110* instance)
{
    fn_802F04D4(lbl_806E201C->resources->rpcController, instance);
    RpcListEntry_802F2110* start = instance->rpcEntries;
    RpcListEntry_802F2110* entry = start;
    while (entry != 0)
    {
        RpcListEntry_802F2110* next = entry->next;
        entry->next = (RpcListEntry_802F2110*)instance->entryPool->m_FreeList;
        instance->entryPool->m_FreeList = (SlotPoolEntry*)entry;
        entry = next == start ? 0 : next;
    }
    instance->rpcEntries = 0;
}
