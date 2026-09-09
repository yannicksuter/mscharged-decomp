#ifndef GAME_FIXED_UPDATE_TASK_H
#define GAME_FIXED_UPDATE_TASK_H

#include "Game/Task/DispatchEventsTask.h"
#include "Game/InputFrameProvider.h"
#include "NL/nlTask.h"

extern float g_fFixedUpdateTick;
extern float g_fSimulationTick;
extern bool g_bRunSimAndRenderInLockStep;

class FixedUpdateTask : public nlTask, public InputFrameProvider
{
public:
    FixedUpdateTask()
    {
        mUnidentified28 = mAccumulatedDeltaT = g_fFixedUpdateTick;
        mSimulationTime = 0.0f;
        mTimeScale = 1.0f;
        mfFrameLockTime = 0.0f;
        mFrame = 0;
        mUnidentified38 = false;

        mEventDispatcher.Clear();
        BasicSlotPool<DLListEntry<EventCallback> >* pool = &mEventDispatcher.callbacks.m_Allocator;
        pool->FreeBlocks();
    }

    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Game Fixed Update";
    }

    virtual int GetFrame()
    {
        return mFrame;
    }
    virtual float GetFixedUpdateMilliseconds();
    virtual u32 CalculateChecksum();
    virtual u32 WriteSyncLog();
    virtual void OnSyncError();
    virtual void OnInputQueueOverflow();
    virtual u16 GetInputRemapAngle();
    virtual bool IsInPauseMenu();

    void Reset();

    static void SetTimeScale(float timeScale);
    static float GetTargetTimeScale();
    static void SetTimeScale(float timeScale, float transitionTime);
    static float GetTimeScale();
    static void SetFrameLock(float frameLockTime);
    static void DecrementFrameLock(float fDeltaT);
    static float GetPhysicsUpdateTick();

    void CallFixedUpdateTasks();

    /* 0x24 */ float mAccumulatedDeltaT;
    /* 0x28 */ float mUnidentified28;
    /* 0x2C */ float mSimulationTime;
    /* 0x30 */ float mfFrameLockTime;
    /* 0x34 */ u32 mFrame;
    /* 0x38 */ bool mUnidentified38;
    /* 0x3C */ EventDispatcher mEventDispatcher;
    /* 0x60 */ float mTimeScale;
    /* 0x64 */ float mTimeScaleTransitionTime;
    /* 0x68 */ float mTimeScaleTransitionRemaining;
    /* 0x6C */ float mTimeScaleTransitionStart;
    /* 0x70 */ float mTargetTimeScale;
};


FixedUpdateTask* GetFixedUpdateTask();
EventDispatcher* GetFixedUpdateEventDispatcher();
void fn_80111654();
void fn_80111658(bool);
void fn_8011165C();
void fn_80111660();
bool fn_80111664();

#endif // GAME_FIXED_UPDATE_TASK_H
