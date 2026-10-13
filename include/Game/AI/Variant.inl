#ifndef GAME_AI_VARIANT_INL
#define GAME_AI_VARIANT_INL

#include "Game/AI/Variant.h"

inline int Variant::AsInt() const
{
    return mData.i;
}

inline bool Variant::AsBool() const
{
    return mData.b;
}

inline bool Variant::IsSet() const
{
    return mType != FT_UNSPECIFIED;
}

inline bool Variant::IsPointerType() const
{
    return mType == FT_POINTER || mType == FT_STRING;
}

#endif // GAME_AI_VARIANT_INL
