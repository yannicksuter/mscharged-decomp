#include "Game/AI/Scripts/ScriptCaching.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/AIContext.h"

#include "Game/AI/Desire.h"
#include "Game/AI/TransitionFunc.h"
#include "Game/MathHelpers.h"
#include "Game/TweakConfig.h"
#include "NL/nlAVLTree.h"
#include "NL/nlFileGC.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlPrint.h"


struct FuzzyRuntimeList
{
    FuzzyRuntimeList(
        FuzzyRuntimeBase* head,
        FuzzyRuntimeBase* tail)
    {
        mTail = tail;
        mHead = head;
    }

    void AddEnd(FuzzyRuntimeBase* runtime)
    {
        nlListAddEnd(&mHead, &mTail, runtime);
    }

    FuzzyRuntimeBase* mHead;
    FuzzyRuntimeBase* mTail;
};

ScriptQuestionCache::ScriptQuestionCache()
    : mQuestionCacheMap(16, 16)
{
}

void* g_pFuzzyByteCode;
FuzzyRuntimeList g_FuzzyRuntimes(0, 0);
ScriptQuestionCache g_FuzzyQuestionCache;
FuzzyParameterList g_FuzzyParameters;
SlotPool<FuzzyActionQueueEntry> g_FuzzyActionQueuePool(16, 16);

FuzzyRuntimeBase* shdStateMachine::GetFuzzyRuntime()
{
    return mScriptMachine->mAIContext->mRuntime;
}

FuzzyRuntimeBase* UnidentifiedScriptMachine::GetFuzzyRuntime()
{
    return mAIContext->mRuntime;
}

FuzzyRuntimeBase* UnidentifiedFuzzyRuntimeValue::GetRuntime()
{
    return mRuntime;
}

FuzzyRuntimeBase::FuzzyRuntimeBase(
    AIContext* context)
    : InterpreterCore(0x100)
    , mActionQueues(0, 0)
    , mReturnValues(16, 16)
{
    mCaptureReturnValue = false;
    mFunctionHash = 0;
    mReturnInstructionOffset = -1;
    mAIContext = context;
    mCurrentContext = 0;
    next = 0;
    g_FuzzyRuntimes.AddEnd(this);

    if (mAIContext != 0)
    {
        mAIContext->mRuntime = this;
    }

    if (g_pFuzzyByteCode != 0)
    {
        LoadByteCode(g_pFuzzyByteCode);
    }
}

FuzzyRuntimeBase::~FuzzyRuntimeBase()
{
    nlListRemoveElement(
        &g_FuzzyRuntimes.mHead, this, &g_FuzzyRuntimes.mTail);

    if (g_FuzzyRuntimes.mHead == 0 && g_pFuzzyByteCode != 0)
    {
        delete[] (u8*)g_pFuzzyByteCode;
        g_pFuzzyByteCode = 0;

        g_FuzzyQuestionCache.FreeBlocks();
        lbl_80584200.FreeBlocks();
        lbl_80584228.FreeBlocks();
        g_FuzzyActionQueuePool.FreeBlocks();
        lbl_805842C8.FreeBlocks();

        nlDeleteList(&g_FuzzyParameters.mHead, &g_FuzzyParameters.mTail);
    }

    while (mActionQueues.mHead != 0)
    {
        FuzzyActionQueueEntry* entry =
            nlListRemoveStart(
                &mActionQueues.mHead, &mActionQueues.mTail);
        if (entry != 0)
        {
            if (entry->mOwnsQueue)
            {
                delete entry->mQueue;
            }
            g_FuzzyActionQueuePool.DeleteEntry(entry);
        }
    }
}

extern "C" void LoadFuzzyByteCode(const char* filename, bool async)
{
    bool reload = false;
    if (g_pFuzzyByteCode != 0)
    {
        FuzzyRuntimeBase* runtime = g_FuzzyRuntimes.mHead;
        while (runtime != 0)
        {
            if (!runtime->IsFinished())
            {
                runtime->StopWithoutUndo();
            }
            runtime = runtime->next;
        }

        delete[] (u8*)g_pFuzzyByteCode;
        g_pFuzzyByteCode = 0;
        reload = true;
    }

    if (g_pFuzzyByteCode == 0)
    {
        if (async)
        {
            nlLoadEntireFileAsync(
                filename, (LoadAsyncCallback)FuzzyByteCodeLoaded,
                0, 0x20, AllocateStart, 0, 0, 0);
        }
        else
        {
            unsigned long size = 0;
            if (reload && g_bSupportReloading)
            {
                g_pFuzzyByteCode = nlLoadEntireHostFile(
                    filename, &size, 0x20, AllocateStart, 0, 0);
            }
            else
            {
                g_pFuzzyByteCode = nlLoadEntireFile(
                    filename, &size, 0x20, AllocateStart, 0, 0, 0);
            }
            if (g_pFuzzyByteCode != 0)
            {
                FuzzyRuntimeBase* runtime =
                    g_FuzzyRuntimes.mHead;
                while (runtime != 0)
                {
                    runtime->LoadByteCode(g_pFuzzyByteCode);
                    runtime = runtime->next;
                }
            }
        }
    }
}

