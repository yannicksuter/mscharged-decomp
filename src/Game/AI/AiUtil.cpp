#include "Game/AI/AiUtil.h"

#include "math.h"

unsigned int nlRandom(unsigned int range);

char* g_sPowerupNames[21] = {
    "GreenShell",
    "RedShell",
    "SpinyShell",
    "FreezeShell",
    "Banana",
    "BoBomb",
    "ChainChomp",
    "Mushroom",
    "Star",
    "Mario",
    "Peach",
    "DK",
    "Bowser",
    "Luigi",
    "Daisy",
    "Wario",
    "Waluigi",
    "BowserJr",
    "Diddy",
    "Yoshi",
    "Petey"
};

char* g_sSurfaceNames[6] = {
    "Dry",
    "Normal",
    "Slow",
    "Wet",
    "Slippery",
    "Thunder"
};

void MakePerpendicularPlane(const nlVector3& v3Position, unsigned short aNormalAngle, nlVector4& v4Plane, float fPlaneOffset)
{
    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, aNormalAngle);

    v4Plane.x = fCos;
    v4Plane.y = fSin;
    v4Plane.z = 0.0f;

    float t0 = v3Position.y * fSin;
    float t1 = v3Position.x * fCos + t0;
    v4Plane.w = fPlaneOffset + t1;
}

void MakePerpendicularPlane(const nlVector3& v3Position, const nlVector3& v3Normal, nlVector4& v4Plane, float fPlaneOffset)
{
    f32 lenSq = const_cast<nlVector3&>(v3Normal).GetLengthSq3D();
    f32 invLen = nlRecipSqrt(lenSq, true);

    f32 normX = invLen * v3Normal.x;
    f32 normY = invLen * v3Normal.y;
    f32 normZ = invLen * v3Normal.z;
    v4Plane.x = normX;
    v4Plane.y = normY;
    v4Plane.z = normZ;

    v4Plane.w = fPlaneOffset + (v3Position.x * v4Plane.x + v3Position.y * v4Plane.y + v3Position.z * v4Plane.z);
}

bool IsPointInCone(const nlVector3& v3Point, const nlVector3& v3Pivot, const nlVector3& v3Plane1, const nlVector3& v3Plane2)
{
    float distSqA = nlVec3DistanceSquared2D(v3Plane1, v3Pivot);
    float distSqP = nlVec3DistanceSquared2D(v3Plane1, v3Point);

    if (distSqP < distSqA)
    {
        nlVector3 v3Direction;
        float dirY = v3Pivot.y - v3Point.y;
        float dirZ = v3Pivot.z - v3Point.z;
        float dirX = v3Pivot.x - v3Point.x;
        nlVec3Set(v3Direction, dirX, dirY, dirZ);

        nlVector3 v3Normal;
        nlVec3Set(v3Normal, -v3Direction.y, v3Direction.x, 0.0f);
        float invLen = nlRecipSqrt(v3Normal.GetLengthSq3D(), true);

        nlVector4 v4Plane;
        v4Plane.x = invLen * v3Normal.x;
        v4Plane.y = invLen * v3Normal.y;
        v4Plane.z = invLen * v3Normal.z;
        float zeroVal = 0.0f;
        v4Plane.w = (v3Pivot.x * v4Plane.x + v3Pivot.y * v4Plane.y + v3Pivot.z * v4Plane.z) + zeroVal;

        f32 sideLeft = nlPlaneSide(v3Plane1, v4Plane);
        f32 sideRight = nlPlaneSide(v3Plane2, v4Plane);

        if (sideLeft * sideRight < zeroVal)
        {
            return true;
        }
    }

    return false;
}

#undef abs
extern "C" int abs(int n);

unsigned short SeekDirection(unsigned short aCurrent, unsigned short aDesired, float fSeekSpeed, float fFalloff, float fDeltaT)
{
    u16 current = aCurrent;
    s16 diff = nlAngleDiff(aDesired, current);
    signed short nDeltaDisplacement;

    if (diff != 0)
    {
        f32 fDiff = (f32)(s16)diff;
        f64 absDiffD = __fabs((f64)fDiff);
        f32 absDiff = (f32)absDiffD;

        if (absDiff < fFalloff)
        {
            f32 fSeekCoefficient = fFalloff * fFalloff / fSeekSpeed;
            nDeltaDisplacement = (s16)(s32)(absDiff - fSeekCoefficient / (fDeltaT + fSeekCoefficient / absDiff));
        }
        else
        {
            nDeltaDisplacement = (s16)(s32)(fDeltaT * fSeekSpeed);
        }

        if (abs(diff) < (s16)nDeltaDisplacement)
        {
            return aDesired;
        }

        if (diff > 0)
        {
            return (u16)(current + (s16)nDeltaDisplacement);
        }
        else
        {
            return (u16)(current - (s16)nDeltaDisplacement);
        }
    }

    return aDesired;
}

