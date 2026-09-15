#ifndef NL_NLREGISTRYLOOKUP_H
#define NL_NLREGISTRYLOOKUP_H

#include "types.h"

// Hash-keyed entry in a packed registry's sorted named-entry table.
struct PackedRegistryEntry
{
    /* 0x00 */ u32 mHash;
    /* 0x04 */ void* mData;
};

const PackedRegistryEntry* FindPackedRegistryEntry(
    const u32& key, const PackedRegistryEntry* entries, int count);

#endif // NL_NLREGISTRYLOOKUP_H
