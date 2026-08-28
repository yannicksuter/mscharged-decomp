#include <dwc/dwc_memfunc.h>
#include <dwc/dwci_memfunc.h>

#include <string.h>

static DWCAllocEx s_alloc;
static DWCFreeEx s_free;

DWCi_AllocateHeader* DWCi_GetAllocateHeader(void* block) {
  return (DWCi_AllocateHeader*)((u32)block - sizeof(DWCi_AllocateHeader));
}

void* DWCi_SetAllocateHeader(DWCi_AllocateHeader* blockHeader, u32 size) {
  blockHeader->magic = DWCi_MEMFUNC_SIGNATURE;
  blockHeader->size = size;
  return (void*)&blockHeader[1];
}

u32 DWCi_GetAllocateSize(void* block) {
  DWCi_AllocateHeader* pHeader = DWCi_GetAllocateHeader(block);
  return pHeader->size;
}

void DWCi_SetMemFunc(DWCAllocEx allocator, DWCFreeEx freer) {
  s_alloc = allocator;
  s_free = freer;
}

void* DWC_Alloc(DWCAllocType type, u32 size) {
  return DWC_AllocEx(type, size, 32);
}

void* DWC_AllocEx(DWCAllocType type, u32 size, int align) {
  void* block;

  block = s_alloc(type, size + sizeof(DWCi_AllocateHeader), align);
  return DWCi_SetAllocateHeader(block, size);
}

void DWC_Free(DWCAllocType name, void* block, u32 size) {
  if (block) {
    block = (void*)DWCi_GetAllocateHeader(block);
    s_free(name, block, size);
  }
}

void* DWC_Realloc(DWCAllocType type, void* block, u32 before, u32 after) {
  return DWC_ReallocEx(type, block, before, after, 32);
}

void* DWC_ReallocEx(DWCAllocType type, void* block, u32 before, u32 after, int align) {
  u32 allocSize;
  void* newBlock = DWC_AllocEx(type, after, align);

  if (newBlock == NULL)
    return 0;

  if (block) {
    allocSize = DWCi_GetAllocateSize(block);
    memcpy(newBlock, block, allocSize);
    DWC_Free(type, block, before);
  }
  return newBlock;
}

void* DWCi_GsMalloc(u32 size) {
  return DWC_Alloc(DWC_ALLOCTYPE_GS, size);
}

void* DWCi_GsRealloc(void* block, u32 size) {
  return DWC_Realloc(DWC_ALLOCTYPE_GS, block, size, size);
}

void DWCi_GsFree(void* block) {
  DWC_Free(DWC_ALLOCTYPE_GS, block, 0);
}

void* DWCi_GsMemalign(u32 align, u32 size) {
  return DWC_AllocEx(DWC_ALLOCTYPE_GS, size, align);
}