float SeekSpeed(float fCurrent, float fDesired, float fSeekAccel, float fSeekDecel, float fDeltaT)
{
    float seekRate = (fCurrent <= fDesired) ? fSeekAccel : fSeekDecel;
    float delta = seekRate * fDeltaT;

    if (fCurrent <= fDesired)
    {
        float newValue = fCurrent + delta;
        if (newValue > fDesired)
        {
            return fDesired;
        }
        return newValue;
    }

    float newValue = fCurrent - delta;
    if (newValue < fDesired)
    {
        return fDesired;
    }
    return newValue;
}

void CalcInterceptXY(const nlVector3& pos1, f32 speed1, f32 speed2, const nlVector3& pos2, const nlVector3& vel, int& count, f32* times)
{
    int numRoots;
    f32 roots[2];
    nlVector2 delta;
    f32 limbSq = speed2 * speed2;
    f32 speedSq = speed1 * speed1;
    delta.y = pos2.y - pos1.y;
    delta.x = pos2.x - pos1.x;
    f32 velSq = vel.x * vel.x + vel.y * vel.y;
    f32 distSq = delta.x * delta.x + delta.y * delta.y;
    f32 dot = vel.x * delta.x + vel.y * delta.y;
    f32 a = velSq - speedSq;

    if (distSq <= limbSq)
    {
        count = 1;
        times[0] = 0.0f;
        return;
    }

    f32 b = 2.0f * (dot - speed2 * speed1);
    f32 c = distSq - limbSq;

    SolveQuadratic(a, b, c, numRoots, roots[0], roots[1]);

    count = 0;
    for (int i = 0; i < numRoots; i++)
    {
        f32 root = roots[i];
        if (root > 0.0f)
        {
            times[count] = root;
            count++;
        }
    }
}


bool TestCollision(float rp, const nlVector3& p1, const nlVector3& p2, float rq, const nlVector3& q1, const nlVector3& q2)
{
    float combinedRadius = rp + rq;
    float radiusSq = combinedRadius * combinedRadius;

    // Check start positions
    nlVector3 diff;
    nlVec3Sub(diff, q1, p1);
    float distSq = nlGetLengthSquared3D(diff.x, diff.y, diff.z);

    if (distSq <= radiusSq)
    {
        return true;
    }

    // Check end positions
    float dy = q2.y - p2.y;
    float dx = q2.x - p2.x;
    float dz = q2.z - p2.z;
    nlVec3Set(diff, dx, dy, dz);
    distSq = nlGetLengthSquared3D(diff.x, diff.y, diff.z);

    if (distSq <= radiusSq)
    {
        return true;
    }

    // Do sweep test
    float t = SweepSpheres(rp, p1, p2, rq, q1, q2);
    if (t >= 0.0f && t <= 1.0f)
    {
        return true;
    }

    return false;
}

float Exp(float k)
{
    float result = 1.0f;
    float powk;
    float negk = -k;
    powk = negk * negk;
    result += 0.24999869f * negk;
    result += 0.031257585f * powk;
    powk = powk * negk;
    result += 0.0025913713f * powk;
    powk = powk * negk;
    result += 0.000171562f * powk;
    powk = powk * negk;
    result += 0.0000054302f * powk;
    powk = powk * negk;
    result += 0.0000006906f * powk;

    float t0 = result * result;
    float t1 = result * t0;
    float t2 = result * t1;

    return 1.0f / t2;
}

float GetClosingSpeed2D(const nlVector3& pos1, const nlVector3& vel1, const nlVector3& pos2, const nlVector3& vel2)
{
    nlVector3 delta;
    delta.Sub2D(pos2, pos1);
    float distSq = delta.GetLengthSq2D();

    if (distSq < 0.001f)
    {
        delta.Sub2D(vel1, vel2);
        return nlSqrt(delta.GetLengthSq2D(), true);
    }

    float invDist = nlRecipSqrt(distSq, true);
    nlVec2Set(*(nlVector2*)&delta, invDist * delta.x, invDist * delta.y);
    return nlVec3DotProduct2D(delta, vel1) - nlVec3DotProduct2D(delta, vel2);
}

