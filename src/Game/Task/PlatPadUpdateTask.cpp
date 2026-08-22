#include "Game/Task/PlatPadUpdateTask.h"

#include "Game/RumbleActions.h"

void PlatPadUpdateTask::Run(float dt)
{
    UpdateRumbleActions(dt);
}
