#ifndef GAME_AI_FUZZYVARIANT_H
#define GAME_AI_FUZZYVARIANT_H

#include "Game/AI/Variant.h"
#include "NL/nlAVLTree.h"
#include "NL/nlList.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTimer.h"

class cTeam;
class cBall;
class cFielder;
class cGame;
class InterpreterCore;
class UnidentifiedScriptMachine;
class UnidentifiedFielderInput;

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

    template <typename T>
    FuzzyVariant(const T& value)
        : Variant(VariantTypeOf(value), value)
    {
    }

    FuzzyVariant(cPlayer* value)
        : Variant()
    {
        mType = FT_PLAYER;
        mData.pPlayer = value;
    }

    FuzzyVariant(cTeam* value)
        : Variant()
    {
        mType = FT_TEAM;
        mData.pTeam = value;
    }

    FuzzyVariant(cBall* value)
        : Variant()
    {
        mType = FT_BALL;
        mData.pointer = value;
    }

    FuzzyVariant(cGame* value)
        : Variant()
    {
        mType = FT_GAME;
        mData.pointer = value;
    }

    FuzzyVariant(eVariantType type, const nlVector3& value)
        : Variant(type, value)
    {
    }

    template <typename T>
    FuzzyVariant(eVariantType type, T value)
        : Variant(type, value)
    {
    }

    FuzzyVariant& operator=(const FuzzyVariant& other)
    {
        Variant value(other);
        Reset();
        CopyFrom(value);
        return *this;
    }

    virtual unsigned long GetHash() const;
    virtual NLString ToString() const;
    virtual bool IsPointerType() const;
};

class UnidentifiedFuzzyVariantData : public FuzzyVariant
{
public:
    UnidentifiedFuzzyVariantData()
        : FuzzyVariant()
        , mIndex(-1)
    {
    }

    UnidentifiedFuzzyVariantData(
        int index, FuzzyVariant value);

    ~UnidentifiedFuzzyVariantData()
    {
    }

    static void operator delete(void* entry);

    UnidentifiedFuzzyVariantData& operator=(
        const UnidentifiedFuzzyVariantData& other)
    {
        FuzzyVariant::operator=(other);
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
    void Set(int index, float value)
    {
        FuzzyVariant variant(value);
        Set(index, (const Variant&)variant);
    }

    UnidentifiedVariantCollection& operator=(
        const UnidentifiedVariantCollection& other);

    UnidentifiedFuzzyVariantData* mData[19];
};

class UnidentifiedVariant_80054AB8 : public FuzzyVariant
{
public:
    UnidentifiedVariant_80054AB8()
        : FuzzyVariant()
        , ExtraData()
        , mTemporary(false)
    {
    }

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

    UnidentifiedVariant_80054AB8(const UnidentifiedVariant_80054AB8& other,
        float fParam1 = -1.0f, float fParam2 = -1.0f);
    template <typename T>
    UnidentifiedVariant_80054AB8(const T& value, float fParam1, float fParam2)
        : FuzzyVariant(value)
        , mTemporary(false)
    {
        if (fParam1 > -1.0f)
            ExtraData.Set(4, fParam1);
        if (fParam2 > -1.0f)
            ExtraData.Set(6, fParam2);
    }
    UnidentifiedVariant_80054AB8(UnidentifiedVariant_80054AB8* other);

    ~UnidentifiedVariant_80054AB8()
    {
    }

    static void operator delete(void* entry);

    UnidentifiedVariant_80054AB8& operator=(const UnidentifiedVariant_80054AB8& other);

    UnidentifiedVariant_80054AB8& operator=(int input);

    UnidentifiedVariant_80054AB8& operator=(const FuzzyVariant& other)
    {
        Variant value(other);
        Reset();
        CopyFrom(value);
        return *this;
    }

    Variant* fn_800C2C00(int index) { return ExtraData.Get(index); }
    UnidentifiedVariantCollection* fn_800C2C08() { return &ExtraData; }
    bool fn_800C2C10(int index) const { return ExtraData.IsSet(index); }

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

    float UnidentifiedGetFloat(int index)
    {
        if (ExtraData.IsSet(index))
            return ExtraData.Get(index)->mData.f;
        return 0.0f;
    }

