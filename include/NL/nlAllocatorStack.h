#ifndef NL_NLALLOCATORSTACK_H
#define NL_NLALLOCATORSTACK_H

class MemoryAllocator;

void PushMemoryAllocator(MemoryAllocator* allocator);
MemoryAllocator* PopMemoryAllocator();

#endif // NL_NLALLOCATORSTACK_H
