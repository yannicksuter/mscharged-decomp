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
    static void Init(cNet* net0, cNet* net1);
    static cNet* GetNet(float side);
    static float GetGoalLineX(float side);
    static float GetGoalLineX(unsigned int side);
    static float GetSidelineY(unsigned int side);
    static float GetCornerRadius();
    static float GetPenaltyBoxX(unsigned int side);
    static float GetPenaltyBoxY();
    static bool IsOnField(const nlVector3& location);
    static bool IsOnField(const nlVector2& location);
    static void SetFieldDimensions(float fX, float fY, float fZ);

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

    static nlVector3 mv3FieldPosition;
    static sSideLinePlane mSidelines[4];
    static sCornerSegment mCorners[4];
    static float mfPenaltyBoxX;
    static float mfPenaltyBoxY;
    static cNet* mpNet[2];
};

#endif // GAME_FIELD_H
