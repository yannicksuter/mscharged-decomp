#ifndef REVOLUTION_OS_CACHE_H
#define REVOLUTION_OS_CACHE_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void DCInvalidateRange(void* address, u32 length);
void ICFlashInvalidate(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_CACHE_H
