#include "Game/Audio/AudioSequenceInstance.h"
#include "Game/Audio/SoundInstance.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioCalculation.h"
#include "Game/Audio/AudioRpc.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/XSoundHandle.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/Transition.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

class AudioSource;

struct AudioBundleManagerSoundInstanceView
{
    u8 pad_00[0x50];
    AudioRpcController* rpcController;
};

static inline AudioRpcController* GetSoundInstanceRpcController()
{
    return ((AudioBundleManagerSoundInstanceView*)GetAudioBundleManager())
        ->rpcController;
}

SlotPool<SoundInstance> sSoundInstancePool(32, 16);

static inline SlotPool<AudioRpcListEntry>& GetSoundInstanceRpcEntryPool()
{
    return sAudioRpcListEntryPool;
}

SoundInstance::SoundInstance(
    XSoundHandle* owner, AudioVoiceDefinition* definition)
    : owner(owner)
    , definition(definition)
    , voices(0)
    , rpcEntries(GetSoundInstanceRpcEntryPool())
    , state(SOUND_INSTANCE_STATE_INITIAL)
    , previousTime(-1.0f)
    , currentTime(0.0f)
    , activeRpc(0)
    , transitionTime(0.0f)
    , volumeOffset(0.0f)
    , releaseTime(-1.0f)
    , nextInstance(0)
{
    SoundInstance* instance = this;
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

    AudioSequenceInstance* previous = 0;
    for (u32 i = 0; i < instance->definition->sequenceCount; i++)
    {
        AudioSequenceInstance* voice = new AudioSequenceInstance(
            instance, instance->definition->sequences[i]);
        if (previous == 0)
            instance->voices = voice;
        else
            previous->next = voice;
        previous = voice;
    }
}

void SoundInstance::Play(float)
{
    SoundInstance* instance = this;
    instance->volume.Update(0.0f, 1.0f);
    instance->pitch.Update(0.0f, 1.0f);
    if (instance->voices != 0)
        instance->voices->Play();
    instance->state = SOUND_INSTANCE_STATE_PLAYING;
}

void SoundInstance::Prepare()
{
    volume.SetTarget(definition->volume, 0.0f);
    pitch.SetTarget(definition->pitch, 0.0f);

    AudioRpcController* controller = GetSoundInstanceRpcController();
    AudioRpcGroup* group;
    u32 definitionIndex;
    AudioRpcRuntimeNode* node;
    for (u32 groupIndex = 0;
        groupIndex < definition->rpcGroupCount;
        groupIndex++)
    {
        group = &controller->groups[
            definition->rpcGroupIndices[groupIndex]];
        for (definitionIndex = 0;
            definitionIndex < group->dynamicDefinitionCount;
            definitionIndex++)
        {
            AudioRpcDefinition* rpcDefinition =
                &group->dynamicDefinitions[definitionIndex];
            node = AddAudioRpcRuntimeNode(
                controller, rpcDefinition, (AudioRpcOwner*)this);
            activeRpc = rpcDefinition->sliderIndex == 2 ? node : 0;
            rpcEntries.AddEnd(node);
        }
    }

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
    SoundInstance* instance = this;
    instance->volume.SetTarget(target, duration);
    if (releaseAfterTransition)
        instance->releaseTime = duration;
}

