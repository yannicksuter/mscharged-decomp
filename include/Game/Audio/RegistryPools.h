#ifndef GAME_AUDIO_REGISTRY_POOLS_H
#define GAME_AUDIO_REGISTRY_POOLS_H

#include "NL/nlSlotPool.h"
#include "NL/nlRegistry.h"

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

#endif // GAME_AUDIO_REGISTRY_POOLS_H
