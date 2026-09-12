#include "Game/Debug/FrameCounter.h"
#include "Game/Debug/Histogram.h"
#include "Game/GL/GLColourMeshWriter.h"
#include "Game/Task/SmokeTestUpdateTask.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glFont.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "unclassified/tu_802B7798.h"

#include "Game/Debug/TimeRegions.h"
#include "NL/nlColour.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"
#include "unclassified/tu_802BAE84.h"

#include <string.h>
#include "NL/nlstring_tmpl.h"

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

const float lbl_806E6178 = 17.0f;
const float lbl_806E617C = 10.0f;

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

    float (*history)[200] = m_ContinuousFrameHistory;

    totalFrameTime += m_CurrTimer[0];
    m_CurrFrame[0] += m_CurrTimer[0];
    history[0][m_ContinuousFrameHistoryIndex] = m_CurrTimer[0];

    totalFrameTime += m_CurrTimer[1];
    m_CurrFrame[1] += m_CurrTimer[1];
    history[1][m_ContinuousFrameHistoryIndex] = m_CurrTimer[1];

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
            region->m_Histogram.AddSample(totalFrameTime);
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
    fn_802BB11C(GetDebugFontView(), FrameBackgroundColour, 0.0f, 0.0f,
        (float)length, 1.0f, 0.0f, 0, true, true);
    glFontBegin(false);
    glFontPrint(GetDebugFontView(), (eGLView)0, 0, 0, str);
    glFontEnd();
}

static inline TimeRegion* FindGameplayRegion()
{
    ListEntry<TimeRegion*>* entry = TimeRegion::sTimeRegionList.m_Head;
    while (entry != 0)
    {
        TimeRegion* region = entry->entry;
        if (nlStrICmp(region->m_pName, "during gameplay") == 0)
        {
            return region;
        }
        entry = entry->next;
    }
    return 0;
}

void FrameCounter::fn_802B7FD4()
{
    TimeRegion* region = FindGameplayRegion();
    if (region != 0)
    {
        static bool initialized = false;
        if (!initialized)
        {
            HistogramDisplay::GetInstance()->AddHistogram(&region->m_Histogram);
            initialized = true;
        }
        HistogramDisplay::GetInstance()->Draw();
    }
}

void FrameCounter::fn_802B80C4()
{
    TimeRegion* region = FindGameplayRegion();
    if (region != 0)
    {
        char name[128];
        Histogram* data = &region->m_Histogram;
        for (int index = 0; index < data->m_NumBins - 1; ++index)
        {
            float threshold = data->GetBinBoundary(index);
            int count = data->GetCumulativePercentage(index);
            nlSNPrintf(name, sizeof(name), "percent of gameplay frames below %0.0f ms", threshold);
            fn_802BD718(name, 0, (float)count);
        }
    }
}

void FrameCounter::DisplayFrameTicker()
{
    GLColourMeshWriter m0;
    GLColourMeshWriter m1;

    glSetDefaultState(false);

    if (m0.Begin(8, GLP_LineList, 0))
    {
        m0.Colour(255, 255, 0, 255);
        m0.Vertex(0.0f, 32.0f, 0.0f);
        m0.Colour(255, 255, 0, 255);
        m0.Vertex(640.0f, 32.0f, 0.0f);
        m0.Colour(64, 64, 64, 255);
        m0.Vertex(0.0f, 1.25f * 8.333f + 32.0f, 0.0f);
        m0.Colour(64, 64, 64, 255);
        m0.Vertex(640.0f, 1.25f * 8.333f + 32.0f, 0.0f);
        m0.Colour(255, 0, 0, 255);
        m0.Vertex(0.0f, 1.25f * 16.666f + 32.0f, 0.0f);
        m0.Colour(255, 0, 0, 255);
        m0.Vertex(640.0f, 1.25f * 16.666f + 32.0f, 0.0f);
        m0.Colour(0, 255, 0, 255);
        m0.Vertex(0.0f, 1.25f * 33.333f + 32.0f, 0.0f);
        m0.Colour(0, 255, 0, 255);
        m0.Vertex(640.0f, 1.25f * 33.333f + 32.0f, 0.0f);

        if (m0.End())
        {
            if (GetDebugFontView() != 0)
            {
                GetDebugFontView()->AttachModel(m0.GetModel(), 0);
            }
        }
    }

    if (m1.Begin(640, GLP_LineStrip, 0))
    {
        for (unsigned int i = 0; i < 640; i++)
        {
            unsigned int historyLoc = (i + m_NextHistoryPos) % 640;
            m1.Colour(255, 255, 255, 255);
            m1.Vertex((float)i, 1.25f * m_FrameHistory[historyLoc] + 32.0f, 0.0f);
        }

        if (m1.End())
        {
            if (GetDebugFontView() != 0)
            {
                GetDebugFontView()->AttachModel(m1.GetModel(), 0);
            }
        }
    }
}

