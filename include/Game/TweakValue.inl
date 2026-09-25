#ifndef GAME_TWEAK_VALUE_INL
#define GAME_TWEAK_VALUE_INL

#include "Game/TweakValue.h"

inline TweakIntBinding::~TweakIntBinding()
{
}

inline TweakFloatBinding::TweakFloatBinding(float* value)
    : m_pValue(value)
{
}

inline TweakIntBinding::TweakIntBinding(int* value)
    : m_pValue(value)
{
}

#endif // GAME_TWEAK_VALUE_INL
