#include <revolution/ipc.h>
#include <revolution/os.h>
#include <revolution/vi.h>

typedef enum OSStateTMIoctl
{
    STM_IOCTL_REG_STM_EVENT = 0x1000,
    STM_IOCTL_HOT_RESET = 0x2001,
    STM_IOCTL_SHUTDOWN_TO_SBY = 0x2003,
    STM_IOCTL_UNREG_STM_EVENT = 0x3002,
    STM_IOCTL_SET_VI_DIM = 0x5001,
    STM_IOCTL_SET_IDLE_LED_MODE = 0x6002
} OSStateTMIoctl;

static u8 StmEhInBuf[32] __attribute__((aligned(32)));
static u8 StmEhOutBuf[32] __attribute__((aligned(32)));

static u8 StmImInBuf[32] __attribute__((aligned(32)));
static u8 StmImOutBuf[32] __attribute__((aligned(32)));

static u8 StmVdInBuf[32] __attribute__((aligned(32)));
static u8 StmVdOutBuf[32] __attribute__((aligned(32)));

static OSStateCallback ResetCallback;
static OSStateCallback PowerCallback;

static bool StmVdInUse;

static bool StmEhRegistered;
static s32 StmEhDesc;

static s32 StmImDesc;

static bool StmReady;
static bool ResetDown;

static s32 AccessVIDimRegs(void);
static s32 __OSVIDimReplyHandler(s32 result, void* arg);
static void __OSRegisterStateEvent(void);
static void __OSDefaultResetCallback(void);
static void __OSDefaultPowerCallback(void);
static s32 __OSStateEventHandler(s32 result, void* arg);
static void LockUp(void);

static inline s32 AccessVIDimRegs(void)
{
    IPCResult result = IOS_IoctlAsync(StmImDesc, STM_IOCTL_SET_VI_DIM, StmVdInBuf, sizeof(StmVdInBuf), StmVdOutBuf, sizeof(StmVdOutBuf), __OSVIDimReplyHandler, NULL);
    return result != IPC_RESULT_OK ? result : 1;
}

static inline void __OSRegisterStateEvent(void)
{
    bool enabled = OSDisableInterrupts();

    if (IOS_IoctlAsync(StmEhDesc, STM_IOCTL_REG_STM_EVENT, StmEhInBuf, sizeof(StmEhInBuf), StmEhOutBuf, sizeof(StmEhOutBuf), __OSStateEventHandler, NULL) == IPC_RESULT_OK)
    {
        StmEhRegistered = true;
    }
    else
    {
        StmEhRegistered = false;
    }

    OSRestoreInterrupts(enabled);
}

static inline void LockUp(void)
{
    OSDisableInterrupts();
    ICFlashInvalidate();

    while (true)
    {
        ;
    }
}

OSStateCallback OSSetPowerCallback(OSStateCallback callback)
{
    bool enabled;
    OSStateCallback old;

    enabled = OSDisableInterrupts();

    old = PowerCallback;
    PowerCallback = callback;

    if (!StmEhRegistered)
    {
        __OSRegisterStateEvent();
    }

    OSRestoreInterrupts(enabled);
    return old;
}

BOOL OSGetResetButtonState(void)
{
    BOOL state;
    int enabled;

    enabled = OSDisableInterrupts();
    state = ResetDown;
    ResetDown = FALSE;
    OSRestoreInterrupts(enabled);

    if (!StmEhRegistered)
    {
        __OSRegisterStateEvent();
    }

    return state;
}

bool __OSInitSTM(void)
{
    bool success;

    PowerCallback = __OSDefaultPowerCallback;
    ResetCallback = __OSDefaultResetCallback;
    ResetDown = false;

    if (StmReady)
    {
        return true;
    }

    StmVdInUse = false;
    StmImDesc = IOS_Open("/dev/stm/immediate", IPC_OPEN_NONE);
    if (StmImDesc < 0)
    {
        StmReady = false;
        success = false;
        goto exit;
    }

    StmEhDesc = IOS_Open("/dev/stm/eventhook", IPC_OPEN_NONE);
    if (StmEhDesc < 0)
    {
        StmReady = false;
        success = false;
        goto exit;
    }

    __OSRegisterStateEvent();
    StmReady = true;
    success = true;

exit:
    return success;
}

