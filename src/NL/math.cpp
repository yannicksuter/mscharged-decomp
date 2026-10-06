#include "NL/nlMath.h"

nlMatrix4& nlMakeRotationMatrixAxisAngle(
    nlMatrix4& out, const nlVector3& v3RotationAxis, float ang_rad)
{
    nlMatrix4 result;
    nlVector3 axis = v3RotationAxis;
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (u16)(s32)(10430.378f * ang_rad));

    float oneMinusCos = 1.0f - fCos;
    result.m11 = oneMinusCos * (axis.x * axis.x) + fCos;
    result.m21 = oneMinusCos * (axis.x * axis.y) - axis.z * fSin;
    result.m31 = oneMinusCos * (axis.x * axis.z) + axis.y * fSin;
    result.m41 = 0.0f;
    result.m12 = oneMinusCos * (axis.x * axis.y) + axis.z * fSin;
    result.m22 = oneMinusCos * (axis.y * axis.y) + fCos;
    result.m32 = oneMinusCos * (axis.y * axis.z) - axis.x * fSin;
    result.m42 = 0.0f;
    result.m13 = oneMinusCos * (axis.x * axis.z) - axis.y * fSin;
    result.m23 = oneMinusCos * (axis.y * axis.z) + axis.x * fSin;
    result.m33 = oneMinusCos * (axis.z * axis.z) + fCos;
    result.m43 = 0.0f;
    result.m14 = 0.0f;
    result.m24 = 0.0f;
    result.m34 = 0.0f;
    result.m44 = 1.0f;
    out = result;
    return out;
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

extern "C" void fn_802B53EC(nlQuaternion& out, unsigned short angle)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (u16)((u32)angle >> 1));
    nlVec4Set(*(nlVector4*)&out, fSin, 0.0f, 0.0f, fCos);
}

void nlMakeQuatY(nlQuaternion& out, unsigned short angle)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (u16)((u32)angle >> 1));
    nlVec4Set(*(nlVector4*)&out, 0.0f, fSin, 0.0f, fCos);
}

void fn_802B549C(nlQuaternion& out, unsigned short angle)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, (u16)((u32)angle >> 1));
    nlVec4Set(*(nlVector4*)&out, 0.0f, 0.0f, fSin, fCos);
}

void nlQuatNormalize(nlQuaternion& out, const nlQuaternion& in)
{
    float fLenSquared = nlQuatDot(in, in);
    float fOneOverSqrt = nlRecipSqrt(fLenSquared, true);
    nlQuatScale(out, in, fOneOverSqrt);
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

void GetRotationBetweenVectors(
    nlQuaternion& quat, const nlVector3& v3Vec1, const nlVector3& v3Vec2)
{
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
        if (v3Vec1.x > v3Vec1.z || v3Vec1.y > v3Vec1.z)
        {
            nlVec3Set(axis, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            nlVec3Set(axis, 1.0f, 0.0f, 0.0f);
        }

        nlVec3CrossProduct(axis, axis, v3Vec1);
        nlVec3Normalize(*(nlVector3*)&quat, axis);
        quat.w = 0.0f;
    }
    else
    {
        float fMagic = nlSqrt(2.0f * (1.0f + fCosAngle), true);
        float fMultiplier = fInvR1R2 / fMagic;

        nlVector3 cross;
        nlVec3CrossProduct(cross, v3Vec1, v3Vec2);
        quat.w = 0.5f * fMagic;
        quat.x = cross.x * fMultiplier;
        quat.y = cross.y * fMultiplier;
        quat.z = cross.z * fMultiplier;
    }
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
