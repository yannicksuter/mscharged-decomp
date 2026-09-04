#include "Game/AI/FuzzyAIRuntime.h"

#include "Game/AI/Desire.h"
#include "Game/AI/UnidentifiedStringHash.h"
#include "Game/MathHelpers.h"
#include "NL/nlAVLTree.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

extern int nlPrintf(const char*, ...);
extern "C" int fn_802DF9FC(InterpreterCore*);
extern "C" void fn_800B6A1C(
    UnidentifiedVariant_80054AB8*, int, const Variant&);
extern "C" void fn_8031A04C();
extern "C" void fn_80311C54(void*, unsigned long, void*);

struct UnidentifiedFuzzyRuntimeReference
{
    u8 mPadding000[0x18];
    UnidentifiedFuzzyRuntimeBase* mRuntime;
};

struct UnidentifiedTransitionOwner
{
    u8 mPadding000[0x0C];
    UnidentifiedStateTransition mTransition;
};

struct UnidentifiedTransitionReference
{
    u8 mPadding000[0x18];
    UnidentifiedTransitionOwner* mOwner;
};

struct UnidentifiedFuzzyRuntimeList
{
    UnidentifiedFuzzyRuntimeList(
        UnidentifiedFuzzyRuntimeBase* head,
        UnidentifiedFuzzyRuntimeBase* tail)
    {
        mTail = tail;
        mHead = head;
    }

    void AddEnd(UnidentifiedFuzzyRuntimeBase* runtime)
    {
        nlListAddEnd(&mHead, &mTail, runtime);
    }

    UnidentifiedFuzzyRuntimeBase* mHead;
    UnidentifiedFuzzyRuntimeBase* mTail;
};

char lbl_80530154[] = "Undefined";
char lbl_80530160[] = "Confidence";
char lbl_8053016C[] = "ConfThreshold";
char lbl_8053017C[] = "SelectChance";
char lbl_8053018C[] = "Duration";
char lbl_80530198[] = "Concurrent";
char lbl_805301A4[] = "Transition";
char lbl_805301B0[] = "Modifier";
char lbl_805301BC[] = "AllowReinit";

char lbl_806DF570[] = "Arg1";
char lbl_806DF578[] = "Arg2";
char lbl_806DF580[] = "Arg3";
char lbl_806DF588[] = "Arg4";
char lbl_806DF590[] = "StateID";
char lbl_806DF598[] = "%f";

void* lbl_806E20A0;
UnidentifiedFuzzyRuntimeList lbl_806E20A8(0, 0);
nlAVLTreeSlotPool<unsigned long, UnidentifiedVariant_80054AB8,
    DefaultKeyCompare<unsigned long> > lbl_805842EC(16, 16);
UnidentifiedRuntimeTypeList lbl_806E20B0;
BasicSlotPool<UnidentifiedRuntimeActionQueue> lbl_80584328(16, 16);

extern "C" UnidentifiedFuzzyRuntimeBase* fn_80311734(
    UnidentifiedFuzzyRuntimeReference* reference)
{
    return reference->mRuntime->mUnidentified064->mRuntime;
}

extern "C" UnidentifiedFuzzyRuntimeBase* fn_80311744(
    UnidentifiedFuzzyRuntimeBase* runtime)
{
    return runtime->mUnidentified064->mRuntime;
}

extern "C" UnidentifiedFuzzyRuntimeBase* fn_80311750(
    UnidentifiedFuzzyRuntimeValue* value)
{
    return value->mRuntime;
}

UnidentifiedFuzzyRuntimeBase::UnidentifiedFuzzyRuntimeBase(
    UnidentifiedFielderInput* value)
    : InterpreterCore(0x100)
    , mCollection(0, 0)
    , mUnidentified038(16, 16)
{
    mUnidentified060 = false;
    mUnidentified05C = 0;
    mUnidentified058 = -1;
    mValue = value;
    mUnidentified064 = 0;
    next = 0;
    lbl_806E20A8.AddEnd(this);

    if (mValue != 0)
    {
        mValue->mUnidentified14 =
            (UnidentifiedFielderInputOwner*)this;
    }

    if (lbl_806E20A0 != 0)
    {
        LoadByteCode(lbl_806E20A0);
    }
}

