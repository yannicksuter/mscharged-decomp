#include <revolution/base.h>
#include <revolution/db.h>
#include <revolution/os.h>

BOOL DBVerbose;
OSDebugInterface* __DBInterface;

void DBInit(void)
{
    __DBInterface = (OSDebugInterface*)OSPhysicalToCached(OS_PHYS_DEBUG_INTERFACE);
    __DBInterface->exceptionHook = OSCachedToPhysical(__DBExceptionDestination);
    DBVerbose = TRUE;
}

void __DBExceptionDestinationAux(void)
{
    const void* physCtx = (void*)OS_PHYS_CURRENT_CONTEXT_PHYS;
    OSContext* ctx = (OSContext*)OSPhysicalToCached(*(u32*)physCtx);

    OSReport("DBExceptionDestination\n");
    OSDumpContext(ctx);
    PPCHalt();
}

// clang-format off
asm void __DBExceptionDestination(void)
{
    nofralloc

    mfmsr r3
    ori r3, r3, (MSR_IR | MSR_DR)
    mtmsr r3

    b __DBExceptionDestinationAux
}
// clang-format on

BOOL __DBIsExceptionMarked(u8 exc)
{
    return __DBInterface->exceptionMask & (1 << exc);
}

void DBPrintf(const char* msg, ...)
{
#pragma unused(msg)
}
