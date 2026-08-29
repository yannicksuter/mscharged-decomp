#ifndef GAME_FIXED_UPDATE_TASK_H
#define GAME_FIXED_UPDATE_TASK_H

#include "NL/nlTask.h"

class FixedUpdateTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName();

    static float GetPhysicsUpdateTick();
};

#endif // GAME_FIXED_UPDATE_TASK_H
