#include "Game/Camera/noisefilter.h"

#include <math.h>

namespace
{
inline float Noise(int value)
{
    int n = (value << 13) ^ value;
    int hash = n * (n * n * 15731 + 789221) + 1376312589;
    return (float)(1.0 - (double)(hash & 0x7fffffff) / 1073741824.0);
}
} // namespace

cNoiseFilter::cNoiseFilter()
{
    Reset();
}

void cNoiseFilter::Start(
    nlVector3 amplitude, float frequency, float duration)
{
    Reset();

    if (frequency < 0.0001f)
    {
        return;
    }

    mSeed.x = (float)nlRandom(5000, &nlDefaultSeed);
    mSeed.y = (float)nlRandom(5000, &nlDefaultSeed);
    mSeed.z = (float)nlRandom(5000, &nlDefaultSeed);
    mAmplitude = amplitude;
    mFrequency = frequency;
    mSampleInterval = 1.0f / mFrequency;
    mTimeRemaining = duration;
    mActive = true;
}

void cNoiseFilter::Reset()
{
    mSampleIndex = -1;
    mElapsedTime = 0.0f;
    mActive = false;
    mTimeRemaining = -1.0f;
    mSamples[0].mTime = -1;
    mSamples[1].mTime = -1;
}

inline void cNoiseFilter::UpdateSample(int index, int sample)
{
    mSamples[index].mTime = sample;
    if (sample <= 0)
        nlVec3Set(mSamples[index].mValue, 0.0f, 0.0f, 0.0f);
    else
    {
        mSamples[index].mValue.x = mAmplitude.x * Noise(sample + (int)mSeed.x);
        mSamples[index].mValue.y = mAmplitude.y * Noise(sample + (int)mSeed.y);
        mSamples[index].mValue.z = mAmplitude.z * Noise(sample + (int)mSeed.z);
    }
}

void cNoiseFilter::Update(float dt)
{
    nlVec3Set(mDisplacement, 0.0f, 0.0f, 0.0f);
    if (!mActive)
    {
        return;
    }

    mElapsedTime += dt;
    if (mTimeRemaining >= 0.0f)
    {
        if (dt >= mTimeRemaining)
        {
            Reset();
            return;
        }

        nlVec3Scale(mAmplitude, 1.0f - dt / mTimeRemaining);
        mTimeRemaining -= dt;
    }

    mSampleInterval = 1.0f / mFrequency;
    int i;
    int sample = (int)(mElapsedTime * mFrequency);
    float alpha = (mElapsedTime - mSampleInterval * (float)sample) /
                  mSampleInterval;

    if (mSampleIndex < 0)
    {
        mSampleIndex = 0;
        for (i = 0; i < 2; i++)
            UpdateSample(i, sample + i);
    }
    else
    {
        if (sample != mSamples[mSampleIndex].mTime)
        {
            UpdateSample(mSampleIndex, sample + 1);
            mSampleIndex++;
            if (mSampleIndex >= 2)
                mSampleIndex = 0;
        }
    }

    Sample& point0 = mSamples[mSampleIndex];
    Sample& point1 = mSamples[(mSampleIndex + 1) % 2];
    float cosine = (float)cos(3.1415927f * alpha);
    float weight0;
    float weight1 = (1.0f - cosine) * 0.5f;
    weight0 = 1.0f - weight1;
    mDisplacement.x = point0.mValue.x * weight0 +
                      point1.mValue.x * weight1;
    mDisplacement.y = point0.mValue.y * weight0 +
                      point1.mValue.y * weight1;
    mDisplacement.z = point0.mValue.z * weight0 +
                      point1.mValue.z * weight1;
}

void cNoiseFilter::Filter(
    const nlMatrix4& matViewIn, nlMatrix4& matViewOut)
{
    matViewOut = matViewIn;
    matViewOut.m41 += mDisplacement.x;
    matViewOut.m42 += mDisplacement.y;
    matViewOut.m43 += mDisplacement.z;
}

int cNoiseFilter::GetFilterIndex()
{
    return 1;
}

