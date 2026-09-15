#include "NL/nlPolygonRegion.h"
#include "Game/MathHelpers.h"

void nlPolygonRegion::Allocate(int capacity)
{
    mPoints =
        (nlVector3*)nlMalloc(capacity * sizeof(nlVector3), 8, false);
    mCapacity = capacity;
    mNumPoints = 0;
    mOwnsPoints = 1;
}

inline void nlPolygonRegion::UpdateBoundingSphere()
{
    nlVector3 centre = { 0.0f, 0.0f, 0.0f };
    for (int i = 0; i < mNumPoints; i++)
    {
        nlVec3Add(centre, centre, mPoints[i]);
    }

    float radiusSquared = 0.0f;
    float scale = 1.0f / (float)mNumPoints;
    nlVec3Scale(centre, scale);

    for (int i = 0; i < mNumPoints; i++)
    {
        float distanceSquared = CalculateDistanceSquared(centre, mPoints[i]);
        if (distanceSquared > radiusSquared)
        {
            radiusSquared = distanceSquared;
        }
    }

    float radius = nlSqrt(radiusSquared, true);
    nlVec3Set(mCentre, centre.x, centre.y, centre.z);
    mRadius = radius;
}

bool nlPolygonRegion::AddPoint(const nlVector3& point)
{
    if (mNumPoints < mCapacity)
    {
        mPoints[mNumPoints] = point;
        mNumPoints++;

        UpdateBoundingSphere();
        return true;
    }
    return false;
}

bool nlPolygonRegion::ContainsPoint2D(const nlVector2& point) const
{
    float pointX = point.x;
    float pointY = point.y;
    bool inside = false;
    int current = 0;
    int previous = mNumPoints - 1;
    for (; current < mNumPoints; previous = current++)
    {
        if (((mPoints[current].y <= pointY
                 && pointY < mPoints[previous].y)
                || (mPoints[previous].y <= pointY
                    && pointY < mPoints[current].y))
            && pointX
                < (mPoints[previous].x
                        - mPoints[current].x)
                        * (pointY - mPoints[current].y)
                        / (mPoints[previous].y
                            - mPoints[current].y)
                    + mPoints[current].x)
        {
            inside = !inside;
        }
    }
    return inside;
}
