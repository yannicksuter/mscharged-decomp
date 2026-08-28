#ifndef REVOLUTION_OS_H
#define REVOLUTION_OS_H

#include <revolution/types.h>
#include <revolution/os/OSMemMap.h>

#define OS_CACHED_REGION_PREFIX 0x8000
#define OS_BASE_CACHED (OS_CACHED_REGION_PREFIX << 16)

#define OSPhysicalToCached(paddr) ((void*)((u32)(paddr) + OS_BASE_CACHED))
#define OSCachedToPhysical(caddr) ((void*)((u32)(caddr)-OS_BASE_CACHED))
#define OSPhysicalToUncached(paddr) ((void*)((u32)(paddr) + 0xC0000000))

#define OSRoundUp32B(x) ROUNDUP((unsigned long)(x), 32)
#define OSRoundDown32B(x) ROUNDDOWN((unsigned long)(x), 32)

#ifdef DEBUG
#define ASSERTLINE(x, line) \
    (void)((x) || (OSPanic(__FILE__, line, "Failed assertion " #x), 0))
#define ASSERTMSGLINE(x, line, ...) \
    (void)((x) || (OSPanic(__FILE__, line, __VA_ARGS__), 0))
#else
#define ASSERTLINE(x, line) ((void)0)
#define ASSERTMSGLINE(x, line, ...) ((void)0)
#endif

#include <revolution/os/OS.h>
#include <revolution/os/__ppc_eabi_init.h>
#include <revolution/os/OSAlloc.h>
#include <revolution/os/OSAlarm.h>
#include <revolution/os/OSArena.h>
#include <revolution/os/OSAudioSystem.h>
#include <revolution/os/OSCache.h>
#include <revolution/os/OSContext.h>
#include <revolution/os/OSError.h>
#include <revolution/os/OSExec.h>
#include <revolution/os/OSFastCast.h>
#include <revolution/os/OSFatal.h>
#include <revolution/os/OSFont.h>
#include <revolution/os/OSHardware.h>
#include <revolution/os/OSInterrupt.h>
#include <revolution/os/OSIpc.h>
#include <revolution/os/OSLink.h>
#include <revolution/os/OSMessage.h>
#include <revolution/os/OSMemory.h>
#include <revolution/os/OSMutex.h>
#include <revolution/os/OSPlayRecord.h>
#include <revolution/os/OSReboot.h>
#include <revolution/os/OSReset.h>
#include <revolution/os/OSRtc.h>
#include <revolution/os/OSStateFlags.h>
#include <revolution/os/OSStateTM.h>
#include <revolution/os/OSThread.h>
#include <revolution/os/OSTime.h>
#include <revolution/os/OSUtf.h>

#endif  // REVOLUTION_OS_H
