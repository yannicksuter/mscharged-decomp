#ifndef UNCLASSIFIED_TU_802B68E8_H
#define UNCLASSIFIED_TU_802B68E8_H

#include <stddef.h>
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

class UnidentifiedGameRegion
{
public:
    UnidentifiedGameRegion()
        : mPoints(NULL)
        , mNumPoints(0)
        , mCapacity(-1)
        , mOwnsPoints(0)
    {
    }

    ~UnidentifiedGameRegion()
    {
        if (mOwnsPoints && mPoints != NULL)
        {
            delete[] mPoints;
            mPoints = NULL;
            mNumPoints = 0;
            mOwnsPoints = 0;
        }
    }

    /* 0x00 */ nlVector3 mCentre;
    /* 0x0C */ float mRadius;
    /* 0x10 */ unsigned char mUnidentified10[0x10];
    /* 0x20 */ nlVector3* mPoints;
    /* 0x24 */ int mNumPoints;
    /* 0x28 */ int mCapacity;
    /* 0x2C */ int mOwnsPoints;
};

extern "C" void fn_802B68E8(UnidentifiedGameRegion*, int);
extern "C" bool fn_802B6944(UnidentifiedGameRegion*, const nlVector3*);
extern "C" bool fn_802B6AF8(const UnidentifiedGameRegion*, const nlVector2*);

#endif // UNCLASSIFIED_TU_802B68E8_H
