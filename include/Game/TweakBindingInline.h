#ifndef GAME_TWEAK_BINDING_INLINE_H
#define GAME_TWEAK_BINDING_INLINE_H

#include "Game/TweakValue.h"

inline TweakIntBinding::~TweakIntBinding()
{
}

template <typename T>
inline TweakBinding<T>::TweakBinding(T* value)
    : m_pValue(value)
{
}

#endif // GAME_TWEAK_BINDING_INLINE_H