static void DrawCircle(nlVector3 p0, float fRadius, float fScaleX, nlColour colour)
{
    GLColourMeshWriter mesh;

    glSetDefaultState(true);
    glSetCurrentMatrix(glGetIdentityMatrix());

    int numVerts = 30;
    if (mesh.Begin(numVerts + 1, GLP_TriFan, 0))
    {
        nlVector3 v3point;

        v3point.z = p0.z;
        v3point.x = p0.x;
        v3point.y = p0.y;

        float fRadians = 0.0f;

        mesh.Colour(colour);
        mesh.Vertex(v3point);

        const float angleScale = 10430.378f;
        int i = 0;

        while (i < numVerts)
        {
            nlSinCos(&v3point.x, &v3point.y, (unsigned short)(int)(angleScale * fRadians));
            v3point.x = p0.x + fScaleX * (v3point.x * fRadius);
            v3point.y = p0.y + v3point.y * fRadius;

            mesh.Colour(colour);
            mesh.Vertex(v3point);

            i++;
            fRadians += 6.2831855f / (numVerts - 1);
        }

        if (!mesh.End())
        {
            return;
        }

        GetDebugFontView()->AttachModel(mesh.GetModel(), 2);
    }
}

static void DrawSmile(nlVector3 p0, float fRadius, float fScaleX, nlColour colour, float fLineThickness)
{
    GLColourMeshWriter mesh;
    float degrees = sfSmileAngle;

    glSetDefaultState(true);
    glSetCurrentMatrix(glGetIdentityMatrix());

    float yScale = (2.0f * sfHappiness) + -1.0f;

    int numVerts = 10;
    if (mesh.Begin(numVerts * 2, GLP_TriStrip, 0))
    {
        nlVector3 v3point;

        float fRadians = -((3.1415927f * (0.5f * degrees)) / 180.0f);
        v3point.z = p0.z;
        nlSinCos(&v3point.x, &v3point.y, (u16)(int)(10430.378f * fRadians));

        float fXFromAngle = v3point.x * fRadius;
        v3point.x = (fScaleX * fXFromAngle) + p0.x;
        v3point.y = (v3point.y * fRadius) + p0.y;
        float fYFromAngle = v3point.y;
        float fYTop = p0.y + fRadius;
        float middleY = 0.5f * (fYFromAngle + fYTop);

        degrees = (3.1415927f * degrees) / 180.0f;
        int i = 0;
        while (i < numVerts)
        {
            nlSinCos(&v3point.x, &v3point.y, (u16)(int)(10430.378f * fRadians));

            v3point.x = p0.x + fScaleX * (v3point.x * fRadius);
            v3point.y = p0.y + v3point.y * fRadius;

            v3point.y = v3point.y - middleY;
            v3point.y = v3point.y * yScale;
            v3point.y = v3point.y + middleY;

            mesh.Colour(colour);
            mesh.Vertex(v3point);

            v3point.y += fLineThickness;

            mesh.Colour(colour);
            mesh.Vertex(v3point);

            i++;
            fRadians += degrees / (numVerts - 1);
        }

        if (mesh.End() == 0)
        {
            return;
        }

        GetDebugFontView()->AttachModel(mesh.GetModel(), 2);
    }
}

