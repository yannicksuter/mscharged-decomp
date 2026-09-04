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

    UnidentifiedDesireUpdate& operator=(
        const UnidentifiedVariant_80054AB8& other)
    {
        {
            FuzzyVariant base((const FuzzyVariant&)other);
            Variant value(base);
            Reset();
            CopyFrom(value);
        }

        for (int i = 0; i < 19; i++)
        {
            if (other.ExtraData.IsSet(i))
            {
                if (mExtraData.mData[i] == 0)
                {
                    mExtraData.mData[i] = new (lbl_80584200.Allocate())
                        UnidentifiedFuzzyVariantData(
                            i, FuzzyVariant(*other.ExtraData.mData[i]));
                }
                else
                {
                    *mExtraData.mData[i] = *other.ExtraData.mData[i];
                }
            }
            else if (mExtraData.IsSet(i))
            {
                mExtraData.Remove(i);
            }
        }

        mTemporary = false;
        return *this;
    }

    float mConfidence;
    UnidentifiedVariantCollection mExtraData;
    bool mTemporary;
};

#endif // GAME_AI_DESIREUPDATE_H
