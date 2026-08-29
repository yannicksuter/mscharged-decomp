#include "revolution/hbm/nw4hbm/db/assert.h"

#include "revolution/hbm/nw4hbm/db/console.h"
#include "revolution/hbm/nw4hbm/db/directPrint.h"
#include "revolution/hbm/nw4hbm/db/mapFile.h"

#include "revolution/base.h"
#include "revolution/os.h"
#include "revolution/vi.h"

#include "decomp.h"

namespace nw4hbm {
namespace db {

static void Assertion_Printf_(char const* fmt, ...);
static bool ShowMapInfoSubroutine_(u32 address, bool preCRFlag);
static void ShowStack_(register_t sp);
static void WarningAlarmFunc_(OSAlarm* alarm, OSContext* ctx);

static u32 sWarningTime;
static ConsoleHandle sAssertionConsole;
static bool sDispWarningAuto;

static void Assertion_Printf_(char const* fmt, ...) {
    va_list vlist;

    va_start(vlist, fmt);

    if (sAssertionConsole) {
        Console_VPrintf(sAssertionConsole, fmt, vlist);
    } else {
        OSVReport(fmt, vlist);
    }

    va_end(vlist);
}

static bool ShowMapInfoSubroutine_(u32 address, bool preCRFlag) {
    u8 strBuf[260];

    if (MapFile_Exists() == false) {
        return false;
    }
    if (0x80000000 > address || address <= 0x82FFFFFF == false) {
        return false;
    }

    if (MapFile_QuerySymbol(address, strBuf, sizeof(strBuf))) {
        if (preCRFlag) {
            Assertion_Printf_("\n");
        }

        Assertion_Printf_("%s\n", strBuf);
        return true;
    }

    return false;
}

static void ShowStack_(register_t sp) {
    u32 i;
    register_t* p;

    Assertion_Printf_("-------------------------------- TRACE\n");
    Assertion_Printf_("Address:   BackChain   LR save\n");

    p = reinterpret_cast<register_t*>(sp);

    for (i = 0; i < 16; i++) {
        if (reinterpret_cast<u32>(p) == 0x00000000) {
            break;
        }

        if (reinterpret_cast<u32>(p) == 0xFFFFFFFF) {
            break;
        }

        if (!(reinterpret_cast<u32>(p) & 0x80000000)) {
            break;
        }

        Assertion_Printf_("%08X:  %08X    %08X ", p, p[0], p[1]);

        if (!ShowMapInfoSubroutine_(p[1], false)) {
            Assertion_Printf_("\n");
        }

        p = reinterpret_cast<register_t*>(*p);
    }
}

// clang-format off
DECL_WEAK void VPanic(const char* file, int line, const char* fmt, va_list vlist) {
    register register_t stackPointer;

    asm { mr stackPointer, r1 }

    stackPointer = *reinterpret_cast<register_t*>(stackPointer);

    OSDisableInterrupts();

    VISetPreRetraceCallback(NULL);
    VISetPostRetraceCallback(NULL);

    if (sAssertionConsole) {
        detail::DirectPrint_SetupFB(NULL);
    }

    ShowStack_(stackPointer);

    if (sAssertionConsole) {
        Console_Printf(sAssertionConsole, "%s:%d Panic:", file, line);
        Console_VPrintf(sAssertionConsole, fmt, vlist);
        Console_Printf(sAssertionConsole, "\n");

        Console_ShowLatestLine(sAssertionConsole);
        Console_SetVisible(sAssertionConsole, true);
        Console_DrawDirect(sAssertionConsole);
    } else {
        OSReport("%s:%d Panic:", file, line);
        OSVReport(fmt, vlist);
        OSReport("\n");
    }

    PPCHalt();
}
// clang-format on

DECL_WEAK void Panic(char const* file, int line, char const* msg, ...) {
    va_list vlist;

    va_start(vlist, msg);
    VPanic(file, line, msg, vlist);
    va_end(vlist);
}

DECL_WEAK void VWarning(char const* file, int line, char const* fmt, va_list vlist) {
    if (sAssertionConsole) {
        Console_Printf(sAssertionConsole, "%s:%d Warning:", file, line);
        Console_VPrintf(sAssertionConsole, fmt, vlist);
        Console_Printf(sAssertionConsole, "\n");

        Console_ShowLatestLine(sAssertionConsole);
        Console_SetVisible(sAssertionConsole, true);
    } else {
        OSReport("%s:%d Warning:", file, line);
        OSVReport(fmt, vlist);
        OSReport("\n");
    }
}

DECL_WEAK void Warning(char const* file, int line, char const* msg, ...) {
    static bool sInitializedAlarm = false;
    static OSAlarm sWarningAlarm;

    va_list vlist;

    if (!sInitializedAlarm) {
        OSCreateAlarm(&sWarningAlarm);
        sInitializedAlarm = true;
    }

    va_start(vlist, msg);
    VWarning(file, line, msg, vlist);
    va_end(vlist);

    if (sWarningTime) {
        OSCancelAlarm(&sWarningAlarm);
        OSSetAlarm(&sWarningAlarm, sWarningTime, WarningAlarmFunc_);
    }
}

static void WarningAlarmFunc_(OSAlarm* alarm, OSContext* ctx) { Console_SetVisible(sAssertionConsole, false); }

} // namespace db
} // namespace nw4hbm
