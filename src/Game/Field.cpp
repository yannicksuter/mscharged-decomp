#include "Game/Field.h"

#include "Game/MathHelpers.h"
#include "math.h"

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
    { { -cField::mv3FieldPosition.x + cornerRadius, cField::mv3FieldPosition.y - cornerRadius }, 0x4000, 0x8000, cornerRadius },
    { { -cField::mv3FieldPosition.x + cornerRadius, -cField::mv3FieldPosition.y + cornerRadius }, 0x8000, 0xC000, cornerRadius },
    { { cField::mv3FieldPosition.x - cornerRadius, -cField::mv3FieldPosition.y + cornerRadius }, 0xC000, 0x0000, cornerRadius }
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

bool cField::IsOnField(const nlVector3& location)
{
    if ((float)fabs(location.x) <= mv3FieldPosition.x + 0.18f)
    {
        if ((float)fabs(location.y) <= mv3FieldPosition.y + 0.18f)
        {
            return true;
        }
    }
    return false;
}

bool cField::IsOnField(const nlVector2& location)
{
    if ((float)fabs(location.x) <= mv3FieldPosition.x + 0.18f)
    {
        if ((float)fabs(location.y) <= mv3FieldPosition.y + 0.18f)
        {
            return true;
        }
    }
    return false;
}

static float FixComponent(const float& component, float fMin, float fMax)
{
    float value = component;
    value = (value >= fMin) ? value : fMin;
    value = (value <= fMax) ? value : fMax;
    return value;
}

static bool FixOutOfBoundsY(nlVector3& v, float fMinDistanceFromWall)
{
    float fOldY = v.y;
    v.y = FixComponent(v.y, -cField::mv3FieldPosition.y + fMinDistanceFromWall,
        cField::mv3FieldPosition.y - fMinDistanceFromWall);
    return v.y != fOldY;
}

bool cField::FixOutOfBoundsPosition(nlVector3& v, float fMinDistanceFromWall, bool bExcludeNet)
{
    bool bFixed;
    bool bFixCorners = fMinDistanceFromWall < GetCorner(0).fRadius;

    if (bFixCorners)
    {
        bFixed = true;
        if (!FixCornerPosition(v, fMinDistanceFromWall))
        {
            bFixed = false;
        }
        if (!bFixed)
        {
            bFixed = FixOutOfBoundsX(v, bExcludeNet, fMinDistanceFromWall);
        }
        if (!bFixed)
        {
            bFixed = FixOutOfBoundsY(v, fMinDistanceFromWall);
        }
    }
    else
    {
        bool bFixedX = true;
        if (!FixOutOfBoundsX(v, bExcludeNet, fMinDistanceFromWall))
        {
            bFixedX = false;
        }
        bool bFixedY = FixOutOfBoundsY(v, fMinDistanceFromWall);
        bFixed = bFixedY || bFixedX;
    }

    return bFixed;
}

bool cField::FixCornerPosition(nlVector3& v, float fMinDistanceFromWall)
{
    float fOldX = v.x;
    float fOldY = v.y;

    for (int i = 0; i < 4; i++)
    {
        sCornerSegment corner = GetCorner(i);
        nlVector2 vFromCorner;
        nlVector2 vToCorner;
        nlVec2Set(vToCorner, corner.vCenter.x - v.x, corner.vCenter.y - v.y);
        float fDistanceToCorner = nlVec2Length(vToCorner);
        float fLimitRadius = corner.fRadius - fMinDistanceFromWall;

        if ((float)fabs(v.x) > (float)fabs(corner.vCenter.x)
            && (float)fabs(v.y) > (float)fabs(corner.vCenter.y))
        {
            vFromCorner.x = v.x - corner.vCenter.x;
            vFromCorner.y = v.y - corner.vCenter.y;
            float fDistance = nlVec2Length(vFromCorner);
            float x = vFromCorner.x;
            float y = vFromCorner.y;
            u16 uAngle = RadToAng16(nlATan2f(y, x));

            if (abs_ang16(nlAngleDiff(uAngle, corner.thetaStart)) <= 0x4000
                && abs_ang16(nlAngleDiff(uAngle, corner.thetaEnd)) <= 0x4000
                && fDistance > fLimitRadius)
            {
                float fInvLength = nlRecipSqrt(
                    nlVec2DotProduct(vFromCorner, vFromCorner), true);
                nlVec2Scale(vFromCorner, vFromCorner, fInvLength);
                nlVec2Scale(vFromCorner, vFromCorner, fLimitRadius);
                nlVec2Add(vFromCorner, corner.vCenter, vFromCorner);
                v.x = vFromCorner.x;
                v.y = vFromCorner.y;
            }
        }
    }

    bool bFixed = (v.x != fOldX);
    return bFixed || v.y != fOldY;
}

bool cField::FixOutOfBoundsX(nlVector3& v, bool bExcludeNet, float fMinDistanceFromWall)
{
    float fOldX = v.x;

    if (bExcludeNet)
    {
        v.x = FixComponent(v.x, -mv3FieldPosition.x + fMinDistanceFromWall,
            mv3FieldPosition.x - fMinDistanceFromWall);
    }
    else if ((float)fabs(v.y) > 0.5f * cNet::GetNetWidth() - fMinDistanceFromWall)
    {
        v.x = FixComponent(v.x, -mv3FieldPosition.x + fMinDistanceFromWall,
            mv3FieldPosition.x - fMinDistanceFromWall);
    }
    else
    {
        float fNetBack = mv3FieldPosition.x + cNet::GetNetDepth();
        v.x = FixComponent(v.x, -fNetBack, fNetBack);
    }

    bool bFixed = (v.x != fOldX);
    return bFixed || FixCornerPosition(v, fMinDistanceFromWall);
}

void cField::SetFieldDimensions(float fX, float fY, float fZ)
{
    mpNet[0]->m_v3NetLocation.x = -mv3FieldPosition.x;
    mpNet[1]->m_v3NetLocation.x = mv3FieldPosition.x;
}
