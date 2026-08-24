#ifndef REVOLUTION_OS_H
#define REVOLUTION_OS_H

#include <revolution/types.h>
#include <revolution/os/OSMemMap.h>

#define OS_CACHED_REGION_PREFIX 0x8000
#define OS_BASE_CACHED (OS_CACHED_REGION_PREFIX << 16)

#define OSPhysicalToCached(paddr) ((void*)((u32)(paddr) + OS_BASE_CACHED))

#define OSRoundUp32B(x) ROUNDUP((unsigned long)(x), 32)
#define OSRoundDown32B(x) ROUNDDOWN((unsigned long)(x), 32)

#include <revolution/os/OSAlloc.h>
#include <revolution/os/OSArena.h>
#include <revolution/os/OSCache.h>
#include <revolution/os/OSContext.h>
#include <revolution/os/OSError.h>
#include <revolution/os/OSHardware.h>
#include <revolution/os/OSInterrupt.h>
#include <revolution/os/OSLink.h>
#include <revolution/os/OSMessage.h>
#include <revolution/os/OSMutex.h>
#include <revolution/os/OSReboot.h>
#include <revolution/os/OSReset.h>
#include <revolution/os/OSRtc.h>
#include <revolution/os/OSStateTM.h>
#include <revolution/os/OSThread.h>

#endif  // REVOLUTION_OS_H
