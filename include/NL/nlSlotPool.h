#ifndef NL_SLOT_POOL_H
#define NL_SLOT_POOL_H

#include "NL/nlMemory.h"

typedef void* (*SlotPoolAllocatorFunc)(unsigned long size);
typedef void (*SlotPoolFreeFunc)(void* data);

void fn_802B467C(void* pool);

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

    BasicSlotPool(int initial, int delta)
        : SlotPoolBase()
    {
        m_Delta = delta;
        m_Initial = initial;
        if (m_Initial == 0)
        {
            SlotPoolBase::BaseAddNewBlock(this, sizeof(T));
        }
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
        if (m_Delta == 0)
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

    T* New(const T& data)
    {
        return new (Allocate()) T(data);
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
        entry->~T();
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
        if (this->m_Delta == 0)
        {
            SlotPoolBase::BaseAddNewBlock(this, sizeof(T));
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

template <typename T>
class BasicSlotPoolHigh : public BasicSlotPool<T>
{
public:
    static void* allocFN(unsigned long size)
    {
        return nlMalloc(size, 8, true);
    }

    static void freeFN(void* ptr)
    {
        nlFree(ptr);
    }

    BasicSlotPoolHigh()
        : BasicSlotPool<T>()
    {
        this->m_AllocFn = allocFN;
        this->m_FreeFn = freeFN;
    }

    void DeleteEntry(T* entry)
    {
        SlotPoolEntry* e = (SlotPoolEntry*)entry;
        e->next = this->m_FreeList;
        this->m_FreeList = e;
    }
};

#endif // NL_SLOT_POOL_H
