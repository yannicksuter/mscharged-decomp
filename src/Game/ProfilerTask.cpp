#include "Game/ProfilerTask.h"

#include "types.h"

bool lbl_806E1E10;
bool lbl_806E1E11;
u32 lbl_806E1E14;

void ProfilerTask::Run(float)
{
    ++lbl_806E1E14;
}

extern "C" bool fn_802BDB20()
{
    return lbl_806E1E10 || lbl_806E1E11;
}
