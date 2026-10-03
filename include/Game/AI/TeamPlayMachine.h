#ifndef GAME_AI_TEAM_PLAY_MACHINE_H
#define GAME_AI_TEAM_PLAY_MACHINE_H

#include "Game/AI/Desire.h"
#include "Game/AI/TransitionFunc.h"
#include "Game/AI/Variant.h"

class cTeam;
class ScriptQuestionCache;
class UnidentifiedVariantCollection;

class UnidentifiedScriptMachine
{
public:
    UnidentifiedScriptMachine(
        int, bool, AIContext*, const char*);
    virtual ~UnidentifiedScriptMachine();

    virtual bool UnidentifiedVirtual1() const
    {
        return mUnidentified004 == 0;
    }
    virtual void UnidentifiedVirtual2();
    virtual void Update(float deltaTime);
    virtual void Reset(bool);
    virtual shdStateMachine* UnidentifiedVirtual5(
        int, UnidentifiedVariantCollection*, bool);
    virtual void UnidentifiedVirtual6();
    virtual void UnidentifiedVirtual7();
    virtual void UnidentifiedVirtual8()
    {
    }

    void SetTransition(const char* name)
    {
        ScriptTransitionFunc transition(name);
        mTransition = transition.mValue;
    }

    void UnidentifiedAddState(int, shdStateMachine*, bool);

    shdStateMachine* UnidentifiedGet06C(int state) const
    {
        if (state >= 0 && state < mUnidentified074)
        {
            return mUnidentified06C[state];
        }
        return 0;
    }

    shdStateMachine* fn_800C2F20() const { return mUnidentified004; }
    FuzzyRuntimeBase* GetFuzzyRuntime();

    shdStateMachine* mUnidentified004;
    shdStateMachine* mUnidentified008;
    UnsetTransitionFunc mTransition;
    int mUnidentified014;
    UnidentifiedVariantCollection mUnidentified018;
    AIContext* mAIContext;
    bool mUnidentified068;
    u8 mPadding069[3];
    shdStateMachine** mUnidentified06C;
    shdStateMachine** mUnidentified070;
    int mUnidentified074;
    char mUnidentified078[0x40];
};

class TeamPlayMachine : public UnidentifiedScriptMachine
{
public:
    TeamPlayMachine();
    virtual ~TeamPlayMachine();

    virtual void UnidentifiedVirtual2();
    virtual void Update(float deltaTime);
    virtual void UnidentifiedVirtual7();
    virtual void UnidentifiedVirtual8();
};

class TeamDesire : public shdStateMachine
{
public:
    TeamDesire(
        int state, TransitionFunc& transition);
    virtual ~TeamDesire()
    {
    }

    virtual bool Initialize(void*) = 0;
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void SetContext(UnidentifiedScriptMachine*);

protected:
    cTeam* m_pTeam;
};

class TutorialMegastrikeDesire : public TeamDesire
{
public:
    TutorialMegastrikeDesire(
        int state, TransitionFunc transition)
        : TeamDesire(state, transition)
    {
    }

    virtual ~TutorialMegastrikeDesire();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
};

class cFielder;
UnidentifiedScriptMachine* fn_8002E1A4(cFielder* pFielder);


extern "C" void fn_80319904(UnidentifiedScriptMachine* machine, shdStateMachine* state);
extern "C" AIContext* fn_80317E2C(UnidentifiedScriptMachine* machine);
extern "C" bool fn_80317E88(const shdStateMachine* machine);
extern "C" UnidentifiedVariant_80054AB8 fn_80317EFC(FuzzyRuntimeBase* runtime, const u32& hash, void* argument);
extern "C" UnidentifiedVariant_80054AB8 fn_803184A8(FuzzyRuntimeBase* runtime, const u32& hash, void* argument, float value);
extern "C" void fn_8031998C(UnidentifiedScriptMachine* machine, int state, const UnidentifiedVariantCollection* parameters);

extern const float lbl_806E6880;
extern const float lbl_806E6884;
extern const float lbl_806E6888;
extern const float lbl_806E688C;

bool IsTransitionFuncSet(const TransitionFunc* transition);
bool HasTransitionFunc(const TransitionFunc* transition);
extern "C" void fn_80318D34( UnidentifiedScriptMachine* machine, int state, const char* name, bool secondary);
extern "C" void fn_80319DA0(UnidentifiedScriptMachine* machine);
extern "C" void fn_803198F4(UnidentifiedScriptMachine* machine);
extern "C" void fn_80319E58(UnidentifiedScriptMachine* machine, int state);
extern "C" shdStateMachine* fn_80319E84(UnidentifiedScriptMachine* machine, int state, UnidentifiedVariantCollection* parameters, bool reinitialize);
extern "C" shdStateMachine* fn_80319F94( UnidentifiedScriptMachine* machine, int state);
extern "C" shdStateMachine* fn_80319FC0(UnidentifiedScriptMachine* machine, int state);
extern "C" bool fn_80319FEC(UnidentifiedScriptMachine* machine, int state);
extern "C" void fn_8031A02C(ScriptQuestionCache*);
extern "C" bool fn_8031A04C();
extern "C" float fn_8031A0C8(float start, float end);
extern "C" void fn_8031A0FC(float value);

#endif // GAME_AI_TEAM_PLAY_MACHINE_H
