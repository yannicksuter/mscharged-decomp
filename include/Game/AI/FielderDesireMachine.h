#ifndef GAME_AI_FIELDER_DESIRE_MACHINE_H
#define GAME_AI_FIELDER_DESIRE_MACHINE_H

#include "Game/AI/ScriptMachine.h"

class FielderDesireMachine : public ScriptMachine
{
public:
    FielderDesireMachine();
    virtual ~FielderDesireMachine();

    virtual void Initialize();
    virtual void Update(float deltaTime);
    virtual void Reset(bool deleting);
    virtual shdStateMachine* ActivateState(
        int state, UnidentifiedVariantCollection* params, bool force);
    virtual void DeactivateState();
    virtual void SelectState();
    virtual void OnBudgetCheckFailed();

private:
    cFielder* GetFielder() const;
};

#endif // GAME_AI_FIELDER_DESIRE_MACHINE_H
