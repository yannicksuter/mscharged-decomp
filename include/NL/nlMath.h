#ifndef NL_NL_MATH_H
#define NL_NL_MATH_H

#include "types.h"

float nlSqrt(float x, bool accurate);
float nlRecipSqrt(float x, bool accurate);
float nlSin(u16 angle);
void nlSinCos(float* resultSin, float* resultCos, u16 angle);
u16 nlACos(float x);
float nlTan(u16 angle);
float nlATan(float x);
float nlATan2f(float y, float x);
u32 nlLog2(u32 value);
float nlFastLog2(float value);
float nlFastExp2(float value);

struct nlVector2
{
    float x;
    float y;
};

#endif
