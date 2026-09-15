#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/XSoundHandle.h"
#include "NL/nlMath.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

#include "Game/Audio/SoundInstance_802F2110.h"
#include "Game/Audio/AudioSequenceEvent.h"

SlotPool<SoundPlaybackEvent> sSoundPlaybackEventPool(32, 16);
SlotPool<HitMarkerEvent> sHitMarkerEventPool(32, 16);
SlotPool<ParameterChangeEvent> sParameterChangeEventPool(32, 16);

extern "C" float fn_802F29F8(SoundInstance_802F2110*);
extern "C" float fn_802F2A6C(SoundInstance_802F2110*);

static inline float RandomRange(float minimum, float maximum)
{
    return nlRandomf(minimum, maximum, &nlDefaultSeed);
}

inline SoundPlaybackEvent::SoundPlaybackEvent(
    AudioSequenceInstance* owner, SoundEventDefinition* definition)
    : AudioSequenceEvent(owner)
    , sourceInfo(0)
    , volumeModifier(0.0f)
    , pitchModifier(0.0f)
    , currentVolume(0.0f)
    , currentPitch(0.0f)
    , source(0)
    , flags(0)
{
    this->definition = definition;
    float maximum = definition->delayRange;
    float minimum = definition->delayMinimum;
    if (maximum == 0.0f)
        startTime = minimum;
    else
        startTime = RandomRange(minimum, minimum + maximum);
    sourceInfo = SelectSourceInfo();
    source = g_pAudioBackend->CreateSource(
        sourceInfo, this->owner->soundInstance->owner->m_Owner);
}

inline HitMarkerEvent::HitMarkerEvent(
    AudioSequenceInstance* owner, HitMarkerEventDefinition* definition)
    : AudioSequenceEvent(owner)
    , definition(definition)
{
    float maximum = definition->delayRange;
    float minimum = definition->delayMinimum;
    if (maximum == 0.0f)
        startTime = minimum;
    else
        startTime = RandomRange(minimum, minimum + maximum);
    state = 0;
}

inline ParameterChangeEvent::ParameterChangeEvent(
    AudioSequenceInstance* owner, ParameterChangeEventDefinition* definition)
    : AudioSequenceEvent(owner)
    , definition(definition)
{
    float maximum = definition->delayRange;
    float minimum = definition->delayMinimum;
    if (maximum == 0.0f)
        startTime = minimum;
    else
        startTime = RandomRange(minimum, minimum + maximum);
    state = 0;
}

AudioSequenceEvent* AudioSequenceEvent::Create(AudioSequenceInstance* owner,
    AudioSequenceEventDefinition* definition)
{
    switch (definition->type)
    {
    case 1:
        return new SoundPlaybackEvent(owner, definition->sound);
    case 3:
        return new HitMarkerEvent(owner, definition->hitMarker);
    case 2:
        return new ParameterChangeEvent(owner, definition->parameter);
    default:
        return 0;
    }
}

SoundPlaybackEvent::~SoundPlaybackEvent()
{
    if (source != 0)
        g_pAudioBackend->ReleaseSource(source);
}

void SoundPlaybackEvent::StartPlayback()
{
    if (definition->randomVolume)
        volumeModifier = nlRandomf(definition->volumeMinimum,
            definition->volumeMaximum,
            &nlDefaultSeed);
    else
        volumeModifier = 0.0f;

    if (definition->randomPitch)
        pitchModifier = nlRandomf(definition->pitchMinimum,
            definition->pitchMaximum,
            &nlDefaultSeed);
    else
        pitchModifier = 0.0f;
    currentVolume = -96.0f;
    currentPitch = 0.0f;
    UpdatePlaybackParameters(true);
    source->Play(definition->soundId);
    state = 4;
}

void SoundPlaybackEvent::Prepare()
{
    source->Prepare();
    state = 2;
}

