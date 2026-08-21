#include <revolution/mem/expHeap.h>

#define FillFreeMemory(heap, address, size) ((void)0)
#define FillNoUseMemory(heap, address, size) ((void)0)

typedef struct MemRegion {
    void* start;
    void* end;
} MemRegion;

static inline void* GetMemPtrForMBlock_(MEMiExpHeapMBlockHead* block) {
    return AddU32ToPtr(block, sizeof(MEMiExpHeapMBlockHead));
}

static inline void* GetMBlockEndAddr_(MEMiExpHeapMBlockHead* block) {
    return AddU32ToPtr(GetMemPtrForMBlock_(block), block->blockSize);
}

static inline u16 GetAllocMode_(MEMiExpHeapHead* heap) {
    return heap->feature.fields.allocMode;
}

static MEMiExpHeapMBlockHead* InitMBlock_(MemRegion* region, u16 signature) {
    MEMiExpHeapMBlockHead* block = (MEMiExpHeapMBlockHead*)region->start;

    block->signature = signature;
    block->attribute.val = 0;
    block->blockSize = GetOffsetFromPtr(GetMemPtrForMBlock_(block), region->end);
    block->prev = NULL;
    block->next = NULL;

    return block;
}

static inline void SetAllocDirForMBlock_(MEMiExpHeapMBlockHead* block, u16 mode) {
    block->attribute.fields.allocDir = mode;
}

static inline u16 GetAlignmentForMBlock_(const MEMiExpHeapMBlockHead* block) {
    return block->attribute.fields.alignment;
}

static inline void SetAlignmentForMBlock_(MEMiExpHeapMBlockHead* block, u16 alignment) {
    block->attribute.fields.alignment = alignment;
}

static inline void SetGroupIDForMBlock_(MEMiExpHeapMBlockHead* block, u16 id) {
    block->attribute.fields.groupID = (u8)id;
}

static void GetRegionOfMBlock_(MemRegion* region, MEMiExpHeapMBlockHead* block) {
    region->start = SubU32ToPtr(block, GetAlignmentForMBlock_(block));
    region->end = GetMBlockEndAddr_(block);
}

static inline MEMiHeapHead* GetHeapHeadPtrFromExpHeapHead_(MEMiExpHeapHead* expHeap) {
    return (MEMiHeapHead*)SubU32ToPtr(expHeap, sizeof(MEMiHeapHead));
}

static MEMiExpHeapMBlockHead* InsertMBlock_(MEMiExpMBlockList* list, MEMiExpHeapMBlockHead* target,
                                            MEMiExpHeapMBlockHead* prev) {
    MEMiExpHeapMBlockHead* next;

    target->prev = prev;
    if (prev) {
        next = prev->next;
        prev->next = target;
    } else {
        next = list->head;
        list->head = target;
    }

    target->next = next;
    if (next) {
        next->prev = target;
    } else {
        list->tail = target;
    }

    return target;
}

static inline void AppendMBlock_(MEMiExpMBlockList* list, MEMiExpHeapMBlockHead* block) {
    (void)InsertMBlock_(list, block, list->tail);
}

static inline MEMiExpHeapHead* GetExpHeapHeadPtrFromHeapHead_(MEMiHeapHead* heap) {
    return (MEMiExpHeapHead*)AddU32ToPtr(heap, sizeof(MEMiHeapHead));
}

static MEMiExpHeapMBlockHead* RemoveMBlock_(MEMiExpMBlockList* list, MEMiExpHeapMBlockHead* block) {
    MEMiExpHeapMBlockHead* const prev = block->prev;
    MEMiExpHeapMBlockHead* const next = block->next;

    if (prev) {
        prev->next = next;
    } else {
        list->head = next;
    }

    if (next) {
        next->prev = prev;
    } else {
        list->tail = prev;
    }

    return prev;
}

static inline MEMiExpHeapMBlockHead* InitFreeMBlock_(MemRegion* region) {
    return InitMBlock_(region, 'FR');
}

