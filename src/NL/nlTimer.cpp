#include "NL/nlTimer.h"

void Timer::SetSeconds(float seconds)
{
    m_unk0 = m_uPackedTime != 0;
    m_uPackedTime = (u32)(1024.0f * seconds + 0.5f);
}

f32 Timer::GetSeconds() const
{
    return m_uPackedTime / 1024.0f;
}

bool Timer::Countdown(float dt, float thresh)
{
    m_unk0 = m_uPackedTime != 0;
    if (m_unk0 != 0)
    {
        const u32 subTicks = (u32)(1024.0f * dt + 0.5f);
        if (subTicks > m_uPackedTime)
            m_uPackedTime = 0;
        else
            m_uPackedTime = m_uPackedTime - subTicks;

        const f32 seconds = (f32)m_uPackedTime / 1024.0f;
        return seconds <= thresh;
    }
    return true;
}

bool Timer::Countup(float dt, float thresh)
{
    m_unk0 = m_uPackedTime != 0;
    const u32 addTicks = (u32)(1024.0f * dt + 0.5f);

    m_uPackedTime = m_uPackedTime + addTicks;

    const f32 seconds = (f32)m_uPackedTime / 1024.0f;
    return seconds >= thresh;
}
