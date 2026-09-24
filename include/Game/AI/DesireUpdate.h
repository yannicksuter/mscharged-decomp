#ifndef GAME_AI_DESIREUPDATE_H
#define GAME_AI_DESIREUPDATE_H

#include "Game/AI/FuzzyVariant.h"

enum eDesireUpdateResult
{
    DESIRE_CONTINUE = 0,
    DESIRE_FINISHED = 1,
    DESIRE_CHANGE = 3,
};

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
