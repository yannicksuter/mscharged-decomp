#include "NL/nlMemory.h"

#include "NL/MemAlloc.h"

static inline void FreeMemory(void* ptr)
{
    if (CurrentAllocator != &StandardAllocator && CurrentAllocator != &VirtualAllocator)
    {
        CurrentAllocator->Free(ptr);
        return;
    }

    s8 memoryRegion = (u32)ptr >> 28;
    if (memoryRegion == 8)
    {
        StandardAllocator.Free(ptr);
    }
    else if (memoryRegion == 9)
    {
        VirtualAllocator.Free(ptr);
    }
}

static inline void* AllocateMemory(unsigned long size, unsigned int alignment, bool fromEnd)
{
    if (gMemoryInitialized == 0)
    {
        nlInitMemory();
    }
    return CurrentAllocator->Allocate(size, alignment, fromEnd);
}

void operator delete(void* ptr)
{
    FreeMemory(ptr);
}

void operator delete[](void* ptr)
{
    FreeMemory(ptr);
}

void* operator new(unsigned long size)
{
    return AllocateMemory(size, 8, false);
}

void* operator new[](unsigned long size)
{
    return AllocateMemory(size, 8, false);
}

void* nlMalloc(unsigned long size)
{
    return AllocateMemory(size, 8, false);
}

void* nlMalloc(unsigned long size, unsigned int alignment, bool fromEnd)
{
    return AllocateMemory(size, alignment, fromEnd);
}

void nlFree(void* ptr)
{
    FreeMemory(ptr);
}