UnidentifiedFuzzyRuntimeBase::~UnidentifiedFuzzyRuntimeBase()
{
    nlListRemoveElement(
        &lbl_806E20A8.mHead, this, &lbl_806E20A8.mTail);

    if (lbl_806E20A8.mHead == 0 && lbl_806E20A0 != 0)
    {
        delete[] (u8*)lbl_806E20A0;
        lbl_806E20A0 = 0;

        lbl_805842EC.Clear();
        lbl_805842EC.m_Allocator.FreeBlocks();
        lbl_80584200.FreeBlocks();
        lbl_80584228.FreeBlocks();
        lbl_80584328.FreeBlocks();
        lbl_805842C8.FreeBlocks();

        while (lbl_806E20B0.mHead != 0)
        {
            UnidentifiedRuntimeTypeEntry* entry = lbl_806E20B0.mHead;
            if (lbl_806E20B0.mTail == entry)
            {
                lbl_806E20B0.mTail = 0;
            }
            lbl_806E20B0.mHead = entry->next;
            delete entry;
        }
    }

    while (mCollection.mHead != 0)
    {
        UnidentifiedRuntimeActionQueue* entry = mCollection.mHead;
        if (mCollection.mTail == entry)
        {
            mCollection.mTail = 0;
        }
        mCollection.mHead = entry->next;

        if (entry->mOwnsQueue)
        {
            delete entry->mQueue;
        }
        lbl_80584328.DeleteEntry(entry);
    }
}

extern "C" void fn_80311AFC(const char* filename, bool async)
{
    if (lbl_806E20A0 != 0)
    {
        UnidentifiedFuzzyRuntimeBase* runtime = lbl_806E20A8.mHead;
        while (runtime != 0)
        {
            if (!runtime->IsFinished())
            {
                runtime->StopWithoutUndo();
            }
            runtime = runtime->next;
        }

        delete[] (u8*)lbl_806E20A0;
        lbl_806E20A0 = 0;
    }

    if (lbl_806E20A0 == 0)
    {
        if (async)
        {
            nlLoadEntireFileAsync(
                filename, (LoadAsyncCallback)fn_80311C54,
                0, 0x20, AllocateStart, 0, 0, 0);
        }
        else
        {
            unsigned long size = 0;
            lbl_806E20A0 = nlLoadEntireFile(
                filename, &size, 0x20, AllocateStart, 0, 0, 0);
            if (lbl_806E20A0 != 0)
            {
                UnidentifiedFuzzyRuntimeBase* runtime =
                    lbl_806E20A8.mHead;
                while (runtime != 0)
                {
                    runtime->LoadByteCode(lbl_806E20A0);
                    runtime = runtime->next;
                }
            }
        }
    }
}

extern "C" void fn_80311C54(
    void* byteCode, unsigned long, void*)
{
    lbl_806E20A0 = byteCode;
}

extern "C" bool fn_80311C5C()
{
    if (lbl_806E20A0 == 0)
    {
        return false;
    }

    UnidentifiedFuzzyRuntimeBase* runtime = lbl_806E20A8.mHead;
    while (runtime != 0)
    {
        runtime->LoadByteCode(lbl_806E20A0);
        runtime = runtime->next;
    }
    return true;
}

void UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual15()
{
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_80530154, -1));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DF570, 0));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DF578, 1));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DF580, 2));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DF588, 3));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_80530160, 4));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_8053016C, 5));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_8053017C, 6));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_8053018C, 7));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_806DF590, 8));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_80530198, 9));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_805301A4, 10));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_805301B0, 11));
    lbl_806E20B0.AddEnd(new (nlMalloc(
        sizeof(UnidentifiedRuntimeTypeEntry), 8, false))
            UnidentifiedRuntimeTypeEntry(lbl_805301BC, 12));
}

extern "C" int fn_80312208(unsigned long hash)
{
    UnidentifiedRuntimeTypeEntry* entry = lbl_806E20B0.mHead;
    while (entry != 0)
    {
        if (hash == entry->mHash)
        {
            return entry->mType;
        }
        entry = entry->next;
    }
    return -1;
}

