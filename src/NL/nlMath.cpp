#include "NL/nlMath.h"

#include "types.h"

extern "C" int __fpclassifyf(float);
extern "C" double floor(double);
extern "C" double tan(double);

extern int __float_nan[];
extern int __float_huge[];

struct nlATanCoefficients
{
    float values[8];
};

static inline float nlFAbs(float value)
{
    return __fabs(value);
}

static inline double nlFAbsDouble(double value)
{
    return __fabs(value);
}

float nlSqrt(float x, bool accurate)
{
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        if (accurate)
        {
            guess = 0.5 * guess * (3.0 - guess * guess * x);
            guess = 0.5 * guess * (3.0 - guess * guess * x);
        }
        return (float)(x * guess);
    }
    else if (x < 0.0)
    {
        return *(float*)&__float_nan[0];
    }
    else
    {
        if (__fpclassifyf(x) == 1)
        {
            return *(float*)&__float_nan[0];
        }
        return x;
    }
}

float nlRecipSqrt(float x, bool accurate)
{
    float zero = 0.0f;
    if (x > zero)
    {
        float y = __frsqrte(x);
        y = 0.5f * y * (3.0f - y * y * x);
        y = 0.5f * y * (3.0f - y * y * x);
        y = 0.5f * y * (3.0f - y * y * x);
        return y;
    }
    else if (x != zero)
    {
        return *(float*)&__float_nan[0];
    }
    else
    {
        return *(float*)&__float_huge[0];
    }
}

float nlSin(unsigned short angle)
{
    float angleRadians = 0.0000958738f * (float)angle;
    float reducedAngle = angleRadians;
    float sign = 1.0f;

    if (angleRadians >= 4.7123889f)
    {
        reducedAngle = angleRadians - 6.2831855f;
    }
    else if (angleRadians >= 1.5707964f)
    {
        sign = -sign;
        reducedAngle = angleRadians - 3.1415927f;
    }

    float squared = reducedAngle * reducedAngle;
    float result = reducedAngle
        * (1.0f
            + squared
                * (-0.16666667f
                    + squared
                        * (0.0083333337f
                            + squared * (-0.0001984127f + squared * 0.0000027557319f))));

    return sign * result;
}

void nlSinCos(float* resultSin, float* resultCos, unsigned short angle)
{
    int octant;
    float y;
    float squared;

    float angleRadians = 0.0000958738f * (float)angle;
    float octants = 1.2732395f * angleRadians;
    octant = (int)octants;
    float octantFloat = (float)octant;
    y = angleRadians - 0.7853982f * octantFloat;

    squared = y * y;
    float sinPolynomial = y
        * (squared
                * (squared * (-0.00019516895f * squared + 0.008332208f)
                    + -0.16666657f)
            + 1.0f);
    float cosPolynomial = squared
            * (squared * (-0.0013400711f * squared + 0.04163633f) + -0.49999395f)
        + 0.9999998f;

    float b;
    float t2;
    float t1;
    float a;
    float sinValue;
    float cosValue;

    switch (octant & 7)
    {
    case 0:
        sinValue = sinPolynomial;
        cosValue = cosPolynomial;
        break;
    case 1:
    {
        a = 0.70710677f;
        b = 0.70710677f;
        t1 = a * cosPolynomial;
        t2 = a * sinPolynomial;
        sinValue = b * sinPolynomial + t1;
        cosValue = b * cosPolynomial - t2;
        break;
    }
    case 2:
        sinValue = cosPolynomial;
        cosValue = -sinPolynomial;
        break;
    case 3:
    {
        a = 0.70710677f;
        b = -0.70710677f;
        t1 = a * cosPolynomial;
        t2 = a * sinPolynomial;
        sinValue = b * sinPolynomial + t1;
        cosValue = b * cosPolynomial - t2;
        break;
    }
    case 4:
        sinValue = -sinPolynomial;
        cosValue = -cosPolynomial;
        break;
    case 5:
    {
        t2 = -0.70710677f;
        b = -0.70710677f;
        t1 = t2 * cosPolynomial;
        a = t2 * sinPolynomial;
        sinValue = b * sinPolynomial + t1;
        cosValue = b * cosPolynomial - a;
        break;
    }
    case 6:
        sinValue = -cosPolynomial;
        cosValue = sinPolynomial;
        break;
    case 7:
    {
        a = -0.70710677f;
        b = 0.70710677f;
        t1 = a * cosPolynomial;
        t2 = a * sinPolynomial;
        sinValue = b * sinPolynomial + t1;
        cosValue = b * cosPolynomial - t2;
        break;
    }
    }

    *resultSin = sinValue;
    *resultCos = cosValue;
}

static inline float nlMax(float a, float b)
{
    return a >= b ? a : b;
}

static inline unsigned short nlFloatToUShort(float value)
{
    return (unsigned short)(s32)value;
}

