#ifndef NL_SLOT_POOL_H
#define NL_SLOT_POOL_H

#include "types.h"

void* operator new(unsigned long, void*);

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
        FreeBlocks();
    }

    void FreeBlocks()
    {
        fn_802B467C(this);
        SlotPoolBase::BaseFreeBlocks(this, sizeof(T));
    }

    void Initialize(int initial, int delta)
    {
        m_Delta = delta;
        m_Initial = initial;
        if (m_Initial == 0)
        {
            SlotPoolBase::BaseAddNewBlock(this, sizeof(T));
        }
    }

    void Allocate(T*& out)
    {
        out = 0;
        if (m_FreeList == 0)
        {
            BaseAddNewBlock(this, sizeof(T));
        }
        if (m_FreeList != 0)
        {
            out = (T*)m_FreeList;
            m_FreeList = m_FreeList->next;
        }
        new (out) T;
    }

    void AllocateForReturn(T*& out)
    {
        T* entry = 0;
        if (m_FreeList == 0)
        {
            BaseAddNewBlock(this, sizeof(T));
        }
        if (m_FreeList != 0)
        {
            entry = (T*)m_FreeList;
            m_FreeList = m_FreeList->next;
        }
        out = entry;
    }

    T* Allocate()
    {
        T* out = 0;
        AllocateForReturn(out);
        return out;
    }

    void Free(T* entry)
    {
        SlotPoolEntry* e = (SlotPoolEntry*)entry;
        e->next = m_FreeList;
        m_FreeList = e;
    }

    void DeleteEntry(T* entry)
    {
        Free(entry);
    }

    void Delete(T* entry)
    {
        Free(entry);
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
        SlotPoolEntry* slot = (SlotPoolEntry*)entry;
        slot->next = this->m_FreeList;
        this->m_FreeList = slot;
    }

    void FreeBlocks()
    {
        fn_802B467C(this);
        SlotPoolBase::BaseFreeBlocks(this, sizeof(T));
    }
};

#endif // NL_SLOT_POOL_H
