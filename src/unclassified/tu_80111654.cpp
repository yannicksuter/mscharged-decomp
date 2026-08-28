#include "Game/FixedUpdateTask.h"

#include "types.h"

extern f32 g_fSimulationTick;

float FixedUpdateTask::GetPhysicsUpdateTick()
{
    return g_fSimulationTick;
}