    UnidentifiedVariant_80054AB8* next;
    UnidentifiedVariantCollection ExtraData;
    bool mTemporary;
};

class UnidentifiedActionQueue
{
public:
    UnidentifiedActionQueue();
    ~UnidentifiedActionQueue();

    static void operator delete(void* entry);

    void fn_8030FF6C(bool preserveSelected);
    void fn_8031002C(int actionSelection);
    void fn_80310034(float* weights, int count);
    UnidentifiedVariant_80054AB8* fn_80310040(
        UnidentifiedVariant_80054AB8* pNewAction);
    UnidentifiedVariant_80054AB8* fn_80310B80(
        UnidentifiedVariant_80054AB8* pAction);
    UnidentifiedVariant_80054AB8* SelectAction();

    UnidentifiedVariant_80054AB8* m_pLastQueuedAction;
    UnidentifiedVariant_80054AB8* m_pSelectedAction;
    nlList<UnidentifiedVariant_80054AB8> m_lQueuedActions;
    int mActionSelection;
    float* m_pSelectionWeights;
    int mNumSelectionWeights;
};

extern SlotPool<UnidentifiedFuzzyVariantData> lbl_80584200;
extern SlotPool<UnidentifiedActionQueue> lbl_80584228;
extern SlotPool<UnidentifiedVariant_80054AB8> lbl_805842C8;

extern "C" UnidentifiedVariant_80054AB8 fn_80054AB8(InterpreterCore*, const char*, cTeam*);
extern "C" UnidentifiedVariant_80054AB8 fn_80041AFC(InterpreterCore*, const char*, cFielder*);
extern "C" UnidentifiedVariant_80054AB8 fn_80041B0C(void*, cFielder*, const char*);

inline UnidentifiedVariant_80054AB8::UnidentifiedVariant_80054AB8(
    const UnidentifiedVariant_80054AB8& other, float fParam1, float fParam2)
    : FuzzyVariant((const FuzzyVariant&)other)
    , mTemporary(false)
{
    ExtraData = other.ExtraData;
    if (fParam1 > -1.0f)
        ExtraData.Set(4, fParam1);
    if (fParam2 > -1.0f)
        ExtraData.Set(6, fParam2);
}

inline void UnidentifiedFuzzyVariantData::operator delete(void* entry)
{
    lbl_80584200.DeleteEntry((UnidentifiedFuzzyVariantData*)entry);
}

inline UnidentifiedVariantCollection& UnidentifiedVariantCollection::operator=(
    const UnidentifiedVariantCollection& other)
{
    for (int i = 0; i < 19; i++)
    {
        if (other.IsSet(i))
        {
            if (mData[i] == 0)
            {
                mData[i] = new (lbl_80584200.Allocate())
                    UnidentifiedFuzzyVariantData(
                        i, (const FuzzyVariant&)*other.mData[i]);
            }
            else
            {
                *mData[i] = *other.mData[i];
            }
        }
        else if (IsSet(i))
        {
            Remove(i);
        }
    }
    return *this;
}

inline void UnidentifiedVariant_80054AB8::operator delete(void* entry)
{
    lbl_805842C8.DeleteEntry((UnidentifiedVariant_80054AB8*)entry);
}

inline void UnidentifiedActionQueue::operator delete(void* entry)
{
    lbl_80584228.DeleteEntry((UnidentifiedActionQueue*)entry);
}

inline UnidentifiedVariant_80054AB8::UnidentifiedVariant_80054AB8(
    UnidentifiedVariant_80054AB8* other)
    : FuzzyVariant((const FuzzyVariant&)*other)
{
    ExtraData = other->ExtraData;

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
        FuzzyVariant::operator=(base);
    }

    for (int i = 0; i < 19; i++)
    {
        if (other.ExtraData.IsSet(i))
        {
            if (ExtraData.mData[i] == 0)
            {
                ExtraData.mData[i] = new (lbl_80584200.Allocate())
                    UnidentifiedFuzzyVariantData(
                        i, (const FuzzyVariant&)*other.ExtraData.mData[i]);
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
