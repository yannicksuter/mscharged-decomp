#ifndef GAME_BIT_PACKER_H
#define GAME_BIT_PACKER_H

#include "Game/AI/AiUtil.h"
#include "Game/MathHelpers.h"
#include <string.h>

class BitPacker
{
public:
    BitPacker()
        : mBitCount(0)
    {
        memset(&mBits, 0, sizeof(mBits));
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
        if (bits + mBitCount < 32)
        {
            mBitCount += bits;
            mBits = (mBits << bits) | (value - min);
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
        return mBits;
    }

    unsigned int mBitCount;
    unsigned long mBits;
};

#endif
