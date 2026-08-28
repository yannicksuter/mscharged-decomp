#ifndef _NLMATH_H_
#define _NLMATH_H_

#include "types.h"

float nlBezier(float* fControlPoints, int nNumPoints, float fMu);
float nlATan(float x);
float nlATan2f(float y, float x);
float nlTan(unsigned short angle);
u16 nlACos(float x);
void nlSinCos(float* presult_sin, float* presult_cos, unsigned short angle);
float nlSin(unsigned short angle);
float nlRecipSqrt(float x, bool bAccurate);
float nlSqrt(float x, bool bAccurate);
float nlRandomf(float fMin, float fMax, unsigned int* pSeed);
float nlRandomf(float fMax, unsigned int* pSeed);
float nlRandomf(float fMax);
unsigned int nlRandom(unsigned int range, unsigned int* seed);
unsigned int nlRandom(unsigned int range);
void nlSetRandomSeed(unsigned int value, unsigned int* seed);
void nlInitRandom();
u32 nlLog2(u32 value);
float nlFastLog2(float value);
float nlFastExp2(float value);

inline unsigned int nlAlignUp(unsigned int value, unsigned int alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

inline float nlAbs(const float& value)
{
    return __fabs(value);
}

inline float AngUnitsToRad_fromUnsignedShort(unsigned short sUnits)
{
    return (float)sUnits * 0.0000958738f;
}

inline int DegreesToAngle(float degrees)
{
    return (int)(65536.0f * degrees / 360.0f);
}

inline bool nlNear(float first, float second)
{
    return (float)__fabs(first - second) <= 0.0001f;
}

inline s16 nlAngleDiff(u16 a, u16 b)
{
    return (s16)(a - b);
}

#pragma cpp_extensions on

class nlVector2
{
public:
    union
    {
        float e[2];
        struct
        {
            float x;
            float y;
        };
    };
};

class nlVector3
{
public:
    union
    {
        float e[3];
        u32 as_u32[3];
        struct
        {
            float x;
            float y;
            float z;
        };
    };

    void Set(float _x, float _y, float _z);

    inline float CalculateDistanceSquared2D(const nlVector3& v)
    {
        float dx = x - v.x;
        float dy = y - v.y;
        return dx * dx + dy * dy;
    }

    inline float GetLengthSq2D() const
    {
        return x * x + y * y;
    }

    inline void Sub2D(const nlVector3& a, const nlVector3& b)
    {
        float dy = a.y - b.y;
        float dx = a.x - b.x;
        x = dx;
        y = dy;
    }

    inline float GetLengthSq3D() const
    {
        return x * x + y * y + z * z;
    }
};

inline void nlVec2Set(nlVector2& v0, float _x, float _y)
{
    v0.x = _x;
    v0.y = _y;
}

inline void nlVec2Sub(nlVector2& result, const nlVector2& a, const nlVector2& b)
{
    nlVec2Set(result, a.x - b.x, a.y - b.y);
}

inline float nlGetLengthSquared1D(float x)
{
    return x * x;
}

inline float nlGetLengthSquared2D(float x, float y)
{
    return x * x + y * y;
}

inline float nlGetLength2D(float x, float y)
{
    float xx = x * x;
    float yy = y * y;
    const float lengthSquared = xx + yy;
    return nlSqrt(lengthSquared, true);
}

inline float nlGetLengthSquared3D(float x, float y, float z)
{
    return x * x + y * y + z * z;
}

inline float nlGetLength3D(float x, float y, float z)
{
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    const float lengthSquared = xx + yy + zz;
    return nlSqrt(lengthSquared, true);
}

inline float nlVec2DotProduct(const nlVector2& a, const nlVector2& b)
{
    return a.x * b.x + a.y * b.y;
}

inline float nlVec2LengthSquared(const nlVector2& v)
{
    return nlGetLengthSquared2D(v.x, v.y);
}

/**
 * Vector-reference length. The single-expression form below is required by
 * R4QE01 and is not interchangeable with nlGetLength2D()/nlGetLength3D():
 * those keep their named per-component temporaries, which CodeWarrior
 * schedules differently. Do not "unify" the two bodies.
 */
inline float nlVec2Length(const nlVector2& v)
{
    return nlSqrt(v.x * v.x + v.y * v.y, true);
}

inline float nlVec3Length(const nlVector3& v)
{
    return nlSqrt(v.x * v.x + v.y * v.y + v.z * v.z, true);
}

inline float nlVec3DistanceSquared2D(const nlVector3& a, const nlVector3& b)
{
    nlVector2 delta;
    delta.x = a.x - b.x;
    delta.y = a.y - b.y;
    return nlGetLengthSquared2D(delta.x, delta.y);
}

inline void nlVec3Set(nlVector3& v0, float _x, float _y, float _z)
{
    v0.x = _x;
    v0.y = _y;
    v0.z = _z;
}

inline void nlVec3Sub(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    nlVec3Set(result, a.x - b.x, a.y - b.y, a.z - b.z);
}

inline void nlVec3Sub2D(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    result.x = a.x - b.x;
    result.y = a.y - b.y;
}

inline float nlVec3DotProduct(const nlVector3& a, const nlVector3& b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline float nlVec3DotProduct2D(const nlVector3& a, const nlVector3& b)
{
    return (a.x * b.x) + (a.y * b.y);
}

inline void nlVec3CrossProduct(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    float x = (a.y * b.z) - (a.z * b.y);
    float y = (-a.x * b.z) + (a.z * b.x);
    float z = (a.x * b.y) - (a.y * b.x);
    nlVec3Set(result, x, y, z);
}

inline void nlVec3CrossProductAlt(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    nlVec3Set(result,
        (a.x * b.y) - (a.y * b.x),
        (-a.x * b.z) + (a.z * b.x),
        (a.y * b.z) - (a.z * b.y));
}

inline float nlVec3LengthSquared(const nlVector3& v)
{
    return v.GetLengthSq3D();
}

inline void nlVec3Add(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    nlVec3Set(result, a.x + b.x, a.y + b.y, a.z + b.z);
}

inline void nlVec3Add(nlVector3& result, float x, float y, float z)
{
    nlVec3Set(result, x + result.x, y + result.y, z + result.z);
}

inline void nlVec3Scale(nlVector3& result, const nlVector3& v, const float scale)
{
    nlVec3Set(result, scale * v.x, scale * v.y, scale * v.z);
}

inline void nlVec3Scale(nlVector3& result, float scale)
{
    nlVec3Set(result, scale * result.x, scale * result.y, scale * result.z);
}

inline void nlVecLerp(nlVector3& result, const nlVector3& a, const nlVector3& b, float alpha)
{
    float oneMinusAlpha = 1.0f - alpha;
    result.x = oneMinusAlpha * a.x + alpha * b.x;
    result.y = oneMinusAlpha * a.y + alpha * b.y;
    result.z = oneMinusAlpha * a.z + alpha * b.z;
}

inline void nlVec3Cross(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    nlVec3Set(result,
        (a.y * b.z) - (a.z * b.y),
        (-a.x * b.z) + (a.z * b.x),
        (a.x * b.y) - (a.y * b.x));
}

inline void nlVec3ScaleAdd(nlVector3& result, float scale, const nlVector3& dir, const nlVector3& origin)
{
    nlVec3Set(result,
        scale * dir.x + origin.x,
        scale * dir.y + origin.y,
        scale * dir.z + origin.z);
}

inline void nlVec3WeightedSum(nlVector3& result, float wa, const nlVector3& a, float wb, const nlVector3& b)
{
    result.x = wa * a.x + wb * b.x;
    result.y = wa * a.y + wb * b.y;
    result.z = wa * a.z + wb * b.z;
}

class nlVector4
{
public:
    union
    {
        struct
        {
            float x; // offset 0x0, size 0x4
            float y; // offset 0x4, size 0x4
            float z; // offset 0x8, size 0x4
            float w; // offset 0xC, size 0x4
        };
        float e[4]; // offset 0x0, size 0x10
    };
}; // total size: 0x10

inline void nlVec4Set(nlVector4& v0, float _x, float _y, float _z, float _w)
{
    v0.x = _x;
    v0.y = _y;
    v0.z = _z;
    v0.w = _w;
}

inline float nlPlaneDot(const nlVector4& plane, const nlVector3& v)
{
    return v.x * plane.x + v.y * plane.y + v.z * plane.z + plane.w;
}

/**
 * Signed side of a plane stored as (normal.xyz, distance), where the plane
 * satisfies dot(normal, p) == w. Note the sign convention differs from
 * nlPlaneDot(), which adds w instead of subtracting it.
 */
inline float nlPlaneSide(const nlVector3& point, const nlVector4& plane)
{
    return nlVec3DotProduct(point, *(const nlVector3*)&plane) - plane.w;
}

class nlMatrix3
{
public:
    union
    {                   // inferred
        float e[9];     // offset 0x0, size 0x24
        float e2[3][3]; // offset 0x0, size 0x24
        struct
        {              // inferred
            float m11; // offset 0x0, size 0x4
            float m12; // offset 0x4, size 0x4
            float m13; // offset 0x8, size 0x4
            float m21; // offset 0xC, size 0x4
            float m22; // offset 0x10, size 0x4
            float m23; // offset 0x14, size 0x4
            float m31; // offset 0x18, size 0x4
            float m32; // offset 0x1C, size 0x4
            float m33; // offset 0x20, size 0x4
        };
    };

    inline void SetIdentity()
    {
        m32 = 0.0f;
        m31 = 0.0f;
        m23 = 0.0f;
        m21 = 0.0f;
        m13 = 0.0f;
        m12 = 0.0f;

        m33 = 1.0f;
        m22 = 1.0f;
        m11 = 1.0f;
    }

    inline nlVector2 operator*(const nlVector2& v_in) const
    {
        nlVector2 result;
        result.x = e[6] + ((e[0] * v_in.x) + (e[3] * v_in.y));
        result.y = e[7] + ((e[1] * v_in.x) + (e[4] * v_in.y));
        return result;
    }
};

class nlMatrix4
{
public:
    union
    {
        float e2[4][4]; // offset 0x0, size 0x40
        float e[16];    // offset 0x0, size 0x40
        struct
        {              // inferred
            float m11; // offset 0x0, size 0x4
            float m12; // offset 0x4, size 0x4
            float m13; // offset 0x8, size 0x4
            float m14; // offset 0xC, size 0x4
            float m21; // offset 0x10, size 0x4
            float m22; // offset 0x14, size 0x4
            float m23; // offset 0x18, size 0x4
            float m24; // offset 0x1C, size 0x4
            float m31; // offset 0x20, size 0x4
            float m32; // offset 0x24, size 0x4
            float m33; // offset 0x28, size 0x4
            float m34; // offset 0x2C, size 0x4
            float m41; // offset 0x30, size 0x4
            float m42; // offset 0x34, size 0x4
            float m43; // offset 0x38, size 0x4
            float m44; // offset 0x3C, size 0x4
        };
    };

    void SetIdentity();
    void SetColumn(int col, const nlVector3& v);
    unsigned char IsRigidTransformation(float tolerance) const;

    void SetColumn_(int col, const nlVector3& v)
    {
        e2[0][col] = v.x;
        e2[1][col] = v.y;
        e2[2][col] = v.z;
    }

    void SetRow_(int row, const nlVector3& v)
    {
        e2[row][0] = v.x;
        e2[row][1] = v.y;
        e2[row][2] = v.z;
    }

    void SetRow4_(int row, const float x, const float y, const float z, const float w)
    {
        e2[row][0] = x;
        e2[row][1] = y;
        e2[row][2] = z;
        e2[row][3] = w;
    }

    inline nlVector3& GetTranslation() const
    {
        return *(nlVector3*)&e2[3][0];
    }
    void SetTranslation(const nlVector3& trans);

    inline nlVector4 operator*(const nlVector4& v_in) const
    {
        nlVector4 result;
        result.x = e2[0][0] * v_in.x + e2[1][0] * v_in.y + e2[2][0] * v_in.z + e2[3][0] * v_in.w;
        result.z = e2[0][2] * v_in.x + e2[1][2] * v_in.y + e2[2][2] * v_in.z + e2[3][2] * v_in.w;
        result.y = e2[0][1] * v_in.x + e2[1][1] * v_in.y + e2[2][1] * v_in.z + e2[3][1] * v_in.w;
        result.w = e2[0][3] * v_in.x + e2[1][3] * v_in.y + e2[2][3] * v_in.z + e2[3][3] * v_in.w;
        return result;
    }
};

#pragma cpp_extensions reset

void nlVecAdd(nlVector3& out, const nlVector3& a, const nlVector3& b);

/**
 * Convert a nlMatrix3 [3x3] matrix to a [4x3] ODE matrix
 */
inline void ConvertNLMat3ToDMat3(const nlMatrix3& src, float* dest)
{
    nlVec3Set(*(nlVector3*)dest, src.e[0], src.e[3], src.e[6]);
    nlVec3Set(*(nlVector3*)&dest[4], src.e[1], src.e[4], src.e[7]);
    nlVec3Set(*(nlVector3*)&dest[8], src.e[2], src.e[5], src.e[8]);
}

/**
 * Convert a nlMatrix4 [4x4] matrix to a [4x3] ODE matrix (Column Transpose)
 */
inline void ConvertNLMat4ToDMat3_Transposed(const nlMatrix4& src, float* dest)
{
    nlVec4Set(*(nlVector4*)&dest[0], src.e[0], src.e[4], src.e[8], src.e[12]);
    nlVec4Set(*(nlVector4*)&dest[4], src.e[1], src.e[5], src.e[9], src.e[13]);
    nlVec4Set(*(nlVector4*)&dest[8], src.e[2], src.e[6], src.e[10], src.e[14]);
}

extern unsigned int nlDefaultSeed;

#include "NL/platvmath.h"
#include "NL/platqmath.h"
#include "NL/utility.h"

struct nlPolar
{
    /* 0x00 */ u16 a;
    /* 0x04 */ float r;
}; // size: 0x8

void nlInvertRotTransMatrix(nlMatrix4& out, const nlMatrix4& in);
void nlMakeRotTransMatrix(nlMatrix4& out, const nlVector3& v3ForwardVector,
    const nlVector3& v3UpVector, const nlVector3& v3AlternateUpVector,
    const nlVector3& v3Translation);
void nlMakeRotationMatrixAxisAngle(
    nlMatrix4& out, const nlVector3& v3RotationAxis, float ang_rad);
void GetRotationBetweenVectors(
    nlQuaternion& quat, const nlVector3& v3Vec1, const nlVector3& v3Vec2);
void RotateVector(nlVector3& result, const nlVector3& v, nlQuaternion& q);
void nlQuatNLerp(nlQuaternion& out, const nlQuaternion& q1, const nlQuaternion& q2, float t);
void nlQuatInverse(nlQuaternion& out, const nlQuaternion& in);
void nlQuatNormalize(nlQuaternion& out, const nlQuaternion& in);
void nlMakeQuat(nlQuaternion& out, const nlVector3& v3RotationAxis, float ang_rad);
void nlCartesianToPolar(nlPolar& out, const nlVector3& in);
void nlAddPolarToCartesian(nlVector3& result, const nlPolar& polar);
void nlPolarToCartesian(nlVector3& v, const nlPolar& polar);
void nlPolarToCartesian(float& x, float& y, unsigned short angle, float radius);
void nlCartesianToPolar(nlPolar& out, float x, float y);

#endif
