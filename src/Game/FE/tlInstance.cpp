#include "Game/FE/tlInstance.h"

#include "Game/FE/tlComponent.h"
#include "NL/nlString.h"

#include <math.h>

extern "C" float fn_803020BC(FELibObject* object);
extern "C" float fn_803020C4(FELibObject* object);
extern "C" float fn_803020CC(FELibObject* object);
extern "C" float fn_803020D4(FELibObject* object);

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

extern "C" float fn_80302210(TLInstance* instance)
{
    if (instance->m_overloadFlags & 0x40)
    {
        return instance->m_overloadedAttributes.field_0x38;
    }
    return fn_803020BC(instance->m_component);
}

extern "C" float fn_80302230(TLInstance* instance)
{
    if (instance->m_overloadFlags & 0x80)
    {
        return instance->m_overloadedAttributes.field_0x3C;
    }
    return fn_803020C4(instance->m_component);
}

extern "C" float fn_80302250(TLInstance* instance)
{
    if (instance->m_overloadFlags & 0x100)
    {
        return instance->m_overloadedAttributes.field_0x40;
    }
    return fn_803020CC(instance->m_component);
}

extern "C" float fn_80302270(TLInstance* instance)
{
    if (instance->m_overloadFlags & 0x200)
    {
        return instance->m_overloadedAttributes.field_0x44;
    }
    return fn_803020D4(instance->m_component);
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

extern "C" void fn_803023B4(TLInstance* instance, float x, float y, float z)
{
    instance->m_overloadFlags |= 0x8;
    instance->m_overloadedAttributes.v3Pivot.f.x = x;
    instance->m_overloadedAttributes.v3Pivot.f.y = y;
    instance->m_overloadedAttributes.v3Pivot.f.z = z;
}

extern "C" void fn_803023D0(TLInstance* instance, float value)
{
    instance->m_overloadFlags |= 0x40;
    instance->m_overloadedAttributes.field_0x38 = value;
}

extern "C" void fn_803023E4(TLInstance* instance, float value)
{
    instance->m_overloadFlags |= 0x80;
    instance->m_overloadedAttributes.field_0x3C = value;
}

extern "C" void fn_803023F8(TLInstance* instance, float value)
{
    instance->m_overloadFlags |= 0x100;
    instance->m_overloadedAttributes.field_0x40 = value;
}

extern "C" void fn_8030240C(TLInstance* instance, float value)
{
    instance->m_overloadFlags |= 0x200;
    instance->m_overloadedAttributes.field_0x44 = value;
}

extern "C" void fn_80302420(TLInstance* instance, bool visible)
{
    nlColour color = instance->m_overloadedAttributes.colour;
    color.c[3] = visible ? 0xFF : 0;
    instance->m_overloadFlags |= 0x10;
    instance->m_overloadedAttributes.colour = color;
}

extern "C" TLInstance* fn_80302470(TLInstance* instance, TLComponent* component)
{
    instance->m_next = 0;
    instance->m_prev = 0;
    instance->pChildren = 0;
    instance->m_component = component;
    instance->m_overloadFlags = 0;
    instance->m_bVisible = true;

    const char* name = "<Undefined Instance>";
    nlStrNCpy<char>(instance->m_szName, name, sizeof(instance->m_szName));
    instance->m_hash = nlStringLowerHash(name);
    return instance;
}
