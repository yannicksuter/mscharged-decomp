#include "Game/Debug/FrameCounter.h"
#include "Game/Debug/ProfilerDisplay_802B9A3C.h"
#include "Game/GL/GLColourMeshWriter.h"
#include "Game/Task/SmokeTestUpdateTask.h"
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
    fn_802BB11C(GetDebugFontView(), FrameBackgroundColour, 0.0f, 0.0f,
        (float)length, 1.0f, 0.0f, 0, true, true);
    glFontBegin(false);
    glFontPrint(GetDebugFontView(), (eGLView)0, 0, 0, str);
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
        UnidentifiedTimeRegionData_802B9570* data = &region->m_unk14;
        for (int index = 0; index < data->m_unk08 - 1; ++index)
        {
            float threshold = fn_802B98C8(data, index);
            int count = fn_802B974C(data, index);
            nlSNPrintf(name, sizeof(name), lbl_8052B5A0, threshold);
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
            unsigned int historyLoc = (m_NextHistoryPos + i) % 640;
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
            mesh.Vertex(v3point.x, v3point.y, v3point.z);

            v3point.y += fLineThickness;

            mesh.Colour(colour);
            mesh.Vertex(v3point.x, v3point.y, v3point.z);

            i++;
            fRadians += ((3.1415927f * degrees) / 180.0f) / (numVerts - 1);
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
    if (happiness < 0.5f)
    {
        float alpha = 2.0f * happiness;
        nlColourSet(colour,
            (int)((float)sMediumColour.c[0] * alpha + (float)sMadColour.c[0] * (1.0f - alpha)),
            (int)((float)sMediumColour.c[1] * alpha + (float)sMadColour.c[1] * (1.0f - alpha)),
            (int)((float)sMediumColour.c[2] * alpha + (float)sMadColour.c[2] * (1.0f - alpha)),
            (int)((float)sMediumColour.c[3] * alpha + (float)sMadColour.c[3] * (1.0f - alpha)));
    }
    else
    {
        float alpha = 2.0f * (happiness - 0.5f);
        nlColourSet(colour,
            (int)((float)sHappyColour.c[0] * alpha + (float)sMediumColour.c[0] * (1.0f - alpha)),
            (int)((float)sHappyColour.c[1] * alpha + (float)sMediumColour.c[1] * (1.0f - alpha)),
            (int)((float)sHappyColour.c[2] * alpha + (float)sMediumColour.c[2] * (1.0f - alpha)),
            (int)((float)sHappyColour.c[3] * alpha + (float)sMediumColour.c[3] * (1.0f - alpha)));
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
