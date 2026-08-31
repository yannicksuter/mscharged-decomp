#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

struct WeightedEntry_802F2C3C
{
    u32 index;
    u32 weight;
};

struct PlaybackDefinition_802F2C3C
{
    u32 field_00;
    u32 soundId;
    union
    {
        u32 choiceCount;
        float value;
    };
    u8 mode;
    u8 pad_0D[3];
    WeightedEntry_802F2C3C* choices;
    u8 randomPitch;
    u8 randomVolume;
    u8 pad_16[2];
    float pitchMinimum;
    float pitchMaximum;
    float volumeMinimum;
    float volumeMaximum;
};

struct PlaybackRequest_802F2C3C
{
    u32 kind;
    PlaybackDefinition_802F2C3C* definition;
};

struct CueHandle_802F2C3C
{
    void** vtable;
    void* resource;
};

struct ModifierDefinition_802F2C3C
{
    u8 pad_00[0xC];
    u32 kind;
};

struct ModifierNode_802F2C3C
{
    ModifierDefinition_802F2C3C* definition;
    float value;
};

struct RpcListEntry_802F2C3C
{
    RpcListEntry_802F2C3C* next;
    RpcListEntry_802F2C3C* previous;
    ModifierNode_802F2C3C* node;
};

struct SoundDefinition_802F2C3C
{
    u8 pad_00[0x24];
    u32 modifierCount;
    ModifierNode_802F2C3C** modifiers;
};

struct SoundInstance_802F2C3C
{
    CueHandle_802F2C3C* owner;
    SoundDefinition_802F2C3C* definition;
    void* voices;
    SlotPoolBase* entryPool;
    RpcListEntry_802F2C3C* rpcEntries;
    s32 state;
    float previousTime;
    float currentTime;
    u8 pad_20[0x30];
    float pitch;
};

struct PlaybackOwner_802F2C3C
{
    void* field_00;
    SoundInstance_802F2C3C* instance;
    u8 pad_08[8];
    float volumeOffset;
    float pitchOffset;
};

struct PlaybackBackend_802F2C3C
{
    void** vtable;
};

struct PlaybackObject_802F2C3C
{
    void** vtable;
    u32 field_04;
    PlaybackOwner_802F2C3C* owner;
    s32 state;
    float startTime;
    PlaybackDefinition_802F2C3C* definition;
    u32 selection;
    float volumeModifier;
    float pitchModifier;
    float currentVolume;
    float currentPitch;
    PlaybackBackend_802F2C3C* backend;
    u32 stateAndFlags;
};

extern void* lbl_8052F750[];
extern void* lbl_8052F780[];
extern void* lbl_8052F7B0[];
extern void* lbl_8052F7E8[];
extern SlotPoolBase lbl_8057FAE8;
extern SlotPoolBase lbl_8057FB10;
extern SlotPoolBase lbl_8057FB38;
extern void* lbl_806E2020;

extern "C" void fn_802ED8D0(CueHandle_802F2C3C*, void*);
extern "C" float fn_802F29F8(SoundInstance_802F2C3C*);
extern "C" float fn_802F2A6C(SoundInstance_802F2C3C*);
extern "C" void* fn_8035C298(void*, u32, u32);
extern "C" void fn_8035C51C(void*);
extern "C" void fn_802F4630(PlaybackOwner_802F2C3C*, float);
extern "C" void fn_802F4638(PlaybackOwner_802F2C3C*, float);

static inline void* AllocateFromPool_802F2C3C(SlotPoolBase* pool, u32 size)
{
    void* result = 0;
    if (pool->m_FreeList == 0)
        SlotPoolBase::BaseAddNewBlock(pool, size);
    if (pool->m_FreeList != 0)
    {
        result = pool->m_FreeList;
        pool->m_FreeList = pool->m_FreeList->next;
    }
    return result;
}

static inline float RandomRange_802F2C3C(float minimum, float maximum)
{
    return nlRandomf(minimum, maximum, &nlDefaultSeed);
}

