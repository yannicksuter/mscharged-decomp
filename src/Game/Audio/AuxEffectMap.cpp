#include "Game/Audio/AudioEffect.h"

AuxEffectMap* g_pAuxEffectMap;

AuxEffectMap::AuxEffectMap()
{
    m_Effects[0] = -1;
    m_Effects[1] = -1;
    m_Indices[0] = -1;
    m_Indices[1] = -1;
}

int AuxEffectMap::AssignAuxiliary(const int& effect)
{
    int& effectIndex = m_Indices[effect];
    for (u16 index = 0; index < 2; ++index)
    {
        if (m_Effects[index] == -1)
        {
            m_Effects[index] = effect;
            effectIndex = index;
            break;
        }
    }
    return effectIndex;
}

int AuxEffectMap::GetAuxiliary(const int& effect) const
{
    return m_Indices[effect];
}
