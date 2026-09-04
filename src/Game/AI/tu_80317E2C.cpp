#include "Game/AI/TeamPlayMachine.h"

#include "Game/AI/DesireUpdate.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/InterpreterCore.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include <string.h>

class ScriptQuestionCache;

class UnidentifiedStateMachine_803171D0 : public shdStateMachine
{
public:
    UnidentifiedStateMachine_803171D0(
        int, const char*, UnidentifiedScriptMachine*,
        const UnidentifiedStateTransition&);
    virtual ~UnidentifiedStateMachine_803171D0();
    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void* context)
    {
        return UnidentifiedInitialize(context);
    }
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);

    u32 mUnidentified088;
    u32 mUnidentified08C;
    u32 mUnidentified090;
};

extern "C" UnidentifiedFuzzyRuntimeBase* fn_80311744(void*);
extern "C" UnidentifiedVariant_80054AB8* fn_80312360(
    UnidentifiedFuzzyRuntimeBase*, FunctionEntryPoint*, int, void*, void*);
extern "C" void fn_80315A64(
    UnidentifiedStateTransition*, UnidentifiedFielderInput*,
    UnidentifiedVariant_80054AB8*, UnidentifiedFuzzyRuntimeValue*);
extern "C" void fn_80316980(shdStateMachine*, bool);
extern "C" bool fn_803169DC(
    shdStateMachine*, UnidentifiedVariantCollection*, bool);
extern "C" bool fn_80316A84(
    shdStateMachine*, UnidentifiedVariantCollection*, bool);
extern "C" void fn_80317010(
    shdStateMachine*, UnidentifiedVariant_80054AB8*, bool, float);
extern "C" float fn_80314538(float, float, float, float, float);
extern "C" void fn_8004F594(int, const char*, ...);

extern float (*lbl_806DF560)();
extern UnidentifiedStateTransition lbl_806E20B8;

char lbl_805302A0[]
    = "WARNING! shdStateMachine transition function returned nothing, funcHash=%d\n";
float lbl_806DF5B0[2] = { -1.0f, 0.0f };
char lbl_806DF5B8[] = "Init_%s";

float lbl_806E20C0;
float lbl_806E20C4;
int lbl_806E20C8;
float lbl_806E20CC;

extern "C" UnidentifiedFielderInput* fn_80317E2C(
    UnidentifiedScriptMachine* context)
{
    return context->mUnidentified064;
}

extern "C" bool fn_80317E34(
    const UnidentifiedStateTransition* transition)
{
    return transition->mUnidentifiedFunction != 0
        || (u32)(transition->mUnidentifiedHash + 0x10000) != 0xFFFF;
}

extern "C" bool fn_80317E60(
    const UnidentifiedStateTransition* transition)
{
    return transition->mUnidentifiedFunction != 0
        || transition->mUnidentifiedHash != 0;
}

extern "C" bool fn_80317E88(shdStateMachine* machine)
{
    bool result = false;
    if (machine->mUnidentified078 >= 0.0f)
    {
        if (machine->mUnidentifiedTimer.GetSeconds()
            > machine->mUnidentified078)
        {
            result = true;
        }
    }
    return result;
}

extern "C" UnidentifiedVariant_80054AB8 fn_80317EFC(
    UnidentifiedFuzzyRuntimeBase* runtime, u32* hash, void* argument)
{
    u32 localHash = *hash;
    return UnidentifiedVariant_80054AB8(fn_80312360(
        runtime, fn_802DF3E4(runtime, &localHash), 1, argument, 0));
}

extern "C" UnidentifiedVariant_80054AB8 fn_803184A8(
    UnidentifiedFuzzyRuntimeBase* runtime, u32* hash, void* argument,
    float value)
{
    u32 localHash = *hash;
    u32 valueBits = *(u32*)&value;
    return UnidentifiedVariant_80054AB8(fn_80312360(
        runtime, fn_802DF3E4(runtime, &localHash), 2, argument, (void*)valueBits));
}

