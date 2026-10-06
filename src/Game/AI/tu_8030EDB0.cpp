#include "Game/AI/DesireUpdate.h"

UnidentifiedFuzzyVariantData gDefaultFuzzyVariantData;
SlotPool<UnidentifiedFuzzyVariantData> lbl_80584200(16, 16);

UnidentifiedVariantCollection::UnidentifiedVariantCollection()
{
    for (int i = 0; i < 19; i++)
    {
        mData[i] = 0;
    }
}

UnidentifiedVariantCollection::~UnidentifiedVariantCollection()
{
    Remove(-1);
}

void UnidentifiedVariantCollection::Remove(int index)
{
    if (index > -1 && index < 19)
    {
        if (mData[index] != 0)
        {
            delete mData[index];
            mData[index] = 0;
        }
    }
    else
    {
        for (int i = 0; i < 19; i++)
        {
            if (mData[i] != 0)
            {
                delete mData[i];
                mData[i] = 0;
            }
        }
    }
}

bool UnidentifiedVariantCollection::IsSet(int index) const
{
    return index > -1 && index < 19 && mData[index] != 0;
}

FuzzyVariant* UnidentifiedVariantCollection::Get(int index)
{
    if (IsSet(index))
    {
        return mData[index];
    }

    return &gDefaultFuzzyVariantData;
}

void UnidentifiedVariantCollection::Set(int index, FuzzyVariant value)
{
    if (IsSet(index))
    {
        Variant& current = *mData[index];
        current = value;
    }
    else
    {
        mData[index] = new (lbl_80584200.Allocate())
            UnidentifiedFuzzyVariantData(index, value);
    }
}
