#include "Game/Net.h"

#include "Game/Field.h"

float cNet::m_fNetHeight = 1.0f;
float cNet::m_fNetWidth = 1.0f;
float cNet::m_fNetDepth = 2.2f;
float cNet::m_fNetPostRadius = 0.01f;
float cNet::m_fNetPostOffsetFromGoalLine = 0.0f;

cNet::cNet(int nIndex)
{
    m_nIndex = nIndex;

    m_v3NetLocation.y = 0.0f;
    m_v3NetLocation.z = 0.0f;
    m_v3NetLocation.x = 0.0f;

    if (m_nIndex == 0)
    {
        m_fDirection = -1.0f;
    }
    else
    {
        m_fDirection = +1.0f;
    }
}

cNet::~cNet()
{
}

float cNet::GetGoalLineX() const
{
    return cField::GetGoalLineX(m_fDirection);
}

void cNet::GetPostLocation(
    nlVector3& v3PostPosition, unsigned int uPostNum, float fYAdjust) const
{
    float fAdjust;

    v3PostPosition = m_v3NetLocation;
    v3PostPosition.x
        = -((m_fNetPostRadius * m_fDirection) - v3PostPosition.x);
    v3PostPosition.x += m_fNetPostOffsetFromGoalLine * m_fDirection;
    fAdjust = (m_fNetWidth / 2.0f) + fYAdjust;

    if (uPostNum == 0)
    {
        v3PostPosition.y -= fAdjust;
        return;
    }

    v3PostPosition.y += fAdjust;
}

void cNet::SetNetDimensions(
    float fWidth, float fHeight, float fPostRadius,
    float fPostOffsetFromGoalLine)
{
    m_fNetWidth = fWidth;
    m_fNetHeight = fHeight;
    m_fNetPostRadius = fPostRadius;
    m_fNetPostOffsetFromGoalLine = fPostOffsetFromGoalLine;
}
