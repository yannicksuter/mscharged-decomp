#ifndef GAME_AI_SCRIPT_MACHINE_H
#define GAME_AI_SCRIPT_MACHINE_H

#include "Game/AI/Desire.h"
#include "Game/AI/TransitionFunc.h"
#include "Game/AI/Variant.h"

class ScriptQuestionCache;

class ScriptMachine
{
public:
    ScriptMachine(
        int stateCount, bool ownsStates, AIContext* context, const char* name);
    virtual ~ScriptMachine();

    virtual bool IsIdle() const;
    virtual void Initialize();
    virtual void Update(float deltaTime);
    virtual void Reset(bool deleting);
    virtual shdStateMachine* ActivateState(
        int state, UnidentifiedVariantCollection* parameters, bool reinitialize);
    virtual void DeactivateState();
    virtual void SelectState();
    virtual void OnBudgetCheckFailed()
    {
    }

    void SetTransition(const char* name)
    {
        ScriptTransitionFunc transition(name);
        mTransition = transition.mValue;
    }

    void AddState(int state, shdStateMachine* machine, bool concurrent);

    shdStateMachine* GetState(int state) const
    {
        if (state >= 0 && state < mStateCount)
        {
            return mStates[state];
        }
        return 0;
    }

    shdStateMachine* GetActiveState() const { return mActiveState; }
    FuzzyRuntimeBase* GetFuzzyRuntime();

    shdStateMachine* mActiveState;
    shdStateMachine* mPreviousState;
    UnsetTransitionFunc mTransition;
    int mPendingState;
    UnidentifiedVariantCollection mPendingParameters;
    AIContext* mAIContext;
    bool mOwnsStates;
    u8 mPadding069[3];
    shdStateMachine** mStates;
    shdStateMachine** mConcurrentStates;
    int mStateCount;
    char mName[0x40];
};

void DeactivateScriptMachineState(ScriptMachine* machine, shdStateMachine* state);
AIContext* GetScriptMachineAIContext(ScriptMachine* machine);
bool HasStateMachineTimedOut(const shdStateMachine* machine);
UnidentifiedVariant_80054AB8 ExecuteScriptStateFunction(FuzzyRuntimeBase* runtime, const u32& hash, void* argument);
UnidentifiedVariant_80054AB8 ExecuteScriptStateFunction(FuzzyRuntimeBase* runtime, const u32& hash, void* argument, float value);
void QueueScriptMachineState(ScriptMachine* machine, int state, const UnidentifiedVariantCollection* parameters);

extern const float gStateMachineZeroDuration;
extern const float gStateMachineUnsetDuration;
extern const float gStateMachineNeverActiveTime;
extern const float gStateMachineAgeThreshold;

bool IsTransitionFuncSet(const TransitionFunc* transition);
bool HasTransitionFunc(const TransitionFunc* transition);
void AddScriptState(ScriptMachine* machine, int state, const char* name, bool concurrent);
void DeactivateConcurrentStates(ScriptMachine* machine);
void DeactivateScriptMachine(ScriptMachine* machine);
void DeactivateConcurrentState(ScriptMachine* machine, int state);
shdStateMachine* ActivateConcurrentState(ScriptMachine* machine, int state, UnidentifiedVariantCollection* parameters, bool reinitialize);
shdStateMachine* GetScriptMachineState(ScriptMachine* machine, int state);
shdStateMachine* GetConcurrentState(ScriptMachine* machine, int state);
bool IsConcurrentStateActive(ScriptMachine* machine, int state);
void ResetScriptFrameTime(ScriptQuestionCache*);
bool CheckScriptTimeBudget();
float AccumulateScriptExecutionTime(float start, float end);
void SetScriptTimeBudget(float value);

#endif // GAME_AI_SCRIPT_MACHINE_H
