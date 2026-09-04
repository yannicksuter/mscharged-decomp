#include "Game/AI/Desire.h"
#include "Game/AI/TeamPlayMachine.h"

#include "Game/AI/DesireUpdate.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/InterpreterCore.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

extern "C" void fn_80319904(
    UnidentifiedScriptMachine*, shdStateMachine*);
extern "C" UnidentifiedFuzzyRuntimeBase* fn_80311734(
    shdStateMachine*);
extern "C" void fn_80315A64(
    UnidentifiedStateTransition*, UnidentifiedFuzzyRuntimeValue*,
    UnidentifiedVariant_80054AB8*, UnidentifiedFuzzyRuntimeValue*);
extern "C" void fn_800C2C18(UnidentifiedVariant_80054AB8*, int);
extern "C" int fn_800C2BD4(UnidentifiedVariant_80054AB8*);
extern "C" UnidentifiedFuzzyRuntimeValue* fn_80317E2C(
    UnidentifiedScriptMachine*);
extern "C" bool fn_80317E34(const UnidentifiedStateTransition*);
extern "C" bool fn_80317E60(const UnidentifiedStateTransition*);
extern "C" bool fn_80317E88(shdStateMachine*);
extern "C" UnidentifiedVariant_80054AB8 fn_80317EFC(
    UnidentifiedFuzzyRuntimeBase*, u32*, void*);
extern "C" UnidentifiedVariant_80054AB8 fn_803184A8(
    UnidentifiedFuzzyRuntimeBase*, u32*, void*, float);
extern "C" float fn_8031A0C8(float, float);

extern float (*lbl_806DF560)();
extern float (*lbl_806DF564)();

char lbl_80530248[] = "Cleanup_";
char lbl_806DF5A0[] = "Init_";
char lbl_806DF5A8[] = "Update_";

extern UnidentifiedStateTransition lbl_806E20B8;

class UnidentifiedStateMachine_803171D0 : public shdStateMachine
{
public:
    UnidentifiedStateMachine_803171D0(
        int state, const char* name, UnidentifiedScriptMachine* context,
        const UnidentifiedStateTransition& transition);
    virtual ~UnidentifiedStateMachine_803171D0();

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);

    u32 mUnidentified088;
    u32 mUnidentified08C;
    u32 mUnidentified090;
};

shdStateMachine::shdStateMachine(
    int state, const UnidentifiedStateTransition& transition)
    : mUnidentifiedTimer(1.0f)
    , mUnidentified01C()
{
    mUnidentified068.mUnidentifiedHash = -1;
    mUnidentified068.mUnidentifiedFunction = 0;
    mUnidentified070.mUnidentifiedHash = -1;
    mUnidentified070.mUnidentifiedFunction = 0;
    mUnidentifiedState = state;
    mUnidentified068 = transition;
    mUnidentified018 = 0;
    mUnidentified080 = 1.0f;
    mUnidentified084 = 0.0f;
    UnidentifiedReset(0);
}

void shdStateMachine::UnidentifiedReset(bool)
{
    mUnidentifiedTimer.m_unk0 = mUnidentifiedTimer.m_uPackedTime != 0;
    mUnidentifiedTimer.m_uPackedTime = 0;
    mUnidentifiedActive = false;
    mUnidentified078 = 0.0f;
    mUnidentified07C = 0.0f;
    mUnidentified014 = 0.5f;
    mUnidentified070 = lbl_806E20B8;
}

void shdStateMachine::UnidentifiedSetContext(
    UnidentifiedScriptMachine* context)
{
    mUnidentified018 = context;
}

shdStateMachine::~shdStateMachine()
{
}

extern "C" void fn_80316968(shdStateMachine* machine)
{
    fn_80319904(machine->mUnidentified018, machine);
}

extern "C" UnidentifiedFielderInput* fn_80316974(
    shdStateMachine* machine)
{
    return machine->mUnidentified018->mUnidentified064;
}

