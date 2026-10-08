// Adapted from https://github.com/SMGCommunity/Petari (CC0-1.0).
#include "GCN_mem_alloc.h"
#include <revolution/os/OSAlloc.h>
#include <revolution/os/OSArena.h>
#include <revolution/os/OSError.h>

#define OSRoundUp32B(x) (((unsigned int)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((unsigned int)(x)) & ~(32 - 1))

static inline void InitDefaultHeap()
{
	void* arenaLo;
	void* arenaHi;

	OSReport("GCN_Mem_Alloc.c : InitDefaultHeap. No Heap Available\n");
	OSReport("Metrowerks CW runtime library initializing default heap\n");

	arenaLo = OSGetArenaLo();
	arenaHi = OSGetArenaHi();

	arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
	OSSetArenaLo(arenaLo);

	arenaLo = (void*)OSRoundUp32B(arenaLo);
	arenaHi = (void*)OSRoundDown32B(arenaHi);

	OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
	OSSetArenaLo(arenaLo = arenaHi);
}

void* __sys_alloc(unsigned int size)
{
	if (__OSCurrHeap == -1) {
		InitDefaultHeap();
	}
	return OSAllocFromHeap(__OSCurrHeap, size);
}

__declspec(weak) extern void __sys_free(void* ptr)
{
	if (__OSCurrHeap == -1) {
		InitDefaultHeap();
	}
	OSFreeToHeap(__OSCurrHeap, ptr);
}
