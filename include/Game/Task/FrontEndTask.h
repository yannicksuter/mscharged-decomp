#ifndef _FRONTENDTASK_H_
#define _FRONTENDTASK_H_

#include "NL/nlTask.h"

class FrontEndTask : public nlTask
{
public:
    FrontEndTask();

    virtual void Run(float dt);
    virtual const char* GetName() { return "Front End"; }

    void HandleE3IdleReset(float dt);

private:
    float mValue_20;
    float mValue_24;
    float mValue_28;
    float mValue_2C;
};

#endif // _FRONTENDTASK_H_
