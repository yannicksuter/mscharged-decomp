#ifndef REVOLUTION_OS_CACHE_H
#define REVOLUTION_OS_CACHE_H

#include <revolution/os/OSCache_fwd.h>
#include <revolution/types.h>

typedef struct OSContext OSContext;

#ifdef __cplusplus
extern "C" {
#endif

void DCEnable(void);
void ICFlashInvalidate(void);
void ICEnable(void);
void LCEnable(void);
void LCDisable(void);
void LCLoadBlocks(void* destination, const void* source, u32 blocks);
void LCStoreBlocks(void* destination, const void* source, u32 blocks);
u32 LCStoreData(void* destination, const void* source, u32 length);
u32 LCQueueLength(void);
void LCQueueWait(u32 count);
void L2Enable(void);
void L2Disable(void);
void L2GlobalInvalidate(void);
void DMAErrorHandler(u8 error, OSContext* context, u32 dsisr, u32 dar, ...);
void __OSCacheInit(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_CACHE_H
