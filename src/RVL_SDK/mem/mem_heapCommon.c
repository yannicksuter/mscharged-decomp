#include <revolution/mem/expHeap.h>
#include <revolution/mem/heapCommon.h>
#include <revolution/mem/list.h>

static MEMList sRootList;
static BOOL sRootListInitialized = FALSE;
static OSMutex sRootMutex;

static MEMiHeapHead* FindContainHeap_(MEMList* list, const void* memBlock) {
    MEMiHeapHead* heap = NULL;

    while (NULL != (heap = (MEMiHeapHead*)MEMGetNextListObject(list, heap))) {
        if (GetUIntPtr(heap->heapStart) <= GetUIntPtr(memBlock) && GetUIntPtr(memBlock) < GetUIntPtr(heap->heapEnd)) {
            MEMiHeapHead* childHeap = FindContainHeap_(&heap->childList, memBlock);
            if (childHeap) {
                return childHeap;
            }
            return heap;
        }
    }
    return NULL;
}

static MEMList* FindListContainHeap_(const MEMiHeapHead* heap) {
    MEMList* list = &sRootList;
    MEMiHeapHead* containingHeap = FindContainHeap_(&sRootList, heap);

    if (containingHeap) {
        list = &containingHeap->childList;
    }

    return list;
}

#define FillNoUseMemory(heap, address, size) ((void)0)

void MEMiInitHeapHead(MEMiHeapHead* heap, u32 signature, void* heapStart, void* heapEnd, u16 optFlag) {
    heap->signature = signature;
    heap->heapStart = heapStart;
    heap->heapEnd = heapEnd;
    heap->attribute.val = 0;

    SetOptForHeap(heap, optFlag);

    FillNoUseMemory(heap, heapStart, GetOffsetFromPtr(heapStart, heapEnd));
    MEM_INIT_LIST(&heap->childList, MEMiHeapHead, link);

    if (!sRootListInitialized) {
        MEM_INIT_LIST(&sRootList, MEMiHeapHead, link);
        OSInitMutex(&sRootMutex);
        sRootListInitialized = TRUE;
    }

    OSInitMutex(&heap->mutex);
    OSLockMutex(&sRootMutex);
    MEMAppendListObject(FindListContainHeap_(heap), heap);
    OSUnlockMutex(&sRootMutex);
}

void MEMiFinalizeHeap(MEMiHeapHead* heap) {
    MEMList* list;

    OSLockMutex(&sRootMutex);
    list = FindListContainHeap_(heap);
    MEMRemoveListObject(list, heap);
    OSUnlockMutex(&sRootMutex);
    heap->signature = 0;
}
