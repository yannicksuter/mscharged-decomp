#ifndef GAME_TASK_LOADING_TASK_H
#define GAME_TASK_LOADING_TASK_H

#include "NL/nlTask.h"

class LoadingTask : public nlTask
{
public:
    void Start();

    virtual const char* GetName();
    virtual void Run(float dt);
    virtual void StateTransition(u32 from, u32 to);

    /* 0x20 */ float mElapsed;
}; // size 0x24

inline const char* LoadingTask::GetName()
{
    return "Loading Task";
}

extern LoadingTask sLoadingTask;

#endif // GAME_TASK_LOADING_TASK_H
