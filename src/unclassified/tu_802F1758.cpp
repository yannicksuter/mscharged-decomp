#include "Game/Audio/XSoundHandle_802ED74C.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

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

struct AudioResources_802F1758
{
    u8 pad_00[0xC];
    void* sliderTable;
};

struct AudioSystem_802F1758
{
    u8 pad_00[0xCC];
    AudioResources_802F1758* resources;
};

extern void* lbl_8052F6D0[];
extern char lbl_8052F680[0x38];
extern SlotPoolBase lbl_8057FA68;
extern SlotPoolBase lbl_8057FAA8;
extern int lbl_806E1DC8;
extern AudioSystem_802F1758* lbl_806E201C;

extern "C" CueHandle_802F1758* fn_802ED74C(CueHandle_802F1758*, void*, CueOwner_802F1758*, void*, void (*)(void*, CueHandle_802F1758*, void*), void*);
extern "C" CueHandle_802F1758* fn_802ED7F0(CueHandle_802F1758*, int);
extern "C" LocalSliderSet_802F1758* fn_802EED88(void*, CueHandle_802F1758*);
extern "C" SliderState_802F1758* fn_802EED38(void*, u32, CueHandle_802F1758*);
extern "C" void* fn_802F11A0(CueDefinition_802F1758*);
extern "C" void* fn_802F1460(CueDefinition_802F1758*, float);
extern "C" SoundInstance_802F1758* fn_802F2188(SoundInstance_802F1758*, CueHandle_802F1758*, void*);
extern "C" void fn_802F2320(SoundInstance_802F1758*, float);
extern "C" void fn_802F2398(SoundInstance_802F1758*);
extern "C" void fn_802F2594(SoundInstance_802F1758*, bool, float, float);
extern "C" void fn_802F25D4(SoundInstance_802F1758*, void*);
extern "C" void fn_802F2640(SoundInstance_802F1758*);
extern "C" void fn_802F2648(SoundInstance_802F1758*);
extern "C" void fn_802F2650(SoundInstance_802F1758*, u32, void*);
extern "C" void fn_802F26B0(SoundInstance_802F1758*, float);
extern "C" void fn_8004F594(int, const char*, ...);
extern "C" const char* fn_802B9568(int, const char*);
extern "C" void fn_8035CA84();

static inline SoundInstance_802F1758* AllocateInstance_802F1758()
{
    SoundInstance_802F1758* instance = 0;
    if (lbl_8057FAA8.m_FreeList == 0)
        SlotPoolBase::BaseAddNewBlock(&lbl_8057FAA8, sizeof(SoundInstance_802F1758));
    if (lbl_8057FAA8.m_FreeList != 0)
    {
        instance = (SoundInstance_802F1758*)lbl_8057FAA8.m_FreeList;
        lbl_8057FAA8.m_FreeList = lbl_8057FAA8.m_FreeList->next;
    }
    return instance;
}

static inline void FreeInstance_802F1758(SoundInstance_802F1758* instance)
{
    if (instance != 0)
    {
        instance->~SoundInstance_802F1758();
        instance->owner = lbl_8057FAA8.m_FreeList;
        lbl_8057FAA8.m_FreeList = (SlotPoolEntry*)instance;
    }
}

extern "C" CueHandle_802F1758* fn_802F1758(CueHandle_802F1758* handle,
    void* value, CueOwner_802F1758* owner, u32 cueIndex,
    void (*callback)(void*, CueHandle_802F1758*, void*), void* context)
{
    fn_802ED74C(handle, value, owner, (void*)cueIndex, callback, context);
    handle->instance = 0;
    handle->vtable = lbl_8052F6D0;
    handle->slider = 0;
    handle->sliderValue = 0.0f;
    handle->stateAndFlags |= 0x8000;
    handle->stateAndFlags &= ~0x4000;

    handle->definition = handle->resource->cues->definitions + cueIndex;
    handle->definition->activeCount++;
    if (handle->definition->useSlider)
        handle->stateAndFlags &= ~0x8000;

    if (handle->definition->activeCount > handle->definition->maximumCount)
    {
        handle->state = 6;
        return handle;
    }

    handle->localSliders = fn_802EED88(lbl_806E201C->resources->sliderTable, handle);
    if (handle->localSliders == 0)
        fn_8035CA84();

    if (handle->definition->useSlider)
    {
        handle->slider = fn_802EED38(lbl_806E201C->resources->sliderTable,
            handle->definition->sliderIndex,
            handle);
        handle->sliderValue = handle->slider->value;
    }

    void* selected = handle->definition->useSlider
                       ? fn_802F1460(handle->definition, handle->sliderValue)
                       : fn_802F11A0(handle->definition);
    fn_8004F594(10, lbl_8052F680, fn_802B9568(lbl_806E1DC8, *(const char**)selected), fn_802B9568(lbl_806E1DC8, handle->definition->name));

    SoundInstance_802F1758* instance = AllocateInstance_802F1758();
    if (instance != 0)
        instance = fn_802F2188(instance, handle, selected);
    handle->instance = instance;
    return handle;
}

