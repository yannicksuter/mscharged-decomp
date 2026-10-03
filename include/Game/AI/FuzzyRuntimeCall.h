#ifndef GAME_AI_FUZZYRUNTIMECALL_H
#define GAME_AI_FUZZYRUNTIMECALL_H

#include "Game/AI/FuzzyAIRuntime.h"
#include "NL/nlString.h"

extern "C" inline UnidentifiedVariant_80054AB8 fn_800996D0(
    FuzzyRuntimeBase*, const unsigned int&, cPlayer*);
extern "C" inline UnidentifiedVariant_80054AB8 fn_80099670(
    FuzzyRuntimeBase*, cPlayer*, const char*);

extern "C" inline UnidentifiedVariant_80054AB8 fn_80099660(
    FuzzyRuntimeBase* runtime, const char* name, cPlayer* player)
{
    return fn_80099670(runtime, player, name);
}

extern "C" inline UnidentifiedVariant_80054AB8 fn_80099670(
    FuzzyRuntimeBase* runtime, cPlayer* player, const char* name)
{
    unsigned int functionHash = nlStringHash(name);
    return fn_800996D0(runtime, functionHash, player);
}

extern "C" inline UnidentifiedVariant_80054AB8 fn_800996D0(
    FuzzyRuntimeBase* runtime, const unsigned int& functionHash, cPlayer* player)
{
    unsigned int localHash = functionHash;
    // Pass the pointer through the untyped argument marshaller by reference.
    return UnidentifiedVariant_80054AB8(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash), 1, *(void* const*)&player, NULL));
}

#endif
