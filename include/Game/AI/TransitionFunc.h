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

struct UnsetTransitionFunc : public TransitionFunc
{
    UnsetTransitionFunc()
    {
        mNativeFunc = 0;
        mFuncHash = (u32)-1;
    }
};

// The shared unset value; assigned to clear a transition.
extern UnsetTransitionFunc g_UnsetTransitionFunc;

// A transition bound to a script function by the hash of its name; the hash
// is resolved through the fuzzy runtime's function table when it executes.
struct ScriptTransitionFunc : public TransitionFunc
{
    ScriptTransitionFunc(const char* name);
};

// A transition bound to a compiled function that takes the AI context and
// returns the desire update.
struct NativeTransitionFunc : public TransitionFunc
{
    NativeTransitionFunc(void* function);
};

#endif // GAME_AI_TRANSITION_FUNC_H
