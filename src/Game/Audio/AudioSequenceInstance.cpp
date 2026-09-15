#include "Game/Audio/AudioSequenceInstance.h"
#include "unclassified/tu_802F2C3C.h"

#include "NL/nlMath.h"

SlotPool<AudioSequenceInstance> sAudioSequenceInstancePool(32, 16);

extern "C" PlaybackObject_802F2C3C* fn_802F2CAC(
    AudioSequenceInstance* owner, PlaybackRequest_802F2C3C* request);

/**
 * Address/Size: 0x802F3E20 | size: 0xAC
 */
AudioSequenceInstance::AudioSequenceInstance(SoundInstance_802F2110* instance, AudioSequenceDefinition* sequence)
{
    next = 0;
    this->instance = instance;
    this->sequence = sequence;
    objects = 0;
    volumeOffset = sequence->volume;
    pitchOffset = 0.0f;
    stopped = false;

    PlaybackObject_802F2C3C* previous = 0;
    for (u32 index = 0; index < sequence->count; index++)
    {
        PlaybackObject_802F2C3C* object
            = fn_802F2CAC(this, sequence->requests + index);
        if (previous == 0)
            objects = object;
        else
            previous->next = object;
        previous = object;
    }
}

/**
 * Address/Size: 0x802F3ECC | size: 0xA0
 */
AudioSequenceInstance::~AudioSequenceInstance()
{
    PlaybackObject_802F2C3C* object = objects;
    while (object != 0)
    {
        PlaybackObject_802F2C3C* following = object->next;
        delete object;
        object = following;
    }
    objects = 0;
}

/**
 * Address/Size: 0x802F3F6C | size: 0x104
 */
void AudioSequenceInstance::Play()
{
    for (PlaybackObject_802F2C3C* object = objects; object != 0;
        object = object->next)
    {
        object->Play();
    }
    if (next != 0)
        next->Play();
}

/**
 * Address/Size: 0x802F4070 | size: 0x104
 */
void AudioSequenceInstance::Prepare()
{
    for (PlaybackObject_802F2C3C* object = objects; object != 0;
        object = object->next)
    {
        object->Prepare();
    }
    if (next != 0)
        next->Prepare();
}

/**
 * Address/Size: 0x802F4174 | size: 0x104
 */
void AudioSequenceInstance::Pause()
{
    for (PlaybackObject_802F2C3C* object = objects; object != 0;
        object = object->next)
    {
        object->Pause();
    }
    if (next != 0)
        next->Pause();
}

/**
 * Address/Size: 0x802F4278 | size: 0x104
 */
void AudioSequenceInstance::Resume()
{
    for (PlaybackObject_802F2C3C* object = objects; object != 0;
        object = object->next)
    {
        object->Resume();
    }
    if (next != 0)
        next->Resume();
}

/**
 * Address/Size: 0x802F437C | size: 0x19C
 */
int AudioSequenceInstance::Update(float time)
{
    int result = 9;
    for (PlaybackObject_802F2C3C* object = objects; object != 0;
        object = object->next)
    {
        if (!stopped || object->state != 0)
        {
            int state = object->Update(time);
            result = nlMin(result, state);
        }
    }
    if (next != 0)
    {
        int state = next->Update(time);
        result = nlMin(result, state);
    }
    return result;
}

/**
 * Address/Size: 0x802F4518 | size: 0x118
 */
void AudioSequenceInstance::Stop()
{
    stopped = true;
    for (PlaybackObject_802F2C3C* object = objects; object != 0;
        object = object->next)
    {
        if ((u32)(object->state - 2) <= 3)
            object->Stop();
    }
    if (next != 0)
        next->Stop();
}

/**
 * Address/Size: 0x802F4630 | size: 0x8
 */
void AudioSequenceInstance::SetPitch(float value)
{
    pitchOffset = value;
}

/**
 * Address/Size: 0x802F4638 | size: 0x8
 */
void AudioSequenceInstance::SetVolume(float value)
{
    volumeOffset = value;
}

/**
 * Address/Size: 0x802F4640 | size: 0x7C
 */
void AudioSequenceInstance::GetSources(AudioSource** results, u32* count)
{
    for (PlaybackObject_802F2C3C* object = objects; object != 0;
        object = object->next)
    {
        *count += object->GetSources(results + *count);
    }
}
