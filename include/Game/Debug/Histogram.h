#ifndef GAME_DEBUG_HISTOGRAM_H
#define GAME_DEBUG_HISTOGRAM_H

class Histogram
{
public:
    Histogram(const char* pName, int numBins, float minValue, float binSize);
    virtual ~Histogram();

    void Reset()
    {
        for (int i = 0; i < m_NumBins; ++i)
        {
            m_Bins[i] = 0;
        }
        m_NumSamples = 0;
    }

    void AddSample(float value);
    int GetCumulativePercentage(int index);
    float GetBinBoundary(int index);
    void FormatBinRange(char* range, unsigned long size, int index);

    /* 0x04 */ const char* m_pName;
    /* 0x08 */ int m_NumBins;
    /* 0x0C */ float m_MinValue;
    /* 0x10 */ float m_BinSize;
    /* 0x14 */ int* m_Bins;
    /* 0x18 */ int m_NumSamples;
};

class HistogramDisplay
{
public:
    HistogramDisplay()
        : m_NumHistograms(0)
    {
    }

    static HistogramDisplay* GetInstance();
    void AddHistogram(Histogram* histogram);
    void Draw();
    float GetBinY(int index, float bin);

    Histogram* m_Histograms[8];
    int m_NumHistograms;
};

#endif // GAME_DEBUG_HISTOGRAM_H
