#include "NL/nlMath.h"

#include "math.h"

float nlBezier(float* fControlPoints, int nNumPoints, float fMu)
{
    float oneMinusMu;
    float powVal;
    float result;
    float oneMinusMuToNMinusK;
    float muToK;
    float blend;
    float powTerm;
    int nFactorial;
    int kFactorial;
    int i;
    float* currentPoint;
    int nMinusKFactorial;
    int k;

    if (fMu == 1.0f)
    {
        return fControlPoints[nNumPoints - 1];
    }

    oneMinusMu = 1.0f - fMu;
    nFactorial = 1;
    kFactorial = 1;
    powVal = pow(oneMinusMu, (float)nNumPoints);
    oneMinusMuToNMinusK = powVal;
    muToK = 1.0f;

    for (i = 1; i <= nNumPoints; i++)
    {
        nFactorial *= i;
    }

    nMinusKFactorial = nFactorial;
    currentPoint = fControlPoints + 1;
    result = *fControlPoints * powVal;

    for (k = 1; k <= nNumPoints; k++)
    {
        kFactorial *= k;
        if (k != nNumPoints)
        {
            nMinusKFactorial /= (nNumPoints - k) + 1;
        }

        oneMinusMuToNMinusK /= oneMinusMu;
        muToK *= fMu;
        blend = (float)(nFactorial / (kFactorial * nMinusKFactorial));
        powTerm = muToK * oneMinusMuToNMinusK;
        blend = blend * powTerm;
        blend = blend * *currentPoint;
        result = result + blend;
        currentPoint += 1;
    }

    return result;
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

void nlPolarToCartesian(nlVector3& v, const nlPolar& polar)
{
    float radius = polar.r;
    nlSinCos(&v.y, &v.x, polar.a);
    v.x *= radius;
    v.y *= radius;
}

void nlPolarToCartesian(float& x, float& y, unsigned short angle, float radius)
{
    float* x_ptr = &x;
    float* y_ptr = &y;
    nlSinCos(y_ptr, x_ptr, angle);
    *x_ptr *= radius;
    *y_ptr *= radius;
}

void nlCartesianToPolar(nlPolar& out, float x, float y)
{
    float lenSq = x * x + y * y;
    out.r = nlSqrt(lenSq, true);
    float angle = nlATan2f(y, x);
    out.a = (u16)(s32)(angle * 10430.378f);
}

extern "C" float fn_802B5DD0(const nlVector2& point, const nlVector4& plane)
{
    return plane.x * point.x + plane.y * point.y + plane.w;
}

extern "C" void fn_802B5D74(nlVector3& out, const nlVector3& point, const nlVector4& plane)
{
    float scale = -nlPlaneDot(plane, point)
        / (plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
    out.x = point.x + scale * plane.x;
    out.y = point.y + scale * plane.y;
    out.z = point.z + scale * plane.z;
}

extern "C" void fn_802B5D10(nlVector4& out, const nlVector3& point, const nlVector3& normal)
{
    nlVector3 negativeNormal;
    nlVec3Set(negativeNormal, -normal.x, -normal.y, -normal.z);
    out.x = normal.x;
    out.y = normal.y;
    out.z = normal.z;
    out.w = nlVec3DotProduct(negativeNormal, point);
}

extern "C" void fn_802B5CC0(nlVector4& out, const nlVector2& point, const nlVector2& normal)
{
    nlVector2 negativeNormal;
    nlVec2Set(negativeNormal, -normal.x, -normal.y);
    out.x = normal.x;
    out.y = normal.y;
    out.z = 0.0f;
    out.w = nlVec2DotProduct(negativeNormal, point);
}

void nlMakeRotTransMatrix(
    nlMatrix4& out,
    const nlVector3& v3ForwardVector,
    const nlVector3& v3UpVector,
    const nlVector3& v3AlternateUpVector,
    const nlVector3& v3Translation)
{
    nlVector3 v3Right;
    nlVector3 v3Forward;
    nlVector3 v3Up;

    nlVec3Scale(v3Up, v3UpVector, nlRecipSqrt(nlVec3LengthSquared(v3UpVector), true));
    nlVec3Scale(
        v3Forward, v3ForwardVector, nlRecipSqrt(nlVec3LengthSquared(v3ForwardVector), true));
    nlVec3CrossProduct(v3Right, v3Up, v3Forward);

    if (nlVec3LengthSquared(v3Right) < 0.1f)
    {
        nlVec3Scale(
            v3Up,
            v3AlternateUpVector,
            nlRecipSqrt(nlVec3LengthSquared(v3AlternateUpVector), true));
        nlVec3CrossProduct(v3Right, v3Up, v3Forward);
    }

    nlVec3Scale(v3Right, nlRecipSqrt(nlVec3LengthSquared(v3Right), true));
    nlVec3CrossProduct(v3Up, v3Forward, v3Right);

    out.SetRow4_(0, v3Forward.x, v3Forward.y, v3Forward.z, 0.0f);
    out.SetRow4_(1, v3Right.x, v3Right.y, v3Right.z, 0.0f);
    out.SetRow4_(2, v3Up.x, v3Up.y, v3Up.z, 0.0f);
    out.SetRow4_(3, v3Translation.x, v3Translation.y, v3Translation.z, 1.0f);
}

void nlInvertRotTransMatrix(nlMatrix4& out, const nlMatrix4& in)
{
    nlVector3 negResult;
    nlVector3 translation;
    ((u32*)&translation)[0] = *(u32*)&in.e2[3][0];
    ((u32*)&translation)[1] = *(u32*)&in.e2[3][1];
    ((u32*)&translation)[2] = *(u32*)&in.e2[3][2];

    nlTransposeMatrix(out, in);

    out.e2[2][3] = 0.0f;
    out.e2[1][3] = 0.0f;
    out.e2[0][3] = 0.0f;

    nlMultPosVectorMatrix(negResult, translation, out);

    nlVec3Scale(negResult, -1.0f);

    out.e2[3][0] = negResult.x;
    out.e2[3][1] = negResult.y;
    out.e2[3][2] = negResult.z;
    out.e2[3][3] = 1.0f;
}

void RotateVector(nlVector3& result, const nlVector3& v, nlQuaternion& q)
{
    f32 xz2;
    f32 R;
    f32 xw2;
    f32 P;
    f32 zz;
    f32 S;
    f32 G;
    f32 N;
    f32 z2;
    f32 C;
    f32 K;
    f32 y2;
    f32 O;
    f32 A;
    f32 H;
    f32 yw2;
    f32 xy2;
    f32 yz2;
    f32 vx;
    f32 I;
    f32 xx;
    f32 T;
    f32 J;
    f32 L;
    f32 F;
    f32 yy;
    f32 D;
    f32 M;
    f32 zw2;
    f32 vy;
    f32 vz;
    f32 ww;
    f32 E;
    f32 U;
    f32 x2;
    f32 B;

    xx = q.x * q.x;
    zz = q.z * q.z;
    ww = q.w * q.w;
    x2 = 2.0f * q.x;
    z2 = 2.0f * q.z;
    vy = v.y;
    y2 = 2.0f * q.y;
    vx = v.x;
    yy = q.y * q.y;

    A = ww - xx;
    B = xx + ww;
    vz = v.z;

    xy2 = x2 * q.y;
    zw2 = z2 * q.w;
    C = yy + A;
    D = B - yy;
    E = xy2 - zw2;
    F = C - zz;
    G = A - yy;
    xw2 = x2 * q.w;
    yz2 = y2 * q.z;
    xz2 = x2 * q.z;
    yw2 = y2 * q.w;
    H = xw2 + yz2;
    I = D - zz;
    J = vy * E;
    K = zw2 + xy2;
    L = vy * F;
    M = xz2 + yw2;
    N = vx * I + J;
    O = xz2 - yw2;
    P = vy * H;
    result.x = vz * M + N;
    R = yz2 - xw2;
    S = vx * K + L;
    T = zz + G;
    U = vx * O + P;
    result.y = vz * R + S;
    result.z = vz * T + U;
}

void GetRotationBetweenVectors(
    nlQuaternion& quat, const nlVector3& v3Vec1, const nlVector3& v3Vec2)
{
    float cz;
    float cy;
    float cx;
    float fInvR1R2 =
        nlRecipSqrt(v3Vec1.GetLengthSq3D() * v3Vec2.GetLengthSq3D(), true);
    float fCosAngle = fInvR1R2 * nlVec3DotProduct(v3Vec1, v3Vec2);

    if (fCosAngle > 0.99999f)
    {
        nlQuatIdentity(quat);
    }
    else if (fCosAngle < -0.99999f)
    {
        nlVector3 axis;
        axis.x = 1.0f;

        if (v3Vec1.x > v3Vec1.z || v3Vec1.y > v3Vec1.z)
        {
            axis.x = 0.0f;
            axis.z = 1.0f;
            axis.y = axis.x;
        }
        else
        {
            axis.y = 0.0f;
            axis.z = axis.y;
        }

        cx = axis.y * v3Vec1.z - axis.z * v3Vec1.y;
        cy = -axis.x * v3Vec1.z + axis.z * v3Vec1.x;
        cz = axis.x * v3Vec1.y - axis.y * v3Vec1.x;

        float invLen = nlRecipSqrt(cx * cx + cy * cy + cz * cz, true);

        quat.x = invLen * cx;
        quat.y = invLen * cy;
        quat.z = invLen * cz;
        quat.w = 0.0f;
    }
    else
    {
        float fMagic = nlSqrt((float)(2.0 * (1.0 + fCosAngle)), true);
        float fMultiplier = fInvR1R2 / fMagic;

        cx = v3Vec1.y * v3Vec2.z - v3Vec1.z * v3Vec2.y;
        cy = -v3Vec1.x * v3Vec2.z + v3Vec1.z * v3Vec2.x;
        cz = v3Vec1.x * v3Vec2.y - v3Vec1.y * v3Vec2.x;
        quat.w = 0.5f * fMagic;
        quat.x = cx * fMultiplier;
        quat.y = cy * fMultiplier;
        quat.z = cz * fMultiplier;
    }
}

void nlQuatNLerp(
    nlQuaternion& out, const nlQuaternion& q1, const nlQuaternion& q2, float t)
{
    float dot = nlQuatDot(q1, q2);
    if (dot > 0.0f)
    {
        out.x = t * (q2.x - q1.x) + q1.x;
        out.y = t * (q2.y - q1.y) + q1.y;
        out.z = t * (q2.z - q1.z) + q1.z;
        out.w = t * (q2.w - q1.w) + q1.w;
    }
    else
    {
        out.x = t * (-q2.x - q1.x) + q1.x;
        out.y = t * (-q2.y - q1.y) + q1.y;
        out.z = t * (-q2.z - q1.z) + q1.z;
        out.w = t * (-q2.w - q1.w) + q1.w;
    }
    float fOneOverSqrt = nlRecipSqrt(nlQuatDot(out, out), true);
    nlQuatScale(out, out, fOneOverSqrt);
}

void nlQuatNormalize(nlQuaternion& out, const nlQuaternion& in)
{
    float fLenSquared = nlQuatDot(in, in);
    float fOneOverSqrt = nlRecipSqrt(fLenSquared, true);
    nlQuatScale(out, in, fOneOverSqrt);
}

extern "C" void fn_802B549C(nlQuaternion& out, unsigned short angle)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (unsigned short)(angle >> 1));
    out.z = fSin;
    out.x = 0.0f;
    out.y = 0.0f;
    out.w = fCos;
}