void __OSShutdownToSBY(void)
{
#define in_args ((u32*)StmImInBuf)

    VI_HW_REGS[VI_DCR] = 0;

    OSAssert("OSStateTM.c", 276, StmReady, "Error: The firmware doesn't support shutdown feature.\n");

    in_args[0] = 0;
    IOS_Ioctl(StmImDesc, STM_IOCTL_SHUTDOWN_TO_SBY, StmImInBuf, sizeof(StmImInBuf), StmImOutBuf, sizeof(StmImOutBuf));
    LockUp();

#undef in_args
}

void __OSHotReset(void)
{
    VI_HW_REGS[VI_DCR] = 0;

    OSAssert("OSStateTM.c", 340, StmReady, "Error: The firmware doesn't support reboot feature.\n");

    IOS_Ioctl(StmImDesc, STM_IOCTL_HOT_RESET, StmImInBuf, sizeof(StmImInBuf), StmImOutBuf, sizeof(StmImOutBuf));
    LockUp();
}

static inline bool __OSGetResetButtonStateRaw(void)
{
    return (!(PI_HW_REGS[PI_INTSR] & PI_INTSR_RSWST)) ? true : false;
}

s32 __OSSetVIForceDimming(u32 enabled, u32 yShift, u32 xShift)
{
#define in_args ((u32*)StmVdInBuf)

    bool interrupts;

    if (!StmReady)
    {
        return -10;
    }

    interrupts = OSDisableInterrupts();

    if (StmVdInUse)
    {
        OSRestoreInterrupts(interrupts);
        return 0;
    }

    StmVdInUse = true;
    OSRestoreInterrupts(interrupts);

    in_args[0] = yShift << 3 | xShift | enabled << 7;
    in_args[1] = 0;
    in_args[2] = 0;
    in_args[3] = 0;
    in_args[4] = 0;
    in_args[5] = 0xFFFFFFFF;
    in_args[6] = 0xFFFF0000;
    in_args[7] = 0;

    return AccessVIDimRegs();

#undef in_args
}

static inline s32 __OSSetIdleLEDMode(u32 mode)
{
#define in_args ((u32*)StmImInBuf)

    if (!StmReady)
    {
        return -6;
    }

    in_args[0] = mode;

    return IOS_Ioctl(StmImDesc, STM_IOCTL_SET_IDLE_LED_MODE, StmImInBuf, sizeof(StmImInBuf), StmImOutBuf, sizeof(StmImOutBuf));

#undef in_args
}

s32 __OSUnRegisterStateEvent(void)
{
    IPCResult result;

    if (!StmEhRegistered)
    {
        return 0;
    }

    if (!StmReady)
    {
        return -6;
    }

    result = IOS_Ioctl(StmImDesc, STM_IOCTL_UNREG_STM_EVENT, StmImInBuf, sizeof(StmImInBuf), StmImOutBuf, sizeof(StmImOutBuf));
    if (result == IPC_RESULT_OK)
    {
        StmEhRegistered = false;
    }

    return result;
}

static s32 __OSVIDimReplyHandler(s32 result, void* arg)
{
#pragma unused(result)
#pragma unused(arg)

    StmVdInUse = false;
    return IPC_RESULT_OK;
}

static void __OSDefaultResetCallback(void)
{
}

static void __OSDefaultPowerCallback(void)
{
}

static s32 __OSStateEventHandler(s32 result, void* arg)
{
#pragma unused(result)
#pragma unused(arg)

#define out_args ((u32*)StmEhOutBuf)

    bool enabled;
    OSStateCallback callback;

    OSAssert("OSStateTM.c", 748, result == IPC_RESULT_OK, "Error on STM state event handler\n");

    StmEhRegistered = false;

    if (out_args[0] == 0x20000)
    {
        if (__OSGetResetButtonStateRaw())
        {
            enabled = OSDisableInterrupts();

            callback = ResetCallback;
            ResetDown = true;
            ResetCallback = __OSDefaultResetCallback;
            callback();

            OSRestoreInterrupts(enabled);
        }
        __OSRegisterStateEvent();
    }

    if (out_args[0] == 0x800)
    {
        enabled = OSDisableInterrupts();

        callback = PowerCallback;
        PowerCallback = __OSDefaultPowerCallback;
        callback();

        OSRestoreInterrupts(enabled);
    }

    return IPC_RESULT_OK;

#undef out_args
}
