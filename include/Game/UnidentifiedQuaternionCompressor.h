#ifndef _UNIDENTIFIEDQUATERNIONCOMPRESSOR_H_
#define _UNIDENTIFIEDQUATERNIONCOMPRESSOR_H_

#include <math.h>

class UnidentifiedQuaternionCompressor
{
public:
    UnidentifiedQuaternionCompressor(nlQuaternion& q)
        : mQ(q)
    {
    }

    void Read(LoadFrame& frame, unsigned int& value) const { }
    void Apply(SaveFrame& frame, unsigned int& value) const { }

    void Read(SaveFrame& frame, unsigned int& value) const
    {
        float x = fabsf(mQ.x);
        float y = fabsf(mQ.y);
        float z = fabsf(mQ.z);
        float w = fabsf(mQ.w);
        if (w > x && w > y && w > z)
        {
            if (mQ.w < 0.0f)
            {
                mQ.x = -mQ.x;
                mQ.y = -mQ.y;
                mQ.z = -mQ.z;
                mQ.w = -mQ.w;
            }
            value = (unsigned int)((1024.0f / 1.44f) * (mQ.x - -0.72f)) | ((unsigned int)((1024.0f / 1.44f) * (mQ.y - -0.72f)) << 10) | ((unsigned int)((1024.0f / 1.44f) * (mQ.z - -0.72f)) << 20) | 0xC0000000;
        }
        else if (x > y && x > z)
        {
            if (mQ.x < 0.0f)
            {
                mQ.x = -mQ.x;
                mQ.y = -mQ.y;
                mQ.z = -mQ.z;
                mQ.w = -mQ.w;
            }
            value = (unsigned int)((1024.0f / 1.44f) * (mQ.y - -0.72f)) | ((unsigned int)((1024.0f / 1.44f) * (mQ.z - -0.72f)) << 10) | ((unsigned int)((1024.0f / 1.44f) * (mQ.w - -0.72f)) << 20);
        }
        else if (y > z)
        {
            if (mQ.y < 0.0f)
            {
                mQ.x = -mQ.x;
                mQ.y = -mQ.y;
                mQ.z = -mQ.z;
                mQ.w = -mQ.w;
            }
            value = (unsigned int)((1024.0f / 1.44f) * (mQ.x - -0.72f)) | ((unsigned int)((1024.0f / 1.44f) * (mQ.z - -0.72f)) << 10) | ((unsigned int)((1024.0f / 1.44f) * (mQ.w - -0.72f)) << 20) | 0x40000000;
        }
        else
        {
            if (mQ.z < 0.0f)
            {
                mQ.x = -mQ.x;
                mQ.y = -mQ.y;
                mQ.z = -mQ.z;
                mQ.w = -mQ.w;
            }
            value = (unsigned int)((1024.0f / 1.44f) * (mQ.x - -0.72f)) | ((unsigned int)((1024.0f / 1.44f) * (mQ.y - -0.72f)) << 10) | ((unsigned int)((1024.0f / 1.44f) * (mQ.w - -0.72f)) << 20) | 0x80000000;
        }
    }

    void Apply(LoadFrame& frame, unsigned int& value) const
    {
        unsigned int component = value & 0xC0000000;
        if (component == 0xC0000000)
        {
            mQ.x = (1.44f / 1024.0f) * (float)(value & 0x3FF) + -0.72f;
            mQ.y = (1.44f / 1024.0f) * (float)((value >> 10) & 0x3FF) + -0.72f;
            mQ.z = (1.44f / 1024.0f) * (float)((value >> 20) & 0x3FF) + -0.72f;
            mQ.w = nlSqrt(1.0f - mQ.x * mQ.x - mQ.y * mQ.y - mQ.z * mQ.z, true);
        }
        else if (component == 0)
        {
            mQ.y = (1.44f / 1024.0f) * (float)(value & 0x3FF) + -0.72f;
            mQ.z = (1.44f / 1024.0f) * (float)((value >> 10) & 0x3FF) + -0.72f;
            mQ.w = (1.44f / 1024.0f) * (float)((value >> 20) & 0x3FF) + -0.72f;
            mQ.x = nlSqrt(1.0f - mQ.y * mQ.y - mQ.z * mQ.z - mQ.w * mQ.w, true);
        }
        else if (component == 0x40000000)
        {
            mQ.x = (1.44f / 1024.0f) * (float)(value & 0x3FF) + -0.72f;
            mQ.z = (1.44f / 1024.0f) * (float)((value >> 10) & 0x3FF) + -0.72f;
            mQ.w = (1.44f / 1024.0f) * (float)((value >> 20) & 0x3FF) + -0.72f;
            mQ.y = nlSqrt(1.0f - mQ.x * mQ.x - mQ.z * mQ.z - mQ.w * mQ.w, true);
        }
        else
        {
            mQ.x = (1.44f / 1024.0f) * (float)(value & 0x3FF) + -0.72f;
            mQ.y = (1.44f / 1024.0f) * (float)((value >> 10) & 0x3FF) + -0.72f;
            mQ.w = (1.44f / 1024.0f) * (float)((value >> 20) & 0x3FF) + -0.72f;
            mQ.z = nlSqrt(1.0f - mQ.x * mQ.x - mQ.y * mQ.y - mQ.w * mQ.w, true);
        }
    }

    template <int N, typename T>
    void ReplayInternal(T& frame) const
    {
        unsigned int value = 0;
        Read(frame, value);
        Replayable<N>(frame, value);
        Apply(frame, value);
    }

    template <int N>
    void ReplayInterval(LoadFrame& frame) const
    {
        ReplayInternal<N>(frame);
    }

    template <int N>
    void ReplayInterval(SaveFrame& frame) const
    {
        ReplayInternal<N>(frame);
    }

    void Replay(LoadFrame& frame) const
    {
        ReplayInterval<0>(frame);
    }

    void Replay(SaveFrame& frame) const
    {
        ReplayInterval<0>(frame);
    }

    nlQuaternion& mQ;
};

#endif
