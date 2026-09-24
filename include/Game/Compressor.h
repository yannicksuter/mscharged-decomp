#ifndef _COMPRESSOR_H_
#define _COMPRESSOR_H_

#include <math.h>

template <int MIN, int MAX, int BITS>
class FloatCompressor
{
public:
    FloatCompressor(float& f);
    inline unsigned int Read(LoadFrame& frame) const;
    inline unsigned int Read(SaveFrame& frame) const;
    inline void Transfer(LoadFrame& frame, unsigned int& value) const;
    inline void Transfer(SaveFrame& frame, unsigned int& value) const;
    inline void Apply(LoadFrame& frame, unsigned int value) const;
    inline void Apply(SaveFrame& frame, unsigned int value) const;
    inline void Replay(LoadFrame& frame) const;
    inline void Replay(SaveFrame& frame) const;
    template <int N>
    inline void ReplayInterval(LoadFrame& frame) const;
    template <int N>
    inline void ReplayInterval(SaveFrame& frame) const;

    /* 0x0 */ float& mF;
}; // total size: 0x4

template <int MIN, int MAX, int BITS>
inline FloatCompressor<MIN, MAX, BITS>::FloatCompressor(float& f)
    : mF(f)
{
}

template <int MIN, int MAX, int BITS>
inline unsigned int FloatCompressor<MIN, MAX, BITS>::Read(LoadFrame& frame) const
{
    return 0;
}

template <int MIN, int MAX, int BITS>
inline unsigned int FloatCompressor<MIN, MAX, BITS>::Read(SaveFrame& frame) const
{
    float f = mF;
    if (f > (float)MAX)
        f = (float)MAX;
    if (f < (float)MIN)
        f = (float)MIN;
    f -= (float)MIN;
    f *= (float)(1 << BITS);
    return (unsigned int)f;
}

template <int MIN, int MAX, int BITS>
inline void FloatCompressor<MIN, MAX, BITS>::Transfer(LoadFrame& frame, unsigned int& value) const
{
    if ((MAX - MIN) * (1 << BITS) <= 255)
    {
        const char* cursor = frame.mStream.mStorage;
        value = (unsigned int)(unsigned char)*cursor++;
        frame.mStream.mStorage = cursor;
    }
    else if ((MAX - MIN) * (1 << BITS) <= 65535)
    {
        const char* cursor = frame.mStream.mStorage;
        unsigned char lo = (unsigned char)*cursor++;
        value = (unsigned int)lo | ((unsigned int)(unsigned char)*cursor++ << 8);
        frame.mStream.mStorage = cursor;
    }
    else
    {
        const char* cursor = frame.mStream.mStorage;
        value = (unsigned int)(unsigned char)*cursor++;
        value |= (unsigned int)(unsigned char)*cursor++ << 8;
        value |= (unsigned int)(unsigned char)*cursor++ << 16;
        frame.mStream.mStorage = cursor;
    }
}

template <int MIN, int MAX, int BITS>
inline void FloatCompressor<MIN, MAX, BITS>::Transfer(SaveFrame& frame, unsigned int& value) const
{
    if ((MAX - MIN) * (1 << BITS) <= 255)
    {
        char* p = frame.mStream.mStorage;
        *p++ = (char)value;
        frame.mStream.mStorage = p;
    }
    else if ((MAX - MIN) * (1 << BITS) <= 65535)
    {
        char* p = frame.mStream.mStorage;
        *p++ = (char)(value & 0xFF);
        *p++ = (char)((value >> 8) & 0xFF);
        frame.mStream.mStorage = p;
    }
    else
    {
        char* p = frame.mStream.mStorage;
        *p++ = (char)(value & 0xFF);
        *p++ = value >> 8;
        *p++ = (char)((value >> 16) & 0xFF);
        frame.mStream.mStorage = p;
    }
}

template <int MIN, int MAX, int BITS>
inline void FloatCompressor<MIN, MAX, BITS>::Apply(LoadFrame& frame, unsigned int value) const
{
    mF = (float)value / (float)(1 << BITS);
    mF += (float)MIN;
}

template <int MIN, int MAX, int BITS>
inline void FloatCompressor<MIN, MAX, BITS>::Apply(SaveFrame& frame, unsigned int value) const
{
}

template <int MIN, int MAX, int BITS>
inline void FloatCompressor<MIN, MAX, BITS>::Replay(LoadFrame& frame) const
{
    unsigned int value = Read(frame);
    Transfer(frame, value);
    Apply(frame, value);
}

template <int MIN, int MAX, int BITS>
inline void FloatCompressor<MIN, MAX, BITS>::Replay(SaveFrame& frame) const
{
    unsigned int value = Read(frame);
    Transfer(frame, value);
    Apply(frame, value);
}

template <int MIN, int MAX, int BITS>
template <int N>
inline void FloatCompressor<MIN, MAX, BITS>::ReplayInterval(LoadFrame& frame) const
{
    unsigned int value = Read(frame);
    if (N == 0 || frame.mInterval == N)
    {
        Transfer(frame, value);
    }
    Apply(frame, value);
}

template <int MIN, int MAX, int BITS>
template <int N>
inline void FloatCompressor<MIN, MAX, BITS>::ReplayInterval(SaveFrame& frame) const
{
    unsigned int value = Read(frame);
    if (N == 0 || frame.mInterval == N)
    {
        Transfer(frame, value);
    }
    Apply(frame, value);
}

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

#endif // _COMPRESSOR_H_
