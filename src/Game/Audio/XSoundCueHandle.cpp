#include "NL/nlDebugString.h"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioSlider.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioResourceLoader.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/XSoundHandle.h"
#include "Game/Sys/debug.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

#include "Game/Audio/XSoundCueHandle.h"
#include "Game/Audio/SoundInstance.h"

static char sCueSelectionMessage[] = "XSoundCueHandle::ctor selecting sound %s from cue %s\n";
SlotPool<XSoundCueHandle> sSoundCueHandlePool(32, 16);

extern "C" LocalSliderSet_802F1758* fn_802EED88(void*, XSoundCueHandle*);
void DumpAudioMemory();

inline AudioVoiceDefinition* XSoundCueHandle::SelectSound()
{
    if (definition->useSlider)
        return SelectAudioCueVoiceBySlider(definition, sliderValue);
    return SelectAudioCueVoice(definition);
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

    this->definition = ((AudioResourceLoadOwner*)m_Slot)->m_ResourceObject->cues
                     + cueIndex;
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
        this->slider = (SliderState_802F1758*)GetAudioSlider(
            (AudioSliderTable*)g_pAudioSystem->GetBundleManager()->GetSliderTable(),
            this->definition->sliderIndex,
            this);
        this->sliderValue = this->slider->value;
    }

    AudioVoiceDefinition* selected = SelectSound();
    tDebugPrintManager::Print(DC_SOUND, sCueSelectionMessage, nlLookupDebugString(g_pDebugStringTable, (unsigned long)*(const char**)selected), nlLookupDebugString(g_pDebugStringTable, (unsigned long)this->definition->name));

    SoundInstance* instance = sSoundInstancePool.Allocate();
    instance = new (instance) SoundInstance(this, selected);
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
    cue->instance->GetSources(sources, output);
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
        this->instance->Play(0.0f);
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
    this->instance->Prepare();
    m_State = 2;
    return true;
}

void XSoundCueHandle::Stop(u8 callbackEnabled, void* value)
{
    switch (m_State)
    {
    case 7:
        if (value != 0)
            this->instance->Stop(value);
        break;
    case 8:
        break;
    default:
        this->instance->Stop(value);
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
    this->instance->Pause();
}

void XSoundCueHandle::Resume()
{
    m_State = bits.savedState;
    this->instance->Resume();
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
        this->instance->Update(dt);

        if (m_State != 9)
        {
            if (m_State == 2)
            {
                if (this->instance->state == 3)
                {
                    m_State = 3;
                    if (this->bits.playWhenPrepared)
                    {
                        this->instance->Play(0.0f);
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
        SoundInstance* newInstance;
        AudioVoiceDefinition* selected = SelectSound();
        SoundInstance* oldInstance = this->instance;
        if (selected != oldInstance->definition)
        {
            if (oldInstance->state == 4)
            {
                oldInstance->SetVolume(false, 0.0f, 0.5f);
                oldInstance->releaseTime = 0.5f;
            }
            newInstance = sSoundInstancePool.Allocate();
            newInstance = new (newInstance) SoundInstance(this, selected);
            this->instance = newInstance;
            newInstance->SetVolume(false, 1.0f, 0.5f);
            this->instance->nextInstance = oldInstance;
            m_State = 4;
        }
    }

    SoundInstance* previous = this->instance;
    SoundInstance* instance = previous->nextInstance;
    while (previous != 0 && instance != 0)
    {
        instance->Update(dt);
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
