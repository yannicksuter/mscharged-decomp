#ifndef GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_INL
#define GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_INL

#include "Game/Audio/AudioResourceRuntime.h"
#include "Game/Audio/RegistryPools.h"

inline UnidentifiedAudioPoolOwner::~UnidentifiedAudioPoolOwner()
{
    SlotPoolBase::BaseFreeBlocks(
        &RegistryPoolTypes::sContainerPool,
        sizeof(ScopedRegistryContainer));
    SlotPoolBase::BaseFreeBlocks(
        &RegistryPoolTypes::sNodePool,
        sizeof(RegistryNode));
}

/**
 * Address/Size: 0x802F6680 | size: 0x84
 */
inline AudioResourceRuntime::~AudioResourceRuntime()
{
}

#endif // GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_INL
