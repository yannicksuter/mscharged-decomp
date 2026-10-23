#include <decomp.h>
#include <revolution/ipc.h>
#include <revolution/nwc24/NWC24Schedule.h>
#include <revolution/nwc24/internal/NWC24iTime.h>
#include <revolution/os.h>
#include <revolution/sc.h>

// OS startup provides weak fallbacks when the full WiiConnect24 library is absent.
NWC24Err NWC24iPrepareShutdown(void);

static OSShutdownFunctionInfo ShutdownFuncInfo;
static u32 nwc24TimeCommonResult[8] ALIGN(32);
static u32 nwc24TimeCommonBuffer[8] ALIGN(32);
static s32 nwc24ShtRetryRest;
static s32 nwc24ShtFd = -1;
static BOOL NWC24iIsRequestPending;

void __OSInitNet(void)
{
    s32 error;
    OSIOSRev rev;

    __OSGetIOSRev(&rev);
    if (rev.idLo <= 4 || rev.idLo == 9)
    {
        return;
    }

    error = NWC24iPrepareShutdown();
    if (error != 0)
    {
        if (error < 0)
        {
            OSReport("Failed to register network shutdown function. %d\n",
                error);
        }

        error = NWC24SuspendScheduler();
        if (error < 0)
        {
            OSReport("Failed to suspend the WiiConnect24 scheduler. %d\n",
                error);
        }
    }

    if (!__OSInIPL)
    {
        error = NWC24iSynchronizeRtcCounter(FALSE);
        if (error != 0)
        {
            OSReport(
                "Failed to synchronize time with network resource managers. %d\n",
                error);
        }
    }
}

static BOOL NWC24Shutdown_(BOOL final, u32 event);
static s32 NWC24iOpenResourceManager_(const char* funcName, const char* path, s32* resultPtr, IPCOpenMode mode);
static s32 NWC24iIoctlResourceManager_(const char* funcName, s32 fd, s32 type, void* in, s32 inSize, void* out, s32 outSize);
static s32 NWC24iIoctlResourceManagerAsync_(const char* funcName, s32 fd, s32 type, void* in, s32 inSize, void* out, s32 outSize, void* callbackArg);
static s32 NWC24iCloseResourceManager_(const char* funcName, s32 fd);
static s32 CheckCallingStatus(const char* funcName);

DECL_WEAK NWC24Err NWC24iPrepareShutdown(void)
{
    s32 result;

    result = 0;
    ShutdownFuncInfo.func = NWC24Shutdown_;
    ShutdownFuncInfo.priority = 0x6e;
    OSRegisterShutdownFunction(&ShutdownFuncInfo);

    if (nwc24ShtFd < 0)
    {
        result = NWC24iOpenResourceManager_(__FUNCTION__, "/dev/net/kd/request", &nwc24ShtFd, IPC_OPEN_READ);
    }
    nwc24ShtRetryRest = 5;

    if (result == 0)
    {
        result = 1;
    }
    return result;
}

static s32 GetRTC(s32* destPtr);
static s32 NWC24iSetRtcCounter_(u32 rtc, u32 flags);

DECL_WEAK NWC24Err NWC24iSynchronizeRtcCounter(BOOL forceSave)
{
    s32 result;
    s32 rtc;

    result = GetRTC(&rtc);

    if (result != 0)
    {
        return result;
    }
    else
    {
        return NWC24iSetRtcCounter_(rtc, forceSave != FALSE);
    }
}

DECL_WEAK s32 NWC24SuspendScheduler(void)
{
    s32 result;
    s32 close;
    s32 fd;
    static s32 susResult[8] ALIGN(32);

    result = CheckCallingStatus(__FUNCTION__);
    if (result >= 0)
    {
        result = NWC24iOpenResourceManager_(__FUNCTION__, "/dev/net/kd/request", &fd, IPC_OPEN_NONE);
        if (result >= 0)
        {
            result = NWC24iIoctlResourceManager_(__FUNCTION__, fd, 1, 0, 0, susResult, sizeof(susResult));
            if (result >= 0)
            {
                result = susResult[0];
            }
            close = NWC24iCloseResourceManager_(__FUNCTION__, fd);
            if (close < 0)
            {
                result = close;
            }
        }
    }
    return result;
}

DECL_WEAK s32 NWC24ResumeScheduler(void)
{
    s32 result;
    s32 close;
    s32 fd;
    static s32 susResult[8] ALIGN(32);

    result = CheckCallingStatus(__FUNCTION__);
    if (result >= 0)
    {
        result = NWC24iOpenResourceManager_(__FUNCTION__, "/dev/net/kd/request", &fd, IPC_OPEN_NONE);
        if (result >= 0)
        {
            result = NWC24iIoctlResourceManager_(__FUNCTION__, fd, 3, 0, 0, susResult, sizeof(susResult));
            if (result >= 0)
            {
                result = susResult[0];
            }
            close = NWC24iCloseResourceManager_(__FUNCTION__, fd);
            if (close < 0)
            {
                result = close;
            }
        }
    }
    return result;
}

