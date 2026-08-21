// Adapted from https://github.com/SMGCommunity/Petari (CC0-1.0).
extern void OSReport(const char *, ...);
extern void* OSGetArenaLo();
extern void* OSGetArenaHi();
extern void* OSInitAlloc(void *, void *, int);
extern volatile int __OSCurrHeap;
extern const char lbl_804EC5F8[];

#define OSRoundUp32B(x) (((unsigned int)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((unsigned int)(x)) & ~(32 - 1))

static inline void InitDefaultHeap()
{
	void* arenaLo;
	void* arenaHi;

	OSReport(lbl_804EC5F8);
	OSReport(lbl_804EC5F8 + 0x36);

	arenaLo = OSGetArenaLo();
	arenaHi = OSGetArenaHi();

	arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
	OSSetArenaLo(arenaLo);

	arenaLo = (void*)OSRoundUp32B(arenaLo);
	arenaHi = (void*)OSRoundDown32B(arenaHi);

	OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
	OSSetArenaLo(arenaLo = arenaHi);
}

void __sys_alloc(unsigned int size)
{
	if (__OSCurrHeap == -1) {
		InitDefaultHeap();
	}
	OSAllocFromHeap(__OSCurrHeap, size);
}

__declspec(weak) extern void __sys_free(void* ptr)
{
	if (__OSCurrHeap == -1) {
		InitDefaultHeap();
	}
	OSFreeToHeap(__OSCurrHeap, ptr);
}
