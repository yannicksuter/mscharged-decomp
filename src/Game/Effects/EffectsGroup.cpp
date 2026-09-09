#include "Game/Effects/EffectsGroup.h"

bool EffectsGroup::IsPersistent() const
{
    unsigned long count = m_numSpecs;
    for (unsigned long i = 0; i < count; ++i)
    {
        if (m_specs[i].m_pTemplate->m_fFountainLife >= 1.0e10f)
        {
            return true;
        }
    }

    for (unsigned long i = 0; i < count; ++i)
    {
        if (m_specs[i].m_pTemplate->m_rParticleLife.base >= 1000.0f)
        {
            return true;
        }
    }

    return false;
}

void EffectsGroup::ResolveTemplates(EffectsTemplate** table)
{
    for (unsigned long i = 0; i < m_numSpecs; ++i)
    {
        m_specs[i].m_pTemplate = table[m_specs[i].m_uTemplateIndex];
    }
}
