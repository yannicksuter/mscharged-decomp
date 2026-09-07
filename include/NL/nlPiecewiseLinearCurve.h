#ifndef NL_PIECEWISE_LINEAR_CURVE_H
#define NL_PIECEWISE_LINEAR_CURVE_H

#include "NL/nlMath.h"

class nlPiecewiseLinearCurve
{
public:
    nlPiecewiseLinearCurve(nlVector2* data = 0, int count = 0)
        : mData(data)
        , mCount(count)
    {
    }

    ~nlPiecewiseLinearCurve()
    {
    }

    void Evaluate(float x, float& value) const
    {
        if (mCount == 0)
            return;
        if (x <= mData[0].x)
        {
            value = mData[0].y;
            return;
        }
        if (x >= mData[mCount - 1].x)
        {
            value = mData[mCount - 1].y;
            return;
        }

        int upper = FindUpperPoint(x);
        const nlVector2& lowerPoint = mData[upper - 1];
        const nlVector2& upperPoint = mData[upper];
        float range = upperPoint.x - lowerPoint.x;
        range = range >= 0.0001f ? range : 0.0001f;
        float percent = (x - lowerPoint.x) / range;
        value = (1.0f - percent) * lowerPoint.y + percent * upperPoint.y;
    }

    int FindUpperPoint(float x) const
    {
        int upper = -1;
        for (int i = 1; i < mCount; ++i)
        {
            if (x <= mData[i].x)
            {
                upper = i;
                break;
            }
        }
        return upper;
    }

    nlVector2* mData;
    int mCount;
};

#endif // NL_PIECEWISE_LINEAR_CURVE_H