extern "C" void FuzzyByteCodeLoaded(
    void* byteCode, unsigned long, void*)
{
    g_pFuzzyByteCode = byteCode;
}

extern "C" bool ApplyFuzzyByteCode()
{
    if (g_pFuzzyByteCode != 0)
    {
        FuzzyRuntimeBase* runtime = g_FuzzyRuntimes.mHead;
        while (runtime != 0)
        {
            runtime->LoadByteCode(g_pFuzzyByteCode);
            runtime = runtime->next;
        }
        return true;
    }
    return false;
}

void FuzzyRuntimeBase::RegisterParameters()
{
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Undefined", -1));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Arg1", 0));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Arg2", 1));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Arg3", 2));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Arg4", 3));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Confidence", 4));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("ConfThreshold", 5));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("SelectChance", 6));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Duration", 7));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("StateID", 8));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Concurrent", 9));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Transition", 10));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("Modifier", 11));
    g_FuzzyParameters.AddEnd(new (nlMalloc(
        sizeof(FuzzyParameterEntry), 8, false))
            FuzzyParameterEntry("AllowReinit", 12));
}

extern "C" int FuzzyFindParameterIndex(unsigned long hash)
{
    FuzzyParameterEntry* entry = g_FuzzyParameters.mHead;
    while (entry != 0)
    {
        if (hash == entry->mHash)
        {
            return entry->mIndex;
        }
        entry = entry->next;
    }
    return -1;
}

bool FuzzyRuntimeBase::ExecuteFunction(
    FunctionEntryPoint* function, unsigned int argumentCount,
    u32 arg1, u32 arg2, u32 arg3, u32 arg4)
{
    ListEntry<UnidentifiedVariant_80054AB8*>* entry;
    UnidentifiedVariant_80054AB8* value;

    mFunctionHash = function->hash;
    bool result = InterpreterCore::ExecuteFunction(
        function, argumentCount, arg1, arg2, arg3, arg4);

    UnidentifiedVariant_80054AB8* returnValue =
        mCaptureReturnValue
            ? *(UnidentifiedVariant_80054AB8**)m_SP
            : 0;

    for (entry = mReturnValues.m_Head; entry != 0;
         entry = entry->next)
    {
        value = entry->entry;
        if (returnValue != value)
        {
            delete value;
        }
    }
    mReturnValues.Clear();
    mFunctionHash = 0;
    return result;
}

extern "C" char FuzzyPassThrough_80312358(void*, char value)
{
    return value;
}

extern "C" UnidentifiedVariant_80054AB8* ExecuteFuzzyFunction(
    FuzzyRuntimeBase* runtime,
    FunctionEntryPoint* function, int argumentCount,
    u32 arg1, u32 arg2)
{
    runtime->mCaptureReturnValue = true;
    switch (argumentCount)
    {
    case 0:
        runtime->ExecuteFunction(
            function, 0, 0, 0, 0, 0);
        break;
    case 1:
        runtime->ExecuteFunction(
            function, 1, arg1, 0, 0, 0);
        break;
    case 2:
        runtime->ExecuteFunction(
            function, 2, arg1, arg2, 0, 0);
        break;
    }

    UnidentifiedVariant_80054AB8* result =
        *(UnidentifiedVariant_80054AB8**)runtime->m_SP;
    if (result != 0)
    {
        result->mTemporary = true;
    }
    runtime->mCaptureReturnValue = false;
    return result;
}