static inline void BackendCall_802F2C3C(PlaybackBackend_802F2C3C* backend,
    u32 index)
{
    typedef void (*Method)(PlaybackBackend_802F2C3C*);
    ((Method)backend->vtable[index])(backend);
}

extern "C" SlotPoolBase* fn_802F2C3C(SlotPoolBase* pool, int destroy)
{
    if (pool != 0)
    {
        fn_802B467C(pool);
        SlotPoolBase::BaseFreeBlocks(pool, 0x7C);
        pool->~SlotPoolBase();
        if (destroy > 0)
            operator delete(pool);
    }
    return pool;
}

extern "C" PlaybackObject_802F2C3C* fn_802F3114(PlaybackObject_802F2C3C* object);

extern "C" PlaybackObject_802F2C3C* fn_802F2CAC(PlaybackOwner_802F2C3C* owner,
    PlaybackRequest_802F2C3C* request)
{
    PlaybackObject_802F2C3C* object = 0;
    switch (request->kind)
    {
    case 1:
        object = (PlaybackObject_802F2C3C*)AllocateFromPool_802F2C3C(
            &lbl_8057FAE8, 0x34);
        if (object != 0)
        {
            object->vtable = lbl_8052F7E8;
            object->field_04 = 0;
            object->owner = owner;
            object->state = 0;
            object->startTime = 0.0f;
            object->vtable = lbl_8052F7B0;
            object->selection = 0;
            object->volumeModifier = 0.0f;
            object->pitchModifier = 0.0f;
            object->currentVolume = 0.0f;
            object->currentPitch = 0.0f;
            object->backend = 0;
            object->stateAndFlags &= 0xFFFF;
            object->definition = request->definition;
            float maximum = request->definition->volumeMaximum;
            float minimum = request->definition->volumeMinimum;
            object->startTime = maximum == 0.0f
                                  ? minimum
                                  : RandomRange_802F2C3C(minimum, minimum + maximum);
            object->selection = (u32)fn_802F3114(object);
            object->backend = (PlaybackBackend_802F2C3C*)fn_8035C298(
                lbl_806E2020, object->selection, *(u32*)((u8*)owner->instance->owner->resource + 0x14));
        }
        return object;
    case 3:
        object = (PlaybackObject_802F2C3C*)AllocateFromPool_802F2C3C(
            &lbl_8057FB10, 0x18);
        if (object != 0)
        {
            object->vtable = lbl_8052F7E8;
            object->field_04 = 0;
            object->owner = owner;
            object->state = 0;
            object->startTime = 0.0f;
            object->vtable = lbl_8052F780;
            object->definition = request->definition;
            float maximum = request->definition->pitchMaximum;
            float minimum = request->definition->pitchMinimum;
            object->startTime = maximum == 0.0f
                                  ? minimum
                                  : RandomRange_802F2C3C(minimum, minimum + maximum);
            object->state = 0;
        }
        return object;
    case 2:
        object = (PlaybackObject_802F2C3C*)AllocateFromPool_802F2C3C(
            &lbl_8057FB38, 0x18);
        if (object != 0)
        {
            object->vtable = lbl_8052F7E8;
            object->field_04 = 0;
            object->owner = owner;
            object->state = 0;
            object->startTime = 0.0f;
            object->vtable = lbl_8052F750;
            object->definition = request->definition;
            float maximum = request->definition->pitchMaximum;
            float minimum = request->definition->pitchMinimum;
            object->startTime = maximum == 0.0f
                                  ? minimum
                                  : RandomRange_802F2C3C(minimum, minimum + maximum);
            object->state = 0;
        }
        return object;
    default:
        return 0;
    }
}

extern "C" PlaybackObject_802F2C3C* fn_802F2F8C(
    PlaybackObject_802F2C3C* object, int destroy)
{
    if (object != 0)
    {
        object->vtable = lbl_8052F7B0;
        if (object->backend != 0)
            fn_8035C51C(lbl_806E2020);
        if (destroy > 0)
        {
            object->vtable = (void**)lbl_8057FAE8.m_FreeList;
            lbl_8057FAE8.m_FreeList = (SlotPoolEntry*)object;
        }
    }
    return object;
}

