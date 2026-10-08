#ifndef GAME_AI_FUZZY_VARIANT_COLLECTION_INL
#define GAME_AI_FUZZY_VARIANT_COLLECTION_INL

#include "Game/AI/DesireUpdate.h"

inline IndexedFuzzyVariant::IndexedFuzzyVariant(int index, FuzzyVariant value)
    : FuzzyVariant(value)
    , mIndex(index)
{
}

#endif