static inline void SetAllocMode_(MEMiExpHeapHead* heap, u16 mode) {
    heap->feature.fields.allocMode = mode;
}

static MEMiHeapHead* InitExpHeap_(void* startAddress, void* endAddress, u16 optFlag) {
    MEMiHeapHead* heap = (MEMiHeapHead*)startAddress;
    MEMiExpHeapHead* expHeap = GetExpHeapHeadPtrFromHeapHead_(heap);

    MEMiInitHeapHead(heap, 'EXPH', AddU32ToPtr(expHeap, sizeof(MEMiExpHeapHead)), endAddress, optFlag);

    expHeap->groupID = 0;
    expHeap->feature.val = 0;
    SetAllocMode_(expHeap, 0);

    {
        MEMiExpHeapMBlockHead* block;
        MemRegion region;

        region.start = heap->heapStart;
        region.end = heap->heapEnd;
        block = InitFreeMBlock_(&region);
        expHeap->mbFreeList.head = block;
        expHeap->mbFreeList.tail = block;
        expHeap->mbUsedList.head = NULL;
        expHeap->mbUsedList.tail = NULL;

        return heap;
    }
}

static void* AllocUsedBlockFromFreeBlock_(MEMiExpHeapHead* pEHHead, MEMiExpHeapMBlockHead* pMBHeadFree, void* mblock, u32 size, u16 direction) {
    MemRegion freeRgnT;
    MemRegion freeRgnB;
    MEMiExpHeapMBlockHead* pMBHeadFreePrev;

    GetRegionOfMBlock_(&freeRgnT, pMBHeadFree);
    freeRgnB.end = freeRgnT.end;
    freeRgnB.start = AddU32ToPtr(mblock, size);
    freeRgnT.end = SubU32ToPtr(mblock, sizeof(MEMiExpHeapMBlockHead));

    pMBHeadFreePrev = RemoveMBlock_(&pEHHead->mbFreeList, pMBHeadFree);

    if (GetOffsetFromPtr(freeRgnT.start, freeRgnT.end) < sizeof(MEMiExpHeapMBlockHead) + 4) {
        freeRgnT.end = freeRgnT.start;
    } else {
        pMBHeadFreePrev = InsertMBlock_(&pEHHead->mbFreeList, InitFreeMBlock_(&freeRgnT), pMBHeadFreePrev);
    }

    if (GetOffsetFromPtr(freeRgnB.start, freeRgnB.end) < sizeof(MEMiExpHeapMBlockHead) + 4) {
        freeRgnB.start = freeRgnB.end;
    } else {
        (void)InsertMBlock_(&pEHHead->mbFreeList, InitFreeMBlock_(&freeRgnB), pMBHeadFreePrev);
    }

    FillAllocMemory(GetHeapHeadPtrFromExpHeapHead_(pEHHead), freeRgnT.end, GetOffsetFromPtr(freeRgnT.end, freeRgnB.start));

    {
        MEMiExpHeapMBlockHead* pMBHeadNewUsed;
        MemRegion region;

        region.start = SubU32ToPtr(mblock, sizeof(MEMiExpHeapMBlockHead));
        region.end = freeRgnB.start;

        pMBHeadNewUsed = InitMBlock_(&region, 'UD');
        SetAllocDirForMBlock_(pMBHeadNewUsed, direction);
        SetAlignmentForMBlock_(pMBHeadNewUsed, (u16)GetOffsetFromPtr(freeRgnT.end, pMBHeadNewUsed));
        SetGroupIDForMBlock_(pMBHeadNewUsed, pEHHead->groupID);
        AppendMBlock_(&pEHHead->mbUsedList, pMBHeadNewUsed);
    }

    return mblock;
}

