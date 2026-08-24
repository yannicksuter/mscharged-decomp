#ifndef GAME_SYS_CLOCK_H
#define GAME_SYS_CLOCK_H

#include "NL/nlDLRing.h"

enum eClockState
{
    CLOCK_OFF = 0,
    CLOCK_ON = 1,
    CLOCK_DONE = 2,
    CLOCK_PAUSED = 3,
};

typedef void (*ClockCallback)(unsigned long, unsigned long);

class Clock
{
public:
    Clock(float param1, float param2, float param3, unsigned long param4, ClockCallback callback);
    virtual ~Clock();

    void Reset(float param1, float param2, float param3);
    void Start();
    void Stop();

    /* 0x04 */ float m_fTimeScale;
    /* 0x08 */ float m_fTimer;
    /* 0x0C */ float m_fEndTime;
    /* 0x10 */ ClockCallback m_callback;
    /* 0x14 */ unsigned long m_uActiveStates;
    /* 0x18 */ eClockState m_clockState;
    /* 0x1C */ unsigned long m_uParam1;
    /* 0x20 */ unsigned long m_uParam2;
    /* 0x24 */ Clock* mNext;
    /* 0x28 */ Clock* mPrev;
}; // size 0x2C

class ClockManager
{
public:
    static void Initialize();
    static void Update(float fDeltaT);

    static Clock* m_inactiveList;
    static Clock* m_activeList;
    static Clock* m_pendingActiveList;
    static bool m_bUpdatingClocks;
};

#endif // GAME_SYS_CLOCK_H
