#ifndef REVOLUTION_OS_MEMORY_H
#define REVOLUTION_OS_MEMORY_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_MEM_MB_TO_B(megabytes) ((megabytes) * 1024 * 1024)

u32 OSGetPhysicalMem1Size(void);
u32 OSGetPhysicalMem2Size(void);
u32 OSGetConsoleSimulatedMem1Size(void);
u32 OSGetConsoleSimulatedMem2Size(void);
void __OSInitMemoryProtection(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_MEMORY_H
