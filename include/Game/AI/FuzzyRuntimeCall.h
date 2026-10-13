#ifndef GAME_AI_FUZZYRUNTIMECALL_H
#define GAME_AI_FUZZYRUNTIMECALL_H

#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/InterpreterCore.h"
#include "NL/nlString.h"

inline DesireUpdate CallPlayerFuzzyFunction(
    FuzzyRuntimeBase*, const unsigned int&, cPlayer*);
inline DesireUpdate CallPlayerFuzzyFunction(
    FuzzyRuntimeBase*, cPlayer*, const char*);

inline DesireUpdate CallPlayerFuzzyFunction(
    FuzzyRuntimeBase* runtime, const char* name, cPlayer* player)
{
    return CallPlayerFuzzyFunction(runtime, player, name);
}

inline DesireUpdate CallPlayerFuzzyFunction(
    FuzzyRuntimeBase* runtime, cPlayer* player, const char* name)
{
    unsigned int functionHash = nlStringHash(name);
    return CallPlayerFuzzyFunction(runtime, functionHash, player);
}

inline DesireUpdate CallPlayerFuzzyFunction(
    FuzzyRuntimeBase* runtime, const unsigned int& functionHash, cPlayer* player)
{
    unsigned int localHash = functionHash;
    // Marshal the player pointer into one interpreter stack word.
    return DesireUpdate(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash), 1, FuzzyArgumentBits(player), 0));
}

inline DesireUpdate CallPlayersFuzzyFunction(
    InterpreterCore*, const unsigned int&, cPlayer*, cPlayer*);
inline DesireUpdate CallPlayersFuzzyFunction(
    InterpreterCore*, cPlayer*, const char*, cPlayer*);

inline DesireUpdate CallPlayersFuzzyFunction(
    InterpreterCore* pInterpreter, const char* pFunctionName,
    cPlayer* pPlayer, cPlayer* pTarget)
{
    return CallPlayersFuzzyFunction(
        pInterpreter, pPlayer, pFunctionName, pTarget);
}

inline DesireUpdate CallPlayersFuzzyFunction(
    InterpreterCore* pInterpreter, cPlayer* pPlayer,
    const char* pFunctionName, cPlayer* pTarget)
{
    unsigned int functionHash = nlStringHash(pFunctionName);
    return CallPlayersFuzzyFunction(
        pInterpreter, functionHash, pPlayer, pTarget);
}

inline DesireUpdate CallPlayersFuzzyFunction(
    InterpreterCore* pInterpreter, const unsigned int& functionHash,
    cPlayer* pPlayer, cPlayer* pTarget)
{
    unsigned int localHash = functionHash;
    FuzzyRuntimeBase* runtime = static_cast<FuzzyRuntimeBase*>(pInterpreter);
    return DesireUpdate(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash),
        2, FuzzyArgumentBits(pPlayer), FuzzyArgumentBits(pTarget)));
}

#include "Game/AI/FuzzyRuntimeCall_fwd.h"

inline DesireUpdate CallFielderFuzzyFunction(
    InterpreterCore* runtime, const char* name, cFielder* fielder)
{
    return CallFielderFuzzyFunction(runtime, fielder, name);
}

inline DesireUpdate CallFielderFuzzyFunction(
    void* runtime, cFielder* fielder, const char* name)
{
    unsigned int functionHash = nlStringHash(name);
    return CallFielderFuzzyFunction(runtime, functionHash, fielder);
}

inline DesireUpdate CallFielderFuzzyFunction(
    void* runtime, const unsigned int& hash, cFielder* fielder)
{
    unsigned int functionHash = hash;
    FunctionEntryPoint* entry = ((InterpreterCore*)runtime)->FindFunctionEntryPoint(functionHash);
    return ExecuteFuzzyFunction((FuzzyRuntimeBase*)runtime, entry, 1, FuzzyArgumentBits(fielder), 0);
}

#endif