float GetClosingSpeed(const nlVector3& pos1, const nlVector3& vel1, const nlVector3& pos2, const nlVector3& vel2)
{
    nlVector3 delta;
    float dy = pos2.y - pos1.y;
    float dx = pos2.x - pos1.x;
    float dz = pos2.z - pos1.z;
    nlVec3Set(delta, dx, dy, dz);
    float distSq = delta.GetLengthSq3D();

    if (distSq < 0.001f)
    {
        dy = vel1.y - vel2.y;
        dx = vel1.x - vel2.x;
        dz = vel1.z - vel2.z;
        nlVec3Set(delta, dx, dy, dz);
        return nlSqrt(delta.GetLengthSq3D(), true);
    }

    nlVec3Scale(delta, nlRecipSqrt(distSq, true));
    return nlVec3DotProduct(delta, vel1) - nlVec3DotProduct(delta, vel2);
}

void GetLocalPoint(nlVector3& v3LocalPointOut, const nlVector3& v3WorldPointIn, const nlVector3& v3RefPosition, unsigned short aRefAngle)
{
    float fSin; // r1+0xC
    float fCos; // r1+0x8

    nlSinCos(&fSin, &fCos, aRefAngle);
    float dx = v3WorldPointIn.x - v3RefPosition.x;
    float dy = v3WorldPointIn.y - v3RefPosition.y;

    v3LocalPointOut.x = (fCos * dx) + (fSin * dy);
    v3LocalPointOut.y = (fCos * dy) - (fSin * dx);
    v3LocalPointOut.z = v3WorldPointIn.z;
}

void GetWorldPoint(nlVector3& v3WorldPointOut, const nlVector3& v3LocalPointIn, const nlVector3& v3RefPosition, unsigned short aRefAngle)
{
    f32 fSin;
    f32 fCos;

    float localX = v3LocalPointIn.x;
    float localY = v3LocalPointIn.y;

    nlSinCos(&fSin, &fCos, aRefAngle);

    v3WorldPointOut.x = v3RefPosition.x + ((fCos * localX) - (fSin * localY));
    v3WorldPointOut.y = v3RefPosition.y + ((fCos * localY) + (fSin * localX));
    v3WorldPointOut.z = v3LocalPointIn.z;
}

void RotateVectorZAxis(nlVector3& v3Out, const nlVector3& v3In, unsigned short angle)
{
    f32 fSin;
    f32 fCos;

    f32 localX = v3In.x;
    f32 localY = v3In.y;

    nlSinCos(&fSin, &fCos, angle);

    v3Out.x = (fCos * localX) - (fSin * localY);
    v3Out.y = (fCos * localY) + (fSin * localX);
    v3Out.z = v3In.z;
}


float Interpolate(float fMin, float fMax, float fPercent)
{
    return (fPercent * (fMax - fMin)) + fMin;
}

float InterpolateClamped(float fMin, float fMax, float fPercent)
{
    fPercent = (fPercent >= 0.0f) ? fPercent : 0.0f;
    fPercent = (fPercent <= 1.0f) ? fPercent : 1.0f;
    return (fPercent * (fMax - fMin)) + fMin;
}

float InterpolateRange(float fResultMin, float fResultMax, float fInputMin, float fInputMax, float fInput)
{
    float range = fInputMax - fInputMin;
    if (fabsf(range) < 0.00001f)
    {
        return fResultMax;
    }

    return fResultMin + ((fInput - fInputMin) / range) * (fResultMax - fResultMin);
}

static inline float AiInterpolateRange(float fResultMin, float fResultMax, float fInputMin, float fInputMax, float fInput)
{
    float range = fInputMax - fInputMin;
    if (fabsf(range) < 0.00001f)
    {
        return fResultMax;
    }
    return fResultMin + ((fInput - fInputMin) / range) * (fResultMax - fResultMin);
}

float InterpolateRangeClamped(float fResultMin, float fResultMax, float fInputMin, float fInputMax, float fInput)
{
    if (fInputMin < fInputMax)
    {
        fInput = (fInput >= fInputMin) ? fInput : fInputMin;
        fInput = (fInput <= fInputMax) ? fInput : fInputMax;
    }
    else
    {
        fInput = (fInput >= fInputMax) ? fInput : fInputMax;
        fInput = (fInput <= fInputMin) ? fInput : fInputMin;
    }

    float result = AiInterpolateRange(fResultMin, fResultMax, fInputMin, fInputMax, fInput);
    return result;
}


