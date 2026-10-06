#ifndef GAME_TWEAK_VALUE_INT_H
#define GAME_TWEAK_VALUE_INT_H

#include "Game/TweakValue.h"
#include "NL/nlPrint.h"
#include <stdlib.h>

typedef TweakValue<int> TweakValueInt;

template <typename T>
inline void FormatOwnedTweakValue(char* buffer, unsigned long size, T value)
{
    nlSNPrintf(buffer, size, "%d", value);
}

inline void ParseOwnedTweakValue(int& value, const char* text)
{
    value = atoi(text);
}

#endif // GAME_TWEAK_VALUE_INT_H
