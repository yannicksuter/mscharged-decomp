#ifndef GAME_AI_FUZZYVARIANT_H
#define GAME_AI_FUZZYVARIANT_H

#include "Game/AI/Variant.h"
#include "NL/nlSlotPool.h"

class cTeam;
class InterpreterCore;

class FuzzyVariant : public Variant
{
public:
    FuzzyVariant()
        : Variant()
    {
    }

    FuzzyVariant(const FuzzyVariant& other)
        : Variant(other)
    {
    }

    FuzzyVariant(const Variant& other)
        : Variant(other)
    {
    }

    FuzzyVariant(float value)
        : Variant(FT_FLOAT, value)
    {
    }

    FuzzyVariant(cPlayer* value)
        : Variant()
    {
        mType = FT_PLAYER;
        mData.pPlayer = value;
    }

    template <typename T>
    FuzzyVariant(eVariantType type, T value)
        : Variant(type, value)
    {
    }

    virtual unsigned long GetHash() const;
    virtual NLString ToString() const;
    virtual bool IsPointerType() const;
};

class UnidentifiedFuzzyVariantData : public FuzzyVariant
{
public:
    UnidentifiedFuzzyVariantData(
        int index, const FuzzyVariant& value);

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

class UnidentifiedVariantCollection
{
public:
    UnidentifiedVariantCollection();
    ~UnidentifiedVariantCollection();

    bool IsSet(int index) const;
    Variant* Get(int index);
    void Remove(int index);
    void Set(int index, const Variant& value);

    UnidentifiedFuzzyVariantData* mData[19];
};

class UnidentifiedVariant_80054AB8 : public FuzzyVariant
{
public:
    UnidentifiedVariant_80054AB8(cPlayer* value)
        : FuzzyVariant(value)
        , mTemporary(false)
    {
    }

    template <typename T>
    UnidentifiedVariant_80054AB8(eVariantType type, T value)
        : FuzzyVariant(type, value)
        , mTemporary(false)
    {
    }

    UnidentifiedVariant_80054AB8(const UnidentifiedVariant_80054AB8& other);
    UnidentifiedVariant_80054AB8(UnidentifiedVariant_80054AB8* other);

    ~UnidentifiedVariant_80054AB8()
    {
    }

    static void operator delete(void* entry);

    UnidentifiedVariant_80054AB8& operator=(const UnidentifiedVariant_80054AB8& other);

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
    UnidentifiedVariantCollection ExtraData;
    bool mTemporary;
};

extern BasicSlotPool<UnidentifiedFuzzyVariantData> lbl_80584200;
extern BasicSlotPool<UnidentifiedVariant_80054AB8> lbl_805842C8;

extern "C" void fn_8030EEB8(UnidentifiedVariantCollection*, int);
extern "C" bool fn_8030F030(
    const UnidentifiedVariantCollection*, int);
extern "C" Variant* fn_8030F060(UnidentifiedVariantCollection*, int);
extern "C" UnidentifiedVariant_80054AB8 fn_80054AB8(InterpreterCore*, const char*, cTeam*);

inline bool UnidentifiedVariantCollection::IsSet(int index) const
{
    return fn_8030F030(this, index);
}

inline Variant* UnidentifiedVariantCollection::Get(int index)
{
    return fn_8030F060(this, index);
}

inline void UnidentifiedVariantCollection::Remove(int index)
{
    fn_8030EEB8(this, index);
}

inline void UnidentifiedVariant_80054AB8::operator delete(void* entry)
{
    lbl_805842C8.DeleteEntry((UnidentifiedVariant_80054AB8*)entry);
}

inline UnidentifiedVariant_80054AB8::UnidentifiedVariant_80054AB8(
    const UnidentifiedVariant_80054AB8& other)
    : FuzzyVariant(other)
{
    for (int i = 0; i < 19; i++)
    {
        if (other.ExtraData.IsSet(i))
        {
            if (ExtraData.mData[i] == 0)
            {
                ExtraData.mData[i] = new (lbl_80584200.Allocate())
                    UnidentifiedFuzzyVariantData(
                        i, FuzzyVariant(*other.ExtraData.mData[i]));
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

inline UnidentifiedVariant_80054AB8::UnidentifiedVariant_80054AB8(
    UnidentifiedVariant_80054AB8* other)
    : FuzzyVariant(*other)
{
    for (int i = 0; i < 19; i++)
    {
        if (other->ExtraData.IsSet(i))
        {
            if (ExtraData.mData[i] == 0)
            {
                ExtraData.mData[i] = new (lbl_80584200.Allocate())
                    UnidentifiedFuzzyVariantData(
                        i, FuzzyVariant(*other->ExtraData.mData[i]));
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

inline UnidentifiedVariant_80054AB8& UnidentifiedVariant_80054AB8::operator=(
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
            if (ExtraData.mData[i] == 0)
            {
                ExtraData.mData[i] = new (lbl_80584200.Allocate())
                    UnidentifiedFuzzyVariantData(
                        i, FuzzyVariant(*other.ExtraData.mData[i]));
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
