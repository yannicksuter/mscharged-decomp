#include "Game/AI/TransitionFunc.h"

#include "Game/AI/AIContext.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "NL/nlString.h"

NativeTransitionFunc::NativeTransitionFunc(void* function)
{
    mValue.mFuncHash = -1;
    mValue.mNativeFunc = function;
}

ScriptTransitionFunc::ScriptTransitionFunc(const char* name)
{
    mValue.mNativeFunc = 0;
    mValue.mFuncHash = nlStringHash(name);
}

typedef DesireUpdate (*NativeTransitionFuncPtr)(
    AIContext*, FuzzyRuntimeContext*);

void TransitionFunc::Execute(AIContext* input,
    DesireUpdate* result, FuzzyRuntimeContext* context)
{
    if (mNativeFunc != 0)
    {
        DesireUpdate transitionValue =
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
