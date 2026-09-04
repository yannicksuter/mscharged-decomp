#include "Game/AI/FuzzyVariant.h"

UnidentifiedFuzzyVariantData lbl_805841D8;
BasicSlotPool<UnidentifiedFuzzyVariantData> lbl_80584200(16, 16);

UnidentifiedVariantCollection::UnidentifiedVariantCollection()
{
    for (int i = 0; i < 19; i++)
    {
        mData[i] = 0;
    }
}

UnidentifiedVariantCollection::~UnidentifiedVariantCollection()
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

Variant* UnidentifiedVariantCollection::Get(int index)
{
    if (IsSet(index))
    {
        return mData[index];
    }

    return &lbl_805841D8;
}

void UnidentifiedVariantCollection::Set(int index, const Variant& value)
{
    if (IsSet(index))
    {
        Variant& current = *mData[index];
        current = value;
    }
    else
    {
        mData[index] = new (lbl_80584200.Allocate())
            UnidentifiedFuzzyVariantData(index, FuzzyVariant(value));
    }
}
