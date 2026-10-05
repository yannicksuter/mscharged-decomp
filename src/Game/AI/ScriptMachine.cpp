#include "Game/AI/ScriptMachine.h"
#include "Game/AI/ScriptState.h"
#include "Game/Sys/debug.h"
#include "Game/AI/AIContext.h"

#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/InterpreterCore.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include <string.h>

char gScriptTransitionMissingReturnWarning[]
    = "WARNING! shdStateMachine transition function returned nothing, funcHash=%d\n";
float gScriptTimeBudget[2] = { -1.0f, 0.0f };
char gScriptMachineInitFunctionFormat[] = "Init_%s";

float gScriptFrameTime;
float gScriptPeakCompletedFrameTime;
int gScriptBudgetRejectionCount;
float gScriptPeakAccumulatedFrameTime;

extern const float gScriptTimeZero;
extern const float gScriptMinimumExecutionChance;
extern const float gScriptFullExecutionChance[2];

AIContext* GetScriptMachineAIContext(
    ScriptMachine* context)
{
    return context->mAIContext;
}

bool IsTransitionFuncSet(
    const TransitionFunc* transition)
{
    return !transition->IsUnset();
}

bool HasTransitionFunc(
    const TransitionFunc* transition)
{
    return transition->mNativeFunc != 0
        || transition->mFuncHash != 0;
}

bool HasStateMachineTimedOut(const shdStateMachine* machine)
{
    bool result = false;
    if (machine->mMaxDuration >= gStateMachineZeroDuration)
    {
        if (machine->mAgeTimer.GetSeconds()
            > machine->mMaxDuration)
        {
            result = true;
        }
    }
    return result;
}

bool ScriptState::Reinitialize(void* context)
{
    return Initialize(context);
}

extern const float gStateMachineZeroDuration = 0.0f;
extern const float gStateMachineUnsetDuration = -1.0f;
extern const float gStateMachineNeverActiveTime = -99999.0f;
extern const float gStateMachineAgeThreshold = 10.0f;

UnidentifiedVariant_80054AB8 ExecuteScriptStateFunction(
    FuzzyRuntimeBase* runtime, const u32& hash, void* argument)
{
    u32 localHash = hash;
    return UnidentifiedVariant_80054AB8(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash), 1, FuzzyArgumentBits(argument), 0));
}

UnidentifiedVariant_80054AB8 ExecuteScriptStateFunction(
    FuzzyRuntimeBase* runtime, const u32& hash, void* argument,
    float value)
{
    u32 localHash = hash;
    return UnidentifiedVariant_80054AB8(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash), 2, FuzzyArgumentBits(argument), FuzzyArgumentBits(value)));
}

ScriptMachine::ScriptMachine(
    int stateCount, bool deleteStates, AIContext* input,
    const char* name)
    : mPendingParameters()
{
    mStateCount = stateCount;
    mActiveState = 0;
    mPreviousState = 0;
    mPendingState = -1;
    mAIContext = input;
    mOwnsStates = deleteStates;
    if (input != 0)
    {
        input->mScriptMachine = this;
    }

    unsigned long size = stateCount * sizeof(shdStateMachine*);
    mStates = (shdStateMachine**)nlMalloc(size, 8, false);
    memset(mStates, 0, size);
    mConcurrentStates = (shdStateMachine**)nlMalloc(size, 8, false);
    memset(mConcurrentStates, 0, size);

    mName[0] = 0;
    if (name != 0)
    {
        nlStrNCpy(mName, name, 63);
    }
}

static inline void DeleteOwnedStates(ScriptMachine* machine)
{
    if (machine->mOwnsStates)
    {
        for (int i = 0; i < machine->mStateCount; i++)
        {
            delete machine->mStates[i];
            delete machine->mConcurrentStates[i];
        }
    }
}

ScriptMachine::~ScriptMachine()
{
    DeleteOwnedStates(this);
    delete[] mStates;
    delete[] mConcurrentStates;
}

void ScriptMachine::Initialize()
{
    FuzzyRuntimeBase* runtime = GetFuzzyRuntime();
    if (runtime == 0)
    {
        return;
    }

    char functionName[0x48];
    nlSNPrintf(functionName, 63, gScriptMachineInitFunctionFormat, mName);
    u32 hash = nlStringHash(functionName);
    runtime = GetFuzzyRuntime();
    u32 localHash = hash;
    bool hasFunction = runtime->FindFunctionEntryPoint(localHash) != 0;
    if (hasFunction)
    {
        runtime = GetFuzzyRuntime();
        u32 callHash = hash;
        runtime->ExecuteFunction(
            runtime->FindFunctionEntryPoint(callHash), 1, (u32)this, 0, 0, 0);
    }
}

void AddScriptState(
    ScriptMachine* machine, int state, const char* name,
    bool secondary)
{
    ScriptState* result
        = new (nlMalloc(sizeof(ScriptState), 8, false))
            ScriptState(
                state, name, machine,
                TransitionFunc(g_UnsetTransitionFunc));
    machine->AddState(state, result, secondary);
}

