#ifndef GAME_WORLD_UPDATE_TASK_H
#define GAME_WORLD_UPDATE_TASK_H

#include "NL/nlTask.h"

class WorldUpdateTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "World";
    }
};

#endif // GAME_WORLD_UPDATE_TASK_H
