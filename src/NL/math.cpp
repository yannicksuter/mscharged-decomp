#include "NL/nlMath.h"

#include "math.h"

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

void nlProjectPointOntoPlane(nlVector3& out, const nlVector3& point, const nlVector4& plane)
{
    const nlVector3& normal = *(const nlVector3*)&plane;
    float planeOffset = plane.w;
    float scale
        = -(nlVec3DotProduct(normal, point) + planeOffset) / normal.GetLengthSq3D();
    nlVec3ScaleAdd(out, scale, normal, point);
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
    nlVec2Neg(negativeNormal, normal);
    nlVec4Set(out, normal.x, normal.y, 0.0f, nlVec2DotProduct(negativeNormal, point));
}

nlMatrix4& nlMakeRotTransMatrix(
    nlMatrix4& out,
    const nlVector3& v3ForwardVector,
    const nlVector3& v3UpVector,
    const nlVector3& v3AlternateUpVector,
    const nlVector3& v3Translation)
{
    nlVector3 v3Right;
    nlVector3 v3Forward;
    nlVector3 v3Up;

    nlVec3Normalize(v3Up, v3UpVector);
    nlVec3Normalize(v3Forward, v3ForwardVector);
    nlVec3CrossProduct(v3Right, v3Up, v3Forward);

    if (nlVec3LengthSquared(v3Right) < 0.1f)
    {
        nlVec3Normalize(v3Up, v3AlternateUpVector);
        nlVec3CrossProduct(v3Right, v3Up, v3Forward);
    }

    nlVec3Normalize(v3Right, v3Right);
    nlVec3CrossProduct(v3Up, v3Forward, v3Right);

    out.SetRow_(0, v3Forward);
    out.e2[0][3] = 0.0f;
    out.SetRow_(1, v3Right);
    out.e2[1][3] = 0.0f;
    out.SetRow_(2, v3Up);
    out.e2[2][3] = 0.0f;
    out.SetTranslation(v3Translation);
    return out;
}

nlMatrix4& nlInvertRotTransMatrix(nlMatrix4& out, const nlMatrix4& in)
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
    return out;
}

void RotateVector(nlVector3& result, const nlVector3& v, nlQuaternion& q)
{
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float ww = q.w * q.w;
    float x2 = 2.0f * q.x;
    float xy2 = x2 * q.y;
    float xz2 = x2 * q.z;
    float xw2 = x2 * q.w;
    float yz2 = 2.0f * q.y * q.z;
    float yw2 = 2.0f * q.y * q.w;
    float zw2 = 2.0f * q.z * q.w;
    float vx = v.x;
    float vy = v.y;
    float vz = v.z;

    result.x = vx * (xx + ww - yy - zz) + vy * (xy2 - zw2) + vz * (xz2 + yw2);
    result.y = vx * (zw2 + xy2) + vy * (yy + (ww - xx) - zz) + vz * (yz2 - xw2);
    result.z = vx * (xz2 - yw2) + vy * (xw2 + yz2) + vz * (zz + (ww - xx - yy));
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
        float fMagic = nlSqrt(2.0f * (1.0f + fCosAngle), true);
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

void fn_802B549C(nlQuaternion& out, unsigned short angle)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (u16)((u32)angle >> 1));
    nlVec4Set(*(nlVector4*)&out, 0.0f, 0.0f, fSin, fCos);
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
    nlSinCos(&fSin, &fCos, (u16)((u32)angle >> 1));
    nlVec4Set(*(nlVector4*)&out, fSin, 0.0f, 0.0f, fCos);
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