UnidentifiedVariant_80054AB8* ExecuteScriptFunction(
    FuzzyRuntimeBase* runtime, u32 hash,
    UnidentifiedFuzzyRuntimeValue* action)
{
    if (runtime->mAIContext->IsPointerType())
    {
        runtime->mCurrentContext = action;
        void* value = runtime->mAIContext->mData.pointer;
        u32 localHash = hash;
        FunctionEntryPoint* function = runtime->FindFunctionEntryPoint(localHash);
        runtime->mCaptureReturnValue = true;
        runtime->ExecuteFunction(
            function, 1, (u32)value, 0, 0, 0);

        UnidentifiedVariant_80054AB8* result =
            *(UnidentifiedVariant_80054AB8**)runtime->m_SP;
        if (result != 0)
        {
            result->mTemporary = true;
        }
        runtime->mCaptureReturnValue = false;
        runtime->mCurrentContext = 0;
        return result;
    }
    return 0;
}

float FuzzyRuntimeBase::FuzzyEqual(
    float first, float second)
{
    float result = 1.0f - nlAbs(first - second);
    return result >= 0.0f ? result : 0.0f;
}

float FuzzyRuntimeBase::FLESS(
    float f1, float f2)
{
    float fScore = 0.0f;
    float fDelta = f2 - f1;
    if (fDelta > 0.0f)
    {
        float divisor = 1.0f - f1;
        divisor = divisor >= f2 ? divisor : f2;
        divisor = divisor <= 0.5f ? divisor : 0.5f;
        fScore = fDelta / divisor;
        fScore = fScore >= 0.0f ? fScore : 0.0f;
        fScore = fScore <= 1.0f ? fScore : 1.0f;
    }
    return fScore;
}

float FuzzyRuntimeBase::FuzzyNot(float value)
{
    return 1.0f - value;
}

float FuzzyRuntimeBase::UnidentifiedVirtual6(float value)
{
    float inverse = 1.0f - value;
    float minimum = value <= inverse ? value : inverse;
    float maximum = value >= inverse ? value : inverse;
    return minimum / maximum;
}

float FuzzyRuntimeBase::UnidentifiedVirtual7(
    float first, float second, float third, bool)
{
    third = third <= first ? third : first;
    if (third < first && first < 0.5f)
    {
        third *= second;
    }
    return third;
}

float FuzzyRuntimeBase::BeginActionQueue()
{
    UnidentifiedActionQueue* queue =
        new (lbl_80584228.Allocate()) UnidentifiedActionQueue;
    FuzzyActionQueueEntry* entry =
        g_FuzzyActionQueuePool.Allocate();
    if (entry != 0)
    {
        entry->mQueue = queue;
        entry->mConfidence = 0.0f;
        entry->mOwnsQueue = true;
        entry->mQuestionHash = 0;
    }
    nlListAddStart(
        &mActionQueues.mHead, entry, &mActionQueues.mTail);
    return 1.0f;
}

UnidentifiedVariant_80054AB8*
FuzzyRuntimeBase::EndActionQueue()
{
    FuzzyActionQueueEntry* entry =
        nlListRemoveStart(&mActionQueues.mHead, &mActionQueues.mTail);
    UnidentifiedActionQueue* queue = entry->mQueue;
    UnidentifiedVariant_80054AB8* selected = queue->SelectAction();
    if (queue->m_pSelectedAction == 0)
    {
        selected = new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(lbl_80584250);
    }
    else if (entry->mQuestionHash != 0)
    {
        unsigned long hash = entry->mQuestionHash;
        if (g_bScriptQuestionCachingOn)
        {
            g_FuzzyQuestionCache.mQuestionCacheMap.Add(hash, *selected);
        }
    }
    mReturnValues.AddEnd(selected);
    queue->fn_8030FF6C(true);
    if (entry != 0)
    {
        if (entry->mOwnsQueue)
        {
            delete entry->mQueue;
        }
        g_FuzzyActionQueuePool.DeleteEntry(entry);
    }
    return selected;
}

static inline float GetActionFloatParameter(
    UnidentifiedVariant_80054AB8* action, int index,
    float defaultValue)
{
    if (action->ExtraData.IsSet(index))
    {
        return action->ExtraData.Get(index)->mData.f;
    }
    return defaultValue;
}

static inline unsigned long StrategicQuestionHash(
    unsigned long functionAddress, const Variant& argument)
{
    return functionAddress + argument.GetHash();
}

extern "C" bool FuzzyTryCachedQuestion(
    FuzzyRuntimeBase* runtime, const Variant& value)
{
    UnidentifiedVariant_80054AB8 action;
    unsigned long hash = StrategicQuestionHash(
        runtime->GetInstructionOffset(), value);

    if (g_FuzzyQuestionCache.Lookup(hash, action, 0))
    {
        UnidentifiedVariant_80054AB8* result = FuzzyReturnVariantCopy(
            runtime, action,
            GetActionFloatParameter(&action, 4, 0.0f));
        runtime->AddAction(result);
        return true;
    }

    runtime->mActionQueues.mHead->mQuestionHash = hash;
    return false;
}

