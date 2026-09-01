#ifndef GAME_TASK_TRANSITION_TASK_H
#define GAME_TASK_TRANSITION_TASK_H

#include "NL/nlTask.h"

class TransitionTask : public nlTask
{
public:
    void Initialize();

    virtual void Run(float)
    {
    }
    virtual const char* GetName()
    {
        return "Transition Manager";
    }
    virtual void StateTransition(u32 from, u32 to);
}; // size 0x20

extern TransitionTask gTransitionTask;

#endif // GAME_TASK_TRANSITION_TASK_H
