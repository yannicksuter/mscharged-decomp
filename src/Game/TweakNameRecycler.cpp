#include "Game/TweakRegistry.h"

#include "NL/PointerEntryTable.h"
#include "NL/nlMemory.h"
#include "NL/nlSmallBlockAllocator.h"

class TweakNameAllocator
{
public:
    virtual void* Allocate();
    virtual void UnidentifiedVirtual0C(void* ptr);
};

class TweakNameTable : public PointerEntryTable
{
public:
    TweakNameTable(int capacity, TweakNameAllocator* allocator)
        : PointerEntryTable(capacity, allocator)
    {
    }
    virtual ~TweakNameTable();

    /* 0x14 */ u8 mUnidentified014[4]; // not accessed by retained code
}; // size: 0x18

nlSlotPoolFixed<0x10> gTweakNamePool;
TweakRecycledName* gRecycledTweakNameHead;
TweakRecycledName* gRecycledTweakNameTail;

TweakNameTable::~TweakNameTable()
{
}

void* TweakNameAllocator::Allocate()
{
    if (IsTweakRegistryInitialized() != 0)
    {
        return gTweakNamePool.Allocate();
    }
    return nlMalloc(0x10, 8, false);
}

void TweakNameAllocator::UnidentifiedVirtual0C(void* ptr)
{
    nlFree(ptr);
}

void RecycleTweakNames(void)
{
    TweakRecycledName* entry = gRecycledTweakNameHead;
    TweakRecycledName* next = entry == 0 ? 0 : entry->m_Next;
    while (entry != 0)
    {
        static TweakNameAllocator sRecycledNameAllocator;
        static TweakNameTable sRecycledNameTable(2000, &sRecycledNameAllocator);
        sRecycledNameTable.Remove(entry);
        gTweakNamePool.Free(entry);
        entry = next;
        next = entry == 0 ? 0 : entry->m_Next;
    }
    gRecycledTweakNameHead = 0;
    gRecycledTweakNameTail = 0;
}