void ScriptMachine::AddState(
    int state, shdStateMachine* machine, bool secondary)
{
    if (secondary)
    {
        mConcurrentStates[state] = machine;
    }
    else
    {
        mStates[state] = machine;
    }
    machine->SetContext(this);
}

void ScriptMachine::Reset(bool param)
{
    DeactivateState();
    DeactivateConcurrentStates(this);

    for (int i = 0; i < mStateCount; i++)
    {
        if (mStates[i] != 0)
        {
            mStates[i]->Reset(param);
        }
        if (mConcurrentStates[i] != 0)
        {
            mConcurrentStates[i]->Reset(param);
        }
    }
}

void ScriptMachine::Update(float deltaTime)
{
    bool selectState = false;
    UnidentifiedVariant_80054AB8 update(0, -1.0f, -1.0f);
    shdStateMachine* active = mActiveState;

    if (active != 0)
    {
        UpdateStateMachine(active, &update, true, deltaTime);
        if ((unsigned int)update.GetType() == FT_UNSPECIFIED)
        {
            update = 0;
        }
    }
    if (active == mActiveState)
    {
        if (update.mData.i != 0 && mPendingState > -1)
        {
            bool force = false;
            if (mPendingParameters.IsSet(12))
            {
                force = update.ExtraData.Get(12)->mData.b;
            }
            ActivateState(
                mPendingState, &mPendingParameters, force);
            mPendingState = -1;
        }
        else
        {
            switch (update.mData.i)
            {
            case 3:
            {
                bool force = false;
                if (update.ExtraData.IsSet(12))
                {
                    force = update.ExtraData.Get(12)->mData.b;
                }
                ActivateState(
                    update.ExtraData.Get(8)->mData.i,
                    &update.ExtraData,
                    force);
                break;
            }
            case 1:
            case 2:
                DeactivateState();
                selectState = true;
                break;
            case 4:
                if (mActiveState->mAgeTimer.GetSeconds()
                    >= mActiveState->mMinDuration)
                {
                    selectState = true;
                }
                break;
            case 0:
                break;
            }
        }
    }

    if (IsIdle() || selectState)
    {
        SelectState();
    }

    for (int i = 0; i < mStateCount; i++)
    {
        shdStateMachine* machine = mConcurrentStates[i];
        if (machine == 0 || !machine->IsActive())
        {
            continue;
        }

        UpdateStateMachine(machine, &update, true, deltaTime);
        if (update.mData.i == 0)
        {
            continue;
        }

        DeactivateConcurrentState(this, i);
        if (update.mData.i == 3)
        {
            ActivateConcurrentState(this, update.ExtraData.Get(8)->mData.i, &update.ExtraData, false);
        }
    }
}

void ScriptMachine::SelectState()
{
    if (!CheckScriptTimeBudget())
    {
        if (mActiveState != 0)
        {
            DeactivateState();
        }
        OnBudgetCheckFailed();
    }

    if (IsTransitionFuncSet(&mTransition.mValue))
    {
        float start = gAIProfilingClock();
        UnidentifiedVariant_80054AB8 result;
        mTransition.Execute(mAIContext, &result, 0);
        AccumulateScriptExecutionTime(start, gAIProfilingClock());

        if ((unsigned int)result.GetType() == FT_UNSPECIFIED)
        {
            tDebugPrintManager::Print(DC_AI, gScriptTransitionMissingReturnWarning, mTransition.mValue.mFuncHash);
            DeactivateState();
        }
        else if (result.ExtraData.Get(9)->mData.b)
        {
            if (ActivateConcurrentState(
                    this, result.mData.i, &result.ExtraData, false)
                != 0)
            {
                DeactivateState();
            }
        }
        else
        {
            ActivateState(
                result.mData.i, &result.ExtraData, true);
        }
    }
    else
    {
        DeactivateState();
    }
}

void ScriptMachine::DeactivateState()
{
    if (mActiveState != 0)
    {
        DeactivateStateMachine(mActiveState, true);
        mPreviousState = mActiveState;
    }
    mActiveState = 0;
}

shdStateMachine* ScriptMachine::ActivateState(
    int state, UnidentifiedVariantCollection* parameters, bool reinitialize)
{
    if ((u32)state == 0xA5A5A5A5)
    {
        return 0;
    }
    if (state < 0 || state >= mStateCount)
    {
        return 0;
    }

    UnidentifiedVariantCollection emptyParameters;
    if (parameters == 0)
    {
        parameters = &emptyParameters;
    }

    shdStateMachine* machine = GetState(state);
    shdStateMachine* result = machine;
    if (machine == 0)
    {
        return 0;
    }

    if (machine->IsActive())
    {
        if (reinitialize)
        {
            if (!ReinitializeStateMachine(machine, parameters, true)
                || mActiveState != machine)
            {
                machine->mActive = false;
                result = 0;
            }
        }
        else
        {
            return machine;
        }
    }
    else
    {
        DeactivateState();
        if (!InitializeStateMachine(machine, parameters, true)
            || mActiveState != 0)
        {
            machine->mActive = false;
            result = 0;
        }
    }

    if (mActiveState == 0)
    {
        mActiveState = result;
    }
    return mActiveState;
}

