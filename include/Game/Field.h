#ifndef GAME_FIELD_H
#define GAME_FIELD_H

#include "Game/Net.h"
#include "NL/nlMath.h"

struct sCornerSegment
{
    nlVector2 vCenter;
    u16 thetaStart;
    u16 thetaEnd;
    float fRadius;
};

struct sSideLinePlane
{
    nlVector2 vNormal;
    float fDistance;
};

class cField
{
public:
    static cNet* GetNet(float side);
    static float GetGoalLineX(unsigned int side);
    static float GetSidelineY(unsigned int side);
    static float GetPenaltyBoxX(unsigned int side);
    static float GetPenaltyBoxY();

    static const sSideLinePlane& GetSideline(int index)
    {
        const u8* base = (const u8*)mSidelines;
        return *(const sSideLinePlane*)(base + index * sizeof(sSideLinePlane));
    }

    static const sCornerSegment& GetCorner(int index)
    {
        const u8* base = (const u8*)mCorners;
        return *(const sCornerSegment*)(base + index * sizeof(sCornerSegment));
    }

    static sSideLinePlane mSidelines[4];
    static sCornerSegment mCorners[4];
};

#endif // GAME_FIELD_H
