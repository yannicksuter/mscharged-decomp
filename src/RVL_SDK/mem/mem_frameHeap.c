#include <revolution/mem/frameHeap.h>
#include <revolution/os/OSInterrupt.h>

static MEMHeapHandle InitFrameHeap_(void* start, void* end, u16 flags) {
    MEMHeapHandle heap = (MEMHeapHandle)start;
    MEMiFrmHeapHead* frmHeap = (MEMiFrmHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));

    MEMiInitHeapHead(heap, 'FRMH', AddU32ToPtr(frmHeap, sizeof(MEMiFrmHeapHead)), end, flags);

    frmHeap->head = heap->heapStart;
    frmHeap->tail = heap->heapEnd;
    frmHeap->state = NULL;
    return heap;
}

MEMHeapHandle MEMCreateFrmHeapEx(void* start, u32 size, u16 flags) {
    void* end = RoundDownPtr(AddU32ToPtr(start, size), 4);
    start = RoundUpPtr(start, 4);

    if (GetUIntPtr(start) > GetUIntPtr(end) ||
        GetOffsetFromPtr(start, end) < sizeof(MEMiHeapHead) + sizeof(MEMiFrmHeapHead)) {
        return NULL;
    }

    return InitFrameHeap_(start, end, flags);
}

void* MEMDestroyFrmHeap(MEMHeapHandle heap) {
    MEMiFinalizeHeap(heap);
    return (void*)heap;
}

static void* AllocFromHead_(MEMiFrmHeapHead* frmHeap, u32 size, int align) {
    void* newBlock = RoundUpPtr(frmHeap->head, align);
    void* endAddress = AddU32ToPtr(newBlock, size);
    MEMHeapHandle heap;

    if (GetUIntPtr(endAddress) > GetUIntPtr(frmHeap->tail)) {
        return NULL;
    }

    heap = (MEMHeapHandle)SubU32ToPtr(frmHeap, sizeof(MEMiHeapHead));
    FillAllocMemory(heap, frmHeap->head, GetOffsetFromPtr(frmHeap->head, endAddress));
    frmHeap->head = endAddress;
    return newBlock;
}

static void* AllocFromTail_(MEMiFrmHeapHead* frmHeap, u32 size, int align) {
    void* newBlock = RoundDownPtr(SubU32ToPtr(frmHeap->tail, size), align);
    MEMHeapHandle heap;

    if (GetUIntPtr(newBlock) < GetUIntPtr(frmHeap->head)) {
        return NULL;
    }

    heap = (MEMHeapHandle)SubU32ToPtr(frmHeap, sizeof(MEMiHeapHead));
    FillAllocMemory(heap, newBlock, GetOffsetFromPtr(newBlock, frmHeap->tail));
    frmHeap->tail = newBlock;
    return newBlock;
}

void* MEMAllocFromFrmHeapEx(MEMHeapHandle heap, u32 size, int align) {
    void* memory = NULL;
    MEMiFrmHeapHead* frmHeap = (MEMiFrmHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));

    if (size == 0) {
        size = 1;
    }
    size = RoundUp(size, 4);

    LockHeap(heap);

    if (align >= 0) {
        memory = AllocFromHead_(frmHeap, size, align);
    } else {
        memory = AllocFromTail_(frmHeap, size, -align);
    }

    UnlockHeap(heap);

    return memory;
}

void MEMFreeToFrmHeap(MEMHeapHandle heap, int mode) {
    LockHeap(heap);

    if (mode & MEM_FRM_HEAP_FREE_TO_HEAD) {
        MEMiFrmHeapHead* frmHeap = (MEMiFrmHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));
        frmHeap->head = heap->heapStart;
        frmHeap->state = NULL;
    }
    if (mode & MEM_FRM_HEAP_FREE_TO_TAIL) {
        MEMiFrmHeapHead* frmHeap = (MEMiFrmHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));
        MEMiFrmHeapState* state;
        for (state = frmHeap->state; state; state = state->prevState) {
            state->tail = heap->heapEnd;
        }
        frmHeap->tail = heap->heapEnd;
    }

    UnlockHeap(heap);
}

u32 MEMGetAllocatableSizeForFrmHeapEx(MEMHeapHandle heap, int align) {
    BOOL enabled;
    u32 size;
    MEMiFrmHeapHead* frmHeap;
    void* block;

    align = __abs(align);
    enabled = OSDisableInterrupts();
    frmHeap = (MEMiFrmHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));
    block = RoundUpPtr(frmHeap->head, align);

    if (GetUIntPtr(block) > GetUIntPtr(frmHeap->tail)) {
        size = 0;
    } else {
        size = GetOffsetFromPtr(block, frmHeap->tail);
    }

    OSRestoreInterrupts(enabled);

    return size;
}
