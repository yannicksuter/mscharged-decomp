#include "Game/Debug/FrameCounter.h"
#include "NL/gl/glFont.h"

#include "Game/Debug/TimeRegions.h"
#include "NL/nlColour.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"

#include <string.h>

extern "C" void fn_802B9670(TimeRegionData* data, float value);
extern "C" void* fn_802B77A0();
extern "C" void fn_802BB11C(void* font, const nlColour& colour, float x, float y, float width, float height, float depth, int align, bool drawBackground, bool clip);
extern "C" void* fn_802B9A3C();
extern "C" void fn_802B9A6C(void* display, TimeRegionData* data);
extern "C" void fn_802B9A88(void* display);
extern "C" int fn_802B974C(TimeRegionData* data, int index);
extern "C" float fn_802B98C8(TimeRegionData* data, int index);
extern "C" void fn_802BD718(const char* name, bool enabled, float value);
extern "C" const char lbl_8052B590[];
extern "C" const char lbl_8052B5A0[];

FrameCounter* lbl_806E1DC0;

int FrameCounter::NUM_FRAMES_TO_AVERAGE_OVER = 30;

static float sfHappiness = 1.0f;
static float sfEyeHeight = 0.2f;
static float sfEyeSeparation = 0.4f;
static float sfSmileyRadius = 15.0f;
static float sfSmileRadius = 0.6f;
static float sfEyeRadius = 0.15f;
static float sfSmileAngle = 90.0f;
static int siHappinessLookback = 60;
static nlColour sMadColour = { 140, 48, 0, 255 };
static nlColour sMediumColour = { 192, 192, 0, 255 };
static nlColour sHappyColour = { 0, 192, 0, 255 };
static nlColour FrameBackgroundColour = { 0, 0, 0, 128 };
static const char* FrameCounterOutputString = "%2.2fFPS %0.3fms [%0.3fms %s, %0.3fms %s]";

nlListContainer<TimeRegion*> TimeRegion::sTimeRegionList;

FrameCounter::FrameCounter(const char* first, const char* second)
{
    lbl_806E1DC0 = this;
    m_FirstName = first;
    m_SecondName = second;

    m_Counter = 0;
    m_NextHistoryPos = 0;
    m_ContinuousFrameHistoryIndex = 0;
    m_unk1074 = false;

    m_CurrTimer[0] = 0.0f;
    m_CurrTimer[1] = 0.0f;
    m_CurrTimerNum = -1;

    memset(m_FrameHistory, 0, sizeof(m_FrameHistory));
    memset(m_ContinuousFrameHistory, 0, sizeof(m_ContinuousFrameHistory));
}

void FrameCounter::StartTimer(int timerNum)
{
    u32 currentTick = nlGetTicker();

    if (m_CurrTimerNum != -1)
    {
        m_CurrTimer[m_CurrTimerNum] += nlGetTickerDifference(m_StartTick, currentTick);
    }

    m_StartTick = currentTick;
    m_CurrTimerNum = timerNum;
}

void FrameCounter::FinishTiming()
{
    u32 currentTick = nlGetTicker();
    if (m_CurrTimerNum != -1)
    {
        m_CurrTimer[m_CurrTimerNum] += nlGetTickerDifference(m_StartTick, currentTick);
    }

    m_Counter++;

    float totalFrameTime = 0.0f;

    totalFrameTime += m_CurrTimer[0];
    m_CurrFrame[0] += m_CurrTimer[0];
    m_ContinuousFrameHistory[0][m_ContinuousFrameHistoryIndex] = m_CurrTimer[0];

    totalFrameTime += m_CurrTimer[1];
    m_CurrFrame[1] += m_CurrTimer[1];
    m_ContinuousFrameHistory[1][m_ContinuousFrameHistoryIndex] = m_CurrTimer[1];

    if (m_Counter >= (u32)NUM_FRAMES_TO_AVERAGE_OVER)
    {
        m_LastFrame[0] = m_CurrFrame[0] / (float)m_Counter;
        m_CurrFrame[0] = 0.0f;
        m_LastFrame[1] = m_CurrFrame[1] / (float)m_Counter;
        m_CurrFrame[1] = 0.0f;
        m_Counter = 0;
    }

    m_FrameHistory[m_NextHistoryPos] = totalFrameTime;
    m_NextHistoryPos = (m_NextHistoryPos + 1) % 640;
    m_ContinuousFrameHistoryIndex = (m_ContinuousFrameHistoryIndex + 1) % 200;

    nlListIterator<TimeRegion*> iterator = TimeRegion::sTimeRegionList.Begin();
    while (iterator.IsValid())
    {
        TimeRegion* region = iterator.Current();
        if (region->m_pConditionFunc())
        {
            region->m_unk10++;
            fn_802B9670(&region->m_unk14, totalFrameTime);
            region->m_fThreshold += totalFrameTime;
        }

        iterator.Next();
    }

    m_CurrTimer[0] = 0.0f;
    m_CurrTimer[1] = 0.0f;
    m_CurrTimerNum = -1;
}

void FrameCounter::DisplayFrameRate()
{
    char str[64];
    float totalTime = m_LastFrame[0] + m_LastFrame[1];
    float frameRate;

    if (totalTime != 0.0f)
    {
        frameRate = 1000.0f / totalTime;
    }
    else
    {
        frameRate = totalTime;
    }

    nlSNPrintf(str, sizeof(str), FrameCounterOutputString,
        frameRate, totalTime,
        m_LastFrame[0], m_FirstName, m_LastFrame[1], m_SecondName);

    unsigned long length = nlStrLen(str);
    fn_802BB11C(fn_802B77A0(), FrameBackgroundColour, 0.0f, 0.0f,
        (float)length, 1.0f, 0.0f, 0, true, true);
    glFontBegin(false);
    glFontPrint(fn_802B77A0(), (eGLView)0, 0, 0, str);
    glFontEnd();
}

void FrameCounter::fn_802B7FD4()
{
    ListEntry<TimeRegion*>* entry = TimeRegion::sTimeRegionList.m_Head;
    TimeRegion* region = 0;
    while (entry != 0)
    {
        TimeRegion* current = entry->entry;
        if (nlStrICmp(current->m_pName, lbl_8052B590) == 0)
        {
            region = current;
            break;
        }
        entry = entry->next;
    }

    if (region != 0)
    {
        static bool initialized = false;
        if (!initialized)
        {
            fn_802B9A6C(fn_802B9A3C(), &region->m_unk14);
            initialized = true;
        }
        fn_802B9A88(fn_802B9A3C());
    }
}

void FrameCounter::fn_802B80C4()
{
    ListEntry<TimeRegion*>* entry = TimeRegion::sTimeRegionList.m_Head;
    TimeRegion* region = 0;
    while (entry != 0)
    {
        TimeRegion* current = entry->entry;
        if (nlStrICmp(current->m_pName, lbl_8052B590) == 0)
        {
            region = current;
            break;
        }
        entry = entry->next;
    }

    if (region != 0)
    {
        char name[128];
        TimeRegionData* data = &region->m_unk14;
        for (int index = 0; index < data->m_unk08 - 1; ++index)
        {
            float threshold = fn_802B98C8(data, index);
            int count = fn_802B974C(data, index);
            nlSNPrintf(name, sizeof(name), lbl_8052B5A0, threshold);
            fn_802BD718(name, false, (float)count);
        }
    }
}
