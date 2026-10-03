#ifndef GAME_AI_FUZZY_AI_RUNTIME_H
#define GAME_AI_FUZZY_AI_RUNTIME_H

#include "Game/AI/DesireUpdate.h"
#include "Game/InterpreterCore.h"
#include "NL/nlList.h"
#include "NL/nlString.h"

class FuzzyRuntimeBase;
class ScriptQuestionCache;
struct FuzzyFielderIterator;
struct FuzzyFielderReference;
class shdStateMachine;

struct FuzzyActionQueueEntry
{
    UnidentifiedActionQueue* mQueue;
    bool mOwnsQueue;
    u8 mPadding005[3];
    int mQuestionHash;
    float mConfidence;
    FuzzyActionQueueEntry* next;
};

struct FuzzyActionQueueList
{
    FuzzyActionQueueList(
        FuzzyActionQueueEntry* head,
        FuzzyActionQueueEntry* tail)
    {
        mTail = tail;
        mHead = head;
    }

    FuzzyActionQueueEntry* mHead;
    FuzzyActionQueueEntry* mTail;
};

struct FuzzyParameterEntry
{
    FuzzyParameterEntry(const char* name, int index)
        : mIndex(index)
        , mHash(nlStringLowerHash(name))
    {
    }

    int mIndex;
    unsigned long mHash;
    FuzzyParameterEntry* next;
};

struct FuzzyParameterList
{
    FuzzyParameterList()
    {
        mTail = 0;
        mHead = 0;
    }

    void AddEnd(FuzzyParameterEntry* entry)
    {
        nlListAddEnd(&mHead, &mTail, entry);
    }

    FuzzyParameterEntry* mHead;
    FuzzyParameterEntry* mTail;
};

class UnidentifiedFuzzyRuntimeValue : public FuzzyVariant
{
public:
    FuzzyRuntimeBase* GetRuntime();

    FuzzyRuntimeBase* mRuntime;
    u32 mUnidentified018;
    UnidentifiedVariantCollection ExtraData;
};

class FuzzyRuntimeBase : public InterpreterCore
{
public:
    FuzzyRuntimeBase(AIContext*);
    virtual ~FuzzyRuntimeBase();
    virtual void DoFunctionCall(unsigned int) = 0;
    virtual bool ExecuteFunction(
        FunctionEntryPoint*, unsigned int, u32, u32, u32, u32);
    virtual float FuzzyEqual(float, float);
    virtual float FLESS(float, float);
    virtual float FuzzyNot(float);
    virtual float UnidentifiedVirtual6(float);
    virtual float UnidentifiedVirtual7(float, float, float, bool);
    virtual float BeginActionQueue();
    virtual UnidentifiedVariant_80054AB8* EndActionQueue();
    virtual float BeginConfidenceScope(float);
    virtual float EndConfidenceScope();
    virtual void AddAction(UnidentifiedVariant_80054AB8*);
    virtual UnidentifiedVariant_80054AB8* ReturnValue(
        UnidentifiedVariant_80054AB8*, float);
    virtual void SetActionParameter(
        UnidentifiedVariant_80054AB8*, int, Variant&);
    virtual void RegisterParameters();

    // Build a native-call result and record its confidence and instruction offset.
    template <typename T>
    UnidentifiedVariant_80054AB8* CreateReturnValue(
        eVariantType type, T value, float confidence);
    UnidentifiedVariant_80054AB8* CreateReturnValue(
        UnidentifiedVariant_80054AB8* value, float confidence);

    FuzzyRuntimeBase* next;
    AIContext* mAIContext;
    FuzzyActionQueueList mActionQueues;
    nlListSlotPool<UnidentifiedVariant_80054AB8*> mReturnValues;
    int mReturnInstructionOffset;
    unsigned long mFunctionHash;
    bool mCaptureReturnValue;
    u8 mPadding061[3];
    UnidentifiedFuzzyRuntimeValue* mCurrentContext;
};

class FuzzyAIRuntime : public FuzzyRuntimeBase
{
public:
    FuzzyAIRuntime();
    float GetSkillValue(unsigned long hash);
    virtual ~FuzzyAIRuntime();
    virtual void DoFunctionCall(unsigned int);
    virtual float BeginActionQueue();
    virtual UnidentifiedVariant_80054AB8* EndActionQueue();
    virtual void AddAction(UnidentifiedVariant_80054AB8*);
    virtual void RegisterParameters();
};

extern FuzzyParameterList g_FuzzyParameters;
extern ScriptQuestionCache g_FuzzyQuestionCache;
extern char g_FuzzyAIScriptFilename[];
extern char* g_pFuzzyAIScriptFilename;

extern "C" void FuzzyByteCodeLoaded(void* byteCode, unsigned long, void*);
extern "C" UnidentifiedVariant_80054AB8* FuzzyReturnVariantCopy(
    FuzzyRuntimeBase* runtime, UnidentifiedVariant_80054AB8 value, float confidence);
