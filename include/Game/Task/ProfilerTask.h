#ifndef GAME_PROFILER_TASK_H
#define GAME_PROFILER_TASK_H

#include "NL/nlTask.h"
#include "types.h"

class ProfilerTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Profiler";
    }
};

// IsProfiling combines the two flags; ProfilerTask::Run increments the counter.
extern bool g_bProfiling;
extern bool g_bShowProfiler;
extern u32 g_nProfilerFrame;

bool IsProfiling();

#endif // GAME_PROFILER_TASK_H
