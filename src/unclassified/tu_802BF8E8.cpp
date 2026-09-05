#include "unclassified/tu_802BE64C.h"

const UnidentifiedRegistryPackedEntry* UnidentifiedFindEntry(
    const u32& key, const UnidentifiedRegistryPackedEntry* entries, int count)
{
    int high = count - 1;
    int low = -1;
    while (high - low > 1)
    {
        int middle = (high + low) / 2;
        if (entries[middle].mHash > key)
        {
            high = middle;
        }
        else
        {
            low = middle;
        }
    }
    u32 found = entries[high].mHash;
    if (found == key)
    {
        return &entries[high];
    }
    if (low == -1)
    {
        return 0;
    }
    found = entries[low].mHash;
    if (found == key)
    {
        return &entries[low];
    }
    return 0;
}
