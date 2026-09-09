#ifndef NL_MEM_ALLOC_H
#define NL_MEM_ALLOC_H

#include "types.h"

struct FreeBlockList
{
    FreeBlockList* m_next;
    FreeBlockList* m_prev;
    u32 m_size;
};

class MemoryAllocator
{
public:
    unsigned int LargestFreeBlock();
    unsigned int TotalFreeMemory();
    void Initialize(void* memory, unsigned int size);
    void* Allocate(unsigned long size, unsigned int alignment, bool fromEnd);
    void Free(void* ptr);

    void* AllocateFromStart(unsigned long size, unsigned int alignment);
    void* AllocateFromEnd(unsigned long size, unsigned int alignment);
    void AddBlock(void* memory, unsigned int size);

    FreeBlockList* m_free_block_list;
    u32 m_04;
    u32 m_08;
    void* m_0C;
    u32 m_10;
    u32 m_14;
};

extern MemoryAllocator StandardAllocator;
extern MemoryAllocator VirtualAllocator;
extern MemoryAllocator* CurrentAllocator;
extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

#endif // NL_MEM_ALLOC_H
