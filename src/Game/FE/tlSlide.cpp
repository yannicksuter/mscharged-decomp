#include "Game/FE/tlSlide.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlInstance.inl"

#include "NL/nlDLRing.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

/**
 * Offset/Address/Size: 0x0 | 0x802FFAFC | size: 0x1D8
 */
void TLSlide::UpdateAsset(TLInstance* instance, float time)
{
    if (instance->pChildren == 0)
    {
        return;
    }

    TLInstance* child = nlDLRingGetStart<TLInstance>(instance->pChildren);
    for (;;)
    {
        if (child->GetType() == TLAT_COMPONENT)
        {
            ((TLComponentInstance*)child)->Update(time);
        }
        UpdateAsset(child, time);
        if (nlDLRingIsEnd<TLInstance>(instance->pChildren, child))
        {
            break;
        }
        child = child->m_next;
    }
}

/**
 * Offset/Address/Size: 0x1D8 | 0x802FFCD4 | size: 0x308
 */
void TLSlide::Update(float time)
{
    if (field_0x44)
    {
        time = 0.0f;
    }

    m_time += time;
    float end = GetStartTime() + GetDuration();
    if (m_time > end)
    {
        switch (m_uPlayMode)
        {
        case TLPM_LOOPING:
            m_time = m_time - end;
            break;
        case TLPM_STOP_AT_END:
            m_time = end;
            break;
        case 2:
        default:
            break;
        }
    }

    FEAnimation* anim = nlDLRingGetStart<FEAnimation>(m_animations);
    for (;;)
    {
        if (anim == 0)
        {
            break;
        }
        anim->Update(m_time);
        if (nlDLRingIsEnd<FEAnimation>(m_animations, anim) != 0)
        {
            break;
        }
        anim = anim->m_next;
    }

    TLInstance* instance = nlDLRingGetStart<TLInstance>(pChildren);
    for (;;)
    {
        if (instance == 0)
        {
            break;
        }
        if (instance->GetType() == TLAT_COMPONENT)
        {
            ((TLComponentInstance*)instance)->Update(time);
        }
        UpdateAsset(instance, time);
        if (nlDLRingIsEnd<TLInstance>(pChildren, instance))
        {
            break;
        }
        instance = instance->m_next;
    }
}

inline void TLSlide::SetName(const char* name)
{
    nlStrNCpy<char>(m_szName, name, sizeof(m_szName));
    m_hash = nlStringLowerHash(name);
}

/**
 * Offset/Address/Size: 0x4E0 | 0x802FFFDC | size: 0xA0
 */
TLSlide::TLSlide()
{
    m_animations = 0;
    m_time = 0.0f;
    m_uPlayMode = TLPM_LOOPING;
    field_0x44 = false;
    SetName("<undefined slide>");
}