static void DrawBrow(const nlVector3& leftEyeCentre, const nlVector3& rightEyeCentre, float distanceAboveEye, float width, float height)
{
    GLColourMeshWriter m0;

    glSetDefaultState(false);

    float yScale = (2.0f * sfHappiness) + -1.0f;

    if (m0.Begin(4, GLP_LineList, 0))
    {
        nlVector3 p1;
        nlVector3 p2;

        p1 = leftEyeCentre;
        p2 = leftEyeCentre;

        p1.x = p1.x - width;
        p2.x = p2.x + width;
        p1.y += -distanceAboveEye + (height * yScale);
        p2.y += -distanceAboveEye - (height * yScale);

        m0.Colour(0, 0, 0, 255);
        m0.Vertex(p1.x, p1.y, p1.z);
        m0.Colour(0, 0, 0, 255);
        m0.Vertex(p2.x, p2.y, p2.z);

        p1 = rightEyeCentre;
        p2 = rightEyeCentre;

        p1.x = p1.x + width;
        p2.x = p2.x - width;
        p1.y += -distanceAboveEye + (height * yScale);
        p2.y += -distanceAboveEye - (height * yScale);

        m0.Colour(0, 0, 0, 255);
        m0.Vertex(p1.x, p1.y, p1.z);
        m0.Colour(0, 0, 0, 255);
        m0.Vertex(p2.x, p2.y, p2.z);

        if (m0.End())
        {
            if (GetDebugFontView() != 0)
            {
                GetDebugFontView()->AttachModel(m0.GetModel(), 0);
            }
        }
    }
}

void FrameCounter::DisplayFrameSmiler()
{
    float happiness = 0.0f;
    int i;
    for (i = 0; i < siHappinessLookback; i++)
    {
        float fps = m_FrameHistory[(m_NextHistoryPos - i + 640) % 640] == 0.0f
                      ? 60.0f
                      : 1000.0f / m_FrameHistory[(m_NextHistoryPos - i + 640) % 640];
        happiness += (fps - 30.0f) / 30.0f;
    }
    happiness /= (float)siHappinessLookback;
    if (happiness > 1.0f)
        happiness = 1.0f;
    if (happiness < 0.0f)
        happiness = 0.0f;

    sfHappiness = happiness;

    float circleRadius = sfSmileyRadius;
    float smileRadius = sfSmileRadius * sfSmileyRadius;
    float eyeRadius = sfEyeRadius * sfSmileyRadius;

    nlVector3 circleCentre = { 0, 0, 0 };
    circleCentre.x = 2.0f * sfSmileyRadius;
    circleCentre.y = 480.0f - 3.0f * sfSmileyRadius;

    nlColour black = { 0, 0, 0, 255 };

    nlColour colour;
    if (sfHappiness < 0.5f)
    {
        for (i = 0; i < 4; i++)
        {
            float alpha = 2.0f * sfHappiness;
            colour.c[i] = (int)((float)sMediumColour.c[i] * alpha + (float)sMadColour.c[i] * (1.0f - alpha));
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            float alpha = 2.0f * (sfHappiness - 0.5f);
            colour.c[i] = (int)((float)sHappyColour.c[i] * alpha + (float)sMediumColour.c[i] * (1.0f - alpha));
        }
    }

    nlVector3 leftEyeCentre = { 0, 0, 0 };
    nlVector3 rightEyeCentre = { 0, 0, 0 };

    leftEyeCentre.x = -circleRadius * sfEyeSeparation;
    leftEyeCentre.y = -circleRadius * sfEyeHeight;

    rightEyeCentre.x = circleRadius * sfEyeSeparation;
    rightEyeCentre.y = -circleRadius * sfEyeHeight;

    nlVec3Add(leftEyeCentre, leftEyeCentre, circleCentre);
    nlVec3Add(rightEyeCentre, rightEyeCentre, circleCentre);

    DrawCircle(circleCentre, 3.0f + circleRadius, 1.2f, black);
    DrawCircle(circleCentre, circleRadius, 1.2f, colour);
    DrawCircle(leftEyeCentre, eyeRadius, 1.2f, black);
    DrawCircle(rightEyeCentre, eyeRadius, 1.2f, black);
    DrawSmile(circleCentre, smileRadius, 1.2f, black, 3.0f);
    DrawBrow(leftEyeCentre, rightEyeCentre, 3.0f * eyeRadius, 2.0f * eyeRadius, 1.5f * eyeRadius);
}
