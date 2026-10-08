#include "Game/AI/DesireUpdate.h"

IndexedFuzzyVariant gDefaultFuzzyVariantData;
SlotPool<IndexedFuzzyVariant> lbl_80584200(16, 16);

FuzzyVariantCollection::FuzzyVariantCollection()
{
    for (int i = 0; i < 19; i++)
    {
        mData[i] = 0;
    }
}

FuzzyVariantCollection::~FuzzyVariantCollection()
{
    Remove(-1);
}

void FuzzyVariantCollection::Remove(int index)
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

bool FuzzyVariantCollection::IsSet(int index) const
{
    return index > -1 && index < 19 && mData[index] != 0;
}

FuzzyVariant* FuzzyVariantCollection::Get(int index)
{
    if (IsSet(index))
    {
        return mData[index];
    }

    return &gDefaultFuzzyVariantData;
}

void FuzzyVariantCollection::Set(int index, FuzzyVariant value)
{
    if (IsSet(index))
    {
        Variant& current = *mData[index];
        current = value;
    }
    else
    {
        mData[index] = new (lbl_80584200.Allocate())
            IndexedFuzzyVariant(index, value);
    }
}
