#include "NL/nlDebugString.h"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"
#include "Game/Audio/XSoundHandle.h"
#include "Game/Sys/debug.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

#include "Game/Audio/CueHandle_802F1758.h"

extern void* lbl_8052F6D0[];
extern char lbl_8052F680[0x38];
SlotPool<CueHandle_802F1758> lbl_8057FA68(32, 16);

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
extern "C" void fn_802F2650(SoundInstance_802F1758*, PlaybackBackend_802F2C3C**, u32*);
extern "C" void fn_802F26B0(SoundInstance_802F1758*, float);
void DumpAudioMemory();

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

    handle->localSliders = fn_802EED88(g_pAudioSystem->GetBundleManager()->GetSliderTable(), handle);
    if (handle->localSliders == 0)
        DumpAudioMemory();

    if (handle->definition->useSlider)
    {
        handle->slider = fn_802EED38(g_pAudioSystem->GetBundleManager()->GetSliderTable(),
            handle->definition->sliderIndex,
            handle);
        handle->sliderValue = handle->slider->value;
    }

    void* selected = handle->definition->useSlider
                       ? fn_802F1460(handle->definition, handle->sliderValue)
                       : fn_802F11A0(handle->definition);
    tDebugPrintManager::Print(DC_SOUND, lbl_8052F680, nlLookupDebugString(g_pDebugStringTable, (unsigned long)*(const char**)selected), nlLookupDebugString(g_pDebugStringTable, (unsigned long)handle->definition->name));

    SoundInstance_802F1758* instance = lbl_8057FAA8.Allocate();
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
            delete handle->instance;
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

AudioParameter* GetSoundParameter(XSoundHandle* handle, unsigned long index)
{
    return (AudioParameter*)(((CueHandle_802F1758*)handle)->localSliders->sliders + index);
}

void GetSoundSources(void* handle, AudioSource** sources, unsigned int* output)
{
    CueHandle_802F1758* cue = (CueHandle_802F1758*)handle;
    *output = 0;
    fn_802F2650(cue->instance, (PlaybackBackend_802F2C3C**)sources, (u32*)output);
}

extern "C" bool fn_802F1A94(CueHandle_802F1758* handle, u8 callbackEnabled)
{
    switch (handle->state)
    {
    case 1:
        handle->stateAndFlags |= 0x4000;
        ((XSoundHandle*)handle)->Prepare(callbackEnabled);
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
        ((XSoundHandle*)handle)->Release();
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
            SoundInstance_802F1758* instance = lbl_8057FAA8.Allocate();
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
            delete instance;
            instance = 0;
        }
        if (instance != 0)
        {
            previous = instance;
            instance = instance->nextInstance;
        }
    }
}

bool IsSoundHandleValid()
{
    return true;
}