extern "C" void fn_80316980(
    shdStateMachine* machine, bool cleanup)
{
    if (cleanup)
    {
        machine->UnidentifiedCleanup();
    }
    machine->mUnidentifiedActive = false;
    machine->mUnidentified070 = lbl_806E20B8;
}

extern "C" bool fn_80316A84(
    shdStateMachine*, UnidentifiedVariantCollection*, bool);

extern "C" bool fn_803169DC(
    shdStateMachine* machine, UnidentifiedVariantCollection* parameters,
    bool reinitialize)
{
    machine->mUnidentifiedActive = false;
    u32 timerState = machine->mUnidentifiedTimer.m_unk0;
    u32 packedTime = machine->mUnidentifiedTimer.m_uPackedTime;
    float secondDuration = machine->mUnidentified07C;
    float duration = machine->mUnidentified078;

    bool result = fn_80316A84(machine, parameters, false);

    machine->mUnidentifiedTimer.m_unk0 = timerState;
    machine->mUnidentifiedTimer.m_uPackedTime = packedTime;
    machine->mUnidentified07C = secondDuration;
    machine->mUnidentified078 = duration;

    if (reinitialize)
    {
        result = machine->UnidentifiedReinitialize(parameters);
    }
    return result;
}

extern "C" bool fn_80316A84(
    shdStateMachine* machine, UnidentifiedVariantCollection* parameters,
    bool initialize)
{
    machine->mUnidentified078 = 0.0f;
    machine->mUnidentified07C = 0.0f;
    machine->mUnidentified070 = lbl_806E20B8;

    if (parameters->IsSet(10))
    {
        Variant* value = parameters->Get(10);
        switch (value->GetType())
        {
        case FT_INT:
        case FT_U32:
            machine->mUnidentified070.mUnidentifiedHash = value->mData.u;
            machine->mUnidentified070.mUnidentifiedFunction = 0;
            break;
        case FT_POINTER:
            machine->mUnidentified070.mUnidentifiedHash = -1;
            machine->mUnidentified070.mUnidentifiedFunction = value->mData.pointer;
            break;
        case FT_STRING:
            machine->mUnidentified070.mUnidentifiedHash = nlStringHash(value->mData.string);
            machine->mUnidentified070.mUnidentifiedFunction = 0;
            break;
        }
    }

    if (parameters->IsSet(7))
    {
        machine->mUnidentified078 = parameters->Get(7)->mData.f;
    }
    if (machine->mUnidentified078 == 0.0f)
    {
        machine->mUnidentified078 = machine->mUnidentified084;
    }
    if (machine->mUnidentified07C == 0.0f)
    {
        machine->mUnidentified07C = machine->mUnidentified080;
    }

    machine->mUnidentifiedTimer.m_unk0 = machine->mUnidentifiedTimer.m_uPackedTime != 0;
    machine->mUnidentifiedTimer.m_uPackedTime = 0;

    bool result = true;
    if (initialize)
    {
        result = machine->UnidentifiedInitialize(parameters);
    }

    if (result)
    {
        for (int i = 0; i < 19; i++)
        {
            if (parameters->IsSet(i))
            {
                if (machine->mUnidentified01C.mData[i] == 0)
                {
                    machine->mUnidentified01C.mData[i] = new (lbl_80584200.Allocate())
                        UnidentifiedFuzzyVariantData(
                            i, FuzzyVariant(*parameters->mData[i]));
                }
                else
                {
                    *machine->mUnidentified01C.mData[i] = *parameters->mData[i];
                }
            }
            else if (machine->mUnidentified01C.IsSet(i))
            {
                machine->mUnidentified01C.Remove(i);
            }
        }
        machine->mUnidentifiedActive = true;
    }
    return result;
}

