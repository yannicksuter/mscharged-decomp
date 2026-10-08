#ifndef GAME_RESET_TASK_H
#define GAME_RESET_TASK_H

#include "NL/nlTask.h"

enum RESET_MODE
{
    RM_RESTART = 0,
    RM_REBOOT = 1,
    RM_SHUTDOWN = 2,
    RM_RETURN_TO_MENU = 3,
};

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

    void FSCheckForReset()
    {
        Run(1.0f / 60.0f);
    }

    virtual void Run(float dt);
    virtual const char* GetName() { return "Reset"; }

    static RESET_MODE s_ResetMode;
    static RESET_STATE s_ResetState;
    static bool s_AudioInInit;
    static bool s_ResetPressed;
    static bool s_resetPaused;
    static bool s_checkCardRemoved;
};

void OnPowerButtonPressed();

#endif // GAME_RESET_TASK_H