float NormalizeVal(float fromVal, const nlVector2& fromExtrema)
{
    float fromMin = fromExtrema.x;
    float fromMax = fromExtrema.y;
    if (fromMax == fromMin)
        return 1.0f;

    float t = (fromVal - fromMin) / (fromMax - fromMin);

    float lo = (t >= 0.0f) ? t : 0.0f;
    float res = (lo <= 1.0f) ? lo : 1.0f;

    return res;
}

float NormalizeVal(float fromVal, float fromMin, float fromMax)
{
    if (fromMin == fromMax)
        return 1.0f;

    float t = (fromVal - fromMin) / (fromMax - fromMin);

    float lo = (t >= 0.0f) ? t : 0.0f;
    float res = (lo <= 1.0f) ? lo : 1.0f;

    return res;
}

float AIsgn(float fValue)
{
    if (fValue >= 0.0f)
    {
        return 1.0f;
    }
    return -1.0f;
}

/*
 * Matching scaffolding for GetClosestPointOnLineABFromPointC(), not reusable
 * math -- keep all of it local to this file.
 *
 * AiIdentity() and AiMultiply() only fix operand order. AiClosestInitialSub(),
 * AiClosestOffsetSub() and AiClosestSub() are textually identical on purpose:
 * CodeWarrior expands each inline symbol independently, so folding them onto
 * one name -- or onto nlMath.h's nlVec2Sub()/nlVec3Sub() -- shifts register
 * allocation in both overloads and loses the match.
 */
static inline float AiIdentity(float value)
{
    return value;
}

static inline float AiMultiply(float first, float second)
{
    return first * second;
}

static inline bool AiCoincident(const nlVector2& a, const nlVector2& b)
{
    return nlNear(b.x, AiIdentity(a.x))
        && nlNear(b.y, AiIdentity(a.y));
}

static inline bool AiCoincident(const nlVector3& a, const nlVector3& b)
{
    return (nlNear(b.x, AiIdentity(a.x))
        && nlNear(b.y, AiIdentity(a.y)))
        && nlNear(b.z, AiIdentity(a.z));
}

static inline void AiClosestInitialSub(nlVector2& result, const nlVector2& a, const nlVector2& b)
{
    float x;
    float y;
    y = a.y - b.y;
    x = a.x - b.x;
    nlVec2Set(result, x, y);
}

static inline void AiClosestInitialSub(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    float x;
    float y;
    float z;
    z = a.z - b.z;
    y = a.y - b.y;
    x = a.x - b.x;
    nlVec3Set(result, x, y, z);
}

static inline float AiClosestProjectionFactor(const nlVector2& ac, const nlVector2& ab)
{
    float dot = nlVec2DotProduct(ac, ab);
    float length = nlVec2LengthSquared(ab);
    return dot / length;
}

static inline void AiClosestScale(nlVector2& result, const nlVector2& value, float scale)
{
    float y;
    float x;
    y = value.y;
    y = scale * y;
    x = value.x;
    x = scale * x;
    nlVec2Set(result, x, y);
}

static inline void AiClosestScale(nlVector3& result, const nlVector3& value, float scale)
{
    nlVec3Set(result,
        AiMultiply(scale, value.x),
        scale * value.y,
        scale * value.z);
}

static inline void AiClosestAdd(nlVector2& result, const nlVector2& a, const nlVector2& b)
{
    nlVec2Set(result, a.x + b.x, AiIdentity(a.y + b.y));
}

static inline void AiClosestAdd(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    float x;
    float y;
    float z;
    z = a.z + b.z;
    y = a.y + b.y;
    x = AiIdentity(a.x + b.x);
    nlVec3Set(result, x, y, z);
}

static inline void AiClosestOffsetSub(nlVector2& result, const nlVector2& a, const nlVector2& b)
{
    float y;
    float x;
    y = a.y - b.y;
    x = a.x - b.x;
    nlVec2Set(result, x, y);
}

static inline void AiClosestOffsetSub(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    float z;
    float y;
    float x;
    z = a.z - b.z;
    y = a.y - b.y;
    x = a.x - b.x;
    nlVec3Set(result, x, y, z);
}

static inline void AiClosestSub(nlVector2& result, const nlVector2& a, const nlVector2& b)
{
    float x;
    float y;
    y = a.y - b.y;
    x = a.x - b.x;
    nlVec2Set(result, x, y);
}

static inline void AiClosestSub(nlVector3& result, const nlVector3& a, const nlVector3& b)
{
    float x;
    float y;
    float z;
    z = a.z - b.z;
    y = a.y - b.y;
    x = a.x - b.x;
    nlVec3Set(result, x, y, z);
}

