#ifndef GAME_AI_TRANSITION_FUNC_H
#define GAME_AI_TRANSITION_FUNC_H

#include "Game/AI/DesireUpdate.h"
#include "types.h"

class AIContext;
class UnidentifiedFuzzyRuntimeValue;

// A state machine's transition function: either a compiled function
// (mNativeFunc) or a script function named by the hash of its name
// (mFuncHash). Both are executed with the AI context and return the desire
// update. "Unset" is no function and hash -1.
struct TransitionFunc
{
    bool IsUnset() const
    {
        return mNativeFunc == 0 && mFuncHash == (u32)-1;
    }

    // Runs the bound function, or the script function named by the hash
    // through the input's fuzzy runtime, and stores the desire update it
    // returns.
    void Execute(AIContext* input, UnidentifiedVariant_80054AB8* result,
        UnidentifiedFuzzyRuntimeValue* context);

    u32 mFuncHash;
    void* mNativeFunc;
};

// Owns a transition record, initialized to the unset value.
struct UnsetTransitionFunc
{
    UnsetTransitionFunc()
    {
        mValue.mNativeFunc = 0;
        mValue.mFuncHash = (u32)-1;
    }

    operator TransitionFunc&() { return mValue; }
    operator const TransitionFunc&() const { return mValue; }

    UnsetTransitionFunc& operator=(UnsetTransitionFunc& other)
    {
        mValue = other.mValue;
        return *this;
    }

    UnsetTransitionFunc& operator=(const TransitionFunc& transition)
    {
        mValue = transition;
        return *this;
    }

    bool IsUnset() const { return mValue.IsUnset(); }
    void Execute(AIContext* input, UnidentifiedVariant_80054AB8* result,
        UnidentifiedFuzzyRuntimeValue* context)
    {
        mValue.Execute(input, result, context);
    }

    TransitionFunc mValue;
};

extern UnsetTransitionFunc g_UnsetTransitionFunc;

// Constructs a script binding and exposes its record for state construction.
struct ScriptTransitionFunc
{
    ScriptTransitionFunc(const char* name);
    operator TransitionFunc&() { return mValue; }
    operator const TransitionFunc&() const { return mValue; }
    TransitionFunc mValue;
};

// Constructs a native binding and exposes its record for state construction.
struct NativeTransitionFunc
{
    NativeTransitionFunc(void* function);
    operator TransitionFunc&() { return mValue; }
    operator const TransitionFunc&() const { return mValue; }
    TransitionFunc mValue;
};

#endif // GAME_AI_TRANSITION_FUNC_H
