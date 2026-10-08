#ifndef GAME_AI_FUZZY_VARIANT_INL
#define GAME_AI_FUZZY_VARIANT_INL

#include "Game/AI/FuzzyVariant.h"

inline bool FuzzyVariant::IsPointerType() const
{
    return ((mType == FT_POINTER || mType == FT_STRING)
        || ((unsigned int)(mType - FT_PLAYER)
            <= (unsigned int)(FT_BALL - FT_PLAYER)));
}

#endif
