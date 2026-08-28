#ifndef GAME_AI_FUZZYVARIANT_H
#define GAME_AI_FUZZYVARIANT_H

#include "Game/AI/Variant.h"
#include "NL/nlSlotPool.h"

class cTeam;
class InterpreterCore;

class UnidentifiedFuzzyVariantBase : public Variant
{
public:
    UnidentifiedFuzzyVariantBase()
    {
    }

    UnidentifiedFuzzyVariantBase(const UnidentifiedFuzzyVariantBase& other)
        : Variant(other)
    {
    }

    UnidentifiedFuzzyVariantBase(const Variant& other)
        : Variant(other)
    {
    }

};

class UnidentifiedFuzzyVariantData : public Variant
{
public:
    UnidentifiedFuzzyVariantData(
        int index, const UnidentifiedFuzzyVariantBase& value);

    UnidentifiedFuzzyVariantData& operator=(
        const UnidentifiedFuzzyVariantData& other)
    {
        {
            Variant value(other);
            Reset();
            CopyFrom(value);
        }
        mIndex = other.mIndex;
        return *this;
    }

    int mIndex;
};

class UnidentifiedFuzzyVariantDataSet
{
public:
    UnidentifiedFuzzyVariantDataSet();
    ~UnidentifiedFuzzyVariantDataSet();

    bool IsSet(int index) const;
    void Remove(int index);

    UnidentifiedFuzzyVariantData* mData[19];
};

class FuzzyVariant : public UnidentifiedFuzzyVariantBase
{
public:
    FuzzyVariant(const FuzzyVariant& other);
    FuzzyVariant(FuzzyVariant* other);

    ~FuzzyVariant()
    {
    }

    static void operator delete(void* entry);

    FuzzyVariant& operator=(const FuzzyVariant& other);

    int GetInt() const
    {
        if ((unsigned int)GetType() == FT_INT)
        {
            return mData.i;
        }
        if ((unsigned int)GetType() == FT_U32)
        {
            return mData.u;
        }
        return -1;
    }

    float Confidence;
    UnidentifiedFuzzyVariantDataSet ExtraData;
    bool mTemporary;
};

extern BasicSlotPool<UnidentifiedFuzzyVariantData> lbl_80584200;
extern BasicSlotPool<FuzzyVariant> lbl_805842C8;

extern "C" void fn_8030EDB0(UnidentifiedFuzzyVariantDataSet*);
extern "C" void fn_8030EEB8(UnidentifiedFuzzyVariantDataSet*, int);
extern "C" bool fn_8030F030(
    const UnidentifiedFuzzyVariantDataSet*, int);
extern "C" FuzzyVariant fn_80054AB8(InterpreterCore*, const char*, cTeam*);

inline UnidentifiedFuzzyVariantDataSet::UnidentifiedFuzzyVariantDataSet()
{
    fn_8030EDB0(this);
}

inline bool UnidentifiedFuzzyVariantDataSet::IsSet(int index) const
{
    return fn_8030F030(this, index);
}

inline void UnidentifiedFuzzyVariantDataSet::Remove(int index)
{
    fn_8030EEB8(this, index);
}

inline void FuzzyVariant::operator delete(void* entry)
{
    lbl_805842C8.DeleteEntry((FuzzyVariant*)entry);
}

inline FuzzyVariant::FuzzyVariant(const FuzzyVariant& other)
    : UnidentifiedFuzzyVariantBase(other)
{
    for (int i = 0; i < 19; i++)
    {
        if (other.ExtraData.IsSet(i))
        {
            if (ExtraData.mData[i] == 0)
            {
                ExtraData.mData[i] = new (lbl_80584200.Allocate())
                    UnidentifiedFuzzyVariantData(
                        i, UnidentifiedFuzzyVariantBase(*other.ExtraData.mData[i]));
            }
            else
            {
                *ExtraData.mData[i] = *other.ExtraData.mData[i];
            }
        }
        else if (ExtraData.IsSet(i))
        {
            ExtraData.Remove(i);
        }
    }

    mTemporary = false;
}

inline FuzzyVariant::FuzzyVariant(FuzzyVariant* other)
    : UnidentifiedFuzzyVariantBase(*other)
{
    for (int i = 0; i < 19; i++)
    {
        if (other->ExtraData.IsSet(i))
        {
            if (ExtraData.mData[i] == 0)
            {
                ExtraData.mData[i] = new (lbl_80584200.Allocate())
                    UnidentifiedFuzzyVariantData(
                        i, UnidentifiedFuzzyVariantBase(*other->ExtraData.mData[i]));
            }
            else
            {
                *ExtraData.mData[i] = *other->ExtraData.mData[i];
            }
        }
        else if (ExtraData.IsSet(i))
        {
            ExtraData.Remove(i);
        }
    }

    mTemporary = false;
    if (other->mTemporary)
    {
        delete other;
    }
}

inline FuzzyVariant& FuzzyVariant::operator=(const FuzzyVariant& other)
{
    {
        UnidentifiedFuzzyVariantBase base(
            (const UnidentifiedFuzzyVariantBase&)other);
        Variant value(base);
        Reset();
        CopyFrom(value);
    }

    for (int i = 0; i < 19; i++)
    {
        if (other.ExtraData.IsSet(i))
        {
            if (ExtraData.mData[i] == 0)
            {
                ExtraData.mData[i] = new (lbl_80584200.Allocate())
                    UnidentifiedFuzzyVariantData(
                        i, UnidentifiedFuzzyVariantBase(*other.ExtraData.mData[i]));
            }
            else
            {
                *ExtraData.mData[i] = *other.ExtraData.mData[i];
            }
        }
        else if (ExtraData.IsSet(i))
        {
            ExtraData.Remove(i);
        }
    }

    mTemporary = false;
    return *this;
}

#endif // GAME_AI_FUZZYVARIANT_H
