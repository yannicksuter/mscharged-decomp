#ifndef NL_MEM_ALLOC_H
#define NL_MEM_ALLOC_H

#include "types.h"

class MemoryAllocator
{
public:
    void* Allocate(unsigned long size, unsigned int alignment, bool fromEnd);
    void Free(void* ptr);

private:
    u8 mData[0x18];
};

extern MemoryAllocator StandardAllocator;
extern MemoryAllocator VirtualAllocator;
extern MemoryAllocator* CurrentAllocator;

#endif // NL_MEM_ALLOC_H
