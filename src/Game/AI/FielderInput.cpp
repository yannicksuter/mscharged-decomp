#include "NL/nlPrint.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/TeamPlayMachine.h"

#include "Game/MathHelpers.h"
#include "NL/nlMath.h"
#include "NL/nlTicker.h"
#include "NL/nlPrint.h"


class UnidentifiedTimerCountdown
{
public:
    UnidentifiedTimerCountdown(float dt)
        : mDeltaTime(dt)
    {
    }

    void fn_8030F9A4(const unsigned long&, Timer* timer)
    {
        timer->Countdown(mDeltaTime, 0.0f);
    }

    float mDeltaTime;
};

float fn_8030F5DC()
{
    return nlTicksToMilliseconds(nlGetTicker());
}

UnidentifiedFielderInput::~UnidentifiedFielderInput()
{
}

void UnidentifiedFielderInput::fn_8030F74C(
    bool deleteOwner, bool deleteController)
{
    if (deleteController && mUnidentified18 != 0)
    {
        mUnidentified18->UnidentifiedVirtual4(true);
        delete mUnidentified18;
        mUnidentified18 = 0;
    }

    if (deleteOwner)
    {
        delete mUnidentified14;
        mUnidentified14 = 0;
    }
}

void UnidentifiedFielderInput::fn_8030F800(
    bool updateController, float dt)
{
    UnidentifiedTimerCountdown callback(dt);
    mTimers.Walk(&callback, &UnidentifiedTimerCountdown::fn_8030F9A4);

    if (updateController && mUnidentified18 != 0)
    {
        mUnidentified18->UnidentifiedVirtual3(dt);
    }
}

unsigned long UnidentifiedFielderInput::fn_8030F9B4(
    unsigned long key, unsigned long concurrent) const
{
    return concurrent * key;
}

Timer* UnidentifiedFielderInput::fn_8030F9BC(unsigned long key)
{
    Timer* timer = 0;
    mTimers.FindGet(key, &timer);
    return timer;
}

Timer* UnidentifiedFielderInput::fn_8030FA10(
    unsigned long key, float seconds)
{
    Timer* timer = fn_8030F9BC(key);
    if (timer == 0)
    {
        Timer newTimer;
        mTimers.Add(key, newTimer);
        timer = fn_8030F9BC(key);
    }

    if (seconds == 0.0f)
    {
        timer->m_unk0 = timer->m_uPackedTime != 0;
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

bool UnidentifiedFielderInput::fn_8030FB7C(unsigned long key)
{
    Timer* timer = fn_8030F9BC(key);
    return timer != 0 && timer->m_uPackedTime != 0;
}
