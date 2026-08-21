#ifndef REVOLUTION_MEM_FRAME_HEAP_H
#define REVOLUTION_MEM_FRAME_HEAP_H

#include <revolution/mem/heapCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MEM_FRM_HEAP_FREE_TO_HEAD = (1 << 0),
    MEM_FRM_HEAP_FREE_TO_TAIL = (1 << 1),
    MEM_FRM_HEAP_FREE_ALL = MEM_FRM_HEAP_FREE_TO_HEAD | MEM_FRM_HEAP_FREE_TO_TAIL
} MEMiFrmFreeFlag;

typedef struct MEMiFrmHeapState MEMiFrmHeapState;

struct MEMiFrmHeapState {
    u32 tag;
    void* head;
    void* tail;
    MEMiFrmHeapState* prevState;
};

typedef struct MEMiFrmHeapHead {
    void* head;
    void* tail;
    MEMiFrmHeapState* state;
} MEMiFrmHeapHead;

MEMHeapHandle MEMCreateFrmHeapEx(void* start, u32 size, u16 flags);
void* MEMDestroyFrmHeap(MEMHeapHandle heap);
void* MEMAllocFromFrmHeapEx(MEMHeapHandle heap, u32 size, int align);
void MEMFreeToFrmHeap(MEMHeapHandle heap, int mode);
u32 MEMGetAllocatableSizeForFrmHeapEx(MEMHeapHandle heap, int align);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_MEM_FRAME_HEAP_H
