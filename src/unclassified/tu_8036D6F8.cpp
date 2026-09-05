#include "NL/MemAlloc.h"

#include "unclassified/tu_8036D6F8.h"

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

void fn_8036D6F8(MemoryAllocator* allocator)
{
    CurrentAllocator = allocator;
    AllocatorStack[AllocatorStackDepth++] = allocator;
}

MemoryAllocator* fn_8036D71C()
{
    MemoryAllocator* allocator = AllocatorStack[--AllocatorStackDepth];
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return allocator;
}
