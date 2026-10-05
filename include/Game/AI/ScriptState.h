#ifndef GAME_AI_SCRIPT_STATE_H
#define GAME_AI_SCRIPT_STATE_H

#include "Game/AI/Desire.h"

class ScriptState : public shdStateMachine
{
public:
    ScriptState(
        int state, const char* name, ScriptMachine* context,
        TransitionFunc transition);
    virtual ~ScriptState();

    virtual bool Initialize(void*);
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);

    u32 mInitFunctionHash;
    u32 mUpdateFunctionHash;
    u32 mCleanupFunctionHash;
};

#endif // GAME_AI_SCRIPT_STATE_H
