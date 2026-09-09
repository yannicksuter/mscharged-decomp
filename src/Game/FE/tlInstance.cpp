#include "Game/FE/tlInstance.h"

#include "NL/nlString.h"

#include <math.h>
#include "NL/nlstring_tmpl.h"

feVector3& TLInstance::GetAssetPosition() const
{
    return const_cast<feVector3&>(m_overloadedAttributes.v3Position);
}

feVector3& TLInstance::GetAssetRotation() const
{
    return const_cast<feVector3&>(m_overloadedAttributes.v3Rotation);
}

feVector3& TLInstance::GetAssetScale() const
{
    return const_cast<feVector3&>(m_overloadedAttributes.v3Scale);
}

nlColour& TLInstance::GetAssetColour() const
{
    return const_cast<nlColour&>(m_overloadedAttributes.colour);
}

feVector3& TLInstance::GetPosition() const
{
    if (m_overloadFlags & 0x1)
    {
        return const_cast<feVector3&>(m_overloadedAttributes.v3Position);
    }
    return m_component->GetPosition();
}

feVector3& TLInstance::GetRotation() const
{
    if (m_overloadFlags & 0x2)
    {
        return const_cast<feVector3&>(m_overloadedAttributes.v3Rotation);
    }
    return m_component->GetRotation();
}

feVector3& TLInstance::GetScale() const
{
    if (m_overloadFlags & 0x4)
    {
        return const_cast<feVector3&>(m_overloadedAttributes.v3Scale);
    }
    return m_component->GetScale();
}

nlColour& TLInstance::GetColour() const
{
    if (m_overloadFlags & 0x10)
    {
        return const_cast<nlColour&>(m_overloadedAttributes.colour);
    }
    return m_component->GetColour();
}

feVector3& TLInstance::GetPivot() const
{
    if (m_overloadFlags & 0x8)
    {
        return const_cast<feVector3&>(m_overloadedAttributes.v3Pivot);
    }
    return m_component->GetPivot();
}

float TLInstance::GetUVX() const
{
    if (m_overloadFlags & 0x40)
    {
        return m_overloadedAttributes.fUVX;
    }
    return m_component->GetUVX();
}

float TLInstance::GetUVY() const
{
    if (m_overloadFlags & 0x80)
    {
        return m_overloadedAttributes.fUVY;
    }
    return m_component->GetUVY();
}

float TLInstance::GetUVWidth() const
{
    if (m_overloadFlags & 0x100)
    {
        return m_overloadedAttributes.fUVWidth;
    }
    return m_component->GetUVWidth();
}

float TLInstance::GetUVHeight() const
{
    if (m_overloadFlags & 0x200)
    {
        return m_overloadedAttributes.fUVHeight;
    }
    return m_component->GetUVHeight();
}

bool TLInstance::IsValidAtTime(float fCurrentTime)
{
    float sinceStart;
    float duration;
    float elapsed;
    bool valid;

    valid = true;
    sinceStart = fCurrentTime - m_fStartTime;
    if (!(sinceStart > 0.0001f))
    {
        if (!((float)fabs(sinceStart) <= 0.0001f))
        {
            valid = false;
        }
    }

    if (valid != 0)
    {
        valid = 1;
        duration = m_fDuration;
        elapsed = fCurrentTime - m_fStartTime;
        if (!((m_fDuration - elapsed) > 0.0001f))
        {
            if (!((float)fabs(elapsed - duration) <= 0.0001f))
            {
                valid = false;
            }
        }

        if (valid)
        {
            return true;
        }
    }

    return false;
}

void TLInstance::SetAssetPosition(float x, float y, float z)
{
    m_overloadFlags |= 0x1;
    m_overloadedAttributes.v3Position.f.x = x;
    m_overloadedAttributes.v3Position.f.y = y;
    m_overloadedAttributes.v3Position.f.z = z;
}

void TLInstance::SetAssetRotation(float x, float y, float z)
{
    m_overloadFlags |= 0x2;
    m_overloadedAttributes.v3Rotation.f.x = x;
    m_overloadedAttributes.v3Rotation.f.y = y;
    m_overloadedAttributes.v3Rotation.f.z = z;
}

void TLInstance::SetAssetScale(float x, float y, float z)
{
    m_overloadFlags |= 0x4;
    m_overloadedAttributes.v3Scale.f.x = x;
    m_overloadedAttributes.v3Scale.f.y = y;
    m_overloadedAttributes.v3Scale.f.z = z;
}

void TLInstance::SetAssetColour(const nlColour& color)
{
    m_overloadFlags |= 0x10;
    m_overloadedAttributes.colour = color;
}

void TLInstance::SetAssetPivot(float x, float y, float z)
{
    m_overloadFlags |= 0x8;
    m_overloadedAttributes.v3Pivot.f.x = x;
    m_overloadedAttributes.v3Pivot.f.y = y;
    m_overloadedAttributes.v3Pivot.f.z = z;
}

void TLInstance::SetAssetUVX(float value)
{
    m_overloadFlags |= 0x40;
    m_overloadedAttributes.fUVX = value;
}

void TLInstance::SetAssetUVY(float value)
{
    m_overloadFlags |= 0x80;
    m_overloadedAttributes.fUVY = value;
}

void TLInstance::SetAssetUVWidth(float value)
{
    m_overloadFlags |= 0x100;
    m_overloadedAttributes.fUVWidth = value;
}

void TLInstance::SetAssetUVHeight(float value)
{
    m_overloadFlags |= 0x200;
    m_overloadedAttributes.fUVHeight = value;
}

void TLInstance::SetAssetVisible(bool visible)
{
    nlColour color = m_overloadedAttributes.colour;
    color.c[3] = visible ? 0xFF : 0;
    m_overloadFlags |= 0x10;
    m_overloadedAttributes.colour = color;
}

TLInstance::TLInstance(FELibObject* component)
{
    m_next = 0;
    m_prev = 0;
    pChildren = 0;
    m_component = component;
    m_overloadFlags = 0;
    m_bVisible = true;

    const char* name = "<Undefined Instance>";
    nlStrNCpy<char>(m_szName, name, sizeof(m_szName));
    m_hash = nlStringLowerHash(name);
}