float FuzzyRuntimeBase::BeginConfidenceScope(float value)
{
    FuzzyActionQueueEntry* entry =
        g_FuzzyActionQueuePool.Allocate();
    if (entry != 0)
    {
        entry->mQueue = mActionQueues.mHead->mQueue;
        entry->mConfidence = 0.0f;
        entry->mOwnsQueue = false;
        entry->mQuestionHash = 0;
    }
    nlListAddStart(
        &mActionQueues.mHead, entry, &mActionQueues.mTail);
    return value;
}

float FuzzyRuntimeBase::EndConfidenceScope()
{
    FuzzyActionQueueEntry* entry =
        nlListRemoveStart(&mActionQueues.mHead, &mActionQueues.mTail);
    float confidence = entry->mConfidence;
    if (entry != 0)
    {
        if (entry->mOwnsQueue)
        {
            delete entry->mQueue;
        }
        g_FuzzyActionQueuePool.DeleteEntry(entry);
    }

    mActionQueues.mHead->mConfidence =
        mActionQueues.mHead->mConfidence >= confidence
            ? mActionQueues.mHead->mConfidence
            : confidence;
    return confidence;
}

void FuzzyRuntimeBase::AddAction(
    UnidentifiedVariant_80054AB8* action)
{
    FuzzyActionQueueEntry* entry = mActionQueues.mHead;
    UnidentifiedVariant_80054AB8* value =
        entry->mQueue->fn_80310040(action);
    if (value != 0)
    {
        float confidence;
        if (value->fn_800C2C10(4))
        {
            confidence = value->fn_800C2C00(4)->mData.f;
        }
        else
        {
            confidence = 0.0f;
        }
        entry->mConfidence = nlMaxEquals(entry->mConfidence, confidence);
    }
    mReturnInstructionOffset = -1;
}

void FuzzyRuntimeBase::SetActionParameter(
    UnidentifiedVariant_80054AB8* action, int index,
    Variant& value)
{
    action->ExtraData.Set(index, FuzzyVariant(value));
}

extern "C" void FuzzySetBoolParameter(
    FuzzyRuntimeBase* runtime, bool value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    int index = FuzzyFindParameterIndex(hash);
    FuzzyVariant variant(FT_BOOL, value);
    runtime->SetActionParameter(
        action, index, variant);
}

extern "C" void FuzzySetFloatParameter(
    FuzzyRuntimeBase* runtime, float value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    int index = FuzzyFindParameterIndex(hash);
    FuzzyVariant variant(FT_FLOAT, value);
    runtime->SetActionParameter(
        action, index, variant);
}

extern "C" void FuzzySetIntParameter(
    FuzzyRuntimeBase* runtime, int value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    int index = FuzzyFindParameterIndex(hash);
    FuzzyVariant variant(FT_INT, value);
    runtime->SetActionParameter(
        action, index, variant);
}

extern "C" void FuzzySetU32Parameter(
    FuzzyRuntimeBase* runtime, unsigned long value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    int index = FuzzyFindParameterIndex(hash);
    FuzzyVariant variant(FT_U32, value);
    runtime->SetActionParameter(
        action, index, variant);
}

extern "C" void FuzzySetVariantParameter(
    FuzzyRuntimeBase* runtime, Variant& value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    runtime->SetActionParameter(
        action, FuzzyFindParameterIndex(hash), value);
}

extern "C" void FuzzySetStringParameter(
    FuzzyRuntimeBase* runtime, const char* value,
    unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    int index = FuzzyFindParameterIndex(hash);
    if (index == 10)
    {
        unsigned long hashed = 0;
        if (nlStrLen(value) != 0)
        {
            hashed = nlStringHash(value);
        }
        FuzzyVariant variant(FT_U32, hashed);
        runtime->SetActionParameter(action, index, variant);
    }
    else
    {
        FuzzyVariant variant(value);
        runtime->SetActionParameter(action, index, variant);
    }
}

extern "C" float FuzzyGetQueueConfidence(
    FuzzyRuntimeBase* runtime)
{
    return runtime->mActionQueues.mHead->mConfidence;
}

extern "C" void FuzzyNoOp_80314434(void*, UnidentifiedVariant_80054AB8*, float)
{
}

extern "C" void FuzzyNoOp_80314438(void*, UnidentifiedVariant_80054AB8*)
{
}

