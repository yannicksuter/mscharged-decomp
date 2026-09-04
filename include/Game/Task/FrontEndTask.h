#ifndef _FRONTENDTASK_H_
#define _FRONTENDTASK_H_

#include "NL/nlTask.h"

class FrontEndTask : public nlTask
{
public:
    FrontEndTask();

    virtual void Run(float dt);
    virtual const char* GetName() { return "Front End"; }

    void HandleE3IdleReset(float dt);
    void HandleSoftReset(float dt);

private:
    /* 0x20 */ float softResetTime[4];
};

// Seconds the front end has been running.
extern float g_fUpTime;
// Seconds without controller input, reset by HandleE3IdleReset.
extern float g_fIdleGameTime;
// Set when the E3 idle timeout returns to the front end from gameplay;
// LoadingTask picks a different loading path while it is set.
extern bool g_bE3IdleReset;

// Only ever written (by the E3 controller shortcut in AsyncLoading); the
// retail build never reads the value back, so its role is not recoverable.
extern float g_fE3DebugTime;
void SetE3DebugTime(float value);

#endif // _FRONTENDTASK_H_
