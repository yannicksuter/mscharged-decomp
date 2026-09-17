#include "Game/Render/CrowdImpostorManager.h"

extern "C" void fn_802D88F4(
    CrowdLayoutObject* object)
{
    GetCrowdImpostorManager()->AddObject(object, false);
}

extern "C" void fn_802D892C(CrowdLayoutObject*)
{
}

void CrowdLayoutObject::Initialize()
{
    mIsOcclusionVolume = 0;
    mStartWidth = 0.0f;
    mEndWidth = 0.0f;
    mLength = 0.0f;
    mEndOffset = 0.0f;
}

void CrowdLayoutObject::GetCorners(nlVector4* corners)
{
    nlVec4Set(corners[1], 0.0f, 0.0f, 0.0f, 1.0f);
    nlVec4Set(corners[0], mStartWidth, 0.0f, 0.0f, 1.0f);

    float offset = 0.5f
        * (mStartWidth - mEndWidth);
    nlVec4Set(corners[2], mEndOffset + offset,
        mLength, 0.0f, 1.0f);
    nlVec4Set(corners[3], mEndWidth
            + mEndOffset + offset,
        mLength, 0.0f, 1.0f);
}

void GetCrowdLayoutBounds(
    const nlVector4* points, nlVector4* boundsMin, nlVector4* boundsMax)
{
    *boundsMin = points[0];
    *boundsMax = points[0];

    for (int i = 1; i < 4; ++i)
    {
        if (points[i].x < boundsMin->x)
            boundsMin->x = points[i].x;
        if (points[i].y < boundsMin->y)
            boundsMin->y = points[i].y;
        if (points[i].z < boundsMin->z)
            boundsMin->z = points[i].z;

        if (points[i].x > boundsMax->x)
            boundsMax->x = points[i].x;
        if (points[i].y > boundsMax->y)
            boundsMax->y = points[i].y;
        if (points[i].z > boundsMax->z)
            boundsMax->z = points[i].z;
    }
}

bool CrowdLayoutObject::ContainsLocalPoint(const nlVector3* point)
{
    nlVector4 corners[4];
    nlVector4 boundsMin;
    nlVector4 boundsMax;
    GetCorners(corners);
    GetCrowdLayoutBounds(corners, &boundsMin, &boundsMax);

    float depth = 10.0f;
    if (point->x > boundsMin.x && point->x < boundsMax.x
        && point->y > boundsMin.y && point->y < boundsMax.y
        && point->z > -depth && point->z < depth)
    {
        return true;
    }
    return false;
}
