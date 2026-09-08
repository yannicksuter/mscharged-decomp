#include "Game/FE/feLibObject.h"

#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

FELibObject::FELibObject()
{
    nlStrNCpy<char>(m_szName, "<undefined>", sizeof(m_szName));
    m_hashID = 0;

    m_attributes.v3Position.f.x = 0.0f;
    m_attributes.v3Position.f.y = 0.0f;
    m_attributes.v3Position.f.z = 0.0f;
    m_attributes.v3Rotation.f.x = 0.0f;
    m_attributes.v3Rotation.f.y = 0.0f;
    m_attributes.v3Rotation.f.z = 0.0f;
    m_attributes.v3Scale.f.x = 1.0f;
    m_attributes.v3Scale.f.y = 1.0f;
    m_attributes.v3Scale.f.z = 1.0f;
    m_attributes.v3Pivot.f.x = 0.0f;
    m_attributes.v3Pivot.f.y = 0.0f;
    m_attributes.v3Pivot.f.z = 0.0f;
    m_attributes.fUVX = 0.0f;
    m_attributes.fUVY = 0.0f;
    m_attributes.fUVWidth = 1.0f;
    m_attributes.fUVHeight = 1.0f;
    m_attributes.bVisible = true;

    next = 0;
    prev = 0;
    m_type = FEOT_UNKNOWN;
}

nlColour& FELibObject::GetColour() const
{
    return const_cast<nlColour&>(m_attributes.colour);
}

feVector3& FELibObject::GetPivot() const
{
    return const_cast<feVector3&>(m_attributes.v3Pivot);
}

feVector3& FELibObject::GetPosition() const
{
    return const_cast<feVector3&>(m_attributes.v3Position);
}

feVector3& FELibObject::GetRotation() const
{
    return const_cast<feVector3&>(m_attributes.v3Rotation);
}

feVector3& FELibObject::GetScale() const
{
    return const_cast<feVector3&>(m_attributes.v3Scale);
}

float FELibObject::GetUVX() const
{
    return m_attributes.fUVX;
}

float FELibObject::GetUVY() const
{
    return m_attributes.fUVY;
}

float FELibObject::GetUVWidth() const
{
    return m_attributes.fUVWidth;
}

float FELibObject::GetUVHeight() const
{
    return m_attributes.fUVHeight;
}