extern "C" bool FuzzyTryCachedQuestion(FuzzyRuntimeBase* runtime, const Variant& value);
extern "C" void FuzzySetBoolParameter(FuzzyRuntimeBase*, bool, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzySetStringParameter(FuzzyRuntimeBase*, const char*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzySetFloatParameter(FuzzyRuntimeBase*, float, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzySetIntParameter(FuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzySetVariantParameter(FuzzyRuntimeBase*, Variant&, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzySetU32Parameter(FuzzyRuntimeBase*, unsigned long, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" bool FuzzyHasContextParameter(FuzzyRuntimeBase*, unsigned long);
extern "C" Variant* FuzzyGetContextParameter(FuzzyRuntimeBase*, unsigned long);
extern "C" bool FuzzyIsTimerRunning(FuzzyRuntimeBase*, unsigned long);
extern "C" bool FuzzyWasTimerRunning(FuzzyRuntimeBase*, unsigned long);
extern "C" float FuzzyGetTimerSeconds(FuzzyRuntimeBase*, unsigned long);
extern "C" float FuzzySetTimerSeconds(FuzzyRuntimeBase*, unsigned long, float);

extern "C" int FuzzyFindParameterIndex(unsigned long hash);
// Executes the script function named by hash through the runtime for its
// current value and returns the result variant it leaves on the stack.
UnidentifiedVariant_80054AB8* ExecuteScriptFunction(
    FuzzyRuntimeBase* runtime, u32 hash,
    UnidentifiedFuzzyRuntimeValue* action);
extern "C" FuzzyRuntimeBase* FuzzyAIGetFielderRuntime(cFielder*);


// Shared functions and data from Game/AI/Scripts/FuzzyAIRuntime.cpp.
extern "C" FuzzyRuntimeBase* FuzzyAIGetTeamRuntime(cTeam*);
extern "C" const char* GetFuzzyAIScriptFilename();
extern "C" cPlayer* FuzzyAIGetBallOwner();
extern "C" cBall* FuzzyAIGetBall();
extern "C" void* FuzzyAIGetGame();
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E35D4(FuzzyAIRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E3700(FuzzyAIRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E382C(FuzzyAIRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E3958(FuzzyAIRuntime*, int, float);
extern "C" void FuzzyAISetPlayerParameter_800E3A84(void*, cPlayer*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzyAISetBallParameter(void*, cBall*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void* fn_800E3BE4(void*, void*);
extern "C" void* fn_800E3BEC(void*, void*);
extern "C" void* fn_800E3BF4(void*, void*);
extern "C" void* fn_800E3BFC(void*, void*);
extern "C" void* fn_800E3C04(void*, void*);
extern "C" void* fn_800E3C0C(void*, void*);
extern "C" void* fn_800E3C14(void*, Variant*);
extern "C" void* fn_800E3C2C(void*, Variant*);
extern "C" void* fn_800E3C44(void*, Variant*);
extern "C" void* fn_800E3C5C(void*, Variant*);
extern "C" FuzzyFielderIterator* FuzzyAICreateTeamFielderIterator(void*, cTeam*);
extern "C" FuzzyFielderIterator* FuzzyAICreateOpponentFielderIterator(void*, cFielder*);
extern "C" FuzzyFielderIterator* FuzzyAICreateTeammateIterator(void*, cFielder*);
extern "C" FuzzyFielderIterator* FuzzyAIAdvanceFielderIterator(void*, FuzzyFielderIterator*);
extern "C" bool FuzzyAIHasNextFielder(void*, FuzzyFielderIterator*);
extern "C" void FuzzyAIDestroyFielderIterator(void*, FuzzyFielderIterator*);
extern "C" cFielder* FuzzyAIGetIteratorFielder_800E3F10(void*, FuzzyFielderIterator*);
extern "C" cFielder* FuzzyAIGetIteratorFielder_800E3F1C(void*, FuzzyFielderIterator*);
extern "C" AIContext* FuzzyAIGetIteratorAIContext(void*, FuzzyFielderIterator*);
extern "C" unsigned long FuzzyAIStringHash(const char*);
extern "C" float fn_800E3FE0();
extern "C" float fn_800E3FE4();
extern "C" float fn_800E3FE8();
extern "C" float fn_800E3FEC();
extern "C" bool FuzzyAIIsUndoingCall(InterpreterCore*);
extern "C" float fn_800E7ECC(void*, Variant*);
extern "C" unsigned long fn_800E7ED4(void*, Variant*);
extern "C" unsigned long fn_800E7EDC(void*, Variant*);
extern "C" unsigned long fn_800E7EE4(void*, Variant*);
extern "C" float fn_800E7EEC(void*, Variant*);
extern "C" float FuzzyAIGetConfidence(void*, UnidentifiedVariant_80054AB8*);
extern "C" float FuzzyAIBoolToFloat(bool);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnBool(FuzzyAIRuntime*, bool, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E8090(FuzzyAIRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E81BC(FuzzyAIRuntime*, int, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnFloat_800E82E8(FuzzyAIRuntime*, float, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnFloat_800E8414(FuzzyAIRuntime*, float, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnVariant(FuzzyAIRuntime*, UnidentifiedVariant_80054AB8*, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnU32(FuzzyAIRuntime*, unsigned long, float);
extern "C" float FuzzyAIPassThrough_800E8CAC(void*, float, bool);
extern "C" unsigned long fn_800E8CB0(void*, Variant*);
extern "C" void FuzzyAISetPlayerParameter_800E8CB8(void*, cPlayer*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzyAISetIntParameter_800E8D68(FuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzyAISetIntParameter_800E8D6C(FuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzyAISetIntParameter_800E8D70(FuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzyAISetIntParameter_800E8D74(FuzzyRuntimeBase*, int, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" void FuzzyAISetFielderParameter(void*, FuzzyFielderReference*, unsigned long, UnidentifiedVariant_80054AB8*);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnPlayer(FuzzyAIRuntime*, cPlayer*, float);
extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnFielder(FuzzyAIRuntime*, FuzzyFielderReference*, float);
extern "C" bool FuzzyAITryCachedPlayerQuestion_800E90EC(FuzzyRuntimeBase*, cPlayer*);
extern "C" bool FuzzyAITryCachedPlayerQuestion_800E9194(FuzzyRuntimeBase*, cPlayer*);
extern "C" bool FuzzyAITryCachedTeamQuestion(FuzzyRuntimeBase*, cTeam*);
extern "C" void FuzzyAISetTransition(UnidentifiedScriptMachine*, const char*);


extern "C" void LoadFuzzyByteCode(const char* filename, bool async);

// Script arguments occupy one stack word, including the bits of float values.
template <typename T>
inline u32 FuzzyArgumentBits(const T& value)
{
    typedef char ArgumentMustFitWord[sizeof(T) == sizeof(u32) ? 1 : -1];
    return *reinterpret_cast<const u32*>(&value);
}

extern "C" UnidentifiedVariant_80054AB8* ExecuteFuzzyFunction(FuzzyRuntimeBase* runtime, FunctionEntryPoint* function, int argumentCount, u32 arg1, u32 arg2);
extern "C" bool ApplyFuzzyByteCode();
extern "C" char FuzzyPassThrough_80312358(void*, char value);
extern "C" float FuzzyGetQueueConfidence( FuzzyRuntimeBase* runtime);
extern "C" void FuzzyNoOp_80314434(void*, UnidentifiedVariant_80054AB8*, float);
extern "C" void FuzzyNoOp_80314438(void*, UnidentifiedVariant_80054AB8*);
extern "C" void* FuzzyPassThrough_8031443C(void*, void* value, bool);
extern "C" float FuzzyPassThrough_80314444(void*, float value, bool);
extern "C" float FuzzyNormalize(float value, float minimum, float maximum);
extern "C" float FuzzyClamp(float value, float minimum, float maximum);
extern "C" float FuzzyInterpolate(float first, float second, float amount);
extern "C" float FuzzyInterpolateClamped(float first, float second, float amount);
extern "C" float FuzzyInterpolateRange(float first, float second, float minimum, float maximum, float value);
extern "C" float FuzzyInterpolateRangeClamped(float first, float second, float minimum, float maximum, float value);
extern "C" void FuzzyNoOp_80314740(void*, bool);
extern "C" void FuzzySetActionSelection( FuzzyRuntimeBase* runtime, int selection);
extern "C" void FuzzySetTransition(void*, shdStateMachine* state, const char* name);
extern "C" bool fn_80314798(void*);
extern "C" UnidentifiedFuzzyRuntimeValue* FuzzyGetCurrentContext( void*, FuzzyRuntimeBase* runtime);
extern "C" int FuzzyGetCurrentContextType(FuzzyRuntimeBase* runtime);
extern "C" void FuzzyPrintFloat(float value);
extern "C" void FuzzyPrintString(void*, const char* value);

template <typename T>
inline UnidentifiedVariant_80054AB8*
FuzzyRuntimeBase::CreateReturnValue(
    eVariantType type, T value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(type, value);
    result->SetParameter(4, FuzzyVariant(confidence));
    mReturnInstructionOffset = GetInstructionOffset() + 1;
    return ReturnValue(result, confidence);
}

inline UnidentifiedVariant_80054AB8*
FuzzyRuntimeBase::CreateReturnValue(
    UnidentifiedVariant_80054AB8* value, float confidence)
{
    UnidentifiedVariant_80054AB8* result;
    lbl_805842C8.Allocate(result);
    result = new (result) UnidentifiedVariant_80054AB8(value);
    result->SetParameter(4, FuzzyVariant(confidence));
    mReturnInstructionOffset = GetInstructionOffset() + 1;
    return ReturnValue(result, confidence);
}

extern "C" inline UnidentifiedVariant_80054AB8* FuzzyReturnVariantCopy(
    FuzzyRuntimeBase* runtime,
    UnidentifiedVariant_80054AB8 value, float confidence)
{
    UnidentifiedVariant_80054AB8* result =
        new (lbl_805842C8.Allocate())
            UnidentifiedVariant_80054AB8(value);
    result->SetParameter(4, FuzzyVariant(confidence));
    runtime->mReturnInstructionOffset = runtime->GetInstructionOffset() + 1;
    return runtime->ReturnValue(result, confidence);
}

#endif // GAME_AI_FUZZY_AI_RUNTIME_H
