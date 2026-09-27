#include "NL/utility.h"

#include "math.h"

static const f32 CANT_COLLIDE = 3.402823466e+38F;

void SolveQuadratic(float a, float b, float c, int& numRoots, float& x1,
    float& x2)
{
    float absA = (float)fabs(a);
    float absB = (float)fabs(b);
    float absC = (float)fabs(c);
    float p;
    float q;
    float r;

    if (absA < 1e-10f && absB < 1e-10f)
    {
        numRoots = 0;
        return;
    }

    if (absA > absB && absA > absC)
    {
        q = b / a;
        p = 1.0f;
        r = c / a;
    }
    else if (absB > absA && absB > absC)
    {
        p = a / b;
        q = 1.0f;
        r = c / b;
    }
    else
    {
        p = a / c;
        r = 1.0f;
        q = b / c;
    }

    if (absA < 1e-10f)
    {
        numRoots = 1;
        x1 = -r / q;
        return;
    }

    float d = q * q - (4.0f * p * r);
    if (nlNear(d, 0.0f))
    {
        d = 0.0f;
    }
    if (d < 0.0f)
    {
        numRoots = 0;
        return;
    }

    float sqrtd = nlSqrt(d, true);
    float z;
    if (q > 0.0f)
    {
        z = -0.5f * (q + sqrtd);
    }
    else
    {
        z = -0.5f * (q - sqrtd);
    }

    x1 = z / p;
    if ((float)fabs(z) < 1e-10f)
    {
        numRoots = 1;
        return;
    }

    x2 = r / z;
    numRoots = 2;
}

float SweepSpheres(float rp, const nlVector3& p1, const nlVector3& p2,
    float rq, const nlVector3& q1, const nlVector3& q2)
{
    nlVector3 d, v;
    d.x = p1.x - q1.x;
    d.y = p1.y - q1.y;
    d.z = p1.z - q1.z;
    v.x = (p2.x - p1.x) - (q2.x - q1.x);
    v.y = (p2.y - p1.y) - (q2.y - q1.y);
    v.z = (p2.z - p1.z) - (q2.z - q1.z);

    float dsq = d.GetLengthSq3D();
    float a = v.GetLengthSq3D();

    if ((float)fabs(a) <= 0.00001f)
    {
        return CANT_COLLIDE;
    }

    float dot = d.x * v.x + d.y * v.y + d.z * v.z;
    float dotSq = dot * dot;
    float rsq = (rp + rq) * (rp + rq);
    float closest = dsq - dotSq / a;
    if (closest > rsq)
    {
        return CANT_COLLIDE;
    }

    if (dsq <= rsq)
    {
        return 0.0f;
    }

    float disc = dotSq - a * (dsq - rsq);
    if (disc < 0.0f)
    {
        return CANT_COLLIDE;
    }

    float t = (-dot - nlSqrt(disc, true)) / a;
    return t;
}

float nlBezier(float* fControlPoints, int nNumPoints, float fMu)
{
    int nFactorial;
    int kFactorial;
    int nMinusKFactorial;
    float powVal;
    float oneMinusMuToNMinusK;
    float muToK;
    int i;
    float result;
    int k;
    float blend;

    if (fMu == 1.0f)
    {
        return fControlPoints[nNumPoints - 1];
    }

    nFactorial = 1;
    kFactorial = 1;
    powVal = pow(1.0f - fMu, (float)nNumPoints);
    oneMinusMuToNMinusK = powVal;
    muToK = 1.0f;

    for (i = 1; i <= nNumPoints; i++)
    {
        nFactorial *= i;
    }

    nMinusKFactorial = nFactorial;
    result = *fControlPoints * oneMinusMuToNMinusK;

    for (k = 1; k <= nNumPoints; k++)
    {
        kFactorial *= k;
        if (k != nNumPoints)
        {
            nMinusKFactorial /= (nNumPoints - k) + 1;
        }

        oneMinusMuToNMinusK /= 1.0f - fMu;
        muToK *= fMu;
        blend = (float)(nFactorial / (kFactorial * nMinusKFactorial));
        blend *= muToK * oneMinusMuToNMinusK;
        blend *= fControlPoints[k];
        result += blend;
    }

    return result;
}
