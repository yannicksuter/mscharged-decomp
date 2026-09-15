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

#endif
