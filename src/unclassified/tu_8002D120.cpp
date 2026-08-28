#include "Game/Field.h"

static const float cornerRadius = 3.0f;

nlVector3 cField::mv3FieldPosition = { 20.6f, 12.5f, 0.0f };
sSideLinePlane cField::mSidelines[4] = {
    { { 1.0f, 0.0f }, cField::mv3FieldPosition.x },
    { { -1.0f, 0.0f }, cField::mv3FieldPosition.x },
    { { 0.0f, 1.0f }, cField::mv3FieldPosition.y },
    { { 0.0f, -1.0f }, cField::mv3FieldPosition.y }
};
sCornerSegment cField::mCorners[4] = {
    { { cField::mv3FieldPosition.x - cornerRadius, cField::mv3FieldPosition.y - cornerRadius }, 0x0000, 0x4000, cornerRadius },
    { { cornerRadius - cField::mv3FieldPosition.x, cField::mv3FieldPosition.y - cornerRadius }, 0x4000, 0x8000, cornerRadius },
    { { cornerRadius - cField::mv3FieldPosition.x, cornerRadius - cField::mv3FieldPosition.y }, 0x8000, 0xC000, cornerRadius },
    { { cField::mv3FieldPosition.x - cornerRadius, cornerRadius - cField::mv3FieldPosition.y }, 0xC000, 0x0000, cornerRadius }
};
float cField::mfPenaltyBoxX = 13.5f;
float cField::mfPenaltyBoxY = 4.5f;
cNet* cField::mpNet[2];

void cField::Init(cNet* net0, cNet* net1)
{
    mpNet[0] = net0;
    mpNet[1] = net1;
    net0->m_v3NetLocation.x = -mv3FieldPosition.x;
    net1->m_v3NetLocation.x = mv3FieldPosition.x;
}

float cField::GetGoalLineX(float side)
{
    if (side > 0.0f)
    {
        return mv3FieldPosition.x;
    }
    return -mv3FieldPosition.x;
}

float cField::GetGoalLineX(unsigned int side)
{
    if (side > 0)
    {
        return mv3FieldPosition.x;
    }
    return -mv3FieldPosition.x;
}

float cField::GetSidelineY(unsigned int side)
{
    if (side > 0)
    {
        return mv3FieldPosition.y;
    }
    return -mv3FieldPosition.y;
}

float cField::GetCornerRadius()
{
    return cornerRadius;
}

float cField::GetPenaltyBoxX(unsigned int side)
{
    return (side > 0) ? mfPenaltyBoxX : -mfPenaltyBoxX;
}

float cField::GetPenaltyBoxY()
{
    return mfPenaltyBoxY;
}

cNet* cField::GetNet(float side)
{
    if (side > 0.0f)
    {
        return mpNet[1];
    }
    return mpNet[0];
}
