#ifndef REVOLUTION_MEM_HEAP_COMMON_H
#define REVOLUTION_MEM_HEAP_COMMON_H

#include <mem.h>
#include <revolution/mem/list.h>
#include <revolution/os/OSMutex.h>
#include <revolution/types.h>

typedef struct MEMiHeapHead MEMiHeapHead;

struct MEMiHeapHead {
    u32 signature;
    MEMLink link;
    MEMList childList;
    void* heapStart;
    void* heapEnd;
    OSMutex mutex;
    union {
        u32 val;
        struct {
            u32 reserved : 24;
            u32 optFlag : 8;
        } fields;
    } attribute;
};

typedef MEMiHeapHead* MEMHeapHandle;
typedef u32 UIntPtr;

static inline UIntPtr GetUIntPtr(const void* ptr) {
    return (UIntPtr)ptr;
}

static inline u32 GetOffsetFromPtr(const void* start, const void* end) {
    return GetUIntPtr(end) - GetUIntPtr(start);
}

static inline void* SubU32ToPtr(void* ptr, u32 value) {
    return (void*)(GetUIntPtr(ptr) - value);
}

static inline void* AddU32ToPtr(void* ptr, u32 value) {
    return (void*)(GetUIntPtr(ptr) + value);
}

static inline void SetOptForHeap(MEMiHeapHead* heap, u16 optFlag) {
    heap->attribute.fields.optFlag = (u8)optFlag;
}

static inline u16 GetOptForHeap(const MEMiHeapHead* heap) {
    return (u16)heap->attribute.fields.optFlag;
}

static inline void FillAllocMemory(MEMiHeapHead* heap, void* address, u32 size) {
    if (GetOptForHeap(heap) & 1) {
        (void)memset(address, 0, size);
    }
}

static inline int ComparePtr(const void* a, const void* b) {
    const u8* left = (const u8*)a;
    const u8* right = (const u8*)b;
    return left - right;
}

#define RoundUp(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1))
#define RoundUpPtr(ptr, alignment) ((void*)RoundUp(GetUIntPtr(ptr), (alignment)))
#define RoundDown(value, alignment) ((value) & ~((alignment) - 1))
#define RoundDownPtr(ptr, alignment) ((void*)RoundDown(GetUIntPtr(ptr), (alignment)))

static inline void LockHeap(MEMiHeapHead* heap) {
    if (GetOptForHeap(heap) & 4) {
        OSLockMutex(&heap->mutex);
    }
}

static inline void UnlockHeap(MEMiHeapHead* heap) {
    if (GetOptForHeap(heap) & 4) {
        OSUnlockMutex(&heap->mutex);
    }
}

void MEMiInitHeapHead(MEMiHeapHead* heap, u32 signature, void* heapStart, void* heapEnd, u16 optFlag);
void MEMiFinalizeHeap(MEMiHeapHead* heap);

#endif  // REVOLUTION_MEM_HEAP_COMMON_H