void SoundInstance::Stop(void* value)
{
    SoundInstance* instance = this;
    if (instance->state != SOUND_INSTANCE_STATE_PLAYING
        || value != 0 || instance->activeRpc == 0)
    {
        instance->activeRpc = 0;
        if (instance->voices != 0)
            instance->voices->Stop();
    }
    instance->state = SOUND_INSTANCE_STATE_STOPPING;
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
    SoundInstance* instance = this;
    if (instance->state == SOUND_INSTANCE_STATE_PLAYING || instance->state == SOUND_INSTANCE_STATE_STOPPING)
    {
        instance->volume.Update(dt, 1.0f);
        instance->pitch.Update(dt, 1.0f);
        instance->previousTime = instance->currentTime;
        instance->currentTime += dt;
    }

    if (instance->activeRpc != 0 && instance->state == SOUND_INSTANCE_STATE_STOPPING)
    {
        instance->transitionTime += dt;
        Transition* slider = (Transition*)GetSoundParameter((XSoundHandle*)instance->owner, 2);
        slider->SetTarget(instance->transitionTime, 0.0f);
        if (instance->activeRpc->value < -94.0f)
        {
            instance->voices->Stop();
            instance->activeRpc = 0;
        }
    }

    int voiceState = instance->voices != 0
                       ? instance->voices->Update(dt)
                       : 8;
    switch (instance->state)
    {
    case SOUND_INSTANCE_STATE_PLAYING:
        if (voiceState == SOUND_INSTANCE_STATE_STOPPED)
        {
            RemoveAudioRpcRuntimeNodes(
                GetSoundInstanceRpcController(),
                (AudioRpcOwner*)instance);
            instance->rpcEntries.Clear();
            AudioSequenceInstance* voice = instance->voices;
            while (voice != 0)
            {
                AudioSequenceInstance* next = voice->next;
                delete voice;
                voice = next;
            }
            instance->voices = 0;
            instance->state = SOUND_INSTANCE_STATE_STOPPED;
        }
        break;
    case SOUND_INSTANCE_STATE_STOPPING:
        if (voiceState == SOUND_INSTANCE_STATE_STOPPED)
        {
            RemoveAudioRpcRuntimeNodes(
                GetSoundInstanceRpcController(),
                (AudioRpcOwner*)instance);
            instance->rpcEntries.Clear();
            AudioSequenceInstance* voice = instance->voices;
            while (voice != 0)
            {
                AudioSequenceInstance* next = voice->next;
                delete voice;
                voice = next;
            }
            instance->voices = 0;
            instance->state = SOUND_INSTANCE_STATE_STOPPED;
        }
        break;
    case SOUND_INSTANCE_STATE_PREPARING:
        if (voiceState == SOUND_INSTANCE_STATE_PREPARED)
            instance->state = SOUND_INSTANCE_STATE_PREPARED;
        break;
    case SOUND_INSTANCE_STATE_FAILED:
        instance->state = SOUND_INSTANCE_STATE_STOPPED;
        break;
    case SOUND_INSTANCE_STATE_INITIAL:
    case SOUND_INSTANCE_STATE_PENDING:
    case SOUND_INSTANCE_STATE_PREPARED:
    case SOUND_INSTANCE_STATE_PAUSED:
    case SOUND_INSTANCE_STATE_STOPPED:
        break;
    }

    if (instance->releaseTime >= 0.0f)
    {
        instance->releaseTime -= dt;
        if (instance->releaseTime < 0.0f)
        {
            if (instance->state != SOUND_INSTANCE_STATE_PLAYING || instance->activeRpc == 0)
            {
                instance->activeRpc = 0;
                if (instance->voices != 0)
                    instance->voices->Stop();
            }
            instance->state = SOUND_INSTANCE_STATE_STOPPING;
        }
    }
}

float SoundInstance::GetVolume()
{
    SoundInstance* instance = this;
    float volume = instance->volume.value;
    AudioCalculationTable* table = (AudioCalculationTable*)
        g_pAudioSystem->GetBundleManager()->GetCalculationTable();
    AudioCalculationSlider* entry =
        table->sliders + instance->definition->sliderIndex;
    float value = entry->GetValue();
    return instance->volumeOffset + (value + volume);
}

float SoundInstance::GetPitch()
{
    return pitch.value;
}

void SoundInstance::Destroy()
{
    RemoveAudioRpcRuntimeNodes(
        GetSoundInstanceRpcController(), (AudioRpcOwner*)this);
    rpcEntries.Clear();
}
