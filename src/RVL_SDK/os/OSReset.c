#include <decomp.h>
#include <revolution/os.h>
#include <mem.h>

typedef struct OSStateFlags {
    u32 checkSum;
    u8 lastAppType;
    u8 shutdownType;
    u8 discState;
    u8 menuMode;
    u8 padding[24];
} OSStateFlags;

typedef struct SCIdleModeInfo {
    u8 standby;
    u8 wc24;
} SCIdleModeInfo;

typedef struct OSIOSRev {
    u8 data[8];
} OSIOSRev;

typedef struct OSNandbootInfo {
    u8 unk_0x00[8];
    u32 returnValue;
    u32 argValue;
    u8 unk_0x10[0x1020 - 0x10];
} OSNandbootInfo;

enum {
    OS_STATE_FLAGS_SHUTDOWN_NONE = 0,
    OS_STATE_FLAGS_SHUTDOWN_POWER_OFF,
    OS_STATE_FLAGS_SHUTDOWN_RESTART,
    OS_STATE_FLAGS_SHUTDOWN_RETURN_MENU,
    OS_STATE_FLAGS_SHUTDOWN_LAUNCH_TITLE,
    OS_STATE_FLAGS_SHUTDOWN_IDLE
};

enum {
    DVD_STATE_BUSY = 1,
    DVD_STATE_WAITING,
    DVD_STATE_COVER_CLOSED
};

#define DVD_COVER_CLOSED 2

#define SC_STATUS_BUSY 1

#define OS_APP_TYPE_IPL 0x40
#define OS_APP_TYPE_DVD 0x80
#define OS_APP_TYPE_CHANNEL 0x81

#define EXEC_WORK_ARENA_LO ((void*)0x81280000)
#define EXEC_WORK_ARENA_HI ((void*)0x812F0000)

OSThreadQueue __OSActiveThreadQueue AT_ADDRESS(OS_BASE_CACHED | 0x000000DC);

void OSDisableScheduler(void);
void OSEnableScheduler(void);
BOOL __OSSyncSram(void);
BOOL __OSGetRTCFlags(u32* flags);
BOOL __OSClearRTCFlags(void);
void __OSHotReset(void);
void __OSShutdownToSBY(void);
void __OSGetIOSRev(OSIOSRev* rev);
void SCInit(void);
u32 SCCheckStatus(void);
void* OSAllocFromMEM1ArenaLo(u32 size, u32 align);
void OSSetArenaLo(void* lo);
void OSSetArenaHi(void* hi);

void __OSStopAudioSystem(void);
void LCDisable(void);
u8 fn_803B3CD4(void);
void __OSLaunchMenu(void);
void fn_803B6B00(void);
void __OSWriteStateFlags(OSStateFlags* flags);
BOOL __OSReadStateFlags(OSStateFlags* flags);
void __OSStopPlayRecord(void);
void fn_803BE578(OSNandbootInfo* info);
void fn_803BE6D0(OSNandbootInfo* info);
BOOL __PADDisableRecalibration(BOOL disable);
void __VISetRGBModeImm(void);
u32 __DVDGetCoverStatus(void);
void __DVDPrepareReset(void);
void fn_8040C264(SCIdleModeInfo* info);

typedef struct OSExecParams {
    BOOL valid;
    u32 restartCode;
    u32 bootDol;
    void* regionStart;
    void* regionEnd;
    BOOL argsUseDefault;
    void* argsAddr;
} OSExecParams;

extern OSExecParams __OSRebootParams;

static u32 bootThisDol = 0;
volatile BOOL __OSIsReturnToIdle = FALSE;
static OSShutdownFunctionQueue ShutdownFunctionQueue;

#define ENQUEUE_INFO(info, queue)                       \
    {                                                   \
        OSShutdownFunctionInfo* __prev = (queue)->tail; \
        if (__prev == 0) {                              \
            (queue)->head = (info);                     \
        } else {                                        \
            __prev->next = (info);                      \
        }                                               \
        (info)->prev = __prev;                          \
        (info)->next = 0;                               \
        (queue)->tail = (info);                         \
    }

