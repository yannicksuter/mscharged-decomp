#ifndef GAME_BIT_PACKER_H
#define GAME_BIT_PACKER_H

#include "Game/AI/AiUtil.h"
#include "Game/MathHelpers.h"
#include <string.h>

class BitPacker
{
public:
    BitPacker()
        : mUnidentified00(0)
    {
        memset(&mUnidentified04, 0, sizeof(mUnidentified04));
    }

    void Pack(int value, int min, int max)
    {
        value = value >= min ? value : min;
        value = value <= max ? value : max;
        unsigned long range = max - min;
        unsigned int i;
        unsigned int bits = 0;
        for (i = 0; i < 64; ++i)
        {
            if ((1 << i) & range)
                bits = i + 1;
        }
        if (bits + mUnidentified00 < 32)
        {
            mUnidentified00 += bits;
            mUnidentified04 = (mUnidentified04 << bits) | (value - min);
        }
    }

    void Pack(float value, float min, float max, float precision)
    {
        value = nlMaxEquals(value, min);
        value = nlMinEquals(value, max);
        float scale = 1.0f / precision;
        int unidentifiedValue = (int)(value * scale + 0.5f * AIsgn(value));
        int unidentifiedMin = (int)(min * scale + 0.5f * AIsgn(min));
        int unidentifiedMax = (int)(max * scale + 0.5f * AIsgn(max));
        Pack(unidentifiedValue, unidentifiedMin, unidentifiedMax);
    }

    unsigned long GetBits() const
    {
        return mUnidentified04;
    }

    unsigned int mUnidentified00;
    unsigned long mUnidentified04;
};

#endif
