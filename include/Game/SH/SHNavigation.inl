#ifndef GAME_SH_SHNAVIGATION_INL
#define GAME_SH_SHNAVIGATION_INL

#include "Game/SH/SHNavigation.h"
#include "Game/FE/tlInstance.inl"

inline void SHNavigation::SetTimerVisible(bool visible)
{
    mTimer->SetVisible(visible);
}

#endif