extern "C" CueHandle_802F1758* fn_802F194C(CueHandle_802F1758* handle, int destroy)
{
    if (handle != 0)
    {
        handle->vtable = lbl_8052F6D0;
        handle->definition->activeCount--;
        if (handle->localSliders != 0)
            handle->localSliders->owner = 0;

        if (handle->instance != 0)
        {
            fn_802F2A74(handle->instance);
            SoundInstance_802F1758* instance = handle->instance;
            while (instance != 0)
            {
                SoundInstance_802F1758* next = instance->nextInstance;
                FreeInstance_802F1758(instance);
                instance = next;
            }
            handle->instance = 0;
        }
        fn_802ED7F0(handle, 0);
        if (destroy > 0)
        {
            handle->vtable = (void**)lbl_8057FA68.m_FreeList;
            lbl_8057FA68.m_FreeList = (SlotPoolEntry*)handle;
        }
    }
    return handle;
}

extern "C" SliderState_802F1758* fn_802F1A70(CueHandle_802F1758* handle, u32 index)
{
    return handle->localSliders->sliders + index;
}

extern "C" void fn_802F1A84(CueHandle_802F1758* handle, u32 value, void** output)
{
    *output = 0;
    fn_802F2650(handle->instance, value, output);
}

extern "C" bool fn_802F1A94(CueHandle_802F1758* handle, u8 callbackEnabled)
{
    switch (handle->state)
    {
    case 1:
        handle->stateAndFlags |= 0x4000;
        ((XSoundHandle_802ED74C*)handle)->fn_802ED74C_4(callbackEnabled);
        return false;
    case 2:
    case 5:
        handle->stateAndFlags |= 0x4000;
        return false;
    case 3:
        fn_802F2320(handle->instance, 0.0f);
        handle->state = handle->instance->state;
        return handle->state == 4;
    case 4:
        break;
    case 6:
        return false;
    default:
        break;
    }
    return false;
}

extern "C" bool fn_802F1B60(CueHandle_802F1758* handle, u8 callbackEnabled)
{
    if (handle->state == 6)
        return false;
    handle->callbackEnabled = callbackEnabled;
    fn_802F2398(handle->instance);
    handle->state = 2;
    return true;
}

extern "C" void fn_802F1BB4(CueHandle_802F1758* handle, u8 callbackEnabled, void* value)
{
    switch (handle->state)
    {
    case 7:
        if (value != 0)
            fn_802F25D4(handle->instance, value);
        break;
    case 8:
        break;
    default:
        fn_802F25D4(handle->instance, value);
        break;
    }
    handle->callbackEnabled = callbackEnabled;
    handle->state = 7;
    handle->stateAndFlags |= 0x8000;
}

extern "C" void fn_802F1C40(CueHandle_802F1758* handle)
{
    handle->bits.savedState = handle->state;
    handle->state = 5;
    fn_802F2640(handle->instance);
}

extern "C" void fn_802F1C60(CueHandle_802F1758* handle)
{
    handle->state = handle->stateAndFlags >> 16;
    fn_802F2648(handle->instance);
}

extern "C" void fn_802F1DC4(CueHandle_802F1758* handle, float dt);

extern "C" void fn_802F1C74(CueHandle_802F1758* handle, float dt)
{
    if (handle->state == 4)
    {
        handle->previousTime = handle->currentTime;
        handle->currentTime += dt;
    }
    if (handle->state == 6)
        handle->state = 8;

    if (handle->state != 8 && handle->state != 5 && handle->state != 9)
    {
        if (handle->definition->useSlider)
            fn_802F1DC4(handle, dt);
        fn_802F26B0(handle->instance, dt);

        if (handle->state != 9)
        {
            if (handle->state == 2)
            {
                if (handle->instance->state == 3)
                {
                    handle->state = 3;
                    if (handle->bits.field_4000)
                    {
                        fn_802F2320(handle->instance, 0.0f);
                        handle->state = handle->instance->state;
                    }
                }
            }
            else if (handle->instance->nextInstance != 0)
                handle->state = 4;
            else
                handle->state = handle->instance->state;
        }
    }

    if (handle->state == 8 && handle->callbackEnabled)
        ((XSoundHandle_802ED74C*)handle)->fn_802ED74C_10();
}

extern "C" void fn_802F1DC4(CueHandle_802F1758* handle, float dt)
{
    if (handle->state != 4 && handle->state != 8)
        return;

    float previousValue = handle->sliderValue;
    float value = handle->slider->value;
    handle->sliderValue = value;
    if (previousValue != value)
    {
        void* selected = handle->definition->useSlider
                           ? fn_802F1460(handle->definition, value)
                           : fn_802F11A0(handle->definition);
        SoundInstance_802F1758* oldInstance = handle->instance;
        if (selected != oldInstance->definition)
        {
            if (oldInstance->state == 4)
            {
                fn_802F2594(oldInstance, false, 0.0f, 0.5f);
                oldInstance->field_74 = 0.5f;
            }
            SoundInstance_802F1758* instance = AllocateInstance_802F1758();
            if (instance != 0)
                instance = fn_802F2188(instance, handle, selected);
            handle->instance = instance;
            fn_802F2594(instance, false, 1.0f, 0.5f);
            handle->instance->nextInstance = oldInstance;
            handle->state = 4;
        }
    }

    SoundInstance_802F1758* previous = handle->instance;
    SoundInstance_802F1758* instance = previous->nextInstance;
    while (previous != 0 && instance != 0)
    {
        fn_802F26B0(instance, dt);
        if (instance->state == 8 && instance->nextInstance == 0)
        {
            previous->nextInstance = 0;
            FreeInstance_802F1758(instance);
            instance = 0;
        }
        if (instance != 0)
        {
            previous = instance;
            instance = instance->nextInstance;
        }
    }
}
