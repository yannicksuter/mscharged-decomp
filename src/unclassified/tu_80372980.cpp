#include <revolution/dvd/dvd.h>
#include <revolution/os/OS.h>
#include <revolution/os/OSAlloc.h>
#include <revolution/os/OSArena.h>
#include <revolution/os/OSError.h>
#include <revolution/os/OSMemory.h>
#include <revolution/gx/GXTypes.h>
#include <revolution/vi/vi.h>
#include <string.h>

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"

void nlInitMemory()
{
    u32 simulatedSize = OSGetConsoleSimulatedMem2Size();
    u32 reserveSize = 0x2000;
    if (simulatedSize == 0x8000000)
    {
        reserveSize = 0x4000000;
    }

    if (gMemoryInitialized == 0)
    {
        gMemoryInitialized = 1;
        OSInit();
        DVDInit();
        VIInit();

        int i = 0;
        for (;;)
        {
            void* arenaLo = i != 0 ? OSGetMEM1ArenaLo() : OSGetMEM2ArenaLo();
            void* arenaHi = i != 0 ? OSGetMEM1ArenaHi() : OSGetMEM2ArenaHi();

            u32 heapSize = (u32)arenaHi - (u32)arenaLo - (i == 0 ? reserveSize : 0x400);
            void* ptr = (i != 0 ? OSAllocFromMEM1ArenaLo : OSAllocFromMEM2ArenaLo)(heapSize, 32);

            if (i == 0)
            {
                OSSetCurrentHeap(OSCreateHeap(OSInitAlloc(OSGetMEM2ArenaLo(), OSGetMEM2ArenaHi(), 1), OSGetMEM2ArenaHi()));
            }

            memset(ptr, 0xCD, heapSize);

            MemoryAllocator* allocator = i != 0 ? &StandardAllocator : &VirtualAllocator;
            allocator->Initialize(ptr, heapSize);
            OSReport("nlInitMemory - %s\n", i != 0 ? "Internal" : "External");
            OSReport("Free Memory: %u\n", allocator->TotalFreeMemory());
            OSReport("Largest Free Block: %u\n", allocator->LargestFreeBlock());

            if (i != 0)
            {
                break;
            }
            i = 1;
        }
    }
}
