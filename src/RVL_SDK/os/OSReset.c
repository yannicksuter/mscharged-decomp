#include <decomp.h>
#include <revolution/os.h>
#include <revolution/pad.h>
#include <revolution/vi/vi3in1.h>
#include <revolution/os/OSNandbootInfo.h>
#include <revolution/sc_fwd.h>
#include <mem.h>

#if defined(VERSION_R4QK01)
#include <revolution/esp/esp.h>
#include <revolution/os/OSLaunch.h>
#include <revolution/os/OSPlayTime.h>
#endif

typedef struct SCIdleModeInfo {
    u8 standby;
    u8 wc24;
} SCIdleModeInfo;

enum {
    OS_STATE_FLAGS_SHUTDOWN_NONE = 0,
    OS_STATE_FLAGS_SHUTDOWN_POWER_OFF,
    OS_STATE_FLAGS_SHUTDOWN_RESTART,
    OS_STATE_FLAGS_SHUTDOWN_RETURN_MENU,
    OS_STATE_FLAGS_SHUTDOWN_LAUNCH_TITLE,
    OS_STATE_FLAGS_SHUTDOWN_IDLE
};

#define DVD_COVER_CLOSED 2

#define SC_STATUS_BUSY 1

#define OS_APP_TYPE_IPL 0x40
#define OS_APP_TYPE_DVD 0x80
#define OS_APP_TYPE_CHANNEL 0x81

#define EXEC_WORK_ARENA_LO ((void*)0x81280000)
#define EXEC_WORK_ARENA_HI ((void*)0x812F0000)

OSThreadQueue __OSActiveThreadQueue AT_ADDRESS(OS_BASE_CACHED | 0x000000DC);

void __OSGetIOSRev(OSIOSRev* rev);

void __OSLaunchMenu(void);
#if !defined(VERSION_R4QK01)
void __OSRelaunchTitle(void);
#endif
u32 __DVDGetCoverStatus(void);
void __DVDPrepareReset(void);

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
        next = thread->nextActive;

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
#if !defined(VERSION_R4QK01)
    case OS_SHUTDOWN_RESTART:
#endif
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

#if defined(VERSION_R4QK01)
static u8 __OSGetDiscState(u8 last) {
    u32 flags;

    if (__DVDGetCoverStatus() != DVD_COVER_CLOSED) {
        return DVD_STATE_COVER_CLOSED;
    } else if (last == DVD_STATE_BUSY && __OSGetRTCFlags(&flags) && !flags) {
        return DVD_STATE_BUSY;
    } else {
        return DVD_STATE_WAITING;
    }
}
#endif

void OSRebootSystem(void) {
    OSStateFlags state;
#if !defined(VERSION_R4QK01)
    u32 rtcFlags;
#endif

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);

#if defined(VERSION_R4QK01)
    state.discState = __OSGetDiscState(state.discState);
#else
    if (__DVDGetCoverStatus() != DVD_COVER_CLOSED) {
        state.discState = DVD_STATE_COVER_CLOSED;
    } else if (state.discState != DVD_STATE_BUSY ||
               (__OSGetRTCFlags(&rtcFlags) && rtcFlags)) {
        state.discState = DVD_STATE_WAITING;
    } else {
        state.discState = DVD_STATE_BUSY;
    }
#endif

    state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_RESTART;

    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);
    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_REBOOT);
#if defined(VERSION_R4QK01)
    __OSHotResetForError();
#else
    __OSHotReset();
#endif
}

void OSShutdownSystem(void) {
    SCIdleModeInfo idleInfo;
#if !defined(VERSION_R4QK01)
    u32 rtcFlags;
#endif
    OSIOSRev iosRev;
    OSStateFlags state;

    memset(&idleInfo, 0, sizeof(idleInfo));

    SCInit();
    while (SCCheckStatus() == SC_STATUS_BUSY) {
    }

    SCGetIdleMode(&idleInfo);

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);

#if defined(VERSION_R4QK01)
    state.discState = __OSGetDiscState(state.discState);
#else
    if (__DVDGetCoverStatus() != DVD_COVER_CLOSED) {
        state.discState = DVD_STATE_COVER_CLOSED;
    } else if (state.discState != DVD_STATE_BUSY ||
               (__OSGetRTCFlags(&rtcFlags) && rtcFlags)) {
        state.discState = DVD_STATE_WAITING;
    } else {
        state.discState = DVD_STATE_BUSY;
    }
#endif

    if (idleInfo.standby == TRUE) {
        state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_IDLE;
    } else {
        state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_POWER_OFF;
    }

    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);
    __OSGetIOSRev(&iosRev);

    if (idleInfo.standby == TRUE) {
#if defined(VERSION_R4QK01)
        __OSIsReturnToIdle = TRUE;
#endif
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

#if !defined(VERSION_R4QK01)
void OSShutdownSystemForBS(u32 resetCode) {
    OSNandbootInfo* nandInfo;
    OSStateFlags state;

    OSSetArenaLo(EXEC_WORK_ARENA_LO);
    OSSetArenaHi(EXEC_WORK_ARENA_HI);

    nandInfo = OSAllocFromMEM1ArenaLo(sizeof(OSNandbootInfo), 32);
    memset(nandInfo, 0, sizeof(OSNandbootInfo));

    __OSReadNandbootInfo(nandInfo);
    nandInfo->returnValue = 1;
    nandInfo->argValue = resetCode | 0x80000000;
    __OSWriteNandbootInfo(nandInfo);

    if (__OSReadStateFlags(&state)) {
        state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_RETURN_MENU;
        __OSWriteStateFlags(&state);
    }

    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_RESTART);
    OSEnableScheduler();

    __OSRelaunchTitle();
}
#endif

