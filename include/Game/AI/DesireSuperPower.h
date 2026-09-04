#ifndef GAME_AI_DESIRE_SUPER_POWER_H
#define GAME_AI_DESIRE_SUPER_POWER_H

#include "Game/AI/Desire.h"

extern UnidentifiedStateTransition lbl_806E20B8;

class DesireSuperPower : public Desire
{
public:
    DesireSuperPower();
    virtual ~DesireSuperPower();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedSetContext(UnidentifiedScriptMachine*);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    void* mpDKShockAvoidable;
    cFielder* mpTarget;
    nlVector2 mUnidentifiedPositions[8];
};

#endif // GAME_AI_DESIRE_SUPER_POWER_H
