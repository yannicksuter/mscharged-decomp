#include "Game/Task/ProfilerTask.h"

bool g_bProfiling;
bool g_bShowProfiler;
u32 g_nProfilerFrame;

void ProfilerTask::Run(float)
{
    ++g_nProfilerFrame;
}

bool IsProfiling()
{
    return g_bProfiling || g_bShowProfiler;
}
