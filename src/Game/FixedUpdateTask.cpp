#include "Game/FixedUpdateTask.h"

#include "Game/AI/AiUtil.h"
#include "Game/NetworkSession.h"
#include "Game/Pad/FlickDetection.h"
#include "types.h"

float g_fFixedUpdateTick = 0.02f;
bool g_bRunSimAndRenderInLockStep;
float g_fSimulationTick = g_fFixedUpdateTick;

extern u16 m_aJoystickRemap__14cCameraManager;
extern "C" bool lbl_806E180D;
extern "C" void* fn_80284A58();
extern "C" bool fn_80287AB0(void*);
extern "C" void fn_802C084C(void*, int);
extern "C" void fn_802C07AC(void*, float);
extern "C" void fn_8037537C(void*);
extern "C" int fn_803328B4(void*);
extern "C" void fn_803328FC(void*);
extern "C" bool fn_80332A00(void*);

extern void* lbl_806E1E28;
extern void* lbl_806E2478;
extern void* lbl_806E2138;

extern "C" void fn_80111654()
{
}

extern "C" void fn_80111658()
{
}

extern "C" void fn_8011165C()
{
}

extern "C" void fn_80111660()
{
}

extern "C" bool fn_80111664()
{
    return false;
}

static FixedUpdateTask lbl_8056E35C;

extern "C" FixedUpdateTask* fn_8011166C()
{
    return &lbl_8056E35C;
}

extern "C" EventDispatcher* fn_80111678()
{
    return &lbl_8056E35C.mEventDispatcher;
}

void FixedUpdateTask::Reset()
{
    mAccumulatedDeltaT = g_fFixedUpdateTick;
    mUnidentified28 = g_fFixedUpdateTick;
    mSimulationTime = 0.0f;
    mTimeScale = 1.0f;
    mfFrameLockTime = 0.0f;
    mFrame = 0;
    mUnidentified38 = false;

    mEventDispatcher.Clear();
    mEventDispatcher.callbacks.m_Allocator.FreeBlocks();
}

float FixedUpdateTask::GetFixedUpdateMilliseconds()
{
    return 1000.0f * g_fFixedUpdateTick;
}

u16 FixedUpdateTask::UnidentifiedVirtual18()
{
    return m_aJoystickRemap__14cCameraManager - 0x4000;
}

bool FixedUpdateTask::UnidentifiedVirtual1C()
{
    return lbl_806E180D;
}

const char* FixedUpdateTask::GetName()
{
    return "Game Fixed Update";
}

float FixedUpdateTask::GetPhysicsUpdateTick()
{
    return g_fSimulationTick;
}

void FixedUpdateTask::SetTimeScale(float timeScale)
{
    lbl_8056E35C.mTimeScale = timeScale;
    lbl_8056E35C.mTargetTimeScale = timeScale;
}

float FixedUpdateTask::GetTargetTimeScale()
{
    return lbl_8056E35C.mTargetTimeScale;
}

void FixedUpdateTask::SetTimeScale(float timeScale, float transitionTime)
{
    lbl_8056E35C.mTimeScaleTransitionTime = transitionTime;
    lbl_8056E35C.mTimeScaleTransitionRemaining = transitionTime;
    lbl_8056E35C.mTimeScaleTransitionStart = lbl_8056E35C.mTimeScale;
    lbl_8056E35C.mTargetTimeScale = timeScale;
}

float FixedUpdateTask::GetTimeScale()
{
    return lbl_8056E35C.mTimeScale;
}

void FixedUpdateTask::SetFrameLock(float frameLockTime)
{
    lbl_8056E35C.mfFrameLockTime = frameLockTime;
    nlTaskManager::SetNextState(1);
}

void FixedUpdateTask::DecrementFrameLock(float fDeltaT)
{
    lbl_8056E35C.mfFrameLockTime -= fDeltaT;
    if (lbl_8056E35C.mfFrameLockTime < 0.0f)
    {
        if (nlTaskManager::m_pInstance->mCurrentState == 1
            && nlTaskManager::m_pInstance->mPendingState != 16)
        {
            nlTaskManager::SetNextState(2);
        }
        lbl_8056E35C.mfFrameLockTime = 0.0f;
    }
}

void FixedUpdateTask::Run(float dt)
{
    bool runFixedUpdate = !fn_80287AB0(fn_80284A58());
    if (!mUnidentified38)
    {
        runFixedUpdate = false;
    }
    if (nlTaskManager::m_pInstance->mPendingState == 16)
    {
        runFixedUpdate = false;
    }

    if (runFixedUpdate
        && nlTaskManager::m_pInstance->mCurrentState == 2
        && !lbl_806E10EC->fn_80123A00())
    {
        if (mTimeScaleTransitionRemaining != 0.0f)
        {
            mTimeScaleTransitionRemaining -= dt * mTimeScale;
            if (mTimeScaleTransitionRemaining < 0.0f)
            {
                mTimeScaleTransitionRemaining = 0.0f;
            }

            float percent = 1.0f
                - mTimeScaleTransitionRemaining / mTimeScaleTransitionTime;
            mTimeScale = Interpolate(
                mTimeScaleTransitionStart, mTargetTimeScale, percent);
        }

        mAccumulatedDeltaT += dt * mTimeScale;
        mUnidentified28 = mAccumulatedDeltaT;

        while (g_bRunSimAndRenderInLockStep
            || mAccumulatedDeltaT >= g_fFixedUpdateTick)
        {
            fn_802C084C(lbl_806E1E28, 1);
            fn_8037537C(lbl_806E2478);
            fn_802C07AC(lbl_806E1E28, g_fFixedUpdateTick);
            FlickDetection::Update();

            mAccumulatedDeltaT -= g_fFixedUpdateTick;
            if (g_bRunSimAndRenderInLockStep)
            {
                mAccumulatedDeltaT = 0.0f;
            }

            int updateCount = fn_803328B4(lbl_806E2138);
            fn_803328FC(lbl_806E2138);

            bool updated = false;
            for (int i = 0; i < updateCount; ++i)
            {
                if (fn_80332A00(lbl_806E2138))
                {
                    CallFixedUpdateTasks();
                    updated = true;
                }
                if (fn_80287AB0(fn_80284A58()))
                {
                    break;
                }
            }

            mUnidentified28 = updated ? mAccumulatedDeltaT : g_fFixedUpdateTick;

            if (fn_80287AB0(fn_80284A58())
                || g_bRunSimAndRenderInLockStep)
            {
                break;
            }
        }
    }

    fn_802C084C(lbl_806E1E28, 0);
    fn_8037537C(lbl_806E2478);
    fn_802C07AC(lbl_806E1E28, dt);
    FlickDetection::Update();
}

EventDispatcher::~EventDispatcher()
{
    callbacks.m_Allocator.FreeBlocks();
}

void EventDispatcherBase::Clear()
{
    if (!state.fields.dispatching)
    {
        nlDLListIterator<EventCallback> iterator = callbacks.Begin();
        while (iterator.hasNext())
        {
            (*iterator)(false);
            iterator.next();
        }

        callbacks.Clear();
        state.fields.callbackCount = 0;
    }
}

void EventDispatcherBase::Add(const EventCallback& callback)
{
    callbacks.AddEnd(callback);
    state.fields.callbackCount++;
}
