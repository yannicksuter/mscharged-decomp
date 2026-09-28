#include "Game/AI/TransitionFunc.h"

#include "Game/AI/AIContext.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "NL/nlString.h"

TransitionFunc* BindNativeTransitionFunc(
    TransitionFunc* transition, void* function)
{
    transition->mFuncHash = -1;
    transition->mNativeFunc = function;
    return transition;
}

ScriptTransitionFunc::ScriptTransitionFunc(const char* name)
{
    mNativeFunc = 0;
    mFuncHash = nlStringHash(name);
}

typedef UnidentifiedVariant_80054AB8 (*NativeTransitionFuncPtr)(
    AIContext*, UnidentifiedFuzzyRuntimeValue*);

void TransitionFunc::Execute(AIContext* input,
    UnidentifiedVariant_80054AB8* result, UnidentifiedFuzzyRuntimeValue* context)
{
    if (mNativeFunc != 0)
    {
        UnidentifiedVariant_80054AB8 transitionValue =
            ((NativeTransitionFuncPtr)mNativeFunc)(
                input, context);
        *result = transitionValue;
    }
    else if (input->mRuntime != 0)
    {
        *result = ExecuteScriptFunction(
            input->mRuntime, mFuncHash, context);
    }
}