UnidentifiedScriptMachine::UnidentifiedScriptMachine(
    int stateCount, bool deleteStates, UnidentifiedFielderInput* input,
    const char* name)
    : mUnidentified018()
{
    mUnidentified00C.mUnidentifiedFunction = 0;
    mUnidentified00C.mUnidentifiedHash = -1;
    mUnidentified074 = stateCount;
    mUnidentified004 = 0;
    mUnidentified008 = 0;
    mUnidentified014 = -1;
    mUnidentified064 = input;
    mUnidentified068 = deleteStates;
    if (input != 0)
    {
        input->mUnidentified18 = this;
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
    UnidentifiedFuzzyRuntimeBase* runtime = fn_80311744(this);
    if (runtime == 0)
    {
        return;
    }

    char functionName[0x48];
    nlSNPrintf(functionName, 63, lbl_806DF5B8, mUnidentified078);
    u32 hash = nlStringHash(functionName);
    runtime = fn_80311744(this);
    if (fn_802DF3E4(runtime, &hash) != 0)
    {
        runtime = fn_80311744(this);
        runtime->UnidentifiedVirtual2(
            fn_802DF3E4(runtime, &hash), 1, (u32)this, 0, 0, 0);
    }
}

extern "C" shdStateMachine* fn_80318D34(
    UnidentifiedScriptMachine* machine, int state, const char* name,
    bool secondary)
{
    UnidentifiedStateMachine_803171D0* result
        = new (nlMalloc(sizeof(UnidentifiedStateMachine_803171D0), 8, false))
            UnidentifiedStateMachine_803171D0(
                state, name, machine, lbl_806E20B8);
    machine->UnidentifiedAddState(state, result, secondary);
    return result;
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
    machine->UnidentifiedSetContext(this);
}

void UnidentifiedScriptMachine::UnidentifiedVirtual4(bool param)
{
    UnidentifiedVirtual6();
    for (int i = 0; i < mUnidentified074; i++)
    {
        shdStateMachine* machine = mUnidentified070[i];
        if (machine != 0 && machine->UnidentifiedIsActive())
        {
            fn_80316980(machine, true);
        }
    }

    for (int i = 0; i < mUnidentified074; i++)
    {
        if (mUnidentified06C[i] != 0)
        {
            mUnidentified06C[i]->UnidentifiedReset(param);
        }
        if (mUnidentified070[i] != 0)
        {
            mUnidentified070[i]->UnidentifiedReset(param);
        }
    }
}

extern "C" shdStateMachine* fn_80319E84(
    UnidentifiedScriptMachine*, int, UnidentifiedVariantCollection*, bool);

void UnidentifiedScriptMachine::UnidentifiedVirtual3(float deltaTime)
{
    UnidentifiedVariant_80054AB8 update(FT_INT, 0);
    bool selectState = false;
    shdStateMachine* active = mUnidentified004;

    if (active != 0)
    {
        fn_80317010(active, &update, true, deltaTime);
        int result = update.GetInt();
        if (result != -1 && active == mUnidentified004)
        {
            if (result != 0 && mUnidentified014 > -1)
            {
                bool force = false;
                if (mUnidentified018.IsSet(12))
                {
                    force = mUnidentified018.Get(12)->mData.b;
                }
                UnidentifiedVirtual5(
                    mUnidentified014, &mUnidentified018, force);
                mUnidentified014 = -1;
            }
            else if (result == 3)
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
            }
            else if (result == 1 || result == 2)
            {
                UnidentifiedVirtual6();
                selectState = true;
            }
            else if (result == 4
                     && active->mUnidentifiedTimer.GetSeconds()
                            >= active->mUnidentified07C)
            {
                selectState = true;
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
        if (machine == 0 || !machine->UnidentifiedIsActive())
        {
            continue;
        }

        fn_80317010(machine, &update, true, deltaTime);
        int result = update.GetInt();
        if (result == 0)
        {
            continue;
        }

        fn_80316980(machine, true);
        if (result == 3)
        {
            fn_80319E84(this, update.ExtraData.Get(8)->mData.i, &update.ExtraData, false);
        }
    }
}

extern "C" bool fn_8031A04C();
extern "C" float fn_8031A0C8(float, float);

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

    if (!fn_80317E34(&mUnidentified00C))
    {
        UnidentifiedVirtual6();
        return;
    }

    float start = lbl_806DF560();
    UnidentifiedVariant_80054AB8 result;
    fn_80315A64(&mUnidentified00C, mUnidentified064, &result, 0);
    fn_8031A0C8(start, lbl_806DF560());

    if (result.GetInt() == -1)
    {
        fn_8004F594(4, lbl_805302A0, mUnidentified00C.mUnidentifiedHash);
        UnidentifiedVirtual6();
    }
    else if (result.ExtraData.Get(9)->mData.b)
    {
        if (fn_80319E84(
                this, result.GetInt(), &result.ExtraData, false)
            != 0)
        {
            UnidentifiedVirtual6();
        }
    }
    else
    {
        UnidentifiedVirtual5(
            result.GetInt(), &result.ExtraData, true);
    }
}

void UnidentifiedScriptMachine::UnidentifiedVirtual6()
{
    if (mUnidentified004 != 0)
    {
        fn_80316980(mUnidentified004, true);
        mUnidentified008 = mUnidentified004;
    }
    mUnidentified004 = 0;
}

shdStateMachine* UnidentifiedScriptMachine::UnidentifiedVirtual5(
    int state, UnidentifiedVariantCollection* parameters, bool reinitialize)
{
    if ((u32)state == 0xA5A5A5A5 || state < 0
        || state >= mUnidentified074)
    {
        return 0;
    }

    UnidentifiedVariantCollection emptyParameters;
    if (parameters == 0)
    {
        parameters = &emptyParameters;
    }

    shdStateMachine* machine = mUnidentified06C[state];
    if (machine == 0)
    {
        return 0;
    }

    shdStateMachine* result = machine;
    if (machine->UnidentifiedIsActive())
    {
        if (!reinitialize)
        {
            return machine;
        }
        if (!fn_803169DC(machine, parameters, true)
            || mUnidentified004 != machine)
        {
            machine->mUnidentifiedActive = false;
            result = 0;
        }
    }
    else
    {
        UnidentifiedVirtual6();
        if (!fn_80316A84(machine, parameters, true)
            || mUnidentified004 != 0)
        {
            machine->mUnidentifiedActive = false;
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
    if (machine->mUnidentified004 == state)
    {
        machine->UnidentifiedVirtual6();
        return;
    }

    int index = state->mUnidentifiedState;
    if (index >= 0 && index < machine->mUnidentified074
        && machine->mUnidentified070[index] == state
        && state->UnidentifiedIsActive())
    {
        fn_80316980(state, true);
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
        shdStateMachine* state = machine->mUnidentified070[i];
        if (state != 0 && state->UnidentifiedIsActive())
        {
            fn_80316980(state, true);
        }
    }
}

extern "C" void fn_80319E58(
    UnidentifiedScriptMachine* machine, int state)
{
    shdStateMachine* value = machine->mUnidentified070[state];
    if (value != 0 && value->UnidentifiedIsActive())
    {
        fn_80316980(value, true);
    }
}

extern "C" shdStateMachine* fn_80319E84(
    UnidentifiedScriptMachine* machine, int state,
    UnidentifiedVariantCollection* parameters, bool reinitialize)
{
    if ((u32)state == 0xA5A5A5A5 || state < 0
        || state >= machine->mUnidentified074)
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
    if (value->UnidentifiedIsActive())
    {
        if (!reinitialize)
        {
            return value;
        }
        active = fn_803169DC(value, parameters, true);
    }
    else
    {
        active = fn_80316A84(value, parameters, true);
    }
    return active ? value : 0;
}

extern "C" shdStateMachine* fn_80319F94(
    UnidentifiedScriptMachine* machine, int state)
{
    if (state >= 0 && state < machine->mUnidentified074)
    {
        return machine->mUnidentified06C[state];
    }
    return 0;
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
        return value->mUnidentifiedActive;
    }
    return false;
}

extern "C" void fn_8031A02C(ScriptQuestionCache*)
{
    if (lbl_806E20C0 > lbl_806E20C4)
    {
        lbl_806E20C4 = lbl_806E20C0;
    }
    lbl_806E20C0 = 0.0f;
}

extern "C" bool fn_8031A04C()
{
    if (lbl_806DF5B0[0] > 0.0f)
    {
        float chance = fn_80314538(
            0.2f, 1.0f, lbl_806DF5B0[0], 0.0f, lbl_806E20C0);
        if (nlRandomf(1.0f, &nlDefaultSeed) > chance)
        {
            lbl_806E20C8++;
            return false;
        }
    }
    return true;
}

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
