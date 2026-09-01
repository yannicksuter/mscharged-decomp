#ifndef GAME_FIXED_UPDATE_TASK_H
#define GAME_FIXED_UPDATE_TASK_H

#include "Game/Task/DispatchEventsTask.h"
#include "NL/nlTask.h"

class UnidentifiedFixedUpdateTaskBase
{
public:
    virtual u32 GetFrame() = 0;
    virtual float GetFixedUpdateMilliseconds() = 0;
    virtual u32 CalculateChecksum() = 0;
    virtual u32 WriteSyncLog() = 0;
    virtual void UnidentifiedVirtual10() = 0;
    virtual void UnidentifiedVirtual14() = 0;
    virtual u16 UnidentifiedVirtual18() = 0;
    virtual bool UnidentifiedVirtual1C() = 0;
};

class FixedUpdateTask : public nlTask, public UnidentifiedFixedUpdateTaskBase
{
public:
    FixedUpdateTask()
    {
        Reset();
    }

    virtual void Run(float dt);
    virtual const char* GetName();

    virtual u32 GetFrame();
    virtual float GetFixedUpdateMilliseconds();
    virtual u32 CalculateChecksum();
    virtual u32 WriteSyncLog();
    virtual void UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14();
    virtual u16 UnidentifiedVirtual18();
    virtual bool UnidentifiedVirtual1C();

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

extern float g_fFixedUpdateTick;
extern float g_fSimulationTick;
extern bool g_bRunSimAndRenderInLockStep;

#endif // GAME_FIXED_UPDATE_TASK_H