static void* AllocFromHead_(MEMiHeapHead* heap, u32 size, int alignment) {
    MEMiExpHeapHead* expHeap = GetExpHeapHeadPtrFromHeapHead_(heap);
    const BOOL allocFirst = GetAllocMode_(expHeap) == 0;
    MEMiExpHeapMBlockHead* block = NULL;
    MEMiExpHeapMBlockHead* foundBlock = NULL;
    u32 foundSize = 0xFFFFFFFF;
    void* foundMemBlock = NULL;

    for (block = expHeap->mbFreeList.head; block; block = block->next) {
        void* const memBlock = GetMemPtrForMBlock_(block);
        void* const requestedMemBlock = RoundUpPtr(memBlock, alignment);
        const u32 offset = GetOffsetFromPtr(memBlock, requestedMemBlock);

        if (block->blockSize >= size + offset && foundSize > block->blockSize) {
            foundBlock = block;
            foundSize = block->blockSize;
            foundMemBlock = requestedMemBlock;

            if (allocFirst || foundSize == size) {
                break;
            }
        }
    }

    if (!foundBlock) {
        return NULL;
    }

    return AllocUsedBlockFromFreeBlock_(expHeap, foundBlock, foundMemBlock, size, 0);
}

static void* AllocFromTail_(MEMiHeapHead* heap, u32 size, int alignment) {
    MEMiExpHeapHead* expHeap = GetExpHeapHeadPtrFromHeapHead_(heap);
    const BOOL allocFirst = GetAllocMode_(expHeap) == 0;
    MEMiExpHeapMBlockHead* block = NULL;
    MEMiExpHeapMBlockHead* foundBlock = NULL;
    u32 foundSize = 0xFFFFFFFF;
    void* foundMemBlock = NULL;

    for (block = expHeap->mbFreeList.tail; block; block = block->prev) {
        void* const memBlock = GetMemPtrForMBlock_(block);
        void* const memBlockEnd = AddU32ToPtr(memBlock, block->blockSize);
        void* const requestedMemBlock = RoundDownPtr(SubU32ToPtr(memBlockEnd, size), alignment);

        if (ComparePtr(requestedMemBlock, memBlock) >= 0 && foundSize > block->blockSize) {
            foundBlock = block;
            foundSize = block->blockSize;
            foundMemBlock = requestedMemBlock;

            if (allocFirst || foundSize == size) {
                break;
            }
        }
    }

    if (!foundBlock) {
        return NULL;
    }

    return AllocUsedBlockFromFreeBlock_(expHeap, foundBlock, foundMemBlock, size, 1);
}

static BOOL RecycleRegion_(MEMiExpHeapHead* expHeap, const MemRegion* region) {
    MEMiExpHeapMBlockHead* previousFreeBlock = NULL;
    MemRegion freeRegion = *region;

    {
        MEMiExpHeapMBlockHead* block;

        for (block = expHeap->mbFreeList.head; block; block = block->next) {
            if (block < region->start) {
                previousFreeBlock = block;
                continue;
            }

            if (block == region->end) {
                freeRegion.end = GetMBlockEndAddr_(block);
                (void)RemoveMBlock_(&expHeap->mbFreeList, block);
                FillNoUseMemory(GetHeapHeadPtrFromExpHeapHead_(expHeap), block, sizeof(MEMiExpHeapMBlockHead));
            }
            break;
        }
    }

    if (previousFreeBlock && GetMBlockEndAddr_(previousFreeBlock) == region->start) {
        freeRegion.start = previousFreeBlock;
        previousFreeBlock = RemoveMBlock_(&expHeap->mbFreeList, previousFreeBlock);
    }

    if (GetOffsetFromPtr(freeRegion.start, freeRegion.end) < sizeof(MEMiExpHeapMBlockHead)) {
        return FALSE;
    }

    FillFreeMemory(GetHeapHeadPtrFromExpHeapHead_(expHeap), region->start,
                   GetOffsetFromPtr(region->start, region->end));
    (void)InsertMBlock_(&expHeap->mbFreeList, InitFreeMBlock_(&freeRegion), previousFreeBlock);

    return TRUE;
}

