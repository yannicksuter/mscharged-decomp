#ifndef REVOLUTION_MEM_UNIT_HEAP_H
#define REVOLUTION_MEM_UNIT_HEAP_H

#include <revolution/mem/heapCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEMiUntHeapMBlockHead MEMiUntHeapMBlockHead;

struct MEMiUntHeapMBlockHead {
    MEMiUntHeapMBlockHead* next;
};

typedef struct MEMiUntHeapHead {
    MEMiUntHeapMBlockHead* freeList;
    u32 blockSize;
} MEMiUntHeapHead;

MEMHeapHandle MEMCreateUnitHeapEx(void* start, u32 heapSize, u32 memBlockSize, int align, u16 flags);
void* MEMAllocFromUnitHeap(MEMHeapHandle heap);
void MEMFreeToUnitHeap(MEMHeapHandle heap, void* address);
u32 MEMCountFreeBlockForUnitHeap(MEMHeapHandle heap);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_MEM_UNIT_HEAP_H