void OSRestart(u32 resetCode) {
    u8 appType;

    appType = OSGetAppType();

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();

    if (appType == OS_APP_TYPE_CHANNEL) {
#if defined(VERSION_R4QK01)
        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_RESTART);
        OSEnableScheduler();
        __OSRelaunchTitle(resetCode);
#else
        OSShutdownSystemForBS(resetCode);
#endif
    } else if (appType == OS_APP_TYPE_DVD) {
        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_RESTART);
        OSEnableScheduler();
        __OSReboot(resetCode, bootThisDol);
    }

    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_REBOOT);
#if defined(VERSION_R4QK01)
    __OSHotResetForError();
#else
    __OSHotReset();
#endif
}

#if defined(VERSION_R4QK01)
static void __OSReturnToMenu(u8 menuMode) {
    OSStateFlags state;
    ESPTicketView* ticket;

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);
    state.discState = __OSGetDiscState(state.discState);
    state.shutdownType = OS_STATE_FLAGS_SHUTDOWN_RETURN_MENU;
    state.menuMode = menuMode;
    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);

    OSSetArenaLo(EXEC_WORK_ARENA_LO);
    OSSetArenaHi(EXEC_WORK_ARENA_HI);
    if (ESP_InitLib() != 0) {
        __OSReturnToMenuForError();
    }

    ticket = OSAllocFromMEM1ArenaLo(OSRoundUp32B(sizeof(ESPTicketView)), 32);
    if (ticket == NULL) {
        __OSReturnToMenuForError();
    }

    memset(ticket, 0, OSRoundUp32B(sizeof(ESPTicketView)));
    if (ESP_DiGetTicketView(NULL, ticket) == 0 && OSPlayTimeIsLimited()) {
        OSPlayTimeType type = OS_PLAYTIME_PERMANENT;
        u32 remain = 0xFFFFFFFF;

        __OSGetPlayTime(ticket, &type, &remain);
        if (remain == 0) {
            __OSWriteExpiredFlagIfSet();
        }
    }

    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_RETURN_MENU);
    OSEnableScheduler();
    __OSLaunchMenu();
    OSDisableScheduler();
    __VISetRGBModeImm();
    __OSHotResetForError();
}

void OSReturnToMenu(void) {
    __OSReturnToMenu(0);
    OSPanic(__FILE__, 895, "OSReturnToMenu(): Falied to boot system menu.\n");
}

void OSReturnToDataManager(void) {
    __OSReturnToMenu(1);
    OSPanic(__FILE__, 913, "OSReturnToDataManager(): Falied to boot system menu.\n");
}

void OSReturnToSetting(u8 setting) {
    const char* url;

    switch (setting) {
    case OS_SETTING_CALENDAR:
        url = "Calendar/Calendar_index.html";
        break;
    case OS_SETTING_DISPLAY:
        url = "Display/Display_index.html";
        break;
    case OS_SETTING_SOUND:
        url = "Sound/Sound_index.html";
        break;
    case OS_SETTING_PARENTAL:
        url = "Parental_Control/Parental_Control_index.html";
        break;
    case OS_SETTING_INTERNET:
        url = "Internet/Internet_index.html";
        break;
    case OS_SETTING_WC24:
        url = "WiiConnect24/Wiiconnect24_index.html";
        break;
    case OS_SETTING_UPDATE:
        url = "Update/Update_index.html";
        break;
    default:
        OSReport("OSReturnToSetting(): You can't specify %d.  \n", setting);
        OSPanic(__FILE__, __LINE__, "");
    }

    __OSReturnToMenul(1, url, NULL);
}
#else
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
#endif

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
#if defined(VERSION_R4QK01)
    __OSHotResetForError();
    OSPanic(__FILE__, 1010, "__OSReturnToMenu(): Falied to boot system menu.\n");
#else
    __OSHotReset();
    OSPanic(__FILE__, 869, "__OSReturnToMenu(): Falied to boot system menu.\n");
#endif
}

#if defined(VERSION_R4QK01)
void __OSHotResetForError(void) {
    if (__OSInNandBoot || __OSInReboot) {
        __OSInitSTM();
    }

    __OSHotReset();
    OSPanic(__FILE__, 1034, "__OSHotReset(): Falied to reset system.\n");
}
#endif

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
#if defined(VERSION_R4QK01)
    OSPanic(__FILE__, 1185,
            "OSResetSystem() is obsoleted. It doesn't work any longer.\n");
#else
    OSPanic(__FILE__, 1020,
            "OSResetSystem() is obsoleted. It doesn't work any longer.\n");
#endif
}

#if defined(VERSION_R4QK01)
u32 OSSetBootDol(u32 dolOffset) {
    OSPanic(__FILE__, __LINE__,
            "OSSetBootDol() is obsoleted. It doesn't work any longer.\n");
    return 0;
}
#endif
