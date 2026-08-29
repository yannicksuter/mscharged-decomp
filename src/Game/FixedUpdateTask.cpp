#include "Game/FixedUpdateTask.h"

#include "types.h"

extern f32 g_fSimulationTick;

const char* FixedUpdateTask::GetName()
{
    return "Game Fixed Update";
}

float FixedUpdateTask::GetPhysicsUpdateTick()
{
    return g_fSimulationTick;
}
