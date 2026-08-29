#ifndef GAME_RESET_TASK_H
#define GAME_RESET_TASK_H

#include "NL/nlTask.h"

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

#endif // GAME_RESET_TASK_H
