#include "Game/Audio/AudioSequenceInstance.h"
#include "Game/Audio/SoundInstance.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioCalculation.h"
#include "Game/Audio/AudioRpc.h"
#include "Game/Audio/AudioSlider.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/XSoundHandle.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/Transition.h"
#include "NL/nlSlotPool.h"
#include "types.h"

static inline AudioRpcController* GetAudioRpcController()
{
    return static_cast<UnidentifiedAudioBundleManager_802ECD34*>(
        GetAudioBundleManager())->m_RpcController;
}

SlotPool<SoundInstance> sSoundInstancePool(32, 16);

SoundInstance::SoundInstance(
    XSoundHandle* handle, AudioVoiceDefinition* voiceDefinition)
    : owner(handle)
    , definition(voiceDefinition)
    , voices(0)
    , rpcEntries(sAudioRpcListEntryPool)
    , state(SOUND_INSTANCE_STATE_INITIAL)
    , previousTime(-1.0f)
    , currentTime(0.0f)
    , activeRpc(0)
    , transitionTime(0.0f)
    , volumeOffset(0.0f)
    , releaseTime(-1.0f)
    , nextInstance(0)
{
    volume.Reset(0.0f, -96.0f, 6.0f);
    pitch.Reset(0.0f, -12.0f, 12.0f);

    AudioSequenceInstance* previous = 0;
    for (u32 i = 0; i < definition->sequenceCount; i++)
    {
        AudioSequenceInstance* voice = new AudioSequenceInstance(
            this, definition->sequences[i]);
        if (previous == 0)
            voices = voice;
        else
            previous->next = voice;
        previous = voice;
    }
}

void SoundInstance::Play(float)
{
    volume.Update(0.0f, 1.0f);
    pitch.Update(0.0f, 1.0f);
    if (voices != 0)
        voices->Play();
    state = SOUND_INSTANCE_STATE_PLAYING;
}

static inline void PrepareSoundInstanceRpcNodes(SoundInstance* instance)
{
    AudioRpcGroup* group;
    AudioRpcController* controller = GetAudioRpcController();
    for (u32 groupIndex = 0;
        groupIndex < instance->definition->rpcGroupCount;
        groupIndex++)
    {
        group = &controller->groups[
            instance->definition->rpcGroupIndices[groupIndex]];
        for (u32 definitionIndex = 0;
            definitionIndex < group->dynamicDefinitionCount;
            definitionIndex++)
        {
            AudioRpcDefinition* rpcDefinition =
                &group->dynamicDefinitions[definitionIndex];
            AudioRpcRuntimeNode* node = AddAudioRpcRuntimeNode(
                controller, rpcDefinition, instance);
            instance->activeRpc = rpcDefinition->sliderIndex == 2 ? node : 0;
            instance->rpcEntries.AddEnd(node);
        }
    }
}

void SoundInstance::Prepare()
{
    volume.SetTarget(definition->volume, 0.0f);
    pitch.SetTarget(definition->pitch, 0.0f);

    PrepareSoundInstanceRpcNodes(this);

    if (voices != 0)
    {
        voices->Prepare();
        state = SOUND_INSTANCE_STATE_PREPARING;
    }
    else
        state = SOUND_INSTANCE_STATE_PREPARED;
}

void SoundInstance::SetVolume(
    bool releaseAfterTransition, float target, float duration)
{
    volume.SetTarget(target, duration);
    if (releaseAfterTransition)
        releaseTime = duration;
}

void SoundInstance::Stop(void* force)
{
    if (state != SOUND_INSTANCE_STATE_PLAYING
        || force != 0 || activeRpc == 0)
    {
        activeRpc = 0;
        if (voices != 0)
            voices->Stop();
    }
    state = SOUND_INSTANCE_STATE_STOPPING;
}

void SoundInstance::Pause()
{
    voices->Pause();
}

void SoundInstance::Resume()
{
    voices->Resume();
}

