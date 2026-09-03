#include "NL/nlMath.h"
#include "NL/nlMemory.h"

struct UnidentifiedGameRegion
{
    /* 0x00 */ nlVector3 mCentre;
    /* 0x0C */ float mRadius;
    /* 0x10 */ unsigned char mUnidentified10[0x10];
    /* 0x20 */ nlVector3* mPoints;
    /* 0x24 */ int mNumPoints;
    /* 0x28 */ int mCapacity;
    /* 0x2C */ int mOwnsPoints;
};

extern "C" void fn_802B68E8(UnidentifiedGameRegion* region, int capacity)
{
    region->mPoints =
        (nlVector3*)nlMalloc(capacity * sizeof(nlVector3), 8, false);
    region->mCapacity = capacity;
    region->mNumPoints = 0;
    region->mOwnsPoints = 1;
}

extern "C" bool fn_802B6944(
    UnidentifiedGameRegion* region, const nlVector3* point)
{
    if (region->mNumPoints < region->mCapacity)
    {
        region->mPoints[region->mNumPoints] = *point;
        region->mNumPoints++;

        nlVector3 delta;
        nlVector3 centre;
        nlVec3Set(centre, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < region->mNumPoints; i++)
        {
            nlVec3Add(centre, centre, region->mPoints[i]);
        }

        float scale = 1.0f / (float)region->mNumPoints;
        nlVec3Scale(centre, scale);

        float radiusSquared = 0.0f;
        for (int i = 0; i < region->mNumPoints; i++)
        {
            nlVec3Sub(delta, centre, region->mPoints[i]);
            float distanceSquared = nlVec3LengthSquared(delta);
            if (distanceSquared > radiusSquared)
            {
                radiusSquared = distanceSquared;
            }
        }

        float radius = nlSqrt(radiusSquared, true);
        nlVec3Set(region->mCentre, centre.x, centre.y, centre.z);
        region->mRadius = radius;
        return true;
    }
    return false;
}

extern "C" bool fn_802B6AF8(
    const UnidentifiedGameRegion* region, const nlVector2* point)
{
    float pointX = point->x;
    float pointY = point->y;
    bool inside = false;
    int current = 0;
    int previous = region->mNumPoints - 1;
    for (; current < region->mNumPoints; previous = current++)
    {
        if (((region->mPoints[current].y <= pointY
                 && pointY < region->mPoints[previous].y)
                || (region->mPoints[previous].y <= pointY
                    && pointY < region->mPoints[current].y))
            && pointX
                < (region->mPoints[previous].x
                        - region->mPoints[current].x)
                        * (pointY - region->mPoints[current].y)
                        / (region->mPoints[previous].y
                            - region->mPoints[current].y)
                    + region->mPoints[current].x)
        {
            inside = !inside;
        }
    }
    return inside;
}