MEMHeapHandle MEMCreateExpHeapEx(void* startAddress, u32 size, u16 optFlag) {
    void* endAddress;

    endAddress = RoundDownPtr(AddU32ToPtr(startAddress, size), 4);
    startAddress = RoundUpPtr(startAddress, 4);

    if (GetUIntPtr(startAddress) > GetUIntPtr(endAddress) ||
        GetOffsetFromPtr(startAddress, endAddress) <
            sizeof(MEMiHeapHead) + sizeof(MEMiExpHeapHead) + sizeof(MEMiExpHeapMBlockHead) + 4) {
        return NULL;
    }

    {
        MEMiHeapHead* heap = InitExpHeap_(startAddress, endAddress, optFlag);
        return heap;
    }
}

void* MEMDestroyExpHeap(MEMHeapHandle heap) {
    MEMiFinalizeHeap(heap);
    return (void*)heap;
}

void* MEMAllocFromExpHeapEx(MEMHeapHandle heap, u32 size, int alignment) {
    void* memory = NULL;

    if (size == 0) {
        size = 1;
    }

    size = RoundUp(size, 4);
    LockHeap(heap);

    if (alignment >= 0) {
        memory = AllocFromHead_(heap, size, alignment);
    } else {
        memory = AllocFromTail_(heap, size, -alignment);
    }

    UnlockHeap(heap);
    return memory;
}

static inline MEMiExpHeapMBlockHead* GetMBlockHeadPtr_(void* memBlock) {
    return (MEMiExpHeapMBlockHead*)SubU32ToPtr(memBlock, sizeof(MEMiExpHeapMBlockHead));
}

static inline MEMiExpHeapHead* GetExpHeapHeadPtrFromHandle_(MEMHeapHandle heap) {
    return GetExpHeapHeadPtrFromHeapHead_(heap);
}

void MEMFreeToExpHeap(MEMHeapHandle heap, void* memBlock) {
    MEMiHeapHead* pHeapHd;
    MEMiExpHeapHead* pExpHeapHd;
    MEMiExpHeapMBlockHead* pMBHead;
    MemRegion region;

    if (memBlock == NULL) {
        return;
    }

    pHeapHd = heap;
    pExpHeapHd = GetExpHeapHeadPtrFromHandle_(pHeapHd);
    pMBHead = GetMBlockHeadPtr_(memBlock);

    LockHeap(heap);

    GetRegionOfMBlock_(&region, pMBHead);
    (void)RemoveMBlock_(&pExpHeapHd->mbUsedList, pMBHead);
    (void)RecycleRegion_(pExpHeapHd, &region);

    UnlockHeap(heap);
}

u32 MEMGetAllocatableSizeForExpHeapEx(MEMHeapHandle heap, int alignment) {
    alignment = __abs(alignment);
    LockHeap(heap);

    {
        MEMiExpHeapHead* expHeap = GetExpHeapHeadPtrFromHandle_(heap);
        MEMiExpHeapMBlockHead* block;
        u32 maxSize = 0;
        u32 minOffset = 0xFFFFFFFF;

        for (block = expHeap->mbFreeList.head; block; block = block->next) {
            void* baseAddress = RoundUpPtr(GetMemPtrForMBlock_(block), alignment);

            if (GetUIntPtr(baseAddress) < GetUIntPtr(GetMBlockEndAddr_(block))) {
                const u32 blockSize = GetOffsetFromPtr(baseAddress, GetMBlockEndAddr_(block));
                const u32 offset = GetOffsetFromPtr(GetMemPtrForMBlock_(block), baseAddress);
                if (maxSize < blockSize || (maxSize == blockSize && minOffset > offset)) {
                    maxSize = blockSize;
                    minOffset = offset;
                }
            }
        }

        UnlockHeap(heap);
        return maxSize;
    }
}
