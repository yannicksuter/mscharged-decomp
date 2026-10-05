#include "Game/AI/ScriptState.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/ScriptMachine.h"

#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/InterpreterCore.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

UnsetTransitionFunc g_UnsetTransitionFunc;

shdStateMachine::shdStateMachine(
    int state, TransitionFunc& transition)
    : mAgeTimer(gStateMachineZeroDuration)
    , mParameters()
{
    mState = state;
    mDefaultTransition = transition;
    mScriptMachine = 0;
    mDefaultMinDuration = gStateMachineZeroDuration;
    mDefaultMaxDuration = gStateMachineUnsetDuration;
    Reset(0);
}

void shdStateMachine::Reset(bool)
{
    mAgeTimer.m_uWasRunning = mAgeTimer.m_uPackedTime != 0;
    mAgeTimer.m_uPackedTime = 0;
    mActive = false;
    mMaxDuration = gStateMachineUnsetDuration;
    mMinDuration = gStateMachineUnsetDuration;
    mLastActiveTime = gStateMachineNeverActiveTime;
    mOverrideTransition = g_UnsetTransitionFunc;
}

void shdStateMachine::SetContext(
    ScriptMachine* context)
{
    mScriptMachine = context;
}

shdStateMachine::~shdStateMachine()
{
}

void RequestStateMachineDeactivation(shdStateMachine* machine)
{
    DeactivateScriptMachineState(machine->mScriptMachine, machine);
}

AIContext* GetStateMachineAIContext(
    shdStateMachine* machine)
{
    return machine->mScriptMachine->mAIContext;
}

void DeactivateStateMachine(
    shdStateMachine* machine, bool cleanup)
{
    if (cleanup)
    {
        machine->Cleanup();
    }
    machine->mActive = false;
    machine->mOverrideTransition = g_UnsetTransitionFunc;
}

bool ReinitializeStateMachine(
    shdStateMachine* machine, UnidentifiedVariantCollection* parameters,
    bool reinitialize)
{
    machine->mActive = false;
    u32 timerState = machine->mAgeTimer.m_uWasRunning;
    u32 packedTime = machine->mAgeTimer.m_uPackedTime;
    float secondDuration = machine->mMinDuration;
    float duration = machine->mMaxDuration;

    bool result = InitializeStateMachine(machine, parameters, false);

    machine->mAgeTimer.m_uWasRunning = timerState;
    machine->mAgeTimer.m_uPackedTime = packedTime;
    machine->mMinDuration = secondDuration;
    machine->mMaxDuration = duration;

    if (reinitialize)
    {
        result = machine->Reinitialize(parameters);
    }
    return result;
}

bool InitializeStateMachine(
    shdStateMachine* machine, UnidentifiedVariantCollection* parameters,
    bool initialize)
{
    machine->mMaxDuration = gStateMachineUnsetDuration;
    machine->mMinDuration = gStateMachineUnsetDuration;
    machine->mOverrideTransition = g_UnsetTransitionFunc;

    if (parameters->IsSet(10))
    {
        Variant* value = parameters->Get(10);
        switch (value->GetType())
        {
        case FT_U32:
            machine->mOverrideTransition.mValue.mFuncHash = value->mData.u;
            machine->mOverrideTransition.mValue.mNativeFunc = 0;
            break;
        case FT_INT:
            machine->mOverrideTransition.mValue.mFuncHash = value->mData.i;
            machine->mOverrideTransition.mValue.mNativeFunc = 0;
            break;
        case FT_POINTER:
        {
            void* function = value->mData.pointer;
            machine->mOverrideTransition.mValue.mFuncHash = -1;
            machine->mOverrideTransition.mValue.mNativeFunc = function;
            break;
        }
        case FT_STRING:
            machine->mOverrideTransition.mValue.mFuncHash = nlStringHash(value->mData.string);
            machine->mOverrideTransition.mValue.mNativeFunc = 0;
            break;
        }
    }

    if (parameters->IsSet(7))
    {
        machine->mMaxDuration = parameters->Get(7)->mData.f;
    }
    if (gStateMachineUnsetDuration == machine->mMaxDuration)
    {
        machine->mMaxDuration = machine->mDefaultMaxDuration;
    }
    if (gStateMachineUnsetDuration == machine->mMinDuration)
    {
        machine->mMinDuration = machine->mDefaultMinDuration;
    }

    machine->mAgeTimer.m_uWasRunning = machine->mAgeTimer.m_uPackedTime != 0;
    machine->mAgeTimer.m_uPackedTime = 0;

    bool result = true;
    if (initialize)
    {
        result = machine->Initialize(parameters);
    }

    if (result)
    {
        machine->mParameters = *parameters;
        machine->mActive = true;
    }
    return result;
}

