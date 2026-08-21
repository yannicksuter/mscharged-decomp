#ifndef GAME_PROFILER_TASK_H
#define GAME_PROFILER_TASK_H

#include "NL/nlTask.h"

class ProfilerTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Profiler";
    }
};

extern "C" bool fn_802BDB20();

#endif // GAME_PROFILER_TASK_H
