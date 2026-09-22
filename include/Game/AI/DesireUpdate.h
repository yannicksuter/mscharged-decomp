#ifndef GAME_AI_DESIREUPDATE_H
#define GAME_AI_DESIREUPDATE_H

#include "Game/AI/FuzzyVariant.h"

inline UnidentifiedVariant_80054AB8& UnidentifiedVariant_80054AB8::operator=(int input)
{
    {
        FuzzyVariant other(FT_INT, input);
        FuzzyVariant::operator=(other);
    }
    mTemporary = false;
    return *this;
}

inline UnidentifiedVariant_80054AB8& UnidentifiedVariant_80054AB8::SetDesireFinished()
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

#endif // GAME_AI_DESIREUPDATE_H
