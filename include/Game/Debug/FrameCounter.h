#ifndef GAME_DEBUG_FRAME_COUNTER_H
#define GAME_DEBUG_FRAME_COUNTER_H

#include "types.h"

class FrameCounter
{
public:
    FrameCounter(const char* first, const char* second);

    void StartTimer(int timerNum);
    void FinishTiming();
    void DisplayFrameRate();
    void fn_802B7FD4();
    void fn_802B80C4();
    void DisplayFrameTicker();
    void DisplayFrameSmiler();

    static int NUM_FRAMES_TO_AVERAGE_OVER;

private:
    /* 0x0000 */ const char* m_FirstName;
    /* 0x0004 */ const char* m_SecondName;
    /* 0x0008 */ u32 m_StartTick;
    /* 0x000C */ int m_CurrTimerNum;
    /* 0x0010 */ float m_CurrTimer[2];
    /* 0x0018 */ float m_CurrFrame[2];
    /* 0x0020 */ float m_LastFrame[2];
    /* 0x0028 */ u32 m_Counter;
    /* 0x002C */ u32 m_NextHistoryPos;
    /* 0x0030 */ float m_FrameHistory[640];
    /* 0x0A30 */ int m_ContinuousFrameHistoryIndex;
    /* 0x0A34 */ float m_ContinuousFrameHistory[2][200];
    /* 0x1074 */ bool m_unk1074;
    /* 0x1075 */ u8 m_pad1075[3];
};

extern FrameCounter g_FrameCounter;

#endif // GAME_DEBUG_FRAME_COUNTER_H