void DeactivateScriptMachine(ScriptMachine* machine)
{
    machine->DeactivateState();
}

void DeactivateScriptMachineState(
    ScriptMachine* machine, shdStateMachine* state)
{
    if (state == machine->mActiveState)
    {
        machine->DeactivateState();
        return;
    }

    int index = state->mState;
    if (GetConcurrentState(machine, index) == state
        && state->IsActive())
    {
        DeactivateConcurrentState(machine, index);
    }
}

void QueueScriptMachineState(
    ScriptMachine* machine, int state,
    const UnidentifiedVariantCollection* parameters)
{
    machine->mPendingState = state;
    machine->mPendingParameters.Remove(-1);
    if (parameters == 0)
    {
        return;
    }
    machine->mPendingParameters = *parameters;
}

void DeactivateConcurrentStates(ScriptMachine* machine)
{
    for (int i = 0; i < machine->mStateCount; i++)
    {
        if (IsConcurrentStateActive(machine, i))
        {
            DeactivateConcurrentState(machine, i);
        }
    }
}

void DeactivateConcurrentState(
    ScriptMachine* machine, int state)
{
    shdStateMachine* value = machine->mConcurrentStates[state];
    if (value != 0 && value->IsActive())
    {
        DeactivateStateMachine(value, true);
    }
}

shdStateMachine* ActivateConcurrentState(
    ScriptMachine* machine, int state,
    UnidentifiedVariantCollection* parameters, bool reinitialize)
{
    if ((u32)state == 0xA5A5A5A5)
    {
        return 0;
    }
    if (state < 0 || state >= machine->mStateCount)
    {
        return 0;
    }

    shdStateMachine* value = machine->mConcurrentStates[state];
    UnidentifiedVariantCollection emptyParameters;
    if (parameters == 0)
    {
        parameters = &emptyParameters;
    }
    if (value == 0)
    {
        return 0;
    }

    bool active;
    if (value->IsActive())
    {
        if (reinitialize)
        {
            active = ReinitializeStateMachine(value, parameters, true);
        }
        else
        {
            return value;
        }
    }
    else
    {
        active = InitializeStateMachine(value, parameters, true);
    }
    if (!active)
    {
        value = 0;
    }
    return value;
}

shdStateMachine* GetScriptMachineState(
    ScriptMachine* machine, int state)
{
    return machine->GetState(state);
}

shdStateMachine* GetConcurrentState(
    ScriptMachine* machine, int state)
{
    if (state >= 0 && state < machine->mStateCount)
    {
        return machine->mConcurrentStates[state];
    }
    return 0;
}

bool IsConcurrentStateActive(
    ScriptMachine* machine, int state)
{
    shdStateMachine* value;
    if (state >= 0 && state < machine->mStateCount)
    {
        value = machine->mConcurrentStates[state];
    }
    else
    {
        value = 0;
    }
    if (value != 0)
    {
        return value->mActive;
    }
    return false;
}

void ResetScriptFrameTime(ScriptQuestionCache*)
{
    if (gScriptFrameTime > gScriptPeakCompletedFrameTime)
    {
        gScriptPeakCompletedFrameTime = gScriptFrameTime;
    }
    gScriptFrameTime = gScriptTimeZero;
}

bool CheckScriptTimeBudget()
{
    if (gScriptTimeBudget[0] > gScriptTimeZero)
    {
        float chance = FuzzyInterpolateRangeClamped(gScriptMinimumExecutionChance, gScriptFullExecutionChance[0],
            gScriptTimeBudget[0], gScriptTimeZero, gScriptFrameTime);
        if (nlRandomf(gScriptFullExecutionChance[0], &nlDefaultSeed) > chance)
        {
            gScriptBudgetRejectionCount++;
            return false;
        }
    }
    return true;
}

extern const float gScriptTimeZero = 0.0f;
extern const float gScriptMinimumExecutionChance = 0.2f;
extern const float gScriptFullExecutionChance[2] = { 1.0f, 0.0f };

float AccumulateScriptExecutionTime(float start, float end)
{
    if (end > start)
    {
        gScriptFrameTime += end - start;
    }
    if (gScriptFrameTime > gScriptPeakAccumulatedFrameTime)
    {
        gScriptPeakAccumulatedFrameTime = gScriptFrameTime;
    }
    return gScriptFrameTime;
}

void SetScriptTimeBudget(float value)
{
    gScriptTimeBudget[0] = value;
}
