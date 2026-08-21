#ifndef GAME_END_FRAME_TASK_H
#define GAME_END_FRAME_TASK_H

#include "NL/nlTask.h"

class EndFrameTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "End Frame";
    }
};

#endif // GAME_END_FRAME_TASK_H
