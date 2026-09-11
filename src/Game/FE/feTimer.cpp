#include "Game/FE/feTimer.h"

#include "NL/nlList.h"
#include "NL/nlMemory.h"

FETimer::FETimer(float duration, const Function<FETimer*>& callback)
    : mCallback(callback)
    , mEnabled(true)
    , mDuration(duration)
    , mElapsed(0.0f)
    , mTickCount(0)
    , next(0)
{
}

void FETimer::SetEnabled(bool enabled)
{
    if (mEnabled == enabled)
    {
        return;
    }

    mEnabled = enabled;
    mElapsed = 0.0f;
}

void FETimer::Update(float dt)
{
    if (!mEnabled)
    {
        return;
    }

    mElapsed += dt;
    if (mElapsed >= mDuration)
    {
        mElapsed -= mDuration;
        ++mTickCount;
        mCallback(this);
    }
}

UnidentifiedTimerList_80306294::UnidentifiedTimerList_80306294()
    : mCurrent(0)
    , mTimers(0, 0)
{
}

UnidentifiedTimerList_80306294::~UnidentifiedTimerList_80306294()
{
    mCurrent = 0;
    nlDeleteList(mTimers);
}

FETimer* UnidentifiedTimerList_80306294::fn_8030639C(
    float duration, const Function<FETimer*>& callback)
{
    FETimer* timer = new (8, false) FETimer(duration, callback);
    nlListAddStart(&mTimers.m_pStart, timer, &mTimers.m_pEnd);
    return timer;
}

void UnidentifiedTimerList_80306294::fn_8030648C(FETimer* timer)
{
    if (mCurrent == timer)
    {
        mCurrent = timer->next;
    }

    nlListRemoveElement(&mTimers.m_pStart, timer, &mTimers.m_pEnd);
    delete timer;
}

void UnidentifiedTimerList_80306294::fn_80306524()
{
    mCurrent = 0;
    nlDeleteList(mTimers);
}

void UnidentifiedTimerList_80306294::fn_803065F0(float dt)
{
    mCurrent = mTimers.m_pStart;
    while (mCurrent != 0)
    {
        FETimer* timer = mCurrent;
        timer->Update(dt);
        if (timer == mCurrent)
        {
            mCurrent = mCurrent->next;
        }
    }
    mCurrent = 0;
}