nlVector3 GetClosestPointOnLineABFromPointC(const nlVector3& a, const nlVector3& b, const nlVector3& c)
{
    if (AiCoincident(a, b))
    {
        return a;
    }

    nlVector3 ac;
    nlVector3 ab;
    nlVector3 offset;
    nlVector3 reconstructed;
    nlVector3 projected;
    nlVector3 toA;
    nlVector3 toB;
    nlVector3 scaledAb;
    AiClosestInitialSub(ac, c, a);
    AiClosestInitialSub(ab, b, a);

    float dot = nlVec3DotProduct(ac, ab);
    float length = ab.GetLengthSq3D();
    const float& t = dot / length;
    AiClosestScale(scaledAb, ab, t);
    AiClosestAdd(reconstructed, a, ac);
    AiClosestOffsetSub(offset, ac, scaledAb);
    AiClosestSub(projected, reconstructed, offset);
    AiClosestSub(toA, a, projected);
    AiClosestSub(toB, b, projected);

    if (nlVec3Length(toA) > nlVec3Length(ab))
    {
        goto outside;
    }
    if (!(nlVec3Length(toB) > nlVec3Length(ab)))
    {
        goto projected_result;
    }

outside:
    if (nlVec3Length(toA) < nlVec3Length(toB))
    {
        return a;
    }
    return b;

projected_result:
    return projected;
}

nlVector2 GetClosestPointOnLineABFromPointC(const nlVector2& a, const nlVector2& b, const nlVector2& c)
{
    if (AiCoincident(a, b))
    {
        return a;
    }

    nlVector2 ac;
    nlVector2 ab;
    nlVector2 offset;
    nlVector2 reconstructed;
    nlVector2 projected;
    nlVector2 toA;
    nlVector2 toB;
    nlVector2 scaledAb;
    AiClosestInitialSub(ac, c, a);
    AiClosestInitialSub(ab, b, a);

    float t = AiClosestProjectionFactor(ac, ab);
    AiClosestScale(scaledAb, ab, t);
    AiClosestAdd(reconstructed, a, ac);
    AiClosestOffsetSub(offset, ac, scaledAb);
    AiClosestSub(projected, reconstructed, offset);
    AiClosestSub(toA, a, projected);
    AiClosestSub(toB, b, projected);

    if (nlVec2Length(toA) > nlVec2Length(ab))
    {
        goto outside;
    }
    if (!(nlVec2Length(toB) > nlVec2Length(ab)))
    {
        goto projected_result;
    }

outside:
    if (nlVec2Length(toA) < nlVec2Length(toB))
    {
        return a;
    }
    return b;

projected_result:
    return projected;
}

void SortToMinOrMaxTotalSum(unsigned int* result, float (*data)[4], bool findMin)
{
    f32 bestSum = findMin ? 1e35 : -1e35;

    for (s32 i = 0; i < 4; i++)
    {
        f32 sum = data[0][i];
        for (s32 j = 0; j < 4; j++)
        {
            if (j == i)
                continue;
            f32 save2 = sum;
            sum += data[1][j];
            for (s32 k = 0; k < 4; k++)
            {
                if (k == j)
                    continue;
                if (k == i)
                    continue;
                f32 save3 = sum;
                sum += data[2][k];
                for (s32 l = 0; l < 4; l++)
                {
                    if (l == k)
                        continue;
                    if (l == j)
                        continue;
                    if (l == i)
                        continue;
                    f32 save4 = sum;
                    sum += data[3][l];
                    if ((findMin && sum < bestSum) || (!findMin && sum > bestSum))
                    {
                        result[0] = i;
                        result[1] = j;
                        result[2] = k;
                        result[3] = l;
                        bestSum = sum;
                    }
                    sum = save4;
                }
                sum = save3;
            }
            sum = save2;
        }
    }
}



char** GetPowerupNamesArray()
{
    return g_sSurfaceNames;
}

char* GetPowerupName(int powerup, bool useSpecificName)
{
    if (powerup >= 0)
    {
        if (useSpecificName)
        {
            if (powerup < 21)
            {
                return g_sPowerupNames[powerup];
            }
        }
        else if (powerup < 9)
        {
            return g_sPowerupNames[powerup];
        }
        else if (powerup <= 20)
        {
            return "SuperPower";
        }
    }

    return "Unknown";
}

void MakeRandomDirection2D(nlVector3& direction, float length)
{
    unsigned short angle = (unsigned short)nlRandom(0x10000);
    nlSinCos(&direction.y, &direction.x, angle);
    direction.z = 0.0f;
    nlVec3Scale(direction, length);
}
