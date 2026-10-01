#ifndef GAME_AUDIO_REGISTRY_POOLS_H
#define GAME_AUDIO_REGISTRY_POOLS_H

#include "NL/nlSlotPool.h"
#include "Game/Audio/AudioRegistryOwner.h"

// Shared allocation storage for the registry's concrete container and node types.
// AudioResourceRuntime obtains slots here and releases both pools on destruction.
template <typename Container, typename Node>
struct RegistryPools
{
    static SlotPool<Container> sContainerPool;
    static SlotPool<Node> sNodePool;
};

typedef RegistryPools<ScopedRegistryContainer, RegistryNode> RegistryPoolTypes;

template <typename Container, typename Node>
SlotPool<Container> RegistryPools<Container, Node>::sContainerPool(16, 16);

template <typename Container, typename Node>
SlotPool<Node> RegistryPools<Container, Node>::sNodePool(16, 16);

inline ScopedRegistryContainer* RegistryAllocContainer()
{
    return RegistryPoolTypes::sContainerPool.Allocate();
}

inline RegistryNode* RegistryAllocNode()
{
    return RegistryPoolTypes::sNodePool.Allocate();
}

inline void RegistryFreeContainer(
    ScopedRegistryContainer* container)
{
    RegistryPoolTypes::sContainerPool.Free(container);
}

inline void RegistryFreeNode(RegistryNode* node)
{
    RegistryPoolTypes::sNodePool.Free(node);
}

inline UnidentifiedAudioPoolOwner::~UnidentifiedAudioPoolOwner()
{
    SlotPoolBase::BaseFreeBlocks(&RegistryPoolTypes::sContainerPool, sizeof(ScopedRegistryContainer));
    SlotPoolBase::BaseFreeBlocks(&RegistryPoolTypes::sNodePool, sizeof(RegistryNode));
}

inline RegistryContainer* AudioRegistryOwner::AllocContainer()
{
    return RegistryPoolTypes::sContainerPool.Allocate();
}

inline RegistryNode* AudioRegistryOwner::AllocNode()
{
    return RegistryPoolTypes::sNodePool.Allocate();
}

inline void* AudioRegistryOwner::AllocItem(unsigned int size)
{
    return nlMalloc(size, 8, true);
}

inline void AudioRegistryOwner::FreeContainer(void* container)
{
    RegistryFreeContainer((ScopedRegistryContainer*)container);
}

inline void AudioRegistryOwner::FreeNode(void* node)
{
    RegistryFreeNode((RegistryNode*)node);
}

inline void AudioRegistryOwner::FreeItem(void* data)
{
    nlFree(data);
}

#endif // GAME_AUDIO_REGISTRY_POOLS_H
