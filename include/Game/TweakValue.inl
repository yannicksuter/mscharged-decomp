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

#endif // GAME_TWEAK_VALUE_INL