#define ENQUEUE_INFO_PRIO(info, queue)                                     \
    {                                                                      \
        OSShutdownFunctionInfo* __prev;                                    \
        OSShutdownFunctionInfo* __next;                                    \
        for (__next = (queue)->head;                                       \
             __next && (__next->priority <= (info)->priority);             \
             __next = __next->next)                                        \
            ;                                                              \
                                                                           \
        if (__next == 0) {                                                 \
            ENQUEUE_INFO(info, queue);                                     \
        } else {                                                           \
            (info)->next = __next;                                         \
            __prev = __next->prev;                                         \
            __next->prev = (info);                                         \
            (info)->prev = __prev;                                         \
            if (__prev == 0) {                                             \
                (queue)->head = (info);                                    \
            } else {                                                       \
                __prev->next = (info);                                     \
            }                                                              \
        }                                                                  \
    }

static void KillThreads(void) {
    OSThread* thread;
    OSThread* next;

    for (thread = __OSActiveThreadQueue.head; thread; thread = next) {
        next = thread->linkActive.next;

        switch (thread->state) {
        case 1:
        case 4:
            OSCancelThread(thread);
            break;
        default:
            break;
        }
    }
}

void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info) {
    ENQUEUE_INFO_PRIO(info, &ShutdownFunctionQueue);
}

BOOL __OSCallShutdownFunctions(BOOL final, u32 event) {
    OSShutdownFunctionInfo* info;
    BOOL err;
    u32 priority = 0;

    err = FALSE;
    info = ShutdownFunctionQueue.head;
    while (info) {
        if (err && priority != info->priority) {
            break;
        }

        err |= !info->func(final, event);
        priority = info->priority;
        info = info->next;
    }

    err |= !__OSSyncSram();
    return err ? FALSE : TRUE;
}

void __OSShutdownDevices(u32 event) {
    BOOL rc;
    BOOL disableRecalibration;
    BOOL doRecal;

    switch (event) {
    case OS_SHUTDOWN_FATAL:
    case OS_SHUTDOWN_RESTART:
    case OS_SHUTDOWN_RETURN_MENU:
    case OS_SHUTDOWN_EXEC:
        doRecal = FALSE;
        break;
    case OS_SHUTDOWN_REBOOT:
    case OS_SHUTDOWN_SHUTDOWN:
    case OS_SHUTDOWN_IDLE:
    default:
        doRecal = TRUE;
        break;
    }

    __OSStopAudioSystem();

    if (!doRecal) {
        disableRecalibration = __PADDisableRecalibration(TRUE);
    }

    while (!__OSCallShutdownFunctions(FALSE, event)) {
    }
    while (!__OSSyncSram()) {
    }

    OSDisableInterrupts();
    rc = __OSCallShutdownFunctions(TRUE, event);
    LCDisable();

    if (!doRecal) {
        __PADDisableRecalibration(disableRecalibration);
    }

    KillThreads();
}

void OSRebootSystem(void) {
    OSStateFlags state;
    u32 rtcFlags;

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);

    if (__DVDGetCoverStatus() != DVD_COVER_CLOSED) {
        state.discState = DVD_STATE_COVER_CLOSED;
    } else if (state.discState != DVD_STATE_BUSY ||
               (__OSGetRTCFlags(&rtcFlags) && rtcFlags)) {
        state.discState = DVD_STATE_WAITING;
    } else {
        state.discState = DVD_STATE_BUSY;
    }

    state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_RESTART;

    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);
    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_REBOOT);
    __OSHotReset();
}

