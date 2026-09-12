#include "Game/Debug/Histogram.h"
#include "Game/UnidentifiedStaticStorage.h"
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

Histogram::Histogram(
    const char* pName, int numBins, float minValue, float binSize)
    : m_pName(pName)
    , m_NumBins(numBins)
    , m_MinValue(minValue)
    , m_BinSize(binSize)
    , m_NumSamples(0)
{
    m_Bins = (int*)nlMalloc(m_NumBins * sizeof(int), 8, false);
    Reset();
}

Histogram::~Histogram()
{
    delete m_Bins;
}

void Histogram::AddSample(float value)
{
    ++m_NumSamples;
    float minValue = m_MinValue;
    if (value < minValue)
    {
        ++m_Bins[0];
    }
    else
    {
        float binSize = m_BinSize;
        if (value >= (m_NumBins - 2) * binSize + minValue)
        {
            ++m_Bins[m_NumBins - 1];
        }
        else
        {
            float bin = floor((value - minValue) / binSize);
            ++m_Bins[(int)bin + 1];
        }
    }
}

int Histogram::GetCumulativePercentage(int index)
{
    if (m_NumSamples == 0)
    {
        return 0;
    }

    int count = 0;
    for (int i = 0; i <= index; ++i)
    {
        count += m_Bins[i];
    }
    return (int)(100.0f * ((float)count / (float)m_NumSamples));
}

float Histogram::GetBinBoundary(int index)
{
    return m_MinValue + index * m_BinSize;
}

void Histogram::FormatBinRange(char* range, unsigned long size, int index)
{
    float minValue = GetBinBoundary(index - 1);
    float maxValue = minValue + m_BinSize;
    if (index == 0)
    {
        nlSNPrintf(range, size, "< %0.2f", maxValue);
    }
    else if (index == m_NumBins - 1)
    {
        nlSNPrintf(range, size, "> %0.2f", maxValue);
    }
    else
    {
        nlSNPrintf(range, size, "%0.2f - %0.2f", minValue, maxValue);
    }
}

unsigned long sHistogramSpacing = 35;
unsigned long sHistogramWidth = 300;
unsigned long sBarWidth = 100;
unsigned long sLeftMargin = 40;
int sTopMargin = 50;
unsigned long sDefaultBinHeight = 11;

bool sHidden;

static Histogram sVariableA(
    "variable a", 5, 0.1f, 0.3f);
static Histogram sVariableB(
    "variable b", 9, 0.1f, 0.2f);
static Histogram sVariableC(
    "variable c", 3, 0.25f, 0.5f);
unsigned long sBinHeight = sDefaultBinHeight;

static void DrawHistogramRect(nlColour colour,
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

HistogramDisplay* HistogramDisplay::GetInstance()
{
    static HistogramDisplay display;
    return &display;
}

void HistogramDisplay::AddHistogram(Histogram* histogram)
{
    m_Histograms[m_NumHistograms] = histogram;
    ++m_NumHistograms;
}

float HistogramDisplay::GetBinY(int index, float bin)
{
    int y = sTopMargin;
    for (int i = 0; i < index; ++i)
    {
        y += sHistogramSpacing + sBinHeight * m_Histograms[i]->m_NumBins;
    }
    return y + bin * sBinHeight;
}

void HistogramDisplay::Draw()
{
    const nlColour background = { 0, 0, 0, 192 };
    const nlColour barColour = { 196, 0, 0, 255 };
    if (m_NumHistograms == 0 || sHidden)
    {
        return;
    }

    for (int i = 0; i < m_NumHistograms; ++i)
    {
        int numBins = m_Histograms[i]->m_NumBins;
        DrawHistogramRect(background, sLeftMargin, GetBinY(i, -1.0f), sHistogramWidth + (float)sLeftMargin, GetBinY(i, numBins));
    }

    float maxValue = 0.0f;
    for (int i = 0; i < m_NumHistograms; ++i)
    {
        Histogram* data = m_Histograms[i];
        int numBins = data->m_NumBins;
        for (int j = 0; j < numBins; ++j)
        {
            float value = (float)data->m_Bins[j] / data->m_NumSamples;
            if (value > maxValue)
            {
                maxValue = value;
            }
            float top = GetBinY(i, j);
            float bottom = GetBinY(i, j + 1) - 1.0f;
            DrawHistogramRect(barColour, sLeftMargin, top, sLeftMargin + (float)sBarWidth * value, bottom);
        }
    }

    for (int i = 0; i < m_NumHistograms; ++i)
    {
        GLColourMeshWriter writer;
        int numBins = m_Histograms[i]->m_NumBins;
        float x = (float)sLeftMargin + sBarWidth;
        float top = GetBinY(i, -1.0f);
        float bottom = GetBinY(i, numBins);
        if (writer.Begin(2, GLP_LineList, 0))
        {
            writer.Colour(255, 255, 255, 255);
            writer.Vertex(x, top, 0.0f);
            writer.Colour(255, 255, 255, 255);
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
    for (int i = 0; i < m_NumHistograms; ++i)
    {
        Histogram* data = m_Histograms[i];
        int y = GetBinY(i, -1.0f);
        glFontPrint(GetDebugFontView(), (eGLView)0, sLeftMargin, y, colour, data->m_pName);
        char text[80];
        nlSNPrintf(text, sizeof(text), "%5s   %% bin range", "count");
        int x = (float)sBarWidth + sLeftMargin;
        glFontPrint(GetDebugFontView(), (eGLView)0, x, y, colour, text);
        int numBins = data->m_NumBins;
        for (int j = 0; j < numBins; ++j)
        {
            int percentage = 100.0 * ((float)data->m_Bins[j] / data->m_NumSamples);
            char range[64];
            data->FormatBinRange(range, sizeof(range), j);
            nlSNPrintf(text, sizeof(text), "%5d %2d%% %s", data->m_Bins[j], percentage, range);
            int x = (float)sBarWidth + sLeftMargin;
            int y = GetBinY(i, j);
            glFontPrint(GetDebugFontView(), (eGLView)0, x, y, colour, text);
        }
    }
    glFontVirtualCoordinates(true);
    glFontEnd();
}
