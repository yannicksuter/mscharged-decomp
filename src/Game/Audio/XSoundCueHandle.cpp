#include "NL/nlDebugString.h"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/XSoundHandle.h"
#include "Game/Sys/debug.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

#include "Game/Audio/XSoundCueHandle.h"
#include "Game/Audio/SoundInstance_802F1758.h"

static char sCueSelectionMessage[] = "XSoundCueHandle::ctor selecting sound %s from cue %s\n";
SlotPool<XSoundCueHandle> sSoundCueHandlePool(32, 16);

extern "C" LocalSliderSet_802F1758* fn_802EED88(void*, XSoundCueHandle*);
extern "C" SliderState_802F1758* fn_802EED38(void*, u32, XSoundCueHandle*);
extern "C" void* fn_802F11A0(CueDefinition_802F1758*);
extern "C" void* fn_802F1460(CueDefinition_802F1758*, float);
extern "C" SoundInstance_802F1758* fn_802F2188(SoundInstance_802F1758*, XSoundCueHandle*, void*);
extern "C" void fn_802F2320(SoundInstance_802F1758*, float);
extern "C" void fn_802F2398(SoundInstance_802F1758*);
extern "C" void fn_802F2594(SoundInstance_802F1758*, bool, float, float);
extern "C" void fn_802F25D4(SoundInstance_802F1758*, void*);
extern "C" void fn_802F2640(SoundInstance_802F1758*);
extern "C" void fn_802F2648(SoundInstance_802F1758*);
extern "C" void fn_802F2650(SoundInstance_802F1758*, PlaybackBackend_802F2C3C**, u32*);
extern "C" void fn_802F26B0(SoundInstance_802F1758*, float);
void DumpAudioMemory();

inline void* XSoundCueHandle::SelectSound()
{
    if (definition->useSlider)
        return fn_802F1460(definition, sliderValue);
    return fn_802F11A0(definition);
}

XSoundCueHandle::XSoundCueHandle(void* resource, XSoundOwner* owner, unsigned int cueIndex,
    XSoundHitMarkerCallback callback, void* context)
    : XSoundHandle(resource, owner, (void*)cueIndex, callback, context)
{
    this->instance = 0;
    this->slider = 0;
    this->sliderValue = 0.0f;
    bits.flag8000 = true;
    bits.playWhenPrepared = false;

    this->definition = ((CueResource_802F1758*)m_Slot)->cues->definitions + cueIndex;
    this->definition->activeCount++;
    if (this->definition->useSlider)
        bits.flag8000 = false;

    if (this->definition->activeCount > this->definition->maximumCount)
    {
        m_State = 6;
        return;
    }

    m_LocalSliders = fn_802EED88(g_pAudioSystem->GetBundleManager()->GetSliderTable(), this);
    if (GetLocalSliders() == 0)
        DumpAudioMemory();

    if (this->definition->useSlider)
    {
        this->slider = fn_802EED38(g_pAudioSystem->GetBundleManager()->GetSliderTable(),
            this->definition->sliderIndex,
            this);
        this->sliderValue = this->slider->value;
    }

    void* selected = SelectSound();
    tDebugPrintManager::Print(DC_SOUND, sCueSelectionMessage, nlLookupDebugString(g_pDebugStringTable, (unsigned long)*(const char**)selected), nlLookupDebugString(g_pDebugStringTable, (unsigned long)this->definition->name));

    SoundInstance_802F1758* instance = lbl_8057FAA8.Allocate();
    if (instance != 0)
        instance = fn_802F2188(instance, this, selected);
    this->instance = instance;
    return;
}

XSoundCueHandle::~XSoundCueHandle()
{
    definition->activeCount--;
    if (GetLocalSliders() != 0)
        GetLocalSliders()->owner = 0;
    if (instance != 0)
        delete instance;
}

