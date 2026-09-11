#ifndef GAME_AUDIO_UNIDENTIFIED_REGISTRY_POOLS_H
#define GAME_AUDIO_UNIDENTIFIED_REGISTRY_POOLS_H

#include "NL/nlSlotPool.h"
#include "unclassified/tu_802BE64C.h"

// Shared storage for the dynamic registry: one pool of containers and one of
// nodes, both constructed (16, 16). Sixty-seven translation units emit the
// guarded initializers for this pair; `Game/tu_80009B34.cpp` is first in link
// order and therefore owns the two objects and their guards.
//
// The entry sizes are not guesses. `UnidentifiedRegistryOwner_802BF984`
// allocates every container through `AllocContainer()` and every node through
// `AllocNode()`, and the concrete types placement-constructed into those blocks
// fix the sizes exactly: `CreateRoot` and `UnidentifiedRegistryScoped_802BEF0C::
// AddChild` build a scoped container (0x14, the largest container the owner
// constructs), and `AddNamed`/`AddUnnamed` build an
// `UnidentifiedRegistryNode_802BE64C` (0x40).
// `Game/Audio/AudioResourceRuntime.cpp` is the retained consumer.
//
// Still unidentified, and the reason the names below keep the `Unidentified`
// prefix: the original template's spelling, its tag, and the header path. The
// stripped DOL carries no mangled type or filename evidence for any of them.

template <typename T>
struct UnidentifiedRegistryPools
{
    static SlotPool<UnidentifiedRegistryScoped_802BEF0C> sContainerPool;
    static SlotPool<UnidentifiedRegistryNode_802BE64C> sNodePool;
};

struct UnidentifiedRegistryPoolTag;

template <typename T>
SlotPool<UnidentifiedRegistryScoped_802BEF0C>
    UnidentifiedRegistryPools<T>::sContainerPool(16, 16);

template <typename T>
SlotPool<UnidentifiedRegistryNode_802BE64C>
    UnidentifiedRegistryPools<T>::sNodePool(16, 16);

inline UnidentifiedRegistryScoped_802BEF0C* UnidentifiedRegistryAllocContainer()
{
    return UnidentifiedRegistryPools<
        UnidentifiedRegistryPoolTag>::sContainerPool.Allocate();
}

inline UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryAllocNode()
{
    return UnidentifiedRegistryPools<
        UnidentifiedRegistryPoolTag>::sNodePool.Allocate();
}

inline void UnidentifiedRegistryFreeContainer(
    UnidentifiedRegistryScoped_802BEF0C* container)
{
    UnidentifiedRegistryPools<UnidentifiedRegistryPoolTag>::sContainerPool.Free(
        container);
}

inline void UnidentifiedRegistryFreeNode(UnidentifiedRegistryNode_802BE64C* node)
{
    UnidentifiedRegistryPools<UnidentifiedRegistryPoolTag>::sNodePool.Free(node);
}

#endif // GAME_AUDIO_UNIDENTIFIED_REGISTRY_POOLS_H
