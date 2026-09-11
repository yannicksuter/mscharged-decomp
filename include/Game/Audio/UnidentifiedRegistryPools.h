#ifndef GAME_AUDIO_UNIDENTIFIED_REGISTRY_POOLS_H
#define GAME_AUDIO_UNIDENTIFIED_REGISTRY_POOLS_H

#include "NL/nlSlotPool.h"
#include "unclassified/tu_802BE64C.h"

// Shared allocation storage for the registry's concrete container and node types.
// AudioResourceRuntime obtains slots here and releases both pools on destruction.
template <typename Container, typename Node>
struct UnidentifiedRegistryPools
{
    static SlotPool<Container> sContainerPool;
    static SlotPool<Node> sNodePool;
};

typedef UnidentifiedRegistryPools<UnidentifiedRegistryScoped_802BEF0C, UnidentifiedRegistryNode_802BE64C> UnidentifiedRegistryPoolTypes;

template <typename Container, typename Node>
SlotPool<Container> UnidentifiedRegistryPools<Container, Node>::sContainerPool(16, 16);

template <typename Container, typename Node>
SlotPool<Node> UnidentifiedRegistryPools<Container, Node>::sNodePool(16, 16);

inline UnidentifiedRegistryScoped_802BEF0C* UnidentifiedRegistryAllocContainer()
{
    return UnidentifiedRegistryPoolTypes::sContainerPool.Allocate();
}

inline UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryAllocNode()
{
    return UnidentifiedRegistryPoolTypes::sNodePool.Allocate();
}

inline void UnidentifiedRegistryFreeContainer(
    UnidentifiedRegistryScoped_802BEF0C* container)
{
    UnidentifiedRegistryPoolTypes::sContainerPool.Free(container);
}

inline void UnidentifiedRegistryFreeNode(UnidentifiedRegistryNode_802BE64C* node)
{
    UnidentifiedRegistryPoolTypes::sNodePool.Free(node);
}

#endif // GAME_AUDIO_UNIDENTIFIED_REGISTRY_POOLS_H
