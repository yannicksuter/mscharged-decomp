#include "Game/ResetTask.h"

#include "Game/NetworkSession.h"

#include "NL/globalpad.h"

typedef long long OSTime;
typedef void (*OSStateCallback)(void);

#define OS_BUS_CLOCK_SPEED           (*(volatile u32*)0x800000F8)
#define OS_TIME_SPEED                (OS_BUS_CLOCK_SPEED / 4)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIME_SPEED / 1000))
#define OSMillisecondsToTicks(msec)  ((msec) * (OS_TIME_SPEED / 1000))
#define OSSleepMilliseconds(msec)    OSSleepTicks(OSMillisecondsToTicks((OSTime)msec))

extern "C"
{
    s32 DVDGetDriveStatus();
    s32 OSGetResetButtonState();
    OSStateCallback OSSetPowerCallback(OSStateCallback callback);
    OSTime OSGetTime();
    void OSYieldThread();
    void OSSleepTicks(OSTime ticks);
    void OSRestart(u32 resetCode);
    void OSRebootSystem();
    void OSShutdownSystem();
    void OSReturnToMenu();
    void AXSetMasterVolume(u16 volume);
    void VISetBlack(bool black);
    void VIFlush();
    void VIWaitForRetrace();
}

s32 ResetTask::s_ResetMode = 0;
RESET_STATE ResetTask::s_ResetState = RS_RUNNING;
bool ResetTask::s_AudioInInit = false;
bool ResetTask::s_ResetPressed = false;
bool ResetTask::s_resetPaused = false;
bool ResetTask::s_checkCardRemoved = false;

u32 softResetTime[4] = { 0, 0, 0, 0 };

extern bool g_bEnableGamecubePadMonkey;
extern void* lbl_806E1E28;
extern void* lbl_806E201C;

extern UnidentifiedNetworkSession* lbl_806E10EC;

extern "C" cGlobalPad* fn_802C082C(void* manager, int index);
extern "C" void fn_802EC8A0(void* audioSystem, bool, bool);

bool lbl_806E107C;

extern "C" void fn_8011BD0C()
{
    lbl_806E107C = true;
}

ResetTask::ResetTask()
{
    OSSetPowerCallback(fn_8011BD0C);
}

void ResetTask::Run(float dt)
{
    HandleSoftReset();

    if (DVDGetDriveStatus() == -1)
    {
        return;
    }

    if (OSGetResetButtonState())
    {
        s_ResetPressed = true;
        return;
    }

    if (!OSGetResetButtonState() && s_ResetPressed)
    {
        s_ResetMode = 0;
        s_ResetState = s_ResetState == RS_RUNNING ? RS_STARTRESET : s_ResetState;
    }

    if (lbl_806E107C)
    {
        s_ResetMode = 2;
        s_ResetState = s_ResetState == RS_RUNNING ? RS_STARTRESET : s_ResetState;
    }

    if (s_ResetState != RS_STARTRESET)
    {
        if (s_ResetState < RS_STARTRESET)
        {
            return;
        }
    }
    else if (!s_resetPaused)
    {
        for (s32 i = 0; i < 4; ++i)
        {
            fn_802C082C(lbl_806E1E28, i)->StopRumble();
        }

        while (s_AudioInInit)
        {
            OSYieldThread();
        }

        if (lbl_806E10EC)
        {
            lbl_806E10EC->UnidentifiedVirtual84();
        }

        float volume = 0.99f;
        while (volume > 0.01)
        {
            u16 masterVolume = (u16)(volume * 32768.0f);
            AXSetMasterVolume(masterVolume);
            OSSleepMilliseconds(25);
            volume = volume * 2.0f - 1.0f;
        }

        if (lbl_806E201C)
        {
            fn_802EC8A0(lbl_806E201C, true, true);
        }

        VISetBlack(true);
        VIFlush();
        VIWaitForRetrace();

        switch (s_ResetMode)
        {
        case 0:
            OSRestart(0);
            break;
        case 1:
            OSRebootSystem();
            break;
        case 2:
            OSShutdownSystem();
            break;
        case 3:
            OSReturnToMenu();
            break;
        }

        for (;;)
        {
        }
    }
}

void HandleSoftReset()
{
    static OSTime LastTime;

    if (!LastTime)
    {
        LastTime = OSGetTime();
    }

    const OSTime now = OSGetTime();

    LastTime = OSGetTime();

    if (g_bEnableGamecubePadMonkey)
    {
        return;
    }

    for (s32 i = 0; i < 4; ++i)
    {
        cGlobalPad* pad = fn_802C082C(lbl_806E1E28, i);
        if (!pad)
        {
            break;
        }

        if (pad->IsConnected())
        {
            softResetTime[i] = 0;
        }
    }
}
