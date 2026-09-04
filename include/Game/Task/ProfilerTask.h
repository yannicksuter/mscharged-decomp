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

// g_bProfiling keeps the predecessor's name; retail never writes either flag
// or reads the frame counter, so the second flag and the counter are named
// after their only use here.
extern bool g_bProfiling;
extern bool g_bShowProfiler;
extern u32 g_nProfilerFrame;

bool IsProfiling();

#endif // GAME_PROFILER_TASK_H
