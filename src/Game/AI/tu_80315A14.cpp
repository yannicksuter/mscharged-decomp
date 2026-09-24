#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/UnidentifiedStringHash.h"
#include "NL/nlString.h"

extern "C" UnidentifiedVariant_80054AB8* fn_8031243C(
    UnidentifiedFuzzyRuntimeBase*, u32, UnidentifiedFuzzyRuntimeValue*);

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

typedef UnidentifiedVariant_80054AB8 (*UnidentifiedTransitionFunction)(
    UnidentifiedFuzzyRuntimeValue*, UnidentifiedFuzzyRuntimeValue*);

extern "C" void fn_80315A64(
    UnidentifiedStateTransition* transition,
    UnidentifiedFuzzyRuntimeValue* value,
    UnidentifiedVariant_80054AB8* result,
    UnidentifiedFuzzyRuntimeValue* context)
{
    if (transition->mUnidentifiedFunction != 0)
    {
        UnidentifiedVariant_80054AB8 transitionValue =
            ((UnidentifiedTransitionFunction)
                transition->mUnidentifiedFunction)(value, context);
        *result = transitionValue;
    }
    else if (value->mRuntime != 0)
    {
        UnidentifiedVariant_80054AB8* transitionValue =
            fn_8031243C(
                value->mRuntime,
                transition->mUnidentifiedHash, context);
        *result = *transitionValue;
        if (transitionValue->mTemporary)
        {
            delete transitionValue;
        }
    }
}
