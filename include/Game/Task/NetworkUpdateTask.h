#ifndef GAME_NETWORK_UPDATE_TASK_H
#define GAME_NETWORK_UPDATE_TASK_H

#include "NL/nlTask.h"

class NetworkUpdateTask : public nlTask
{
public:
    void Initialize();
    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Network Update";
    }
};

#endif // GAME_NETWORK_UPDATE_TASK_H
