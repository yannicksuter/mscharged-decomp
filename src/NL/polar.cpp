#include "NL/nlMath.h"

void nlCartesianToPolar(nlPolar& out, float x, float y)
{
    float lenSq = x * x + y * y;
    out.r = nlSqrt(lenSq, true);
    float angle = nlATan2f(y, x);
    out.a = (u16)(s32)(angle * 10430.378f);
}

void nlPolarToCartesian(float& x, float& y, unsigned short angle, float radius)
{
    float* x_ptr = &x;
    float* y_ptr = &y;
    nlSinCos(y_ptr, x_ptr, angle);
    *x_ptr *= radius;
    *y_ptr *= radius;
}

void nlPolarToCartesian(nlVector3& v, const nlPolar& polar)
{
    float radius = polar.r;
    nlSinCos(&v.y, &v.x, polar.a);
    v.x *= radius;
    v.y *= radius;
}

void nlCartesianToPolar(nlPolar& out, const nlVector3& in)
{
    float x = in.x;
    float y = in.y;
    float lenSq = x * x + y * y;
    out.r = nlSqrt(lenSq, true);
    float angle = nlATan2f(y, x);
    out.a = (u16)(s32)(angle * 10430.378f);
}
