#ifndef GAME_AI_FUZZYRUNTIMECALL_H
#define GAME_AI_FUZZYRUNTIMECALL_H

#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/InterpreterCore.h"
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
    // Marshal the player pointer into one interpreter stack word.
    return UnidentifiedVariant_80054AB8(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash), 1, FuzzyArgumentBits(player), 0));
}

extern "C" inline UnidentifiedVariant_80054AB8 fn_800C3448(
    InterpreterCore*, const unsigned int&, cPlayer*, cPlayer*);
extern "C" inline UnidentifiedVariant_80054AB8 fn_800C33D8(
    InterpreterCore*, cPlayer*, const char*, cPlayer*);

extern "C" inline UnidentifiedVariant_80054AB8 fn_800C33C8(
    InterpreterCore* pInterpreter, const char* pFunctionName,
    cPlayer* pPlayer, cPlayer* pTarget)
{
    return fn_800C33D8(
        pInterpreter, pPlayer, pFunctionName, pTarget);
}

extern "C" inline UnidentifiedVariant_80054AB8 fn_800C33D8(
    InterpreterCore* pInterpreter, cPlayer* pPlayer,
    const char* pFunctionName, cPlayer* pTarget)
{
    unsigned int functionHash = nlStringHash(pFunctionName);
    return fn_800C3448(
        pInterpreter, functionHash, pPlayer, pTarget);
}

extern "C" inline UnidentifiedVariant_80054AB8 fn_800C3448(
    InterpreterCore* pInterpreter, const unsigned int& functionHash,
    cPlayer* pPlayer, cPlayer* pTarget)
{
    unsigned int localHash = functionHash;
    FuzzyRuntimeBase* runtime = static_cast<FuzzyRuntimeBase*>(pInterpreter);
    return UnidentifiedVariant_80054AB8(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash),
        2, FuzzyArgumentBits(pPlayer), FuzzyArgumentBits(pTarget)));
}

#include "Game/AI/FuzzyRuntimeCall_fwd.h"

inline UnidentifiedVariant_80054AB8 CallFielderFuzzyFunction(
    InterpreterCore* runtime, const char* name, cFielder* fielder)
{
    return CallFielderFuzzyFunction(runtime, fielder, name);
}

inline UnidentifiedVariant_80054AB8 CallFielderFuzzyFunction(
    void* runtime, cFielder* fielder, const char* name)
{
    unsigned int functionHash = nlStringHash(name);
    return CallFielderFuzzyFunction(runtime, functionHash, fielder);
}

inline UnidentifiedVariant_80054AB8 CallFielderFuzzyFunction(
    void* runtime, const unsigned int& hash, cFielder* fielder)
{
    unsigned int functionHash = hash;
    FunctionEntryPoint* entry = ((InterpreterCore*)runtime)->FindFunctionEntryPoint(functionHash);
    return ExecuteFuzzyFunction((FuzzyRuntimeBase*)runtime, entry, 1, FuzzyArgumentBits(fielder), 0);
}

#endif
