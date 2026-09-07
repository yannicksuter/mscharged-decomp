#include <revolution/ax.h>
#include <revolution/dvd.h>
#include <revolution/os.h>
#include <revolution/vi.h>

#include "Game/Task/ResetTask.h"

#include "Game/Audio/AudioSystem.h"
#include "Game/NetworkSession.h"
#include "NL/globalpad.h"

#define OSSleepMilliseconds(msec) OSSleepTicks(OSMillisecondsToTicks((OSTime)msec))

s32 ResetTask::s_ResetMode = 0;
RESET_STATE ResetTask::s_ResetState = RS_RUNNING;
bool ResetTask::s_AudioInInit = false;
bool ResetTask::s_ResetPressed = false;
bool ResetTask::s_resetPaused = false;
bool ResetTask::s_checkCardRemoved = false;

u32 softResetTime[4] = { 0, 0, 0, 0 };

extern bool g_bEnableGamecubePadMonkey;

bool lbl_806E107C;

void fn_8011BD0C()
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
            g_pPadManager->GetPad(i)->StopRumble();
        }

        while (s_AudioInInit)
        {
            OSYieldThread();
        }

        if (g_pNetworkSession)
        {
            g_pNetworkSession->OnlineVirtual08();
        }

        float volume = 0.99f;
        while (volume > 0.01)
        {
            u16 masterVolume = (u16)(volume * 32768.0f);
            AXSetMasterVolume(masterVolume);
            OSSleepMilliseconds(25);
            volume = volume * 2.0f - 1.0f;
        }

        if (g_pAudioSystem)
        {
            FlushAudio(g_pAudioSystem, true, true);
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
        cGlobalPad* pad = g_pPadManager->GetPad(i);
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
