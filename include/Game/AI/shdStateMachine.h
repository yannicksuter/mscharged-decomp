#ifndef GAME_AI_SHDSTATEMACHINE_H
#define GAME_AI_SHDSTATEMACHINE_H

#include "Game/AI/DesireUpdate.h"
#include "Game/AI/TransitionFunc.h"
#include "NL/nlTimer.h"
#include "types.h"

class AIContext;
class FuzzyRuntimeBase;
class UnidentifiedScriptMachine;
typedef UnidentifiedVariant_80054AB8 DesireUpdate;

class shdStateMachine
{
public:
    shdStateMachine(int, TransitionFunc&);
    virtual ~shdStateMachine();

    virtual bool Initialize(void*) = 0;
    virtual bool Reinitialize(void*) = 0;
    virtual void Cleanup() = 0;
    virtual void Update(DesireUpdate*, float) = 0;
    virtual void Reset(bool);
    virtual void SetContext(UnidentifiedScriptMachine*);

    FuzzyRuntimeBase* GetFuzzyRuntime();

    int GetState() const
    {
        return mState;
    }

    bool IsActive() const
    {
        return mActive;
    }

public:
    UnidentifiedScriptMachine* GetScriptMachine() const { return mScriptMachine; }

    int mState;
    bool mActive;
    u8 mPadding009[3];
    Timer mAgeTimer;
    float mLastActiveTime;
    UnidentifiedScriptMachine* mScriptMachine;
    UnidentifiedVariantCollection mParameters;
    // Copied from the constructor; executed when no override is set.
    UnsetTransitionFunc mDefaultTransition;
    // Supplied with the activation parameters (slot 10); takes precedence
    // over the default while set.
    UnsetTransitionFunc mOverrideTransition;
    float mMaxDuration;
    float mMinDuration;
    float mDefaultMinDuration;
    float mDefaultMaxDuration;
};

void RequestStateMachineDeactivation(shdStateMachine* machine);
AIContext* GetStateMachineAIContext(shdStateMachine* machine);
void DeactivateStateMachine(shdStateMachine* machine, bool cleanup);
bool ReinitializeStateMachine(shdStateMachine* machine, UnidentifiedVariantCollection* parameters, bool reinitialize);
bool InitializeStateMachine(shdStateMachine* machine, UnidentifiedVariantCollection* parameters, bool initialize);
void UpdateStateMachine(shdStateMachine* machine, UnidentifiedVariant_80054AB8* update, bool runUpdate, float deltaTime);

#endif // GAME_AI_SHDSTATEMACHINE_H
