#include "NL/nlDebugString.h"
#include "Game/Debug/ProfilerDisplay_802B9A3C.h"
#include "Game/GL/GLColourMeshWriter.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glFont.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_802B7798.h"

#include <math.h>

unsigned long lbl_806DF2A8 = 35;
unsigned long lbl_806DF2AC = 300;
unsigned long lbl_806DF2B0 = 100;
unsigned long lbl_806DF2B4 = 40;
int lbl_806DF2B8 = 50;
unsigned long lbl_806DF2BC = 11;

bool lbl_806E1DD0;

static UnidentifiedTimeRegionData_802B9570 lbl_8057C114(
    "variable a", 5, 0.1f, 0.3f);
static UnidentifiedTimeRegionData_802B9570 lbl_8057C13C(
    "variable b", 9, 0.1f, 0.2f);
static UnidentifiedTimeRegionData_802B9570 lbl_8057C164(
    "variable c", 3, 0.25f, 0.5f);
unsigned long lbl_806E1DD4 = lbl_806DF2BC;

const char* nlLookupDebugString(void*, unsigned long)
{
    return "unknown";
}

UnidentifiedTimeRegionData_802B9570::UnidentifiedTimeRegionData_802B9570(
    const char* pName, int numBins, float minValue, float binSize)
    : m_unk04(pName)
    , m_unk08(numBins)
    , m_unk0C(minValue)
    , m_unk10(binSize)
    , m_unk18(0)
{
    m_unk14 = (int*)nlMalloc(numBins * sizeof(int), 8, false);
    for (int i = 0; i < m_unk08; ++i)
    {
        m_unk14[i] = 0;
    }
    m_unk18 = 0;
}

UnidentifiedTimeRegionData_802B9570::~UnidentifiedTimeRegionData_802B9570()
{
    delete m_unk14;
}

extern "C" void fn_802B9670(
    UnidentifiedTimeRegionData_802B9570* data, float value)
{
    ++data->m_unk18;
    float minValue = data->m_unk0C;
    if (value < minValue)
    {
        ++data->m_unk14[0];
    }
    else
    {
        float binSize = data->m_unk10;
        if (value >= (data->m_unk08 - 2) * binSize + minValue)
        {
            ++data->m_unk14[data->m_unk08 - 1];
        }
        else
        {
            float bin = floor((value - minValue) / binSize);
            ++data->m_unk14[(int)bin + 1];
        }
    }
}

extern "C" int fn_802B974C(
    UnidentifiedTimeRegionData_802B9570* data, int index)
{
    if (data->m_unk18 == 0)
    {
        return 0;
    }

    int count = 0;
    for (int i = 0; i <= index; ++i)
    {
        count += data->m_unk14[i];
    }
    return (int)(100.0f * ((float)count / (float)data->m_unk18));
}

extern "C" float fn_802B98C8(
    UnidentifiedTimeRegionData_802B9570* data, int index)
{
    return data->m_unk0C + index * data->m_unk10;
}

extern "C" void fn_802B98FC(const nlColour& colour,
    float x, float y, float right, float bottom)
{
    glPoly2 poly;
    float depth = 0.0f;
    float width = right - x;
    float height = bottom - y;
    glSetDefaultState(false);
    if (colour.c[3] != 0xFF)
    {
        glSetRasterState(GLS_AlphaBlend, 1);
        glSetRasterState(GLS_AlphaTest, 1);
        glSetRasterState(GLS_AlphaTestRef, 0);
        glSetCurrentRasterState(glHandleizeRasterState());
    }

    nlVec2Set(poly.m_pos[0], x, y);
    nlVec2Set(poly.m_pos[1], x, y + height);
    nlVec2Set(poly.m_pos[2], x + width, y + height);
    nlVec2Set(poly.m_pos[3], x + width, y);
    poly.m_colour[0] = poly.m_colour[1] = poly.m_colour[2]
        = poly.m_colour[3] = colour;
    poly.depth = depth;
    poly.Attach(GetDebugFontView(), 0, 0);
}

extern "C" ProfilerDisplay_802B9A3C* fn_802B9A3C()
{
    static ProfilerDisplay_802B9A3C display;
    return &display;
}

extern "C" void fn_802B9A6C(ProfilerDisplay_802B9A3C* display,
    UnidentifiedTimeRegionData_802B9570* data)
{
    display->data[display->count] = data;
    ++display->count;
}

