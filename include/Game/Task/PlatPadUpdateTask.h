#ifndef GAME_PLAT_PAD_UPDATE_TASK_H
#define GAME_PLAT_PAD_UPDATE_TASK_H

#include "NL/nlTask.h"

class PlatPadUpdateTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Plat Pad";
    }
};

#endif // GAME_PLAT_PAD_UPDATE_TASK_H
