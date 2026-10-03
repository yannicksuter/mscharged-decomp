#ifndef NL_MEMORY_H
#define NL_MEMORY_H

#include "types.h"

class MemoryAllocator;

inline unsigned long KB(unsigned long size)
{
    return size << 10;
}

inline unsigned long MB(unsigned long size)
{
    return KB(KB(size));
}

extern MemoryAllocator StandardAllocator;
extern MemoryAllocator VirtualAllocator;
extern MemoryAllocator* CurrentAllocator;
extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

void* operator new(unsigned long size);
void* operator new[](unsigned long size);
void operator delete(void* ptr);
void operator delete[](void* ptr);

inline void* operator new(unsigned long, void* ptr)
{
    return ptr;
}

inline void* operator new[](unsigned long, void* ptr)
{
    return ptr;
}

void* nlMalloc(unsigned long size);
void* nlMalloc(unsigned long size, unsigned int alignment, bool fromEnd);
void nlFree(void* ptr);

inline void* operator new(unsigned long size, unsigned int alignment, bool fromEnd)
{
    return nlMalloc(size, alignment, fromEnd);
}

inline void* operator new[](unsigned long size, unsigned int alignment, bool fromEnd)
{
    return nlMalloc(size, alignment, fromEnd);
}

inline void* operator new[](unsigned long size, unsigned int alignment, bool fromEnd, const char*)
{
    return nlMalloc(size, alignment, fromEnd);
}

void nlInitMemory();
void* nlAllocateAlignedMemory(unsigned long size, bool atEnd);
extern u8 gMemoryInitialized;

#endif // NL_MEMORY_H
