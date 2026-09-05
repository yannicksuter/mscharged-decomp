#ifndef GAME_AI_FIELDER_DESIRE_MACHINE_H
#define GAME_AI_FIELDER_DESIRE_MACHINE_H

#include "Game/AI/TeamPlayMachine.h"

class UnidentifiedFielderDesireMachine : public UnidentifiedScriptMachine
{
public:
    UnidentifiedFielderDesireMachine();
    virtual ~UnidentifiedFielderDesireMachine();

    virtual void UnidentifiedVirtual2();
    virtual void UnidentifiedVirtual3(float deltaTime);
    virtual void UnidentifiedVirtual4(bool param);
    virtual shdStateMachine* UnidentifiedVirtual5(
        int state, UnidentifiedVariantCollection* params, bool force);
    virtual void UnidentifiedVirtual6();
    virtual void UnidentifiedVirtual7();
    virtual void UnidentifiedVirtual8();

private:
    cFielder* GetFielder() const;
};

#endif // GAME_AI_FIELDER_DESIRE_MACHINE_H
