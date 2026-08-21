#ifndef REVOLUTION_MEM_ALLOCATOR_H
#define REVOLUTION_MEM_ALLOCATOR_H

#include <revolution/mem/heapCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEMAllocator MEMAllocator;

typedef void* (*MEMAllocatorAllocFunc)(MEMAllocator*, u32);
typedef void (*MEMAllocatorFreeFunc)(MEMAllocator*, void*);

typedef struct MEMAllocatorFunc {
    MEMAllocatorAllocFunc allocFunc;
    MEMAllocatorFreeFunc freeFunc;
} MEMAllocatorFunc;

struct MEMAllocator {
    const MEMAllocatorFunc* func;
    MEMHeapHandle heap;
    u32 heapParam1;
    u32 heapParam2;
};

void* MEMAllocFromAllocator(MEMAllocator* allocator, u32 size);
void MEMFreeToAllocator(MEMAllocator* allocator, void* block);
void MEMInitAllocatorForExpHeap(MEMAllocator* allocator, MEMHeapHandle heap, s32 align);
void MEMInitAllocatorForFrmHeap(MEMAllocator* allocator, MEMHeapHandle heap, s32 align);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_MEM_ALLOCATOR_H
