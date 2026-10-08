#ifndef GAME_AI_SCRIPT_MACHINE_INL
#define GAME_AI_SCRIPT_MACHINE_INL

#include "Game/AI/ScriptMachine.h"

bool ScriptMachine::IsIdle() const
{
    return mActiveState == 0;
}

#endif