bool UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual2(
    FunctionEntryPoint* function, unsigned int argumentCount,
    u32 arg1, u32 arg2, u32 arg3, u32 arg4)
{
    mUnidentified05C = function->hash;
    bool result = InterpreterCore::UnidentifiedVirtual2(
        function, argumentCount, arg1, arg2, arg3, arg4);

    UnidentifiedVariant_80054AB8* returnValue =
        mUnidentified060
            ? *(UnidentifiedVariant_80054AB8**)m_SP
            : 0;

    ListEntry<UnidentifiedVariant_80054AB8*>* entry =
        mUnidentified038.m_Head;
    while (entry != 0)
    {
        UnidentifiedVariant_80054AB8* value = entry->entry;
        if (value != returnValue && value != 0)
        {
            delete value;
        }
        entry = entry->next;
    }
    mUnidentified038.Clear();
    mUnidentified05C = 0;
    return result;
}

extern "C" void* fn_80312358(void*, void* value)
{
    return value;
}

extern "C" UnidentifiedVariant_80054AB8* fn_80312360(
    UnidentifiedFuzzyRuntimeBase* runtime,
    FunctionEntryPoint* function, int argumentCount,
    void* arg1, void* arg2)
{
    runtime->mUnidentified060 = true;
    switch (argumentCount)
    {
    case 0:
        runtime->UnidentifiedVirtual2(
            function, 0, 0, 0, 0, 0);
        break;
    case 1:
        runtime->UnidentifiedVirtual2(
            function, 1, (u32)arg1, 0, 0, 0);
        break;
    case 2:
        runtime->UnidentifiedVirtual2(
            function, 2, (u32)arg1, (u32)arg2, 0, 0);
        break;
    }

    UnidentifiedVariant_80054AB8* result =
        *(UnidentifiedVariant_80054AB8**)runtime->m_SP;
    if (result != 0)
    {
        result->mTemporary = true;
    }
    runtime->mUnidentified060 = false;
    return result;
}

extern "C" UnidentifiedVariant_80054AB8* fn_8031243C(
    UnidentifiedFuzzyRuntimeBase* runtime, u32 hash,
    UnidentifiedFuzzyRuntimeValue* action)
{
    if (!runtime->mValue->IsPointerType())
    {
        return 0;
    }

    runtime->mUnidentified064 = action;
    void* value = runtime->mValue->mData.pointer;
    FunctionEntryPoint* function = fn_802DF3E4(runtime, &hash);
    runtime->mUnidentified060 = true;
    runtime->UnidentifiedVirtual2(
        function, 1, (u32)value, 0, 0, 0);

    UnidentifiedVariant_80054AB8* result =
        *(UnidentifiedVariant_80054AB8**)runtime->m_SP;
    if (result != 0)
    {
        result->mTemporary = true;
    }
    runtime->mUnidentified060 = false;
    runtime->mUnidentified064 = 0;
    return result;
}

float UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual3(
    float first, float second)
{
    float result = 1.0f - nlAbs(first - second);
    return result >= 0.0f ? result : 0.0f;
}

float UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual4(
    float first, float second)
{
    float result = 0.0f;
    float difference = second - first;
    if (difference > 0.0f)
    {
        float divisor = 1.0f - first;
        divisor = divisor >= second ? divisor : second;
        divisor = divisor <= 0.5f ? divisor : 0.5f;
        result = difference / divisor;
        result = result >= 0.0f ? result : 0.0f;
        result = result <= 1.0f ? result : 1.0f;
    }
    return result;
}

float UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual5(float value)
{
    return 1.0f - value;
}

float UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual6(float value)
{
    float inverse = 1.0f - value;
    float minimum = value <= inverse ? value : inverse;
    float maximum = value >= inverse ? value : inverse;
    return minimum / maximum;
}

float UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual7(
    float first, float second, float third)
{
    third = third <= first ? third : first;
    if (third < first && first < 0.5f)
    {
        third *= second;
    }
    return third;
}

float UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual8()
{
    UnidentifiedActionQueue* queue =
        new (lbl_80584228.Allocate()) UnidentifiedActionQueue;
    UnidentifiedRuntimeActionQueue* entry =
        lbl_80584328.Allocate();
    if (entry != 0)
    {
        entry->mQueue = queue;
        entry->mConfidence = 0.0f;
        entry->mOwnsQueue = true;
        entry->mUnidentified008 = 0;
    }
    nlListAddStart(
        &mCollection.mHead, entry, &mCollection.mTail);
    return 1.0f;
}

UnidentifiedVariant_80054AB8*
UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual9()
{
    UnidentifiedRuntimeActionQueue* entry =
        nlListRemoveStart(&mCollection.mHead, &mCollection.mTail);
    UnidentifiedActionQueue* queue = entry->mQueue;
    UnidentifiedVariant_80054AB8* selected = queue->SelectAction();
    queue->fn_8030FF6C(true);
    if (entry->mOwnsQueue)
    {
        delete queue;
    }
    lbl_80584328.DeleteEntry(entry);
    return selected;
}

extern "C" void* fn_80312E0C(
    UnidentifiedFuzzyRuntimeBase*, const Variant& value)
{
    return value.mData.pointer;
}

float UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual10(float value)
{
    UnidentifiedRuntimeActionQueue* entry =
        lbl_80584328.Allocate();
    if (entry != 0)
    {
        entry->mQueue = mCollection.mHead->mQueue;
        entry->mConfidence = 0.0f;
        entry->mOwnsQueue = false;
        entry->mUnidentified008 = 0;
    }
    nlListAddStart(
        &mCollection.mHead, entry, &mCollection.mTail);
    return value;
}

float UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual11()
{
    UnidentifiedRuntimeActionQueue* entry =
        nlListRemoveStart(&mCollection.mHead, &mCollection.mTail);
    float confidence = entry->mConfidence;
    if (entry != 0)
    {
        if (entry->mOwnsQueue)
        {
            delete entry->mQueue;
        }
        lbl_80584328.DeleteEntry(entry);
    }

    mCollection.mHead->mConfidence =
        mCollection.mHead->mConfidence >= confidence
            ? mCollection.mHead->mConfidence
            : confidence;
    return confidence;
}

static inline float UnidentifiedGetExtraFloat(
    UnidentifiedVariant_80054AB8* action, int index,
    float defaultValue)
{
    if (action->ExtraData.IsSet(index))
    {
        return action->ExtraData.Get(index)->mData.f;
    }
    return defaultValue;
}

void UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual12(
    UnidentifiedVariant_80054AB8* action)
{
    UnidentifiedRuntimeActionQueue* entry = mCollection.mHead;
    UnidentifiedVariant_80054AB8* value =
        entry->mQueue->fn_80310040(action);
    if (value != 0)
    {
        float confidence = UnidentifiedGetExtraFloat(value, 4, 0.0f);
        entry->mConfidence =
            nlMaxEquals(entry->mConfidence, confidence);
    }
    mUnidentified058 = -1;
}

void UnidentifiedFuzzyRuntimeBase::UnidentifiedVirtual14(
    UnidentifiedVariant_80054AB8* action, int index,
    const Variant& value)
{
    FuzzyVariant variant(value);
    action->ExtraData.Set(index, variant);
}

extern "C" void fn_80313FA0(
    UnidentifiedFuzzyRuntimeBase* runtime, bool value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    FuzzyVariant variant(FT_BOOL, value);
    runtime->UnidentifiedVirtual14(
        action, fn_80312208(hash), variant);
}

extern "C" void fn_80314034(
    UnidentifiedFuzzyRuntimeBase* runtime, unsigned long hash,
    float value, UnidentifiedVariant_80054AB8* action)
{
    FuzzyVariant variant(FT_FLOAT, value);
    runtime->UnidentifiedVirtual14(
        action, fn_80312208(hash), variant);
}

extern "C" void fn_803140CC(
    UnidentifiedFuzzyRuntimeBase* runtime, int value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    FuzzyVariant variant(FT_INT, value);
    runtime->UnidentifiedVirtual14(
        action, fn_80312208(hash), variant);
}

