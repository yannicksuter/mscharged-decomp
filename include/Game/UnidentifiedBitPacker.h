#ifndef GAME_UNIDENTIFIED_BIT_PACKER_H
#define GAME_UNIDENTIFIED_BIT_PACKER_H

#include "Game/AI/AiUtil.h"
#include "Game/MathHelpers.h"
#include <string.h>

class UnidentifiedBitPacker
{
public:
    UnidentifiedBitPacker()
        : mUnidentified00(0)
    {
        memset(&mUnidentified04, 0, sizeof(mUnidentified04));
    }

    static unsigned int UnidentifiedBitCount(unsigned long value)
    {
        unsigned int bits = 0;
        for (unsigned int i = 0; i < 64; ++i)
        {
            if ((1 << i) & value)
                bits = i + 1;
        }
        return bits;
    }

    void UnidentifiedPack(int value, int min, int max)
    {
        value = value >= min ? value : min;
        value = value <= max ? value : max;
        unsigned int bits = UnidentifiedBitCount(max - min);
        if (mUnidentified00 + bits < 32)
        {
            mUnidentified00 += bits;
            mUnidentified04 = (mUnidentified04 << bits) | (value - min);
        }
    }

    void UnidentifiedPack(float value, float min, float max, float precision)
    {
        value = nlMaxEquals(value, min);
        value = nlMinEquals(value, max);
        float scale = 1.0f / precision;
        int unidentifiedValue = (int)(value * scale + 0.5f * AIsgn(value));
        int unidentifiedMin = (int)(min * scale + 0.5f * AIsgn(min));
        int unidentifiedMax = (int)(max * scale + 0.5f * AIsgn(max));
        UnidentifiedPack(unidentifiedValue, unidentifiedMin, unidentifiedMax);
    }

    unsigned long UnidentifiedGet() const
    {
        return mUnidentified04;
    }

    unsigned int mUnidentified00;
    unsigned long mUnidentified04;
};

#endif