AudioParameter* GetSoundParameter(XSoundHandle* handle, unsigned long index)
{
    return (AudioParameter*)(((XSoundCueHandle*)handle)->GetLocalSliders()->sliders + index);
}

void GetSoundSources(void* handle, AudioSource** sources, unsigned int* output)
{
    XSoundCueHandle* cue = (XSoundCueHandle*)handle;
    *output = 0;
    fn_802F2650(cue->instance, (PlaybackBackend_802F2C3C**)sources, (u32*)output);
}

bool XSoundCueHandle::Play(bool callbackEnabled)
{
    switch (m_State)
    {
    case 1:
        bits.playWhenPrepared = true;
        this->Prepare(callbackEnabled);
        return false;
    case 2:
    case 5:
        bits.playWhenPrepared = true;
        return false;
    case 3:
        fn_802F2320(this->instance, 0.0f);
        m_State = this->instance->state;
        return m_State == 4;
    case 4:
        break;
    case 6:
        return false;
    default:
        break;
    }
    return false;
}

bool XSoundCueHandle::Prepare(bool callbackEnabled)
{
    if (m_State == 6)
        return false;
    m_CallbackEnabled = callbackEnabled;
    fn_802F2398(this->instance);
    m_State = 2;
    return true;
}

void XSoundCueHandle::Stop(u8 callbackEnabled, void* value)
{
    switch (m_State)
    {
    case 7:
        if (value != 0)
            fn_802F25D4(this->instance, value);
        break;
    case 8:
        break;
    default:
        fn_802F25D4(this->instance, value);
        break;
    }
    m_CallbackEnabled = callbackEnabled;
    m_State = 7;
    bits.flag8000 = true;
}

void XSoundCueHandle::Pause()
{
    this->bits.savedState = m_State;
    m_State = 5;
    fn_802F2640(this->instance);
}

void XSoundCueHandle::Resume()
{
    m_State = bits.savedState;
    fn_802F2648(this->instance);
}

void XSoundCueHandle::Update(float dt)
{
    if (m_State == 4)
    {
        m_PreviousTime = m_CurrentTime;
        m_CurrentTime += dt;
    }
    if (m_State == 6)
        m_State = 8;

    if (m_State != 8 && m_State != 5 && m_State != 9)
    {
        if (this->definition->useSlider)
            UpdateSlider(dt);
        fn_802F26B0(this->instance, dt);

        if (m_State != 9)
        {
            if (m_State == 2)
            {
                if (this->instance->state == 3)
                {
                    m_State = 3;
                    if (this->bits.playWhenPrepared)
                    {
                        fn_802F2320(this->instance, 0.0f);
                        m_State = this->instance->state;
                    }
                }
            }
            else if (this->instance->nextInstance != 0)
                m_State = 4;
            else
                m_State = this->instance->state;
        }
    }

    if (m_State == 8 && m_CallbackEnabled)
        this->Release();
}

void XSoundCueHandle::UpdateSlider(float dt)
{
    if (m_State != 4 && m_State != 8)
        return;

    float previousValue = this->sliderValue;
    float value = this->slider->value;
    this->sliderValue = value;
    if (previousValue != value)
    {
        SoundInstance_802F1758* newInstance;
        void* selected = SelectSound();
        SoundInstance_802F1758* oldInstance = this->instance;
        if (selected != oldInstance->definition)
        {
            if (oldInstance->state == 4)
            {
                fn_802F2594(oldInstance, false, 0.0f, 0.5f);
                oldInstance->field_74 = 0.5f;
            }
            newInstance = lbl_8057FAA8.Allocate();
            if (newInstance != 0)
                newInstance = fn_802F2188(newInstance, this, selected);
            this->instance = newInstance;
            fn_802F2594(newInstance, false, 1.0f, 0.5f);
            this->instance->nextInstance = oldInstance;
            m_State = 4;
        }
    }

    SoundInstance_802F1758* previous = this->instance;
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