static NWC24Err NWC24iRequestShutdown(u32 event, NWC24Err* resultOut)
{
    s32 result;
    static s32 shtBuffer[8] ALIGN(32);
    static s32 shtResult[8] ALIGN(32);

    shtBuffer[0] = event;
    result = NWC24iIoctlResourceManagerAsync_(__FUNCTION__, nwc24ShtFd, 0x28, shtBuffer, 0x20, shtResult, 0x20, resultOut);
    return result;
}

static BOOL NWC24iIsAsyncRequestPending_(void);

static BOOL NWC24Shutdown_(BOOL final, u32 event)
{
    static BOOL shuttingdown = FALSE;
    static NWC24Err result = NWC24_OK;

    s32 status;

    if (final != FALSE)
    {
        return TRUE;
    }
    if (!shuttingdown)
    {
        status = NWC24iRequestShutdown(event, &result);
        if (status >= 0)
        {
            shuttingdown = TRUE;
        }
    }
    else
    {
        status = NWC24iIsAsyncRequestPending_();

        if (status != 0)
        {
            return FALSE;
        }
        if (result >= 0)
        {
            return TRUE;
        }
        if (nwc24ShtRetryRest < 1)
        {
            OSReport("NWC24Shutdown_: Give up!\n");
            return TRUE;
        }

        shuttingdown = FALSE;
        nwc24ShtRetryRest--;
    }

    return FALSE;
}

static s32 NWC24iSetRtcCounter_(u32 rtc, u32 flags)
{
    s32 result;
    s32 close;
    s32 fd;

    result = CheckCallingStatus(__FUNCTION__);
    if (result < 0)
    {
        return result;
    }

    result = NWC24iOpenResourceManager_(__FUNCTION__, "/dev/net/kd/time", &fd, IPC_OPEN_NONE);
    if (result >= 0)
    {
        nwc24TimeCommonBuffer[0] = rtc;
        nwc24TimeCommonBuffer[1] = flags;

        result = NWC24iIoctlResourceManager_(__FUNCTION__, fd, 0x17, nwc24TimeCommonBuffer, 0x20, nwc24TimeCommonResult, 0x20);
        if (result >= 0)
        {
            result = nwc24TimeCommonResult[0];
        }

        close = NWC24iCloseResourceManager_(__FUNCTION__, fd);
        if (result >= 0)
        {
            result = close;
        }
    }

    return result;
}

static s32 NWC24iOpenResourceManager_(const char* funcName, const char* path, s32* resultPtr, IPCOpenMode mode)
{
    if (resultPtr == NULL)
    {
        return -3;
    }

    *resultPtr = IOS_Open(path, mode);

    if (*resultPtr < 0)
    {
        if (*resultPtr == -6)
            return -29;
        else
            return -42;
    }
    else
    {
        return 0;
    }
}

static s32 NWC24iCloseResourceManager_(const char* funcName, s32 fd)
{
    if (IOS_Close(fd) < 0)
        return -42;
    else
        return 0;
}

static s32 CallbackAsyncIpc(s32 result, void* arg);

static s32 NWC24iIoctlResourceManager_(const char* funcName, s32 fd, s32 type, void* in, s32 inSize, void* out, s32 outSize)
{
    if (IOS_Ioctl(fd, type, in, inSize, out, outSize) < 0)
        return -42;
    else
        return 0;
}

static s32 NWC24iIoctlResourceManagerAsync_(const char* funcName, s32 fd, s32 type, void* in, s32 inSize, void* out, s32 outSize, void* callbackArg)
{
    if (IOS_IoctlAsync(fd, type, in, inSize, out, outSize, CallbackAsyncIpc, callbackArg) < 0)
        return -42;
    else
    {
        NWC24iIsRequestPending = TRUE;
        return 0;
    }
}

static BOOL NWC24iIsAsyncRequestPending_(void)
{
    return NWC24iIsRequestPending;
}

static s32 CallbackAsyncIpc(s32 result, void* arg)
{
    if (arg != NULL)
    {
        *(s32*)arg = result;
    }
    NWC24iIsRequestPending = FALSE;
    return 0;
}

static s32 CheckCallingStatus(const char* funcName)
{
    if (OSGetCurrentThread() == NULL)
        return -1;
    else
        return 0;
}

static s32 GetRTC(s32* destPtr)
{
    s64 time;
    u32 bias;
    u32 status;

    while (TRUE)
    {
        status = SCCheckStatus();

        if (status == 2)
        {
            return -1;
        }
        if (status == 0)
        {
            bias = SCGetCounterBias();
            time = OSGetTime();
            *destPtr = OS_TICKS_TO_SEC(time) - bias;
            return 0;
        }
    }
}
