#include "Game/Audio/AudioSequenceInstance.h"
#include "Game/Audio/AudioSequenceEvent.h"

#include "NL/nlMath.h"

SlotPool<AudioSequenceInstance> sAudioSequenceInstancePool(32, 16);

/**
 * Address/Size: 0x802F3E20 | size: 0xAC
 */
AudioSequenceInstance::AudioSequenceInstance(
    SoundInstance_802F2110* soundInstance, AudioSequenceDefinition* definition)
{
    next = 0;
    this->soundInstance = soundInstance;
    this->definition = definition;
    events = 0;
    volumeOffset = definition->volumeOffset;
    pitchOffset = 0.0f;
    stopped = false;

    AudioSequenceEvent* previousEvent = 0;
    for (u32 index = 0; index < definition->eventCount; index++)
    {
        AudioSequenceEvent* event
            = AudioSequenceEvent::Create(this, definition->eventDefinitions + index);
        if (previousEvent == 0)
            events = event;
        else
            previousEvent->next = event;
        previousEvent = event;
    }
}

/**
 * Address/Size: 0x802F3ECC | size: 0xA0
 */
AudioSequenceInstance::~AudioSequenceInstance()
{
    AudioSequenceEvent* event = events;
    while (event != 0)
    {
        AudioSequenceEvent* nextEvent = event->next;
        delete event;
        event = nextEvent;
    }
    events = 0;
}

/**
 * Address/Size: 0x802F3F6C | size: 0x104
 */
void AudioSequenceInstance::Play()
{
    for (AudioSequenceEvent* event = events; event != 0;
        event = event->next)
    {
        event->Play();
    }
    if (next != 0)
        next->Play();
}

/**
 * Address/Size: 0x802F4070 | size: 0x104
 */
void AudioSequenceInstance::Prepare()
{
    for (AudioSequenceEvent* event = events; event != 0;
        event = event->next)
    {
        event->Prepare();
    }
    if (next != 0)
        next->Prepare();
}

/**
 * Address/Size: 0x802F4174 | size: 0x104
 */
void AudioSequenceInstance::Pause()
{
    for (AudioSequenceEvent* event = events; event != 0;
        event = event->next)
    {
        event->Pause();
    }
    if (next != 0)
        next->Pause();
}

/**
 * Address/Size: 0x802F4278 | size: 0x104
 */
void AudioSequenceInstance::Resume()
{
    for (AudioSequenceEvent* event = events; event != 0;
        event = event->next)
    {
        event->Resume();
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
    for (AudioSequenceEvent* event = events; event != 0;
        event = event->next)
    {
        if (!stopped || event->state != 0)
        {
            int state = event->Update(time);
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
    for (AudioSequenceEvent* event = events; event != 0;
        event = event->next)
    {
        if ((u32)(event->state - 2) <= 3)
            event->Stop();
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
    for (AudioSequenceEvent* event = events; event != 0;
        event = event->next)
    {
        *count += event->GetSources(results + *count);
    }
}
