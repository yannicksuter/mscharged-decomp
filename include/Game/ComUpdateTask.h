#ifndef GAME_COM_UPDATE_TASK_H
#define GAME_COM_UPDATE_TASK_H

#include "NL/nlTask.h"

class UnidentifiedPingerUpdateTask : public nlTask
{
public:
    virtual const char* GetName()
    {
        return "Pinger Update";
    }
    virtual void Run(float dt);
};

class ComUpdateTask : public nlTask
{
public:
    virtual const char* GetName()
    {
        return "Com Update";
    }
    virtual void Run(float dt);
};

#endif // GAME_COM_UPDATE_TASK_H
