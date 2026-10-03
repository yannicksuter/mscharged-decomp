#include "Game/AI/TeamPlayMachine.h"
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

class ScriptQuestionCache;

inline bool ScriptState::Reinitialize(void* context)
{
    return Initialize(context);
}


char lbl_805302A0[]
    = "WARNING! shdStateMachine transition function returned nothing, funcHash=%d\n";
float lbl_806DF5B0[2] = { -1.0f, 0.0f };
char lbl_806DF5B8[] = "Init_%s";
extern int lbl_806DF5C0[2];

float lbl_806E20C0;
float lbl_806E20C4;
int lbl_806E20C8;
float lbl_806E20CC;

extern const float lbl_806E6890;
extern const float lbl_806E6894;
extern const float lbl_806E6898[2];

extern "C" AIContext* fn_80317E2C(
    UnidentifiedScriptMachine* context)
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

extern "C" bool fn_80317E88(const shdStateMachine* machine)
{
    bool result = false;
    if (machine->mMaxDuration >= lbl_806E6880)
    {
        if (machine->mAgeTimer.GetSeconds()
            > machine->mMaxDuration)
        {
            result = true;
        }
    }
    return result;
}

extern const float lbl_806E6880 = 0.0f;
extern const float lbl_806E6884 = -1.0f;
extern const float lbl_806E6888 = -99999.0f;
extern const float lbl_806E688C = 10.0f;

extern "C" UnidentifiedVariant_80054AB8 fn_80317EFC(
    FuzzyRuntimeBase* runtime, const u32& hash, void* argument)
{
    u32 localHash = hash;
    return UnidentifiedVariant_80054AB8(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash), 1, FuzzyArgumentBits(argument), 0));
}

extern "C" UnidentifiedVariant_80054AB8 fn_803184A8(
    FuzzyRuntimeBase* runtime, const u32& hash, void* argument,
    float value)
{
    u32 localHash = hash;
    return UnidentifiedVariant_80054AB8(ExecuteFuzzyFunction(
        runtime, runtime->FindFunctionEntryPoint(localHash), 2, FuzzyArgumentBits(argument), FuzzyArgumentBits(value)));
}

UnidentifiedScriptMachine::UnidentifiedScriptMachine(
    int stateCount, bool deleteStates, AIContext* input,
    const char* name)
    : mUnidentified018()
{
    mUnidentified074 = stateCount;
    mUnidentified004 = 0;
    mUnidentified008 = 0;
    mUnidentified014 = -1;
    mAIContext = input;
    mUnidentified068 = deleteStates;
    if (input != 0)
    {
        input->mScriptMachine = this;
    }

    unsigned long size = stateCount * sizeof(shdStateMachine*);
    mUnidentified06C = (shdStateMachine**)nlMalloc(size, 8, false);
    memset(mUnidentified06C, 0, size);
    mUnidentified070 = (shdStateMachine**)nlMalloc(size, 8, false);
    memset(mUnidentified070, 0, size);

    mUnidentified078[0] = 0;
    if (name != 0)
    {
        nlStrNCpy(mUnidentified078, name, 63);
    }
}

UnidentifiedScriptMachine::~UnidentifiedScriptMachine()
{
    if (mUnidentified068)
    {
        for (int i = 0; i < mUnidentified074; i++)
        {
            delete mUnidentified06C[i];
            delete mUnidentified070[i];
        }
    }
    delete[] mUnidentified06C;
    delete[] mUnidentified070;
}

