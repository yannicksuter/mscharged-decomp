#ifndef GAME_TASK_LOADING_TASK_H
#define GAME_TASK_LOADING_TASK_H

#include "NL/nlTask.h"

class LoadingTask : public nlTask
{
public:
    void Start();

    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Loading Task";
    }
    virtual void StateTransition(u32 from, u32 to);

    /* 0x20 */ float mElapsed;
}; // size 0x24

extern LoadingTask sLoadingTask;

#endif // GAME_TASK_LOADING_TASK_H