extern "C" void fn_802B5444(nlQuaternion& out, unsigned short angle)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (unsigned short)(angle >> 1));
    out.y = fSin;
    out.x = 0.0f;
    out.z = 0.0f;
    out.w = fCos;
}

extern "C" void fn_802B53EC(nlQuaternion& out, unsigned short angle)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (unsigned short)(angle >> 1));
    out.x = fSin;
    out.y = 0.0f;
    out.z = 0.0f;
    out.w = fCos;
}

extern "C" void fn_802B5370(
    nlQuaternion& out, const nlVector3& v3RotationAxis, unsigned short angle)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, angle >> 1);
    out.x = v3RotationAxis.x * fSin;
    out.y = v3RotationAxis.y * fSin;
    out.z = v3RotationAxis.z * fSin;
    out.w = fCos;
}

void nlMakeRotationMatrixAxisAngle(
    nlMatrix4& out, const nlVector3& v3RotationAxis, float ang_rad)
{
    nlMatrix4 result;
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (u16)(s32)(10430.378f * ang_rad));

    float oneMinusCos = 1.0f - fCos;
    float x = v3RotationAxis.x;
    float y = v3RotationAxis.y;
    float z = v3RotationAxis.z;

    result.SetRow4_(0,
        fCos + oneMinusCos * x * x,
        oneMinusCos * x * y + fSin * z,
        oneMinusCos * x * z - fSin * y,
        0.0f);
    result.SetRow4_(1,
        oneMinusCos * x * y - fSin * z,
        fCos + oneMinusCos * y * y,
        oneMinusCos * y * z + fSin * x,
        0.0f);
    result.SetRow4_(2,
        oneMinusCos * x * z + fSin * y,
        oneMinusCos * y * z - fSin * x,
        fCos + oneMinusCos * z * z,
        0.0f);
    result.SetRow4_(3, 0.0f, 0.0f, 0.0f, 1.0f);
    out = result;
}