extern "C" void* FuzzyPassThrough_8031443C(void*, void* value, bool)
{
    return value;
}

extern "C" float FuzzyPassThrough_80314444(void*, float value, bool)
{
    return value;
}

extern "C" float FuzzyNormalize(
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

extern "C" float FuzzyClamp(
    float value, float minimum, float maximum)
{
    value = value >= minimum ? value : minimum;
    return value <= maximum ? value : maximum;
}

extern "C" float FuzzyInterpolate(
    float first, float second, float amount)
{
    return first + amount * (second - first);
}

extern "C" float FuzzyInterpolateClamped(
    float first, float second, float amount)
{
    amount = amount >= 0.0f ? amount : 0.0f;
    amount = amount <= 1.0f ? amount : 1.0f;
    return first + amount * (second - first);
}

extern "C" float FuzzyInterpolateRange(
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

extern "C" float FuzzyInterpolateRangeClamped(
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
    second = nlAbs(maximum) < 0.00001f ? second
        : first + (value - minimum) / maximum * (second - first);
    return second;
}

extern "C" bool FuzzyIsTimerRunning(
    FuzzyRuntimeBase* runtime, unsigned long concurrent)
{
    AIContext* context = runtime->mAIContext;
    unsigned long key = context->GetTimerKey(
        runtime->mFunctionHash, concurrent);
    Timer* timer = context->FindTimer(key);
    return timer != 0 && timer->m_uPackedTime != 0;
}

extern "C" bool FuzzyWasTimerRunning(
    FuzzyRuntimeBase* runtime, unsigned long concurrent)
{
    AIContext* context = runtime->mAIContext;
    unsigned long key = context->GetTimerKey(
        runtime->mFunctionHash, concurrent);
    Timer* timer = context->FindTimer(key);
    return timer != 0 && timer->m_uWasRunning != 0;
}

extern "C" float FuzzyGetTimerSeconds(
    FuzzyRuntimeBase* runtime, unsigned long concurrent)
{
    AIContext* context = runtime->mAIContext;
    unsigned long key = context->GetTimerKey(
        runtime->mFunctionHash, concurrent);
    Timer* timer = context->FindTimer(key);
    return timer != 0 ? timer->GetSeconds() : 0.0f;
}

extern "C" float FuzzySetTimerSeconds(
    FuzzyRuntimeBase* runtime, unsigned long concurrent,
    float seconds)
{
    AIContext* context = runtime->mAIContext;
    unsigned long key = context->GetTimerKey(
        runtime->mFunctionHash, concurrent);
    return context->SetTimer(key, seconds)->GetSeconds();
}

extern "C" void FuzzyNoOp_80314740(void*, bool)
{
}

extern "C" void FuzzySetActionSelection(
    FuzzyRuntimeBase* runtime, int selection)
{
    runtime->mActionQueues.mHead->mQueue->fn_8031002C(selection);
}

extern "C" void FuzzySetTransition(
    void*, shdStateMachine* state, const char* name)
{
    UnidentifiedScriptMachine* machine = state->mScriptMachine;
    machine->SetTransition(name);
}

extern "C" bool fn_80314798(void*)
{
    return fn_8031A04C();
}

extern "C" UnidentifiedFuzzyRuntimeValue* FuzzyGetCurrentContext(
    void*, FuzzyRuntimeBase* runtime)
{
    return runtime->mCurrentContext;
}

extern "C" int FuzzyGetCurrentContextType(
    FuzzyRuntimeBase* runtime)
{
    return runtime->mCurrentContext != 0
        ? runtime->mCurrentContext->mType
        : -1;
}

extern "C" bool FuzzyHasContextParameter(
    FuzzyRuntimeBase* runtime, unsigned long hash)
{
    int index = FuzzyFindParameterIndex(hash);
    if (runtime->mCurrentContext != 0
        && runtime->mCurrentContext->ExtraData.IsSet(index))
    {
        return true;
    }
    return false;
}

extern "C" Variant* FuzzyGetContextParameter(
    FuzzyRuntimeBase* runtime, unsigned long hash)
{
    int index = FuzzyFindParameterIndex(hash);
    if (runtime->mCurrentContext != 0
        && runtime->mCurrentContext->ExtraData.IsSet(index))
    {
        return runtime->mCurrentContext->ExtraData.Get(index);
    }
    return 0;
}

extern "C" void FuzzyPrintFloat(float value)
{
    nlPrintf("%f", value);
}

extern "C" void FuzzyPrintString(void*, const char* value)
{
    nlPrintf(value);
}
