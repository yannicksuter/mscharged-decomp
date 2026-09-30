#include "NL/nlPrint.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/TeamPlayMachine.h"

#include "Game/MathHelpers.h"
#include "NL/nlMath.h"
#include "NL/nlTicker.h"
#include "NL/nlPrint.h"


class TimerCountdown
{
public:
    TimerCountdown(float deltaTime)
        : mDeltaTime(deltaTime)
    {
    }

    void UpdateTimer(const unsigned long&, Timer* timer)
    {
        timer->Countdown(mDeltaTime, 0.0f);
    }

    float mDeltaTime;
};

float GetTickerMilliseconds()
{
    return nlTicksToMilliseconds(nlGetTicker());
}

AIContext::~AIContext()
{
    mTimers.Clear();
}

void AIContext::Cleanup(
    bool deleteRuntime, bool deleteScriptMachine)
{
    if (deleteScriptMachine && mScriptMachine != 0)
    {
        mScriptMachine->Reset(true);
        delete mScriptMachine;
        mScriptMachine = 0;
    }

    if (deleteRuntime)
    {
        delete mRuntime;
        mRuntime = 0;
    }
}

void AIContext::Update(
    bool updateScriptMachine, float deltaTime)
{
    TimerCountdown callback(deltaTime);
    mTimers.Walk(&callback, &TimerCountdown::UpdateTimer);

    if (updateScriptMachine && mScriptMachine != 0)
    {
        mScriptMachine->Update(deltaTime);
    }
}

unsigned long AIContext::GetTimerKey(
    unsigned long key, unsigned long multiplier) const
{
    return multiplier * key;
}

Timer* AIContext::FindTimer(unsigned long key)
{
    Timer* timer = 0;
    mTimers.FindGet(key, &timer);
    return timer;
}

Timer* AIContext::SetTimer(
    unsigned long key, float seconds)
{
    Timer* timer = FindTimer(key);
    if (timer == 0)
    {
        Timer newTimer;
        mTimers.Add(key, newTimer);
        timer = FindTimer(key);
    }

    if (seconds == 0.0f)
    {
        timer->m_uWasRunning = timer->m_uPackedTime != 0;
        timer->m_uPackedTime = 0;
    }
    else
    {
        float jitter = nlMinEquals(0.185f * seconds, 0.1f);
        timer->SetSeconds(
            seconds + (2.0f * jitter * nlRandomf(1.0f) - jitter));
    }

    return timer;
}

bool AIContext::IsTimerRunning(unsigned long key)
{
    Timer* timer = FindTimer(key);
    return timer != 0 && timer->m_uPackedTime != 0;
}

float (*lbl_806DF560)() = GetTickerMilliseconds;
float (*lbl_806DF564)() = GetTickerMilliseconds;