AudioSourceInfo* SoundPlaybackEvent::SelectSourceInfo()
{
    u32 selectedIndex;
    if (definition->choiceCount == 1)
        selectedIndex = definition->choices[0].index;
    else
    {
        u32 totalWeight = 0;
        for (int i = 0; i < definition->choiceCount; i++)
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

    void* entries = *(void**)((u8*)owner->soundInstance->owner->m_Slot
                              + 0x10);
    return (AudioSourceInfo*)((u8*)*(void**)((u8*)entries + 0x14)
                              + selectedIndex * sizeof(AudioSourceInfo));
}

int SoundPlaybackEvent::Update(float)
{
    if (source != 0)
        source->UpdateState();

    switch (state)
    {
    case 2:
        if (source->GetState() == 3)
            state = 3;
        break;
    case 4:
        if (source->GetState() == 1)
        {
            state = 8;
            flags = 0;
        }
        break;
    case 7:
        if (source->GetState() == 1)
        {
            state = 8;
            flags = 0;
        }
        break;
    case 5:
        return state;
    default:
        break;
    }

    if (flags == 0)
        return state;

    bool condition = owner->soundInstance->previousTime < startTime
                  && owner->soundInstance->currentTime >= startTime;
    if (condition)
        StartPlayback();
    if (state != 4)
        return state;
    UpdatePlaybackParameters(false);
    return state;
}

void SoundPlaybackEvent::Pause()
{
    if (source != 0)
        source->Pause();
    savedState = state;
    state = 5;
}

void SoundPlaybackEvent::Resume()
{
    if (source != 0)
        source->Resume();
    state = savedState;
}

static inline void AccumulateRpcModifier(
    RpcRuntimeNode_802F2110* node, u32& volumeCount, u32& pitchCount,
    float* volume, float* pitch)
{
    if (node->definition->kind == 1)
    {
        pitchCount++;
        float value = 0.01f * node->value;
        value = 0.091f * value;
        *pitch += value;
    }
    else if (node->definition->kind == 0)
    {
        volumeCount++;
        *volume += node->value;
    }
}

void SoundPlaybackEvent::UpdatePlaybackParameters(bool force)
{
    Plat3dSoundSrc* spatialSource
        = (Plat3dSoundSrc*)owner->soundInstance->owner->m_Owner;
    if (spatialSource != 0)
        source->SetSpatialParameters(spatialSource);

    bool hasVolume = false;
    float volume = 0.0f;
    bool hasPitch = false;
    float pitch = 0.0f;
    AccumulateRpcModifiers(&hasVolume, &volume, &hasPitch, &pitch);

    float instanceVolume = fn_802F29F8(owner->soundInstance);
    float volumeModifier = this->volumeModifier;
    float volumeOffset = owner->volumeOffset;
    float playbackVolume = volumeModifier + volumeOffset;
    float currentVolume = volume + (instanceVolume + playbackVolume);
    currentVolume = currentVolume >= -96.0f ? currentVolume : -96.0f;
    currentVolume = currentVolume <= 6.0f ? currentVolume : 6.0f;
    if (force || this->currentVolume != currentVolume)
    {
        this->currentVolume = currentVolume;
        source->SetInputVolume(this->currentVolume);
    }

    float instancePitch = fn_802F2A6C(owner->soundInstance);
    float pitchModifier = this->pitchModifier;
    float playbackPitch = pitchModifier + instancePitch;
    float pitchOffset = owner->pitchOffset;
    float currentPitch = pitch + (pitchOffset + playbackPitch);
    if (this->currentPitch != currentPitch)
    {
        this->currentPitch = currentPitch;
        source->SetPitch(currentPitch);
    }
}

void SoundPlaybackEvent::AccumulateRpcModifiers(
    bool* hasVolume, float* volume, bool* hasPitch, float* pitch)
{
    *hasPitch = false;
    *hasVolume = false;
    u32 volumeCount = 0;
    u32 pitchCount = 0;

    typedef UnidentifiedDLListPool_802F2188<RpcRuntimeNode_802F2110*> RpcList;
    nlDLListIterator<RpcRuntimeNode_802F2110*> iterator;
    iterator = ((RpcList*)&owner->soundInstance->entryPool)->Begin();
    while (iterator.hasNext())
    {
        AccumulateRpcModifier(
            *iterator, volumeCount, pitchCount, volume, pitch);
        iterator.Step();
    }

    VoiceDefinition_802F2110* definition = owner->soundInstance->definition;
    for (u32 i = 0; i < definition->modifierCount; i++)
    {
        RpcRuntimeNode_802F2110*& node = definition->modifiers[i];
        AccumulateRpcModifier(
            node, volumeCount, pitchCount, volume, pitch);
    }
    *hasVolume = volumeCount != 0;
    *hasPitch = pitchCount != 0;
}

void SoundPlaybackEvent::Stop()
{
    source->Stop();
    state = 7;
    flags = 0;
}

u32 SoundPlaybackEvent::GetSources(AudioSource** results)
{
    if (source != 0)
    {
        if (source->HasVoice())
        {
            *results = source;
            return true;
        }
    }
    return false;
}

int HitMarkerEvent::Update(float)
{
    if (state == 2)
        state = 3;
    bool condition = owner->soundInstance->previousTime < startTime
                  && owner->soundInstance->currentTime >= startTime;
    if (condition)
    {
        ((XSoundHandle*)owner->soundInstance->owner)
            ->OnHitMarker(definition->marker);
        state = 8;
    }
    return state;
}

int ParameterChangeEvent::Update(float)
{
    if (state == 2)
        state = 3;
    AudioSequenceInstance* owner = this->owner;
    bool trigger = owner->soundInstance->previousTime < startTime
                && owner->soundInstance->currentTime >= startTime;
    if (trigger)
    {
        if (definition->parameter == 0)
            owner->SetPitch(definition->value);
        else if (definition->parameter == 1)
            owner->SetVolume(definition->value);
        state = 8;
    }
    return state;
}
