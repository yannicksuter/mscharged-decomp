#ifndef GAME_AI_DESIREUPDATE_INL
#define GAME_AI_DESIREUPDATE_INL

#include "Game/AI/DesireUpdate.h"

template <typename T>
inline DesireUpdate& DesireUpdate::operator=(T input)
{
    {
        FuzzyVariant other(VariantTypeOf(input), input);
        FuzzyVariant::operator=(other);
    }
    mTemporary = false;
    return *this;
}

inline DesireUpdate& DesireUpdate::SetDesireFinished()
{
    {
        FuzzyVariant other(FT_INT, 1);
        Variant value(other);
        Reset();
        CopyFrom(value);
    }
    mTemporary = false;
    return *this;
}

#endif // GAME_AI_DESIREUPDATE_INL
