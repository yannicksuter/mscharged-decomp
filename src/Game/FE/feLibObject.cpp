#include "Game/FE/feLibObject.h"

#include "NL/nlString.h"

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
    m_attributes.field_0x38 = 0.0f;
    m_attributes.field_0x3C = 0.0f;
    m_attributes.field_0x40 = 1.0f;
    m_attributes.field_0x44 = 1.0f;
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

extern "C" float fn_803020BC(FELibObject* object)
{
    return object->m_attributes.field_0x38;
}

extern "C" float fn_803020C4(FELibObject* object)
{
    return object->m_attributes.field_0x3C;
}

extern "C" float fn_803020CC(FELibObject* object)
{
    return object->m_attributes.field_0x40;
}

extern "C" float fn_803020D4(FELibObject* object)
{
    return object->m_attributes.field_0x44;
}
