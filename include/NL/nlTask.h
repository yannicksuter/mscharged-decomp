#ifndef NL_TASK_H
#define NL_TASK_H

#include "types.h"

class nlTask
{
public:
    nlTask()
        : mEnabled(true)
    {
    }

    virtual void Run(float) = 0;
    virtual const char* GetName() = 0;
    virtual void StateTransition(u32, u32);

private:
    nlTask* mNext;
    nlTask* mPrev;
    u32 mPriority;
    u32 mActiveStates;
    u32 mPreviousTicker;
    u32 mUnknown;
    bool mEnabled;
};

#endif // NL_TASK_H
