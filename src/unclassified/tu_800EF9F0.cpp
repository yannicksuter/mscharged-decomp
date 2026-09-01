#include "Game/Camera/rumblefilter.h"

#include <math.h>

namespace
{
inline float GetUnidentifiedNoise(int value)
{
    int n = (value << 13) ^ value;
    int hash = n * (n * n * 15731 + 789221) + 1376312589;
    return (float)(1.0 - (double)(hash & 0x7fffffff) / 1073741824.0);
}
} // namespace

int UnidentifiedCameraFilter::vfunc_0x14()
{
    return 1;
}

void UnidentifiedCameraFilter::Filter(
    const nlMatrix4& matViewIn, nlMatrix4& matViewOut)
{
    matViewOut = matViewIn;
    matViewOut.m41 += mUnidentified34.x;
    matViewOut.m42 += mUnidentified34.y;
    matViewOut.m43 += mUnidentified34.z;
}

void UnidentifiedCameraFilter::Update(float dt)
{
    nlVec3Set(mUnidentified34, 0.0f, 0.0f, 0.0f);
    if (!mUnidentified04)
    {
        return;
    }

    mUnidentified14 += dt;
    if (mUnidentified18 >= 0.0f)
    {
        if (dt >= mUnidentified18)
        {
            Reset();
            return;
        }

        float scale = 1.0f - dt / mUnidentified18;
        mUnidentified18 -= dt;
        mUnidentified1C.x *= scale;
        mUnidentified1C.y *= scale;
        mUnidentified1C.z *= scale;
    }

    mUnidentified10 = 1.0f / mUnidentified0C;
    int sample = (int)(mUnidentified14 * mUnidentified0C);
    float alpha = (mUnidentified14 - mUnidentified10 * (float)sample) /
                  mUnidentified10;

    if (mUnidentified08 < 0)
    {
        mUnidentified08 = 0;
        for (int i = 0; i < 2; i++)
        {
            UnidentifiedSample& point = mUnidentified40[i];
            point.mUnidentified00 = sample + i;
            if (point.mUnidentified00 <= 0)
            {
                nlVec3Set(point.mUnidentified04, 0.0f, 0.0f, 0.0f);
            }
            else
            {
                point.mUnidentified04.x = mUnidentified1C.x *
                    GetUnidentifiedNoise(point.mUnidentified00 + (int)mUnidentified28.x);
                point.mUnidentified04.y = mUnidentified1C.y *
                    GetUnidentifiedNoise(point.mUnidentified00 + (int)mUnidentified28.y);
                point.mUnidentified04.z = mUnidentified1C.z *
                    GetUnidentifiedNoise(point.mUnidentified00 + (int)mUnidentified28.z);
            }
        }
    }
    else
    {
        UnidentifiedSample& point = mUnidentified40[mUnidentified08];
        if (sample != point.mUnidentified00)
        {
            point.mUnidentified00 = sample + 1;
            if (point.mUnidentified00 <= 0)
            {
                nlVec3Set(point.mUnidentified04, 0.0f, 0.0f, 0.0f);
            }
            else
            {
                point.mUnidentified04.x = mUnidentified1C.x *
                    GetUnidentifiedNoise(point.mUnidentified00 + (int)mUnidentified28.x);
                point.mUnidentified04.y = mUnidentified1C.y *
                    GetUnidentifiedNoise(point.mUnidentified00 + (int)mUnidentified28.y);
                point.mUnidentified04.z = mUnidentified1C.z *
                    GetUnidentifiedNoise(point.mUnidentified00 + (int)mUnidentified28.z);
            }

            mUnidentified08++;
            if (mUnidentified08 >= 2)
            {
                mUnidentified08 = 0;
            }
        }
    }

    UnidentifiedSample& point0 = mUnidentified40[mUnidentified08];
    UnidentifiedSample& point1 = mUnidentified40[(mUnidentified08 + 1) % 2];
    float cosine = (float)cos(3.1415927f * alpha);
    float weight1 = (1.0f - cosine) * 0.5f;
    float weight0 = 1.0f - weight1;
    mUnidentified34.x = point0.mUnidentified04.x * weight0 +
                        point1.mUnidentified04.x * weight1;
    mUnidentified34.y = point0.mUnidentified04.y * weight0 +
                        point1.mUnidentified04.y * weight1;
    mUnidentified34.z = point0.mUnidentified04.z * weight0 +
                        point1.mUnidentified04.z * weight1;
}

void UnidentifiedCameraFilter::Reset()
{
    mUnidentified08 = -1;
    mUnidentified14 = 0.0f;
    mUnidentified04 = false;
    mUnidentified18 = -1.0f;
    mUnidentified40[0].mUnidentified00 = -1;
    mUnidentified40[1].mUnidentified00 = -1;
}

void UnidentifiedCameraFilter::vfunc_0x18(
    nlVector3 v3Unidentified, float fUnidentified0, float fUnidentified1)
{
    Reset();

    if (fUnidentified0 >= 0.0001f)
    {
        mUnidentified28.x = (float)nlRandom(5000, &nlDefaultSeed);
        mUnidentified28.y = (float)nlRandom(5000, &nlDefaultSeed);
        mUnidentified28.z = (float)nlRandom(5000, &nlDefaultSeed);
        mUnidentified1C = v3Unidentified;
        mUnidentified0C = fUnidentified0;
        mUnidentified10 = 1.0f / fUnidentified0;
        mUnidentified18 = fUnidentified1;
        mUnidentified04 = true;
    }
}

UnidentifiedCameraFilter::UnidentifiedCameraFilter()
{
    Reset();
}
