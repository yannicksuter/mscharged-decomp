#ifndef GAME_AI_DESIRE_SUPER_POWER_H
#define GAME_AI_DESIRE_SUPER_POWER_H

#include "Game/AI/Desire.h"

extern UnidentifiedStateTransition lbl_806E20B8;

class DesireSuperPower;
extern "C" bool fn_800D0DB0(DesireSuperPower*, void*);
extern "C" bool fn_800C9F4C(DesireSuperPower*, void*);
extern "C" bool fn_800CAC8C(DesireSuperPower*, void*);
extern "C" void fn_800C9D74(DesireSuperPower*, int);
extern "C" int fn_800D0004(DesireSuperPower*);
extern "C" void fn_800C9DB4(DesireSuperPower*);

class DesireSuperPower : public Desire
{
    friend bool fn_800D0DB0(DesireSuperPower*, void*);
    friend bool fn_800C9F4C(DesireSuperPower*, void*);
    friend bool fn_800CAC8C(DesireSuperPower*, void*);
    friend void fn_800C9D74(DesireSuperPower*, int);
    friend int fn_800D0004(DesireSuperPower*);
    friend void fn_800C9DB4(DesireSuperPower*);

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
