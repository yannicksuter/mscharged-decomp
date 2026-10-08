#ifndef GAME_AI_DESIREUPDATE_H
#define GAME_AI_DESIREUPDATE_H

#include "Game/AI/FuzzyVariant.h"

enum eDesireUpdateResult
{
    DESIRE_CONTINUE = 0,
    DESIRE_FINISHED = 1,
    DESIRE_CHANGE = 3,
};

class IndexedFuzzyVariant : public FuzzyVariant
{
public:
    IndexedFuzzyVariant()
        : FuzzyVariant()
        , mIndex(-1)
    {
    }

    IndexedFuzzyVariant(
        int index, FuzzyVariant value);

    ~IndexedFuzzyVariant()
    {
    }

    static void operator delete(void* entry);

    IndexedFuzzyVariant& operator=(
        const IndexedFuzzyVariant& other)
    {
        FuzzyVariant::operator=(other);
        mIndex = other.mIndex;
        return *this;
    }

    int mIndex;
};

class FuzzyVariantCollection
{
public:
    FuzzyVariantCollection();
    ~FuzzyVariantCollection();

    bool IsSet(int index) const;
    FuzzyVariant* Get(int index);
    void Remove(int index);
    void Set(int index, FuzzyVariant value);
    void Set(int index, const unsigned long& value)
    {
        Set(index, FuzzyVariant(value));
    }
    void Set(int index, const int& value)
    {
        Set(index, FuzzyVariant(value));
    }
    void Set(int index, float value)
    {
        Set(index, FuzzyVariant(value));
    }

    FuzzyVariantCollection& operator=(
        const FuzzyVariantCollection& other);

    IndexedFuzzyVariant* mData[19];
};

class DesireUpdate : public FuzzyVariant
{
public:
    DesireUpdate()
        : FuzzyVariant()
        , ExtraData()
        , mTemporary(false)
    {
    }

    DesireUpdate(cPlayer* value)
        : FuzzyVariant(value)
        , mTemporary(false)
    {
    }

    template <typename T>
    DesireUpdate(eVariantType type, T value)
        : FuzzyVariant(type, value)
        , mTemporary(false)
    {
    }

    DesireUpdate(const DesireUpdate& other,
        float fParam1 = -1.0f, float fParam2 = -1.0f);
    template <typename T>
    DesireUpdate(const T& value, float fParam1, float fParam2)
        : FuzzyVariant(value)
        , mTemporary(false)
    {
        if (fParam1 > -1.0f)
            ExtraData.Set(4, fParam1);
        if (fParam2 > -1.0f)
            ExtraData.Set(6, fParam2);
    }
    DesireUpdate(DesireUpdate* other);

    ~DesireUpdate()
    {
    }

    static void operator delete(void* entry);

    DesireUpdate& operator=(const DesireUpdate& other);

    DesireUpdate& operator=(DesireUpdate* other);

    template <typename T>
    DesireUpdate& operator=(T input);

    DesireUpdate& SetDesireFinished();

    DesireUpdate& operator=(const FuzzyVariant& other)
    {
        Variant value(other);
        Reset();
        CopyFrom(value);
        return *this;
    }

    void SetParameter(int index, FuzzyVariant value);

    Variant* GetParameter(int index) { return ExtraData.Get(index); }
    FuzzyVariantCollection* GetParameters() { return &ExtraData; }
    bool IsParameterSet(int index) const { return ExtraData.IsSet(index); }

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

    float GetFloatParameter(int index)
    {
        if (ExtraData.IsSet(index))
            return ExtraData.Get(index)->mData.f;
        return 0.0f;
    }

    DesireUpdate* next;
    FuzzyVariantCollection ExtraData;
    bool mTemporary;
};

enum eActionSelection
{
    ACTION_SELECT_FIRST = 0,
    ACTION_SELECT_ORDERED_CHANCE = 1,
    ACTION_SELECT_WEIGHTED_RANDOM = 2,
};

class ScriptActionQueue
{
public:
    ScriptActionQueue();
    ~ScriptActionQueue();

    static void operator delete(void* entry);

    void ClearQueuedActions(bool preserveSelected);
    void SetActionSelection(int actionSelection);
    void SetSelectionWeights(float* weights, int count);
    DesireUpdate* QueueAction(
        DesireUpdate* pNewAction);
    DesireUpdate* FindQueuedAction(
        DesireUpdate* pFind);
    DesireUpdate* SelectAction();

    DesireUpdate* m_pLastQueuedAction;
    DesireUpdate* m_pSelectedAction;
    nlList<DesireUpdate> m_lQueuedActions;
    int mActionSelection;
    float* m_pSelectionWeights;
    int mNumSelectionWeights;
};

extern DesireUpdate lbl_80584250;

extern SlotPool<IndexedFuzzyVariant> lbl_80584200;
extern SlotPool<ScriptActionQueue> g_ScriptActionQueuePool;
extern SlotPool<DesireUpdate> lbl_805842C8;

#include "Game/AI/FuzzyRuntimeCall_fwd.h"

inline DesireUpdate::DesireUpdate(
    const DesireUpdate& other, float fParam1, float fParam2)
    : FuzzyVariant((const FuzzyVariant&)other)
    , mTemporary(false)
{
    ExtraData = other.ExtraData;
    if (fParam1 > -1.0f)
        ExtraData.Set(4, fParam1);
    if (fParam2 > -1.0f)
        ExtraData.Set(6, fParam2);
}

inline void IndexedFuzzyVariant::operator delete(void* entry)
{
    lbl_80584200.DeleteEntry((IndexedFuzzyVariant*)entry);
}

inline FuzzyVariantCollection& FuzzyVariantCollection::operator=(
    const FuzzyVariantCollection& other)
{
    for (int i = 0; i < 19; i++)
    {
        if (other.IsSet(i))
        {
            if (mData[i] == 0)
            {
                mData[i] = new (lbl_80584200.Allocate())
                    IndexedFuzzyVariant(
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

inline void DesireUpdate::operator delete(void* entry)
{
    lbl_805842C8.DeleteEntry((DesireUpdate*)entry);
}

inline void ScriptActionQueue::operator delete(void* entry)
{
    g_ScriptActionQueuePool.DeleteEntry((ScriptActionQueue*)entry);
}

inline DesireUpdate::DesireUpdate(
    DesireUpdate* other)
    : FuzzyVariant((const FuzzyVariant&)*other)
{
    ExtraData = other->ExtraData;

    mTemporary = false;
    if (other->mTemporary)
    {
        delete other;
    }
}

inline DesireUpdate& DesireUpdate::operator=(
    const DesireUpdate& other)
{
    {
        FuzzyVariant base((const FuzzyVariant&)other);
        FuzzyVariant::operator=(base);
    }

    ExtraData = other.ExtraData;
    mTemporary = false;
    return *this;
}

inline DesireUpdate& DesireUpdate::operator=(
    DesireUpdate* other)
{
    FuzzyVariant::operator=(*other);
    ExtraData = other->ExtraData;
    mTemporary = false;
    if (other->mTemporary)
    {
        delete other;
    }
    return *this;
}

#endif // GAME_AI_DESIREUPDATE_H