extern "C" void fn_802F34E4(PlaybackObject_802F2C3C*, bool);

extern "C" void fn_802F3008(PlaybackObject_802F2C3C* object)
{
    object->volumeModifier = object->definition->randomVolume
                               ? RandomRange_802F2C3C(
                                     object->definition->volumeMinimum,
                                     object->definition->volumeMaximum)
                               : 0.0f;
    object->pitchModifier = object->definition->randomPitch
                              ? RandomRange_802F2C3C(
                                    object->definition->pitchMinimum,
                                    object->definition->pitchMaximum)
                              : 0.0f;
    object->currentVolume = -96.0f;
    object->currentPitch = 0.0f;
    fn_802F34E4(object, true);
    typedef void (*StartFunc)(PlaybackBackend_802F2C3C*, u32);
    ((StartFunc)object->backend->vtable[8])(
        object->backend, object->definition->soundId);
    object->state = 4;
}

extern "C" void fn_802F30D0(PlaybackObject_802F2C3C* object)
{
    BackendCall_802F2C3C(object->backend, 7);
    object->state = 2;
}

extern "C" PlaybackObject_802F2C3C* fn_802F3114(
    PlaybackObject_802F2C3C* object)
{
    PlaybackDefinition_802F2C3C* definition = object->definition;
    u32 selectedIndex;
    if (definition->choiceCount == 1)
        selectedIndex = definition->choices[0].index;
    else
    {
        u32 totalWeight = 0;
        for (u32 i = 0; i < definition->choiceCount; i++)
            totalWeight += definition->choices[i].weight;
        u32 choice = nlRandom(totalWeight, &nlDefaultSeed);
        u32 index = 0;
        for (; index < definition->choiceCount; index++)
        {
            totalWeight -= definition->choices[index].weight;
            if (choice >= totalWeight)
                break;
        }
        selectedIndex = definition->choices[index].index;
    }

    u8* resource = (u8*)object->owner->instance->owner->resource;
    void* table = *(void**)(resource + 4);
    void* entries = *(void**)((u8*)table + 0x10);
    return (PlaybackObject_802F2C3C*)((u8*)*(void**)((u8*)entries + 0x14)
                                      + selectedIndex * 0x1C);
}

static inline int BackendState_802F2C3C(PlaybackBackend_802F2C3C* backend)
{
    typedef int (*StateFunc)(PlaybackBackend_802F2C3C*);
    return ((StateFunc)backend->vtable[4])(backend);
}

extern "C" int fn_802F32A0(PlaybackObject_802F2C3C* object)
{
    if (object->backend != 0)
        BackendCall_802F2C3C(object->backend, 3);

    switch (object->state)
    {
    case 2:
        if (BackendState_802F2C3C(object->backend) == 3)
            object->state = 3;
        break;
    case 4:
    case 7:
        if (BackendState_802F2C3C(object->backend) == 1)
        {
            object->state = 8;
            object->stateAndFlags &= 0xFFFF;
        }
        break;
    case 3:
    {
        float previous = object->owner->instance->previousTime;
        float current = object->owner->instance->currentTime;
        if (previous < object->startTime && current >= object->startTime)
        {
            typedef void (*Method)(PlaybackObject_802F2C3C*);
            ((Method)object->vtable[12])(object);
        }
        if (object->state == 4)
            fn_802F34E4(object, false);
        break;
    }
    default:
        break;
    }
    return object->state;
}

extern "C" void fn_802F3430(PlaybackObject_802F2C3C* object)
{
    if (object->backend != 0)
        BackendCall_802F2C3C(object->backend, 10);
    object->stateAndFlags = (object->stateAndFlags & 0xFFFF0000)
                          | (object->state & 0xFFFF);
    object->state = 5;
}

extern "C" void fn_802F3490(PlaybackObject_802F2C3C* object)
{
    if (object->backend != 0)
        BackendCall_802F2C3C(object->backend, 11);
    object->state = (u16)object->stateAndFlags;
}