int nlACos(float x)
{
    u8 complement = x < 0.0f;
    x = 1.0f - nlFAbs(x);

    float polynomial = x
            * (x
                    * (x * (x * (0.015098966f * x + 0.005516444f) + 0.047654245f)
                        + 0.16391061f)
                + 2.0002916f)
        + -0.000007239284f;

    float radicand = nlMax(2.0f * x, 0.00001f);
    float squareRoot;

    if (radicand > 0.0f)
    {
        float estimate = __frsqrte(radicand);
        float refine1 = 0.5f * estimate * -(radicand * (estimate * estimate) - 3.0f);
        float refine2 = 0.5f * refine1 * -(radicand * (refine1 * refine1) - 3.0f);
        squareRoot = 0.5f * refine2 * -(radicand * (refine2 * refine2) - 3.0f);
    }
    else
    {
        float zero = 0.0f;
        if (radicand != zero)
        {
            squareRoot = *(float*)&__float_nan[0];
        }
        else
        {
            squareRoot = *(float*)&__float_huge[0];
        }
    }

    float radians = polynomial * squareRoot;
    return complement
        ? nlFloatToUShort(10430.378f * (3.1415927f - radians))
        : nlFloatToUShort(10430.378f * radians);
}

float nlTan(unsigned short angle)
{
    return (float)tan(0.0000958738f * (float)angle);
}

static const nlATanCoefficients nlATanLow = {
    0.0f, 0.004072621f, 0.0178999677f, 0.0447405465f,
    0.0844737813f, 0.134708926f, 0.192103297f, 0.253371507f
};

static const nlATanCoefficients nlATanHigh = {
    0.997413278f, 0.964989364f, 0.910336077f, 0.839015484f,
    0.759613633f, 0.679214358f, 0.602631152f, 0.532545328f
};

static inline float nlATanLookup(
    float value, const nlATanCoefficients& high, const nlATanCoefficients& low)
{
    int index = (int)(8.0f * value) <= 7 ? (int)(8.0f * value) : 7;
    return value * high.values[index] + low.values[index];
}

struct nlATanScratch
{
    nlATanCoefficients reciprocalHigh;
    nlATanCoefficients reciprocalLow;
    nlATanCoefficients directHigh;
    nlATanCoefficients directLow;
};

static inline float nlATanPositive(float value)
{
    nlATanScratch scratch;

    if (value > 1.0f)
    {
        scratch.reciprocalLow = nlATanLow;
        scratch.reciprocalHigh = nlATanHigh;
        return 1.5707964f
            - nlATanLookup(1.0f / value, scratch.reciprocalHigh, scratch.reciprocalLow);
    }

    scratch.directLow = nlATanLow;
    scratch.directHigh = nlATanHigh;
    return nlATanLookup(value, scratch.directHigh, scratch.directLow);
}

float nlATan(float x)
{
    float value;
    float sign = 1.0f;
    sign -= 2.0f * (float)(x < 0.0f);
    value = (float)nlFAbsDouble(x);
    return sign * nlATanPositive(value);
}

float nlATan2f(float y, float x)
{
    float angle;

    if ((float)nlFAbsDouble(x) > 0.00001f)
    {
        angle = nlATanPositive((float)nlFAbsDouble(y / x));
        if (y >= 0.0f)
        {
            if (x >= 0.0f)
            {
                return angle;
            }
            return 3.1415927f - angle;
        }
        if (x < 0.0f)
        {
            return 3.1415927f + angle;
        }
        return 6.2831855f - angle;
    }
    if (y > 0.0f)
    {
        return 1.5707964f;
    }
    return 4.712389f;
}

unsigned int nlLog2(unsigned int value)
{
    unsigned int result = 0;
    for (;;)
    {
        value >>= 1;
        if (value == 0)
        {
            break;
        }
        ++result;
    }
    return result;
}

extern const float nlFastExpScale;
extern const float nlFastLogCorrection;
extern const float nlFastLogMantissaScale;
extern const float nlFastExponentBias;
extern const float nlFastExpCorrection;

float nlFastLog2(float value)
{
    union
    {
        float f;
        int i;
    } bits;
    bits.f = value;

    float converted = (float)bits.i;
    converted *= nlFastLogMantissaScale;
    float approximation = converted - nlFastExponentBias;
    float correctionScale = nlFastLogCorrection;
    float integral = (float)floor(approximation);
    float fraction = approximation - integral;
    float correction = correctionScale * (fraction - fraction * fraction);
    return approximation + correction;
}

static inline float nlMultiply(float first, float second)
{
    return first * second;
}

float nlFastExp2(float value)
{
    float correctionScale = nlFastExpCorrection;
    float integral = (float)floor(value);
    float fraction = value - integral;
    float correction = correctionScale * (fraction - fraction * fraction);
    float expression = nlFastExponentBias + value - correction;
    union
    {
        int i;
        float f;
    } bits;
    bits.i = (int)nlMultiply(expression, nlFastExpScale);
    return bits.f;
}

const float nlFastExpScale = 8388608.0f;
const float nlFastLogCorrection = 0.346607f;
const float nlFastLogMantissaScale = 1.1920928955078125e-7f;
const float nlFastExponentBias = 127.0f;
const float nlFastExpCorrection = 0.33971f;
