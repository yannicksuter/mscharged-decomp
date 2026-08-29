#ifndef REVOLUTION_MEM_EXP_HEAP_H
#define REVOLUTION_MEM_EXP_HEAP_H

#include <revolution/mem/heapCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEMiExpHeapMBlockHead MEMiExpHeapMBlockHead;

struct MEMiExpHeapMBlockHead {
    u16 signature;
    union {
        u16 val;
        struct {
            u16 allocDir : 1;
            u16 alignment : 7;
            u16 groupID : 8;
        } fields;
    } attribute;
    u32 blockSize;
    MEMiExpHeapMBlockHead* prev;
    MEMiExpHeapMBlockHead* next;
};

typedef struct MEMiExpMBlockList {
    MEMiExpHeapMBlockHead* head;
    MEMiExpHeapMBlockHead* tail;
} MEMiExpMBlockList;

typedef struct MEMiExpHeapHead {
    MEMiExpMBlockList mbFreeList;
    MEMiExpMBlockList mbUsedList;
    u16 groupID;
    union {
        u16 val;
        struct {
            u16 reserved : 14;
            u16 useMarginOfAlign : 1;
            u16 allocMode : 1;
        } fields;
    } feature;
} MEMiExpHeapHead;

MEMHeapHandle MEMCreateExpHeapEx(void* startAddress, u32 size, u16 optFlag);
void* MEMDestroyExpHeap(MEMHeapHandle heap);
void* MEMAllocFromExpHeapEx(MEMHeapHandle heap, u32 size, int alignment);
void MEMFreeToExpHeap(MEMHeapHandle heap, void* memBlock);
u32 MEMGetAllocatableSizeForExpHeapEx(MEMHeapHandle heap, int alignment);

static inline MEMHeapHandle MEMCreateExpHeap(void* startAddress, u32 size) {
    return MEMCreateExpHeapEx(startAddress, size, 0);
}

static inline u32 MEMGetAllocatableSizeForExpHeap(MEMHeapHandle heap) {
    return MEMGetAllocatableSizeForExpHeapEx(heap, 4);
}

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_MEM_EXP_HEAP_H
