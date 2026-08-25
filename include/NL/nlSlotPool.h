#ifndef NL_SLOT_POOL_H
#define NL_SLOT_POOL_H

#include "types.h"

typedef void* (*SlotPoolAllocatorFunc)(unsigned long size);
typedef void (*SlotPoolFreeFunc)(void* data);

extern "C" void fn_802B467C(void* pool);

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

struct SlotPoolBlock
{
    SlotPoolBlock* next;
};

struct SlotPoolEntry
{
    SlotPoolEntry* next;
};

template <typename T>
class BasicSlotPool : public SlotPoolBase
{
public:
    BasicSlotPool()
        : SlotPoolBase()
    {
    }

    ~BasicSlotPool()
    {
        fn_802B467C(this);
        SlotPoolBase::BaseFreeBlocks(this, sizeof(T));
    }

    void Allocate(T*& out)
    {
        if (this->m_FreeList == 0)
        {
            SlotPoolBase::BaseAddNewBlock(this, sizeof(T));
        }
        if (this->m_FreeList != 0)
        {
            out = (T*)this->m_FreeList;
            this->m_FreeList = this->m_FreeList->next;
        }
    }

    void Free(T* entry)
    {
        SlotPoolEntry* e = (SlotPoolEntry*)entry;
        e->next = this->m_FreeList;
        this->m_FreeList = e;
    }

    void FreeBlocks()
    {
        fn_802B467C(this);
        SlotPoolBase::BaseFreeBlocks(this, sizeof(T));
    }
};

template <typename T>
class SlotPool : public BasicSlotPool<T>
{
public:
    SlotPool(int initial, int delta)
        : BasicSlotPool<T>()
    {
        this->m_Delta = delta;
        this->m_Initial = initial;
        if (this->m_Initial == 0)
        {
            SlotPoolBase::BaseAddNewBlock(this, sizeof(T));
        }
    }
};

#endif // NL_SLOT_POOL_H