extern "C" void fn_80314160(
    UnidentifiedFuzzyRuntimeBase* runtime, unsigned long value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    FuzzyVariant variant(FT_U32, value);
    runtime->UnidentifiedVirtual14(
        action, fn_80312208(hash), variant);
}

extern "C" void fn_803141F4(
    UnidentifiedFuzzyRuntimeBase* runtime, const Variant& value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    runtime->UnidentifiedVirtual14(
        action, fn_80312208(hash), value);
}

extern "C" void fn_8031423C(
    UnidentifiedFuzzyRuntimeBase* runtime, const Variant& value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    runtime->UnidentifiedVirtual14(
        action, fn_80312208(hash), value);
}

extern "C" float fn_80314428(
    UnidentifiedFuzzyRuntimeBase* runtime)
{
    return runtime->mCollection.mHead->mConfidence;
}

extern "C" void fn_80314434()
{
}

extern "C" void fn_80314438()
{
}

extern "C" void* fn_8031443C(void*, void* value)
{
    return value;
}

extern "C" void fn_80314444()
{
}

extern "C" float fn_80314448(
    float value, float minimum, float maximum)
{
    if (minimum == maximum)
    {
        return 1.0f;
    }
    float result = (value - minimum) / (maximum - minimum);
    result = result >= 0.0f ? result : 0.0f;
    return result <= 1.0f ? result : 1.0f;
}

extern "C" float fn_80314494(
    float value, float minimum, float maximum)
{
    value = value >= minimum ? value : minimum;
    return value <= maximum ? value : maximum;
}

extern "C" float fn_803144BC(
    float first, float second, float amount)
{
    return first + amount * (second - first);
}

extern "C" float fn_803144C8(
    float first, float second, float amount)
{
    amount = amount >= 0.0f ? amount : 0.0f;
    amount = amount <= 1.0f ? amount : 1.0f;
    return first + amount * (second - first);
}

extern "C" float fn_80314504(
    float first, float second, float minimum,
    float maximum, float value)
{
    float range = maximum - minimum;
    if (nlAbs(range) < 0.00001f)
    {
        return second;
    }
    return first + (value - minimum) / range * (second - first);
}

extern "C" float fn_80314538(
    float first, float second, float minimum,
    float maximum, float value)
{
    if (minimum < maximum)
    {
        value = value >= minimum ? value : minimum;
        value = value <= maximum ? value : maximum;
    }
    else
    {
        value = value >= maximum ? value : maximum;
        value = value <= minimum ? value : minimum;
    }

    maximum -= minimum;
    if (nlAbs(maximum) >= 0.00001f)
    {
        second = first
            + (value - minimum) / maximum * (second - first);
    }
    return second;
}

extern "C" bool fn_803145C8(
    UnidentifiedFuzzyRuntimeBase* runtime, bool concurrent)
{
    UnidentifiedFielderInput* value = runtime->mValue;
    unsigned long key = value->fn_8030F9B4(
        runtime->mUnidentified05C, concurrent);
    Timer* timer = value->fn_8030F9BC(key);
    return timer != 0 && timer->m_uPackedTime != 0;
}

extern "C" bool fn_8031462C(
    UnidentifiedFuzzyRuntimeBase* runtime, bool concurrent)
{
    UnidentifiedFielderInput* value = runtime->mValue;
    unsigned long key = value->fn_8030F9B4(
        runtime->mUnidentified05C, concurrent);
    Timer* timer = value->fn_8030F9BC(key);
    return timer != 0 && timer->m_unk0 != 0;
}

extern "C" float fn_80314690(
    UnidentifiedFuzzyRuntimeBase* runtime, bool concurrent)
{
    UnidentifiedFielderInput* value = runtime->mValue;
    unsigned long key = value->fn_8030F9B4(
        runtime->mUnidentified05C, concurrent);
    Timer* timer = value->fn_8030F9BC(key);
    return timer != 0 ? timer->GetSeconds() : 0.0f;
}

