#ifndef NL_TASK_H
#define NL_TASK_H

#include "types.h"

class nlTask
{
public:
    nlTask()
        : mTimeDilated(true)
    {
    }

    virtual void Run(float) = 0;
    virtual const char* GetName() = 0;
    virtual void StateTransition(u32, u32);

    /* 0x04 */ nlTask* mNext;
    /* 0x08 */ nlTask* mPrev;
    /* 0x0C */ u32 mPriority;
    /* 0x10 */ u32 mActiveStates;
    /* 0x14 */ u32 mPreviousTicker;
    /* 0x18 */ float mExecutionTime;
    /* 0x1C */ bool mTimeDilated;
}; // size 0x20

class nlTaskManager
{
public:
    nlTaskManager() { }

    static void Startup(u32 initialState);
    static void AddTask(nlTask* task, u32 priority, u32 activeStates);
    static void RunAllTasks();
    static void SetNextState(u32 nextState);
    static void SetTimeDilation(float timeDilation);

    static nlTaskManager* m_pInstance;

    /* 0x00 */ float mTimeDilation;
    /* 0x04 */ nlTask* mTaskList;
    /* 0x08 */ u32 mCurrentState;
    /* 0x0C */ u32 mPendingState;
    /* 0x10 */ u32 mPreviousState;
    /* 0x14 */ float mCurrentTimeDelta;
    /* 0x18 */ float mRealTimeDelta;
    /* 0x1C */ bool mLocked;
}; // size 0x20

#endif // NL_TASK_H