void OSShutdownSystem(void) {
    SCIdleModeInfo idleInfo;
    u32 rtcFlags;
    OSIOSRev iosRev;
    OSStateFlags state;

    memset(&idleInfo, 0, sizeof(idleInfo));

    SCInit();
    while (SCCheckStatus() == SC_STATUS_BUSY) {
    }

    fn_8040C264(&idleInfo);

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);

    if (__DVDGetCoverStatus() != DVD_COVER_CLOSED) {
        state.discState = DVD_STATE_COVER_CLOSED;
    } else if (state.discState != DVD_STATE_BUSY ||
               (__OSGetRTCFlags(&rtcFlags) && rtcFlags)) {
        state.discState = DVD_STATE_WAITING;
    } else {
        state.discState = DVD_STATE_BUSY;
    }

    if (idleInfo.standby == TRUE) {
        state.shutdownType = OS_SHUTDOWN_RETURN_MENU;
    } else {
        state.shutdownType = OS_SHUTDOWN_REBOOT;
    }

    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);
    __OSGetIOSRev(&iosRev);

    if (idleInfo.standby == TRUE) {
        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_RETURN_MENU);
        OSEnableScheduler();
        __OSLaunchMenu();
    } else {
        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_SHUTDOWN);
        __OSShutdownToSBY();
    }
}

void OSShutdownSystemForBS(u32 resetCode) {
    OSNandbootInfo* nandInfo;
    OSStateFlags state;

    OSSetArenaLo(EXEC_WORK_ARENA_LO);
    OSSetArenaHi(EXEC_WORK_ARENA_HI);

    nandInfo = OSAllocFromMEM1ArenaLo(sizeof(OSNandbootInfo), 32);
    memset(nandInfo, 0, sizeof(OSNandbootInfo));

    fn_803BE6D0(nandInfo);
    nandInfo->returnValue = 1;
    nandInfo->argValue = resetCode | 0x80000000;
    fn_803BE578(nandInfo);

    if (__OSReadStateFlags(&state)) {
        state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_RETURN_MENU;
        __OSWriteStateFlags(&state);
    }

    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_RESTART);
    OSEnableScheduler();

    fn_803B6B00();
}

void OSRestart(u32 resetCode) {
    u8 appType;

    appType = fn_803B3CD4();

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();

    if (appType == OS_APP_TYPE_CHANNEL) {
        OSShutdownSystemForBS(resetCode);
    } else if (appType == OS_APP_TYPE_DVD) {
        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_RESTART);
        OSEnableScheduler();
        __OSReboot(resetCode, bootThisDol);
    }

    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_REBOOT);
    __OSHotReset();
}

void OSReturnToMenu(void) {
    OSStateFlags state;
    u32 rtcFlags;

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);

    if (__DVDGetCoverStatus() != DVD_COVER_CLOSED) {
        state.discState = DVD_STATE_COVER_CLOSED;
    } else if (state.discState != DVD_STATE_BUSY ||
               (__OSGetRTCFlags(&rtcFlags) && rtcFlags)) {
        state.discState = DVD_STATE_WAITING;
    } else {
        state.discState = DVD_STATE_BUSY;
    }

    state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_RETURN_MENU;

    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);

    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_RETURN_MENU);
    OSEnableScheduler();

    __OSLaunchMenu();

    OSDisableScheduler();
    __VISetRGBModeImm();
    __OSHotReset();
    OSPanic(__FILE__, 843, "OSReturnToMenu(): Falied to boot system menu.\n");
}

void __OSReturnToMenuForError(void) {
    OSStateFlags state;

    __OSReadStateFlags(&state);
    state.discState = DVD_STATE_WAITING;
    state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_RETURN_MENU;
    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);

    __OSLaunchMenu();

    OSDisableScheduler();
    __VISetRGBModeImm();
    __OSHotReset();
    OSPanic(__FILE__, 869, "__OSReturnToMenu(): Falied to boot system menu.\n");
}

u32 OSGetResetCode(void) {
    u32 code;

    if (__OSRebootParams.valid) {
        code = 0x80000000 | __OSRebootParams.restartCode;
    } else {
        code = (PI_HW_REGS[9] & 0xFFFFFFF8) >> 3;
    }

    return code;
}

void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu) {
    OSPanic(__FILE__, 1020,
            "OSResetSystem() is obsoleted. It doesn't work any longer.\n");
}