void SoundInstance::GetSources(AudioSource** sources, unsigned int* count)
{
    for (AudioSequenceInstance* voice = voices;
        voice != 0;
        voice = voice->next)
    {
        voice->GetSources(sources, count);
    }
}

void SoundInstance::Update(float dt)
{
    if (state == SOUND_INSTANCE_STATE_PLAYING || state == SOUND_INSTANCE_STATE_STOPPING)
    {
        volume.Update(dt, 1.0f);
        pitch.Update(dt, 1.0f);
        previousTime = currentTime;
        currentTime += dt;
    }

    if (activeRpc != 0 && state == SOUND_INSTANCE_STATE_STOPPING)
    {
        transitionTime += dt;
        AudioSlider* slider = GetSoundParameter(owner, 2);
        slider->SetTarget(transitionTime, 0.0f);
        if (activeRpc->value < -94.0f)
        {
            voices->Stop();
            activeRpc = 0;
        }
    }

    int voiceState = voices != 0
                       ? voices->Update(dt)
                       : SOUND_INSTANCE_STATE_STOPPED;
    switch (state)
    {
    case SOUND_INSTANCE_STATE_PLAYING:
        if (voiceState == SOUND_INSTANCE_STATE_STOPPED)
        {
            RemoveAudioRpcRuntimeNodes(
                GetAudioRpcController(),
                this);
            rpcEntries.Clear();
            AudioSequenceInstance* voice = voices;
            while (voice != 0)
            {
                AudioSequenceInstance* next = voice->next;
                delete voice;
                voice = next;
            }
            voices = 0;
            state = SOUND_INSTANCE_STATE_STOPPED;
        }
        break;
    case SOUND_INSTANCE_STATE_STOPPING:
        if (voiceState == SOUND_INSTANCE_STATE_STOPPED)
        {
            RemoveAudioRpcRuntimeNodes(
                GetAudioRpcController(),
                this);
            rpcEntries.Clear();
            AudioSequenceInstance* voice = voices;
            while (voice != 0)
            {
                AudioSequenceInstance* next = voice->next;
                delete voice;
                voice = next;
            }
            voices = 0;
            state = SOUND_INSTANCE_STATE_STOPPED;
        }
        break;
    case SOUND_INSTANCE_STATE_PREPARING:
        if (voiceState == SOUND_INSTANCE_STATE_PREPARED)
            state = SOUND_INSTANCE_STATE_PREPARED;
        break;
    case SOUND_INSTANCE_STATE_FAILED:
        state = SOUND_INSTANCE_STATE_STOPPED;
        break;
    case SOUND_INSTANCE_STATE_INITIAL:
    case SOUND_INSTANCE_STATE_PENDING:
    case SOUND_INSTANCE_STATE_PREPARED:
    case SOUND_INSTANCE_STATE_PAUSED:
    case SOUND_INSTANCE_STATE_STOPPED:
        break;
    }

    if (releaseTime >= 0.0f)
    {
        releaseTime -= dt;
        if (releaseTime < 0.0f)
        {
            if (state != SOUND_INSTANCE_STATE_PLAYING || activeRpc == 0)
            {
                activeRpc = 0;
                if (voices != 0)
                    voices->Stop();
            }
            state = SOUND_INSTANCE_STATE_STOPPING;
        }
    }
}

float SoundInstance::GetVolume()
{
    float currentVolume = volume.value;
    AudioCalculationTable* table = (AudioCalculationTable*)
        GetAudioBundleManager()->GetCalculationTable();
    AudioCalculationSlider* entry =
        table->sliders + definition->sliderIndex;
    float sliderValue = entry->GetValue();
    return volumeOffset + (sliderValue + currentVolume);
}

float SoundInstance::GetPitch()
{
    return pitch.value;
}

void SoundInstance::Destroy()
{
    RemoveAudioRpcRuntimeNodes(
        GetAudioRpcController(), this);
    rpcEntries.Clear();
}