void UpdateStateMachine(
    shdStateMachine* machine, UnidentifiedVariant_80054AB8* update,
    bool runUpdate, float deltaTime)
{
    *update = 0;
    machine->mAgeTimer.Countup(deltaTime, gStateMachineAgeThreshold);
    machine->mLastActiveTime = gAIActivityClock();

    float start = gAIProfilingClock();
    if (IsTransitionFuncSet(&machine->mOverrideTransition.mValue))
    {
        if (HasTransitionFunc(&machine->mOverrideTransition.mValue))
        {
            machine->mOverrideTransition.Execute(
                GetScriptMachineAIContext(machine->mScriptMachine),
                update,
                (UnidentifiedFuzzyRuntimeValue*)machine);
        }
    }
    else if (IsTransitionFuncSet(&machine->mDefaultTransition.mValue)
             && HasTransitionFunc(&machine->mDefaultTransition.mValue))
    {
        machine->mDefaultTransition.Execute(
            GetScriptMachineAIContext(machine->mScriptMachine),
            update,
            (UnidentifiedFuzzyRuntimeValue*)machine);
    }
    AccumulateScriptExecutionTime(start, gAIProfilingClock());

    if ((unsigned int)update->GetType() == FT_UNSPECIFIED)
    {
        *update = 0;
    }
    if (HasStateMachineTimedOut(machine) && update->fn_800C2BD4() != 1)
    {
        *update = 2;
    }
    if (runUpdate && update->fn_800C2BD4() != 1)
    {
        machine->Update(
            update, deltaTime);
    }
}

ScriptState::ScriptState(
    int state, const char* name, ScriptMachine* context,
    TransitionFunc transition)
    : shdStateMachine(state, transition)
{
    SetContext(context);

    char functionName[64];
    nlStrNCpy(functionName, "Init_", 63);
    nlStrNCat(functionName, functionName, name, 63);
    mInitFunctionHash = nlStringHash(functionName);

    nlStrNCpy(functionName, "Update_", 63);
    nlStrNCat(functionName, functionName, name, 63);
    mUpdateFunctionHash = nlStringHash(functionName);

    nlStrNCpy(functionName, "Cleanup_", 63);
    nlStrNCat(functionName, functionName, name, 63);
    mCleanupFunctionHash = nlStringHash(functionName);

    FunctionHash hash(mInitFunctionHash);
    if (!GetFuzzyRuntime()->FunctionExists(hash))
    {
        mInitFunctionHash = 0;
    }
    hash = FunctionHash(mUpdateFunctionHash);
    if (!GetFuzzyRuntime()->FunctionExists(hash))
    {
        mUpdateFunctionHash = 0;
    }
    hash = FunctionHash(mCleanupFunctionHash);
    if (!GetFuzzyRuntime()->FunctionExists(hash))
    {
        mCleanupFunctionHash = 0;
    }
}

bool ScriptState::Initialize(void*)
{
    bool initialized = true;
    if (mInitFunctionHash != 0)
    {
        float start = gAIProfilingClock();
        void* context = mScriptMachine->mAIContext->mData.pointer;
        u32 hash = mInitFunctionHash;
        UnidentifiedVariant_80054AB8 result
            = ExecuteScriptStateFunction(GetFuzzyRuntime(), hash, context);
        initialized = result.mData.b;
        AccumulateScriptExecutionTime(start, gAIProfilingClock());
    }
    return initialized;
}

void ScriptState::Update(
    DesireUpdate* update, float deltaTime)
{
    if (update->mData.pointer != 0)
    {
        return;
    }
    if (mUpdateFunctionHash == 0)
    {
        return;
    }

    float start = gAIProfilingClock();
    void* context = mScriptMachine->mAIContext->mData.pointer;
    u32 hash = mUpdateFunctionHash;
    {
        UnidentifiedVariant_80054AB8 result = ExecuteScriptStateFunction(
            GetFuzzyRuntime(), hash, context, deltaTime);
        *update = result;
    }
    AccumulateScriptExecutionTime(start, gAIProfilingClock());
}

void ScriptState::Cleanup()
{
    if (mCleanupFunctionHash == 0)
    {
        return;
    }

    float start = gAIProfilingClock();
    void* context = mScriptMachine->mAIContext->mData.pointer;
    u32 hash = mCleanupFunctionHash;
    ExecuteScriptStateFunction(GetFuzzyRuntime(), hash, context);
    AccumulateScriptExecutionTime(start, gAIProfilingClock());
}

ScriptState::~ScriptState()
{
}
