#ifndef GAME_AI_FIELDER_DESIRE_MACHINE_H
#define GAME_AI_FIELDER_DESIRE_MACHINE_H

#include "Game/AI/TeamPlayMachine.h"

class FielderDesireMachine : public UnidentifiedScriptMachine
{
public:
    FielderDesireMachine();
    virtual ~FielderDesireMachine();

    virtual void UnidentifiedVirtual2();
    virtual void Update(float deltaTime);
    virtual void Reset(bool deleting);
    virtual shdStateMachine* UnidentifiedVirtual5(
        int state, UnidentifiedVariantCollection* params, bool force);
    virtual void UnidentifiedVirtual6();
    virtual void UnidentifiedVirtual7();
    virtual void UnidentifiedVirtual8();

private:
    cFielder* GetFielder() const;
};

#endif // GAME_AI_FIELDER_DESIRE_MACHINE_H
