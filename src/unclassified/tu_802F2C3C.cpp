#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/XSoundHandle.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

#include "Game/Audio/SoundInstance_802F2110.h"
#include "unclassified/tu_802F2C3C.h"


SlotPool<PlaybackObject_8052F7B0> lbl_8057FAE8(32, 16);
SlotPool<PlaybackObject_8052F780> lbl_8057FB10(32, 16);
SlotPool<PlaybackObject_8052F750> lbl_8057FB38(32, 16);

PlaybackObject_802F2C3C::~PlaybackObject_802F2C3C()
{
}

extern "C" float fn_802F29F8(SoundInstance_802F2110*);
extern "C" float fn_802F2A6C(SoundInstance_802F2110*);

static inline float RandomRange_802F2C3C(float minimum, float maximum)
{
    return nlRandomf(minimum, maximum, &nlDefaultSeed);
}

extern "C" PlaybackObject_802F2C3C* fn_802F3114(PlaybackObject_8052F7B0* object);

inline PlaybackObject_8052F7B0::PlaybackObject_8052F7B0(
    AudioSequenceInstance* owner, PlaybackRequest_802F2C3C* request)
    : PlaybackObject_802F2C3C(owner)
{
    selection = 0;
    volumeModifier = 0.0f;
    pitchModifier = 0.0f;
    currentVolume = 0.0f;
    currentPitch = 0.0f;
    backend = 0;
    flags = 0;
    definition = request->definition;
    float maximum = request->definition->volumeMaximum;
    float minimum = request->definition->volumeMinimum;
    startTime = maximum == 0.0f
                    ? minimum
                    : RandomRange_802F2C3C(minimum, minimum + maximum);
    selection = (u32)fn_802F3114(this);
    backend = g_pAudioBackend->CreateSource(
        (AudioSourceInfo*)selection, this->owner->instance->owner->m_Owner);
}

inline PlaybackObject_8052F780::PlaybackObject_8052F780(
    AudioSequenceInstance* owner, PlaybackRequest_802F2C3C* request)
    : PlaybackObject_802F2C3C(owner)
{
    definition = request->definition;
    float maximum = request->definition->pitchMaximum;
    float minimum = request->definition->pitchMinimum;
    startTime = maximum == 0.0f
                    ? minimum
                    : RandomRange_802F2C3C(minimum, minimum + maximum);
    state = 0;
}

inline PlaybackObject_8052F750::PlaybackObject_8052F750(
    AudioSequenceInstance* owner, PlaybackRequest_802F2C3C* request)
    : PlaybackObject_802F2C3C(owner)
{
    definition = request->definition;
    float maximum = request->definition->pitchMaximum;
    float minimum = request->definition->pitchMinimum;
    startTime = maximum == 0.0f
                    ? minimum
                    : RandomRange_802F2C3C(minimum, minimum + maximum);
    state = 0;
}

extern "C" PlaybackObject_802F2C3C* fn_802F2CAC(AudioSequenceInstance* owner,
    PlaybackRequest_802F2C3C* request)
{
    switch (request->kind)
    {
    case 1:
        return new PlaybackObject_8052F7B0(owner, request);
    case 3:
        return new PlaybackObject_8052F780(owner, request);
    case 2:
        return new PlaybackObject_8052F750(owner, request);
    default:
        return 0;
    }
}

PlaybackObject_8052F7B0::~PlaybackObject_8052F7B0()
{
    if (backend != 0)
        g_pAudioBackend->ReleaseSource(backend);
}

extern "C" void fn_802F34E4(PlaybackObject_8052F7B0*, bool);

void PlaybackObject_8052F7B0::UnidentifiedVirtual30()
{
    if (definition->randomVolume)
        volumeModifier = nlRandomf(definition->volumeMinimum,
            definition->volumeMaximum, &nlDefaultSeed);
    else
        volumeModifier = 0.0f;

    if (definition->randomPitch)
        pitchModifier = nlRandomf(definition->pitchMinimum,
            definition->pitchMaximum, &nlDefaultSeed);
    else
        pitchModifier = 0.0f;
    currentVolume = -96.0f;
    currentPitch = 0.0f;
    fn_802F34E4(this, true);
    backend->Play(definition->soundId);
    state = 4;
}