void UnidentifiedScriptMachine::UnidentifiedVirtual2()
{
    FuzzyRuntimeBase* runtime = GetFuzzyRuntime();
    if (runtime == 0)
    {
        return;
    }

    char functionName[0x48];
    nlSNPrintf(functionName, 63, lbl_806DF5B8, mUnidentified078);
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

extern "C" void fn_80318D34(
    UnidentifiedScriptMachine* machine, int state, const char* name,
    bool secondary)
{
    ScriptState* result
        = new (nlMalloc(sizeof(ScriptState), 8, false))
            ScriptState(
                state, name, machine,
                TransitionFunc(g_UnsetTransitionFunc));
    machine->UnidentifiedAddState(state, result, secondary);
}

void UnidentifiedScriptMachine::UnidentifiedAddState(
    int state, shdStateMachine* machine, bool secondary)
{
    if (secondary)
    {
        mUnidentified070[state] = machine;
    }
    else
    {
        mUnidentified06C[state] = machine;
    }
    machine->SetContext(this);
}

void UnidentifiedScriptMachine::Reset(bool param)
{
    UnidentifiedVirtual6();
    fn_80319DA0(this);

    for (int i = 0; i < mUnidentified074; i++)
    {
        if (mUnidentified06C[i] != 0)
        {
            mUnidentified06C[i]->Reset(param);
        }
        if (mUnidentified070[i] != 0)
        {
            mUnidentified070[i]->Reset(param);
        }
    }
}

void UnidentifiedScriptMachine::Update(float deltaTime)
{
    bool selectState = false;
    UnidentifiedVariant_80054AB8 update(FT_INT, lbl_806DF5C0[0]);
    shdStateMachine* active = mUnidentified004;

    if (active != 0)
    {
        UpdateStateMachine(active, &update, true, deltaTime);
        if ((unsigned int)update.GetType() == FT_UNSPECIFIED)
        {
            update = 0;
        }
    }
    if (active == mUnidentified004)
    {
        if (update.mData.i != 0 && mUnidentified014 > -1)
        {
            bool force = false;
            if (mUnidentified018.IsSet(12))
            {
                force = update.ExtraData.Get(12)->mData.b;
            }
            UnidentifiedVirtual5(
                mUnidentified014, &mUnidentified018, force);
            mUnidentified014 = -1;
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
                UnidentifiedVirtual5(
                    update.ExtraData.Get(8)->mData.i,
                    &update.ExtraData,
                    force);
                break;
            }
            case 1:
            case 2:
                UnidentifiedVirtual6();
                selectState = true;
                break;
            case 4:
                if (mUnidentified004->mAgeTimer.GetSeconds()
                    >= mUnidentified004->mMinDuration)
                {
                    selectState = true;
                }
                break;
            case 0:
                break;
            }
        }
    }

    if (UnidentifiedVirtual1() || selectState)
    {
        UnidentifiedVirtual7();
    }

    for (int i = 0; i < mUnidentified074; i++)
    {
        shdStateMachine* machine = mUnidentified070[i];
        if (machine == 0 || !machine->IsActive())
        {
            continue;
        }

        UpdateStateMachine(machine, &update, true, deltaTime);
        if (update.mData.i == 0)
        {
            continue;
        }

        fn_80319E58(this, i);
        if (update.mData.i == 3)
        {
            fn_80319E84(this, update.ExtraData.Get(8)->mData.i, &update.ExtraData, false);
        }
    }
}

void UnidentifiedScriptMachine::UnidentifiedVirtual7()
{
    if (!fn_8031A04C())
    {
        if (mUnidentified004 != 0)
        {
            UnidentifiedVirtual6();
        }
        UnidentifiedVirtual8();
    }

    if (IsTransitionFuncSet(&mTransition.mValue))
    {
        float start = gAIProfilingClock();
        UnidentifiedVariant_80054AB8 result;
        mTransition.Execute(mAIContext, &result, 0);
        fn_8031A0C8(start, gAIProfilingClock());

        if ((unsigned int)result.GetType() == FT_UNSPECIFIED)
        {
            tDebugPrintManager::Print(DC_AI, lbl_805302A0, mTransition.mValue.mFuncHash);
            UnidentifiedVirtual6();
        }
        else if (result.ExtraData.Get(9)->mData.b)
        {
            if (fn_80319E84(
                    this, result.mData.i, &result.ExtraData, false)
                != 0)
            {
                UnidentifiedVirtual6();
            }
        }
        else
        {
            UnidentifiedVirtual5(
                result.mData.i, &result.ExtraData, true);
        }
    }
    else
    {
        UnidentifiedVirtual6();
    }
}

void UnidentifiedScriptMachine::UnidentifiedVirtual6()
{
    if (mUnidentified004 != 0)
    {
        DeactivateStateMachine(mUnidentified004, true);
        mUnidentified008 = mUnidentified004;
    }
    mUnidentified004 = 0;
}

