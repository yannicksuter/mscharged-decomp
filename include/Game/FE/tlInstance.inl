#ifndef GAME_FE_TLINSTANCE_INL
#define GAME_FE_TLINSTANCE_INL

#include "Game/FE/tlInstance.h"

inline void TLInstance::SetVisible(bool visible)
{
    m_bVisible = visible;
}

inline eTimeLineAssetType TLInstance::GetType() const
{
    return m_type;
}

inline bool TLInstance::IsVisible() const
{
    return m_bVisible;
}

inline FELibObject* TLInstance::GetLibRefObject() const
{
    return m_component;
}

#endif