void PlaybackObject_8052F7B0::Prepare()
{
    backend->Prepare();
    state = 2;
}

extern "C" PlaybackObject_802F2C3C* fn_802F3114(
    PlaybackObject_8052F7B0* object)
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

    u8* resource = (u8*)object->owner->instance->owner->m_Slot;
    void* table = *(void**)(resource + 4);
    void* entries = *(void**)((u8*)table + 0x10);
    return (PlaybackObject_802F2C3C*)((u8*)*(void**)((u8*)entries + 0x14)
                                      + selectedIndex * 0x1C);
}

int PlaybackObject_8052F7B0::Update(float)
{
    if (backend != 0)
        backend->UpdateState();

    switch (state)
    {
    case 2:
        if (backend->GetState() == 3)
            state = 3;
        break;
    case 4:
    case 7:
        if (backend->GetState() == 1)
        {
            state = 8;
            flags = 0;
        }
        break;
    case 3:
    {
        float previous = owner->instance->previousTime;
        float current = owner->instance->currentTime;
        if (previous < startTime && current >= startTime)
        {
            UnidentifiedVirtual30();
        }
        if (state == 4)
            fn_802F34E4(this, false);
        break;
    }
    default:
        break;
    }
    return state;
}

void PlaybackObject_8052F7B0::Pause()
{
    if (backend != 0)
        backend->Pause();
    savedState = state;
    state = 5;
}

void PlaybackObject_8052F7B0::Resume()
{
    if (backend != 0)
        backend->Resume();
    state = savedState;
}

extern "C" void fn_802F3648(PlaybackObject_8052F7B0* object,
    u8* hasVolume, float* volume, u8* hasPitch, float* pitch)
{
    u32 volumeCount = 0;
    u32 pitchCount = 0;
    *hasVolume = false;
    *hasPitch = false;

    RpcListEntry_802F2110* start = object->owner->instance->rpcEntries;
    RpcListEntry_802F2110* entry = start != 0 ? start->next : 0;
    while (entry != 0)
    {
        RpcRuntimeNode_802F2110* node = entry->node;
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

    VoiceDefinition_802F2110* definition = object->owner->instance->definition;
    for (u32 i = 0; i < definition->modifierCount; i++)
    {
        RpcRuntimeNode_802F2110* node = definition->modifiers[i];
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
    PlaybackObject_8052F7B0* object, bool force)
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
        object->backend->SetInputVolume(currentVolume);
    }

    float currentPitch = pitch + fn_802F2A6C(object->owner->instance)
                       + object->pitchModifier + object->owner->pitchOffset;
    if (object->currentPitch != currentPitch)
    {
        object->currentPitch = currentPitch;
        object->backend->SetPitch(currentPitch);
    }
}

void PlaybackObject_8052F7B0::Stop()
{
    backend->Stop();
    state = 7;
    flags = 0;
}

u32 PlaybackObject_8052F7B0::GetSources(AudioSource** results)
{
    if (backend != 0)
    {
        if (backend->HasVoice())
        {
            *results = backend;
            return true;
        }
    }
    return false;
}

int PlaybackObject_8052F780::Update(float)
{
    if (state == 2)
        state = 3;
    bool condition = owner->instance->previousTime < startTime
                  && owner->instance->currentTime >= startTime;
    if (condition)
    {
        ((XSoundHandle*)owner->instance->owner)
            ->OnHitMarker((void*)definition->soundId);
        state = 8;
    }
    return state;
}

int PlaybackObject_8052F750::Update(float)
{
    if (state == 2)
        state = 3;
    AudioSequenceInstance* owner = this->owner;
    bool trigger = owner->instance->previousTime < startTime
        && owner->instance->currentTime >= startTime;
    if (trigger)
    {
        if (definition->mode == 0)
            owner->SetPitch(definition->value);
        else if (definition->mode == 1)
            owner->SetVolume(definition->value);
        state = 8;
    }
    return state;
}

unsigned int AudioSource::GetState()
{
    return m_Unknown04;
}
