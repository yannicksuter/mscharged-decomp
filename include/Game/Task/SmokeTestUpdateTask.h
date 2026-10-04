#ifndef GAME_TASK_SMOKE_TEST_UPDATE_TASK_H
#define GAME_TASK_SMOKE_TEST_UPDATE_TASK_H

#include "NL/nlTask.h"

class SmokeTestUpdateTask : public nlTask
{
public:
    SmokeTestUpdateTask()
        : mComplete(false)
        , mElapsed(0.0f)
        , mDuration(1.0f)
    {
    }

    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "SmokeTest Update";
    }

private:
    bool mComplete;
    float mElapsed;
    float mDuration;
};

bool IsSmokeTestEnabled();
void SmokeTestLog(const char* format, ...);
void SmokeTestLogGraphValue(const char* name, const char* units, float value);

#endif // GAME_TASK_SMOKE_TEST_UPDATE_TASK_H