extern "C" void fn_802F3648(PlaybackObject_802F2C3C* object,
    u8* hasVolume, float* volume, u8* hasPitch, float* pitch)
{
    u32 volumeCount = 0;
    u32 pitchCount = 0;
    *hasVolume = false;
    *hasPitch = false;

    RpcListEntry_802F2C3C* start = object->owner->instance->rpcEntries;
    RpcListEntry_802F2C3C* entry = start != 0 ? start->next : 0;
    while (entry != 0)
    {
        ModifierNode_802F2C3C* node = entry->node;
        if (node->definition->kind == 1)
        {
            pitchCount++;
            *pitch += 0.091f * (0.01f * node->value);
        }
        else if (node->definition->kind == 0)
        {
            volumeCount++;
            *volume += node->value;
        }
        if (start == 0 || entry == start)
            entry = 0;
        else
            entry = entry->next;
    }

    SoundDefinition_802F2C3C* definition = object->owner->instance->definition;
    for (u32 i = 0; i < definition->modifierCount; i++)
    {
        ModifierNode_802F2C3C* node = definition->modifiers[i];
        if (node->definition->kind == 1)
        {
            pitchCount++;
            *pitch += 0.091f * (0.01f * node->value);
        }
        else if (node->definition->kind == 0)
        {
            volumeCount++;
            *volume += node->value;
        }
    }
    *hasVolume = volumeCount != 0;
    *hasPitch = pitchCount != 0;
}

extern "C" void fn_802F34E4(
    PlaybackObject_802F2C3C* object, bool force)
{
    u8 hasVolume = false;
    u8 hasPitch = false;
    float volume = 0.0f;
    float pitch = 0.0f;
    fn_802F3648(object, &hasVolume, &volume, &hasPitch, &pitch);

    float currentVolume = volume + fn_802F29F8(object->owner->instance)
                        + object->volumeModifier + object->owner->volumeOffset;
    if (currentVolume < -96.0f)
        currentVolume = -96.0f;
    if (currentVolume > 6.0f)
        currentVolume = 6.0f;
    if (force || object->currentVolume != currentVolume)
    {
        object->currentVolume = currentVolume;
        typedef void (*SetFloatFunc)(PlaybackBackend_802F2C3C*, float);
        ((SetFloatFunc)object->backend->vtable[12])(
            object->backend, currentVolume);
    }

    float currentPitch = pitch + fn_802F2A6C(object->owner->instance)
                       + object->pitchModifier + object->owner->pitchOffset;
    if (object->currentPitch != currentPitch)
    {
        object->currentPitch = currentPitch;
        BackendCall_802F2C3C(object->backend, 14);
    }
}

extern "C" void fn_802F37E8(PlaybackObject_802F2C3C* object)
{
    BackendCall_802F2C3C(object->backend, 9);
    object->state = 7;
    object->stateAndFlags &= 0xFFFF;
}

extern "C" bool fn_802F3838(
    PlaybackObject_802F2C3C* object, PlaybackBackend_802F2C3C** output)
{
    if (object->backend != 0)
    {
        typedef bool (*QueryFunc)(PlaybackBackend_802F2C3C*);
        if (((QueryFunc)object->backend->vtable[25])(object->backend))
        {
            *output = object->backend;
            return true;
        }
    }
    return false;
}

extern "C" int fn_802F38A8(PlaybackObject_802F2C3C* object)
{
    if (object->state == 2)
        object->state = 3;
    float previous = object->owner->instance->previousTime;
    float current = object->owner->instance->currentTime;
    if (previous < object->startTime && current >= object->startTime)
    {
        fn_802ED8D0(object->owner->instance->owner,
            (void*)object->definition->soundId);
        object->state = 8;
    }
    return object->state;
}

extern "C" int fn_802F3938(PlaybackObject_802F2C3C* object)
{
    if (object->state == 2)
        object->state = 3;
    float previous = object->owner->instance->previousTime;
    float current = object->owner->instance->currentTime;
    if (previous < object->startTime && current >= object->startTime)
    {
        if (object->definition->mode == 0)
            fn_802F4630(object->owner, object->definition->value);
        else if (object->definition->mode == 1)
            fn_802F4638(object->owner, object->definition->value);
        object->state = 8;
    }
    return object->state;
}
