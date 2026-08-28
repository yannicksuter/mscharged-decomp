#include "NL/nlTask.h"

typedef void (*OSStateCallback)(void);

extern "C" s32 DVDGetDriveStatus();
extern "C" s32 OSGetResetButtonState();
extern "C" OSStateCallback OSSetPowerCallback(OSStateCallback callback);

enum RESET_STATE
{
    RS_RUNNING = 0,
    RS_STARTRESET = 1,
    RS_DOIT = 2,
};

void HandleSoftReset();

class ResetTask : public nlTask
{
public:
    ResetTask();

    virtual void Run(float dt);
    virtual const char* GetName() { return "Reset"; }

    static s32 s_ResetMode;
    static RESET_STATE s_ResetState;
    static bool s_AudioInInit;
    static bool s_ResetPressed;
    static bool s_resetPaused;
    static bool s_checkCardRemoved;
};

s32 ResetTask::s_ResetMode;
RESET_STATE ResetTask::s_ResetState;
bool ResetTask::s_AudioInInit;
bool ResetTask::s_ResetPressed;
bool ResetTask::s_resetPaused;
bool ResetTask::s_checkCardRemoved;

extern "C"
{
    bool lbl_806E107C;
}

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
        if (s_ResetState == RS_RUNNING)
        {
            s_ResetState = RS_STARTRESET;
        }
    }
}
