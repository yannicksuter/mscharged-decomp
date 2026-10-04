#ifndef GAME_TWEAK_INT_BINDING_INLINE_H
#define GAME_TWEAK_INT_BINDING_INLINE_H

#include "Game/TweakValue.h"

inline TweakIntBinding::TweakIntBinding(int* value)
    : m_pValue(value)
{
}

inline bool TweakIntBinding::BindWithDefault(const char* name, int defaultValue,
    const char* group, bool reload, float value, float min, float max)
{
    bool found = Bind(name, value, group, reload, min, max);
    if (!found)
    {
        *m_pValue = defaultValue;
    }
    return found;
}

#endif // GAME_TWEAK_INT_BINDING_INLINE_H
