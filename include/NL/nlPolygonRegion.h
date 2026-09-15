#ifndef NL_POLYGON_REGION_H
#define NL_POLYGON_REGION_H

#include <stddef.h>
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

class nlPolygonRegion
{
public:
    nlPolygonRegion()
        : mPoints(NULL)
        , mNumPoints(0)
        , mCapacity(-1)
        , mOwnsPoints(0)
    {
    }

    ~nlPolygonRegion()
    {
        if (mOwnsPoints && mPoints != NULL)
        {
            delete[] mPoints;
            mPoints = NULL;
            mNumPoints = 0;
            mOwnsPoints = 0;
        }
    }

    void Allocate(int capacity);
    bool AddPoint(const nlVector3& point);
    bool ContainsPoint2D(const nlVector2& point) const;

    /* 0x00 */ nlVector3 mCentre;
    /* 0x0C */ float mRadius;
    /* 0x10 */ unsigned char mUnknown10[0x10];
    /* 0x20 */ nlVector3* mPoints;
    /* 0x24 */ int mNumPoints;
    /* 0x28 */ int mCapacity;
    /* 0x2C */ int mOwnsPoints;

private:
    void UpdateBoundingSphere();
};

#endif
