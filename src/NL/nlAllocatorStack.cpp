#include "NL/MemAlloc.h"

#include "NL/nlAllocatorStack.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"


void PushMemoryAllocator(MemoryAllocator* allocator)
{
    CurrentAllocator = allocator;
    AllocatorStack[AllocatorStackDepth++] = allocator;
}

MemoryAllocator* PopMemoryAllocator()
{
    MemoryAllocator* allocator = AllocatorStack[--AllocatorStackDepth];
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return allocator;
}
