#ifndef GAME_AI_VARIANT_INL
#define GAME_AI_VARIANT_INL

#include "Game/AI/Variant.h"

inline int Variant::fn_800C2BD4() const
{
    return mData.i;
}

inline bool Variant::fn_800C2BF8() const
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
