#ifndef GAME_TWEAKER_TASK_H
#define GAME_TWEAKER_TASK_H

#include "NL/nlTask.h"

class TweakerTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Tweaker";
    }
};

#endif // GAME_TWEAKER_TASK_H
