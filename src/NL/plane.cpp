#include "NL/nlMath.h"

void nlMakePlaneFromPointNormal(nlVector4& out, const nlVector2& point, const nlVector2& normal)
{
    nlVector2 negativeNormal;
    nlVec2Neg(negativeNormal, normal);
    nlVec4Set(out, normal.x, normal.y, 0.0f, nlVec2DotProduct(negativeNormal, point));
}

void nlMakePlaneFromPointNormal(nlVector4& out, const nlVector3& point, const nlVector3& normal)
{
    nlVector3 negativeNormal;
    nlVec3Neg(negativeNormal, normal);
    nlVec4Set(out, normal.x, normal.y, normal.z,
        nlVec3DotProduct(negativeNormal, point));
}

void nlProjectPointOntoPlane(nlVector3& out, const nlVector3& point, const nlVector4& plane)
{
    const nlVector3& normal = *(const nlVector3*)&plane;
    float planeOffset = plane.w;
    float scale
        = -(nlVec3DotProduct(normal, point) + planeOffset) / normal.GetLengthSq3D();
    nlVec3ScaleAdd(out, scale, normal, point);
}

float nlPlaneDot(const nlVector2& point, const nlVector4& plane)
{
    return plane.x * point.x + plane.y * point.y + plane.w;
}
