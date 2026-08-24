#ifndef _NLTIMER_H_
#define _NLTIMER_H_

#include "types.h"

class Timer
{
public:
    Timer(f32 seconds = 0.0f)
        : m_uPackedTime(0)
    {
        SetSeconds(seconds);
    }

    bool Countup(float dt, float thresh);
    bool Countdown(float dt, float thresh);
    f32 GetSeconds() const;
    void SetSeconds(float seconds);

    u32 m_unk0;
    u32 m_uPackedTime;
};

#endif // _NLTIMER_H_
