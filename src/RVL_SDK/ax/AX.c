#include <revolution/ax/AX.h>
#include <revolution/ax/AXAlloc.h>
#include <revolution/ax/AXVPB.h>
#include <revolution/ax/AXSPB.h>
#include <revolution/ax/AXAux.h>
#include <revolution/ax/AXCL.h>
#include <revolution/ax/AXOut.h>
#include <revolution/os/OS.h>

const char* __AXVersion =
#if defined(VERSION_R4QK01)
    "<< RVL_SDK - AX \trelease build: Feb 27 2009 10:01:36 (0x4302_145) >>";
#else
    "<< RVL_SDK - AX \trelease build: Dec 18 2006 15:43:48 (0x4199_60831) >>";
#endif

static BOOL __init = FALSE;

void AXInit(void) {
    AXInitEx(0);
}

BOOL AXIsInit(void) {
    return __init;
}

void AXInitEx(u32 mode) {
    if (!__init) {
        OSRegisterVersion(__AXVersion);

        __AXAllocInit();
        __AXVPBInit();
        __AXSPBInit();
        __AXAuxInit();
        __AXClInit();
        __AXOutInit(mode);

        __init = TRUE;
    }
}