inline float ProfilerDisplay_802B9A3C::fn_802B9B60(int index, float bin)
{
    int y = lbl_806DF2B8;
    for (int i = 0; i < index; ++i)
    {
        y += lbl_806DF2A8 + lbl_806E1DD4 * data[i]->m_unk08;
    }
    return y + bin * lbl_806E1DD4;
}

extern "C" void fn_802B9A88(ProfilerDisplay_802B9A3C* display)
{
    const nlColour background = { 0, 0, 0, 192 };
    const nlColour barColour = { 196, 0, 0, 255 };
    if (display->count == 0 || lbl_806E1DD0)
    {
        return;
    }

    for (int i = 0; i < display->count; ++i)
    {
        int numBins = display->data[i]->m_unk08;
        fn_802B98FC(background, lbl_806DF2B4, display->fn_802B9B60(i, -1.0f), (float)lbl_806DF2B4 + lbl_806DF2AC, display->fn_802B9B60(i, numBins));
    }

    float maxValue = 0.0f;
    for (int i = 0; i < display->count; ++i)
    {
        UnidentifiedTimeRegionData_802B9570* data = display->data[i];
        int numBins = data->m_unk08;
        for (int j = 0; j < numBins; ++j)
        {
            float value = (float)data->m_unk14[j] / data->m_unk18;
            if (value > maxValue)
            {
                maxValue = value;
            }
            fn_802B98FC(barColour, lbl_806DF2B4, display->fn_802B9B60(i, j), (float)lbl_806DF2B4 + lbl_806DF2B0 * value, display->fn_802B9B60(i, j + 1) - 1.0f);
        }
    }

    for (int i = 0; i < display->count; ++i)
    {
        GLColourMeshWriter writer;
        int numBins = display->data[i]->m_unk08;
        float x = (float)lbl_806DF2B4 + lbl_806DF2B0;
        float top = display->fn_802B9B60(i, -1.0f);
        float bottom = display->fn_802B9B60(i, numBins);
        if (writer.Begin(2, GLP_LineList, 0))
        {
            nlColour colour;
            nlColourSet(colour, 255, 255, 255, 255);
            writer.Colour(colour);
            writer.Vertex(x, top, 0.0f);
            nlColourSet(colour, 255, 255, 255, 255);
            writer.Colour(colour);
            writer.Vertex(x, bottom, 0.0f);
            if (writer.End() && GetDebugFontView() != 0)
            {
                GetDebugFontView()->AttachModel(writer.GetModel(), 0);
            }
        }
    }

    glSetDefaultState(false);
    nlColour colour;
    nlColourSet(colour, 255, 255, 255, 255);
    glFontSetFont(0);
    glFontBegin(false);
    glFontVirtualCoordinates(false);
    for (int i = 0; i < display->count; ++i)
    {
        UnidentifiedTimeRegionData_802B9570* data = display->data[i];
        int y = display->fn_802B9B60(i, -1.0f);
        glFontPrint(GetDebugFontView(), (eGLView)0, lbl_806DF2B4, y, colour, data->m_unk04);
        char text[80];
        nlSNPrintf(text, sizeof(text), "%5s   %% bin range", "count");
        glFontPrint(GetDebugFontView(), (eGLView)0, (float)lbl_806DF2B4 + lbl_806DF2B0, y, colour, text);
        int numBins = data->m_unk08;
        for (int j = 0; j < numBins; ++j)
        {
            int percentage = 100.0 * ((float)data->m_unk14[j] / data->m_unk18);
            float minValue = fn_802B98C8(data, j - 1);
            float maxValue = minValue + data->m_unk10;
            char range[64];
            if (j == 0)
            {
                nlSNPrintf(range, sizeof(range), "< %0.2f", maxValue);
            }
            else if (j == data->m_unk08 - 1)
            {
                nlSNPrintf(range, sizeof(range), "> %0.2f", maxValue);
            }
            else
            {
                nlSNPrintf(range, sizeof(range), "%0.2f - %0.2f", minValue, maxValue);
            }
            nlSNPrintf(text, sizeof(text), "%5d %2d%% %s", data->m_unk14[j], percentage, range);
            glFontPrint(GetDebugFontView(), (eGLView)0, (float)lbl_806DF2B4 + lbl_806DF2B0, display->fn_802B9B60(i, j), colour, text);
        }
    }
    glFontVirtualCoordinates(true);
    glFontEnd();
}