extern "C" void fn_80317010(
    shdStateMachine* machine, UnidentifiedVariant_80054AB8* update,
    bool runUpdate, float deltaTime)
{
    fn_800C2C18(update, 0);
    machine->mUnidentifiedTimer.Countup(deltaTime, 0.00001f);
    machine->mUnidentified014 = lbl_806DF564();

    float start = lbl_806DF560();
    if (fn_80317E34(&machine->mUnidentified070))
    {
        if (fn_80317E60(&machine->mUnidentified070))
        {
            fn_80315A64(&machine->mUnidentified070,
                fn_80317E2C(machine->mUnidentified018),
                update,
                (UnidentifiedFuzzyRuntimeValue*)machine);
        }
    }
    else if (fn_80317E34(&machine->mUnidentified068)
             && fn_80317E60(&machine->mUnidentified068))
    {
        fn_80315A64(&machine->mUnidentified068,
            fn_80317E2C(machine->mUnidentified018),
            update,
            (UnidentifiedFuzzyRuntimeValue*)machine);
    }
    fn_8031A0C8(start, lbl_806DF560());

    if ((unsigned int)update->GetType() == FT_UNSPECIFIED)
    {
        fn_800C2C18(update, 0);
    }
    if (fn_80317E88(machine) && fn_800C2BD4(update) != 1)
    {
        fn_800C2C18(update, 2);
    }
    if (runUpdate && fn_800C2BD4(update) != 1)
    {
        machine->UnidentifiedUpdate(
            (UnidentifiedDesireUpdate*)update, deltaTime);
    }
}

UnidentifiedStateMachine_803171D0::UnidentifiedStateMachine_803171D0(
    int state, const char* name, UnidentifiedScriptMachine* context,
    const UnidentifiedStateTransition& transition)
    : shdStateMachine(state, transition)
{
    UnidentifiedSetContext(context);

    char functionName[64];
    nlStrNCpy(functionName, lbl_806DF5A0, 63);
    nlStrNCat(functionName, functionName, name, 63);
    mUnidentified088 = nlStringHash(functionName);

    nlStrNCpy(functionName, lbl_806DF5A8, 63);
    nlStrNCat(functionName, functionName, name, 63);
    mUnidentified08C = nlStringHash(functionName);

    nlStrNCpy(functionName, lbl_80530248, 63);
    nlStrNCat(functionName, functionName, name, 63);
    mUnidentified090 = nlStringHash(functionName);

    UnidentifiedFuzzyRuntimeBase* runtime = fn_80311734(this);
    if (fn_802DF3E4(runtime, &mUnidentified088) == 0)
    {
        mUnidentified088 = 0;
    }
    if (fn_802DF3E4(runtime, &mUnidentified08C) == 0)
    {
        mUnidentified08C = 0;
    }
    if (fn_802DF3E4(runtime, &mUnidentified090) == 0)
    {
        mUnidentified090 = 0;
    }
}

bool UnidentifiedStateMachine_803171D0::UnidentifiedInitialize(void*)
{
    if (mUnidentified088 == 0)
    {
        return true;
    }

    float start = lbl_806DF560();
    void* context = mUnidentified018->mUnidentified064->mData.pointer;
    UnidentifiedVariant_80054AB8 result = fn_80317EFC(fn_80311734(this), &mUnidentified088, context);
    bool initialized = result.mData.b;
    fn_8031A0C8(start, lbl_806DF560());
    return initialized;
}

void UnidentifiedStateMachine_803171D0::UnidentifiedUpdate(
    UnidentifiedDesireUpdate* update, float deltaTime)
{
    if (update->mData.pointer != 0 || mUnidentified08C == 0)
    {
        return;
    }

    float start = lbl_806DF560();
    void* context = mUnidentified018->mUnidentified064->mData.pointer;
    UnidentifiedVariant_80054AB8 result = fn_803184A8(
        fn_80311734(this), &mUnidentified08C, context, deltaTime);
    *update = result;
    fn_8031A0C8(start, lbl_806DF560());
}

void UnidentifiedStateMachine_803171D0::UnidentifiedCleanup()
{
    if (mUnidentified090 == 0)
    {
        return;
    }

    float start = lbl_806DF560();
    void* context = mUnidentified018->mUnidentified064->mData.pointer;
    fn_80317EFC(fn_80311734(this), &mUnidentified090, context);
    fn_8031A0C8(start, lbl_806DF560());
}

UnidentifiedStateMachine_803171D0::~UnidentifiedStateMachine_803171D0()
{
}