shdStateMachine* UnidentifiedScriptMachine::UnidentifiedVirtual5(
    int state, UnidentifiedVariantCollection* parameters, bool reinitialize)
{
    if ((u32)state == 0xA5A5A5A5)
    {
        return 0;
    }
    if (state < 0 || state >= mUnidentified074)
    {
        return 0;
    }

    UnidentifiedVariantCollection emptyParameters;
    if (parameters == 0)
    {
        parameters = &emptyParameters;
    }

    shdStateMachine* machine = UnidentifiedGet06C(state);
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
                || mUnidentified004 != machine)
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
        UnidentifiedVirtual6();
        if (!InitializeStateMachine(machine, parameters, true)
            || mUnidentified004 != 0)
        {
            machine->mActive = false;
            result = 0;
        }
    }

    if (mUnidentified004 == 0)
    {
        mUnidentified004 = result;
    }
    return mUnidentified004;
}

extern "C" void fn_803198F4(UnidentifiedScriptMachine* machine)
{
    machine->UnidentifiedVirtual6();
}

extern "C" void fn_80319904(
    UnidentifiedScriptMachine* machine, shdStateMachine* state)
{
    if (state == machine->mUnidentified004)
    {
        machine->UnidentifiedVirtual6();
        return;
    }

    int index = state->mState;
    if (fn_80319FC0(machine, index) == state
        && state->IsActive())
    {
        fn_80319E58(machine, index);
    }
}

extern "C" void fn_8031998C(
    UnidentifiedScriptMachine* machine, int state,
    const UnidentifiedVariantCollection* parameters)
{
    machine->mUnidentified014 = state;
    machine->mUnidentified018.Remove(-1);
    if (parameters == 0)
    {
        return;
    }
    machine->mUnidentified018 = *parameters;
}

extern "C" void fn_80319DA0(UnidentifiedScriptMachine* machine)
{
    for (int i = 0; i < machine->mUnidentified074; i++)
    {
        if (fn_80319FEC(machine, i))
        {
            fn_80319E58(machine, i);
        }
    }
}

extern "C" void fn_80319E58(
    UnidentifiedScriptMachine* machine, int state)
{
    shdStateMachine* value = machine->mUnidentified070[state];
    if (value != 0 && value->IsActive())
    {
        DeactivateStateMachine(value, true);
    }
}

extern "C" shdStateMachine* fn_80319E84(
    UnidentifiedScriptMachine* machine, int state,
    UnidentifiedVariantCollection* parameters, bool reinitialize)
{
    if ((u32)state == 0xA5A5A5A5)
    {
        return 0;
    }
    if (state < 0 || state >= machine->mUnidentified074)
    {
        return 0;
    }

    shdStateMachine* value = machine->mUnidentified070[state];
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

extern "C" shdStateMachine* fn_80319F94(
    UnidentifiedScriptMachine* machine, int state)
{
    return machine->UnidentifiedGet06C(state);
}

extern "C" shdStateMachine* fn_80319FC0(
    UnidentifiedScriptMachine* machine, int state)
{
    if (state >= 0 && state < machine->mUnidentified074)
    {
        return machine->mUnidentified070[state];
    }
    return 0;
}

extern "C" bool fn_80319FEC(
    UnidentifiedScriptMachine* machine, int state)
{
    shdStateMachine* value;
    if (state >= 0 && state < machine->mUnidentified074)
    {
        value = machine->mUnidentified070[state];
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

extern "C" void fn_8031A02C(ScriptQuestionCache*)
{
    if (lbl_806E20C0 > lbl_806E20C4)
    {
        lbl_806E20C4 = lbl_806E20C0;
    }
    lbl_806E20C0 = lbl_806E6890;
}

extern "C" bool fn_8031A04C()
{
    if (lbl_806DF5B0[0] > lbl_806E6890)
    {
        float chance = FuzzyInterpolateRangeClamped(lbl_806E6894, lbl_806E6898[0],
            lbl_806DF5B0[0], lbl_806E6890, lbl_806E20C0);
        if (nlRandomf(lbl_806E6898[0], &nlDefaultSeed) > chance)
        {
            lbl_806E20C8++;
            return false;
        }
    }
    return true;
}

extern const float lbl_806E6890 = 0.0f;
extern const float lbl_806E6894 = 0.2f;
extern const float lbl_806E6898[2] = { 1.0f, 0.0f };

extern "C" float fn_8031A0C8(float start, float end)
{
    if (end > start)
    {
        lbl_806E20C0 += end - start;
    }
    if (lbl_806E20C0 > lbl_806E20CC)
    {
        lbl_806E20CC = lbl_806E20C0;
    }
    return lbl_806E20C0;
}

extern "C" void fn_8031A0FC(float value)
{
    lbl_806DF5B0[0] = value;
}
