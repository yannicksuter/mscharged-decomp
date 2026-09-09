#ifndef REVOLUTION_OS_CACHE_FWD_H
#define REVOLUTION_OS_CACHE_FWD_H

#ifdef __cplusplus
extern "C" {
#endif

void DCInvalidateRange(const void* address, unsigned long length);
void DCFlushRange(const void* address, unsigned long length);
void DCStoreRange(const void* address, unsigned long length);
void DCFlushRangeNoSync(const void* address, unsigned long length);
void DCStoreRangeNoSync(const void* address, unsigned long length);
void DCZeroRange(const void* address, unsigned long length);
void ICInvalidateRange(const void* address, unsigned long length);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_CACHE_FWD_H
