#include <revolution/mem/unitHeap.h>

MEMHeapHandle MEMCreateUnitHeapEx(void* start, u32 heapSize, u32 memBlockSize, int align, u16 flags) {
    MEMHeapHandle heap = (MEMHeapHandle)RoundUpPtr(start, 4);
    MEMiUntHeapHead* unitHeap;
    MEMiUntHeapMBlockHead* block;
    int i;
    u32 count;

    void* heapEnd = RoundDownPtr(AddU32ToPtr(start, heapSize), 4);
    void* heapStart;

    if (ComparePtr(heap, heapEnd) > 0) {
        return NULL;
    }

    memBlockSize = RoundUp(memBlockSize, align);

    unitHeap = (MEMiUntHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));
    heapStart = RoundUpPtr(AddU32ToPtr(unitHeap, sizeof(MEMiUntHeapHead)), align);

    if (ComparePtr(heapStart, heapEnd) > 0) {
        return NULL;
    }

    count = GetOffsetFromPtr(heapStart, heapEnd) / memBlockSize;
    if (count == 0) {
        return NULL;
    }

    heapEnd = AddU32ToPtr(heapStart, count * memBlockSize);

    MEMiInitHeapHead(heap, 'UNTH', heapStart, heapEnd, flags);

    unitHeap->freeList = (MEMiUntHeapMBlockHead*)heapStart;
    unitHeap->blockSize = memBlockSize;

    block = unitHeap->freeList;
    for (i = 0; i < count - 1; ++i, block = block->next) {
        block->next = (MEMiUntHeapMBlockHead*)AddU32ToPtr(block, memBlockSize);
    }
    block->next = NULL;

    return heap;
}

void* MEMAllocFromUnitHeap(MEMHeapHandle heap) {
    MEMiUntHeapHead* unitHeap = (MEMiUntHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));
    MEMiUntHeapMBlockHead* block;

    LockHeap(heap);

    block = unitHeap->freeList;
    if (block) {
        unitHeap->freeList = block->next;
    }

    UnlockHeap(heap);

    if (block) {
        FillAllocMemory(heap, block, unitHeap->blockSize);
    }
    return block;
}

void MEMFreeToUnitHeap(MEMHeapHandle heap, void* address) {
    MEMiUntHeapHead* unitHeap;
    MEMiUntHeapMBlockHead* block;

    if (address == NULL) {
        return;
    }

    unitHeap = (MEMiUntHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));

    LockHeap(heap);

    block = (MEMiUntHeapMBlockHead*)address;
    block->next = unitHeap->freeList;
    unitHeap->freeList = block;

    UnlockHeap(heap);
}

u32 MEMCountFreeBlockForUnitHeap(MEMHeapHandle heap) {
    MEMiUntHeapHead* unitHeap = (MEMiUntHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));
    MEMiUntHeapMBlockHead* block;
    u32 count = 0;

    LockHeap(heap);
    for (block = unitHeap->freeList; block; block = block->next) {
        count++;
    }
    UnlockHeap(heap);

    return count;
}
