#ifndef GAME_TASK_TRANSITION_TASK_H
#define GAME_TASK_TRANSITION_TASK_H

#include "NL/nlTask.h"

class TransitionTask : public nlTask
{
public:
    void Initialize();

    virtual const char* GetName();
    virtual void Run(float);
    virtual void StateTransition(u32 from, u32 to);
}; // size 0x20

inline const char* TransitionTask::GetName()
{
    return "Transition Manager";
}

inline void TransitionTask::Run(float)
{
}

extern TransitionTask gTransitionTask;

#endif // GAME_TASK_TRANSITION_TASK_H
