#ifndef GAME_AI_DESIREUPDATE_H
#define GAME_AI_DESIREUPDATE_H

#include "Game/AI/FuzzyVariant.h"

struct UnidentifiedDesireUpdate : public FuzzyVariant
{
    UnidentifiedDesireUpdate& operator=(const FuzzyVariant& other)
    {
        Variant value(other);
        Reset();
        CopyFrom(value);
        return *this;
    }

    float mConfidence;
    UnidentifiedVariantCollection mExtraData;
    bool mTemporary;
};

#endif // GAME_AI_DESIREUPDATE_H
