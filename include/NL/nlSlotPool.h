#ifndef NL_SLOT_POOL_H
#define NL_SLOT_POOL_H

#include "types.h"

typedef void* (*SlotPoolAllocatorFunc)(unsigned long size);
typedef void (*SlotPoolFreeFunc)(void* data);

struct SlotPoolBlock;
struct SlotPoolEntry;

class SlotPoolBase
{
public:
    SlotPoolBase();
    ~SlotPoolBase();

    static void BaseAddNewBlock(SlotPoolBase*, unsigned int entrySize);
    static void BaseFreeBlocks(SlotPoolBase*, unsigned int entrySize);

    u32 m_Initial;
    u32 m_Delta;
    SlotPoolBlock* m_BlockList;
    SlotPoolEntry* m_FreeList;
    SlotPoolAllocatorFunc m_AllocFn;
    SlotPoolFreeFunc m_FreeFn;
};

#endif // NL_SLOT_POOL_H
