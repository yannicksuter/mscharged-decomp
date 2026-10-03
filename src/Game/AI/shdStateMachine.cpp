#include "Game/AI/ScriptState.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/TeamPlayMachine.h"

#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/InterpreterCore.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

UnsetTransitionFunc g_UnsetTransitionFunc;

shdStateMachine::shdStateMachine(
    int state, TransitionFunc& transition)
    : mAgeTimer(lbl_806E6880)
    , mParameters()
{
    mState = state;
    mDefaultTransition = transition;
    mScriptMachine = 0;
    mDefaultMinDuration = lbl_806E6880;
    mDefaultMaxDuration = lbl_806E6884;
    Reset(0);
}

void shdStateMachine::Reset(bool)
{
    mAgeTimer.m_uWasRunning = mAgeTimer.m_uPackedTime != 0;
    mAgeTimer.m_uPackedTime = 0;
    mActive = false;
    mMaxDuration = lbl_806E6884;
    mMinDuration = lbl_806E6884;
    mLastActiveTime = lbl_806E6888;
    mOverrideTransition = g_UnsetTransitionFunc;
}

void shdStateMachine::SetContext(
    UnidentifiedScriptMachine* context)
{
    mScriptMachine = context;
}

shdStateMachine::~shdStateMachine()
{
}

void RequestStateMachineDeactivation(shdStateMachine* machine)
{
    fn_80319904(machine->mScriptMachine, machine);
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
    machine->mMaxDuration = lbl_806E6884;
    machine->mMinDuration = lbl_806E6884;
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
    if (lbl_806E6884 == machine->mMaxDuration)
    {
        machine->mMaxDuration = machine->mDefaultMaxDuration;
    }
    if (lbl_806E6884 == machine->mMinDuration)
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
    machine->mAgeTimer.Countup(deltaTime, lbl_806E688C);
    machine->mLastActiveTime = gAIActivityClock();

    float start = gAIProfilingClock();
    if (IsTransitionFuncSet(&machine->mOverrideTransition.mValue))
    {
        if (HasTransitionFunc(&machine->mOverrideTransition.mValue))
        {
            machine->mOverrideTransition.Execute(
                fn_80317E2C(machine->mScriptMachine),
                update,
                (UnidentifiedFuzzyRuntimeValue*)machine);
        }
    }
    else if (IsTransitionFuncSet(&machine->mDefaultTransition.mValue)
             && HasTransitionFunc(&machine->mDefaultTransition.mValue))
    {
        machine->mDefaultTransition.Execute(
            fn_80317E2C(machine->mScriptMachine),
            update,
            (UnidentifiedFuzzyRuntimeValue*)machine);
    }
    fn_8031A0C8(start, gAIProfilingClock());

    if ((unsigned int)update->GetType() == FT_UNSPECIFIED)
    {
        *update = 0;
    }
    if (fn_80317E88(machine) && update->fn_800C2BD4() != 1)
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
    int state, const char* name, UnidentifiedScriptMachine* context,
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
            = fn_80317EFC(GetFuzzyRuntime(), hash, context);
        initialized = result.mData.b;
        fn_8031A0C8(start, gAIProfilingClock());
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
        UnidentifiedVariant_80054AB8 result = fn_803184A8(
            GetFuzzyRuntime(), hash, context, deltaTime);
        *update = result;
    }
    fn_8031A0C8(start, gAIProfilingClock());
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
    fn_80317EFC(GetFuzzyRuntime(), hash, context);
    fn_8031A0C8(start, gAIProfilingClock());
}

ScriptState::~ScriptState()
{
}
