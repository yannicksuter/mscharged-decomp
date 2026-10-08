#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "NL/nlDebugString.h"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioBackend.h"
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
        m_State = SOUND_HANDLE_FAILED;
        return;
    }

    m_LocalSliders = AllocateLocalAudioSliders(
        (AudioSliderTable*)g_pAudioSystem->GetBundleManager()->GetSliderTable(), this);
    if (GetLocalSliders() == 0)
        DumpAudioMemory();

    if (this->definition->useSlider)
    {
        this->slider = GetAudioSlider(
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

AudioSlider* GetSoundParameter(XSoundHandle* handle, unsigned long index)
{
    return ((XSoundCueHandle*)handle)->GetLocalSliders()->sliders + index;
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
    case SOUND_HANDLE_PENDING:
        bits.playWhenPrepared = true;
        this->Prepare(callbackEnabled);
        return false;
    case SOUND_HANDLE_PREPARING:
    case SOUND_HANDLE_PAUSED:
        bits.playWhenPrepared = true;
        return false;
    case SOUND_HANDLE_PREPARED:
        this->instance->Play(0.0f);
        m_State = static_cast<eSoundHandleState>(this->instance->state);
        return m_State == SOUND_HANDLE_PLAYING;
    case SOUND_HANDLE_PLAYING:
        break;
    case SOUND_HANDLE_FAILED:
        return false;
    default:
        break;
    }
    return false;
}

bool XSoundCueHandle::Prepare(bool callbackEnabled)
{
    if (m_State == SOUND_HANDLE_FAILED)
        return false;
    m_CallbackEnabled = callbackEnabled;
    this->instance->Prepare();
    m_State = SOUND_HANDLE_PREPARING;
    return true;
}

void XSoundCueHandle::Stop(u8 callbackEnabled, void* force)
{
    switch (m_State)
    {
    case SOUND_HANDLE_STOPPING:
        if (force != 0)
            this->instance->Stop(force);
        break;
    case SOUND_HANDLE_STOPPED:
        break;
    default:
        this->instance->Stop(force);
        break;
    }
    m_CallbackEnabled = callbackEnabled;
    m_State = SOUND_HANDLE_STOPPING;
    bits.flag8000 = true;
}

void XSoundCueHandle::Pause()
{
    this->bits.savedState = m_State;
    m_State = SOUND_HANDLE_PAUSED;
    this->instance->Pause();
}

void XSoundCueHandle::Resume()
{
    m_State = static_cast<eSoundHandleState>(bits.savedState);
    this->instance->Resume();
}

void XSoundCueHandle::Update(float dt)
{
    if (m_State == SOUND_HANDLE_PLAYING)
    {
        m_PreviousTime = m_CurrentTime;
        m_CurrentTime += dt;
    }
    if (m_State == SOUND_HANDLE_FAILED)
        m_State = SOUND_HANDLE_STOPPED;

    if (m_State != SOUND_HANDLE_STOPPED && m_State != SOUND_HANDLE_PAUSED && m_State != SOUND_HANDLE_RELEASED)
    {
        if (this->definition->useSlider)
            UpdateSlider(dt);
        this->instance->Update(dt);

        if (m_State != SOUND_HANDLE_RELEASED)
        {
            if (m_State == SOUND_HANDLE_PREPARING)
            {
                if (this->instance->state == SOUND_INSTANCE_STATE_PREPARED)
                {
                    m_State = SOUND_HANDLE_PREPARED;
                    if (this->bits.playWhenPrepared)
                    {
                        this->instance->Play(0.0f);
                        m_State = static_cast<eSoundHandleState>(this->instance->state);
                    }
                }
            }
            else if (this->instance->nextInstance != 0)
                m_State = SOUND_HANDLE_PLAYING;
            else
                m_State = static_cast<eSoundHandleState>(this->instance->state);
        }
    }

    if (m_State == SOUND_HANDLE_STOPPED && m_CallbackEnabled)
        this->Release();
}

void XSoundCueHandle::UpdateSlider(float dt)
{
    if (m_State != SOUND_HANDLE_PLAYING && m_State != SOUND_HANDLE_STOPPED)
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
            if (oldInstance->state == SOUND_INSTANCE_STATE_PLAYING)
            {
                oldInstance->SetVolume(false, 0.0f, 0.5f);
                oldInstance->releaseTime = 0.5f;
            }
            newInstance = sSoundInstancePool.Allocate();
            newInstance = new (newInstance) SoundInstance(this, selected);
            this->instance = newInstance;
            newInstance->SetVolume(false, 1.0f, 0.5f);
            this->instance->nextInstance = oldInstance;
            m_State = SOUND_HANDLE_PLAYING;
        }
    }

    SoundInstance* previous = this->instance;
    SoundInstance* instance = previous->nextInstance;
    while (previous != 0 && instance != 0)
    {
        instance->Update(dt);
        if (instance->state == SOUND_INSTANCE_STATE_STOPPED && instance->nextInstance == 0)
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