extern "C" float fn_803146E8(
    UnidentifiedFuzzyRuntimeBase* runtime, bool concurrent,
    float seconds)
{
    UnidentifiedFielderInput* value = runtime->mValue;
    unsigned long key = value->fn_8030F9B4(
        runtime->mUnidentified05C, concurrent);
    return value->fn_8030FA10(key, seconds)->GetSeconds();
}

extern "C" void fn_80314740()
{
}

extern "C" void fn_80314744(
    UnidentifiedFuzzyRuntimeBase* runtime, int selection)
{
    runtime->mCollection.mHead->mQueue->fn_8031002C(selection);
}

extern "C" void fn_80314750(
    void*, UnidentifiedTransitionReference* reference,
    const char* name)
{
    UnidentifiedStringHash transition(name);
    reference->mOwner->mTransition = transition;
}

extern "C" void fn_80314798()
{
    fn_8031A04C();
}

extern "C" UnidentifiedFuzzyRuntimeValue* fn_8031479C(
    void*, UnidentifiedFuzzyRuntimeBase* runtime)
{
    return runtime->mUnidentified064;
}

extern "C" int fn_803147A4(
    UnidentifiedFuzzyRuntimeBase* runtime)
{
    return runtime->mUnidentified064 != 0
        ? runtime->mUnidentified064->mType
        : -1;
}

extern "C" bool fn_803147C0(
    UnidentifiedFuzzyRuntimeBase* runtime, unsigned long hash)
{
    int index = fn_80312208(hash);
    return runtime->mUnidentified064 != 0
        && runtime->mUnidentified064->ExtraData.IsSet(index);
}

extern "C" Variant* fn_80314830(
    UnidentifiedFuzzyRuntimeBase* runtime, unsigned long hash)
{
    int index = fn_80312208(hash);
    if (runtime->mUnidentified064 != 0
        && runtime->mUnidentified064->ExtraData.IsSet(index))
    {
        return runtime->mUnidentified064->ExtraData.Get(index);
    }
    return 0;
}

extern "C" void fn_803148C4(float value)
{
    nlPrintf(lbl_806DF598, value);
}

extern "C" void fn_803148D0(void*, const char* value)
{
    nlPrintf(value);
}

extern "C" UnidentifiedVariant_80054AB8* fn_803152F0(
    UnidentifiedFuzzyRuntimeBase* runtime,
    const FuzzyVariant& value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8;
    *result = value;
    fn_800B6A1C(result, 4, FuzzyVariant(confidence));
    runtime->mUnidentified058 = fn_802DF9FC(runtime) + 1;
    return runtime->UnidentifiedReturn(result, confidence);
}

extern "C" UnidentifiedStateTransition* fn_80315A14(
    UnidentifiedStateTransition* transition, void* function)
{
    transition->mUnidentifiedHash = -1;
    transition->mUnidentifiedFunction = function;
    return transition;
}

UnidentifiedStringHash::UnidentifiedStringHash(const char* name)
{
    mUnidentifiedFunction = 0;
    mUnidentifiedHash = nlStringHash(name);
}

typedef FuzzyVariant (*UnidentifiedTransitionFunction)(
    UnidentifiedFuzzyRuntimeValue*, UnidentifiedFuzzyRuntimeValue*);

extern "C" void fn_80315A64(
    UnidentifiedStateTransition* transition,
    UnidentifiedFuzzyRuntimeValue* value,
    UnidentifiedVariant_80054AB8* result,
    UnidentifiedFuzzyRuntimeValue* context)
{
    if (transition->mUnidentifiedFunction != 0)
    {
        FuzzyVariant transitionValue =
            ((UnidentifiedTransitionFunction)
                transition->mUnidentifiedFunction)(value, context);
        *result = transitionValue;
    }
    else if (value != 0 && value->mRuntime != 0)
    {
        UnidentifiedVariant_80054AB8* transitionValue =
            fn_8031243C(
                value->mRuntime,
                transition->mUnidentifiedHash, context);
        if (transitionValue != 0)
        {
            *result = *transitionValue;
            if (transitionValue->mTemporary)
            {
                delete transitionValue;
            }
        }
    }
}
