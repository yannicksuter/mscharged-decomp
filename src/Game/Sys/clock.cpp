#include "Game/Sys/clock.h"

#include "NL/nlTask.h"

Clock* ClockManager::m_inactiveList = 0;
Clock* ClockManager::m_activeList = 0;
Clock* ClockManager::m_pendingActiveList = 0;
bool ClockManager::m_bUpdatingClocks = false;

void ClockManager::Initialize()
{
}

void ClockManager::Update(float fDeltaT)
{
    ClockCallback callback;
    ClockCallback callback2;

    Clock* pClock;
    Clock* next;

    if (m_activeList != 0)
    {
        m_bUpdatingClocks = true;
        pClock = m_activeList->m_next;

        for (;;)
        {
            next = pClock->m_next;
            if ((pClock->m_clockState == CLOCK_PAUSED)
                || !(pClock->m_uActiveStates & nlTaskManager::m_pInstance->mCurrentState))
            {
                if (pClock != m_activeList)
                {
                    pClock = next;
                    continue;
                }
            }
            else
            {
                pClock->m_fTimer = (float)((fDeltaT * pClock->m_fTimeScale) + pClock->m_fTimer);
                if (pClock->m_fTimeScale >= 0.0f)
                {
                    if (pClock->m_fTimer >= pClock->m_fEndTime)
                    {
                        pClock->m_clockState = CLOCK_DONE;
                        pClock->m_fTimer = (float)pClock->m_fEndTime;
                        callback = pClock->m_callback;
                        if (callback != 0)
                        {
                            callback(pClock->m_uParam1, pClock->m_uParam2);
                        }
                        nlDLRingRemove<Clock>(&m_activeList, pClock);
                        nlDLRingAddEnd<Clock>(&m_inactiveList, pClock);
                    }
                }
                else
                {
                    if (pClock->m_fTimer <= pClock->m_fEndTime)
                    {
                        pClock->m_clockState = CLOCK_DONE;
                        pClock->m_fTimer = (float)pClock->m_fEndTime;
                        callback2 = pClock->m_callback;
                        if (callback2 != 0)
                        {
                            callback2(pClock->m_uParam1, pClock->m_uParam2);
                        }
                        nlDLRingRemove<Clock>(&m_activeList, pClock);
                        nlDLRingAddEnd<Clock>(&m_inactiveList, pClock);
                    }
                }

                if ((pClock != m_activeList) && (m_activeList != 0))
                {
                    pClock = next;
                    continue;
                }
            }

            break;
        }
        m_bUpdatingClocks = false;
        nlDLRingAppendRing<Clock>(&m_activeList, m_pendingActiveList);
        m_pendingActiveList = 0;
    }
}

Clock::Clock(float param1, float param2, float param3, unsigned long param4, ClockCallback callback)
{
    m_fTimeScale = param3;
    m_fTimer = param1;
    m_fEndTime = param2;
    m_clockState = CLOCK_OFF;
    m_callback = callback;
    m_uActiveStates = param4;
    nlDLRingAddEnd<Clock>(&ClockManager::m_inactiveList, this);
}

Clock::~Clock()
{
    if ((nlDLRingRemoveSafely<Clock>(&ClockManager::m_activeList, this) == 0)
        && (nlDLRingRemoveSafely<Clock>(&ClockManager::m_pendingActiveList, this) == 0))
    {
        nlDLRingRemoveSafely<Clock>(&ClockManager::m_inactiveList, this);
    }
}

void Clock::Reset(float param1, float param2, float param3)
{
    m_fTimeScale = param3;
    m_fTimer = param1;
    m_fEndTime = param2;

    if (m_clockState == CLOCK_ON)
    {
        if (nlDLRingRemoveSafely<Clock>(&ClockManager::m_activeList, this) == 0)
        {
            nlDLRingRemoveSafely<Clock>(&ClockManager::m_pendingActiveList, this);
        }
        nlDLRingAddEnd<Clock>(&ClockManager::m_inactiveList, this);
    }
    m_clockState = CLOCK_OFF;
}

void Clock::Start()
{
    if (m_clockState != CLOCK_ON)
    {
        nlDLRingRemove<Clock>(&ClockManager::m_inactiveList, this);
        if (ClockManager::m_bUpdatingClocks != 0)
        {
            nlDLRingAddEnd<Clock>(&ClockManager::m_pendingActiveList, this);
        }
        else
        {
            nlDLRingAddEnd<Clock>(&ClockManager::m_activeList, this);
        }
    }
    m_clockState = CLOCK_ON;
}

void Clock::Stop()
{
    if (m_clockState == CLOCK_ON)
    {
        if (nlDLRingRemoveSafely<Clock>(&ClockManager::m_activeList, this) == 0)
        {
            nlDLRingRemoveSafely<Clock>(&ClockManager::m_pendingActiveList, this);
        }
        nlDLRingAddEnd<Clock>(&ClockManager::m_inactiveList, this);
    }
    m_clockState = CLOCK_OFF;
}
