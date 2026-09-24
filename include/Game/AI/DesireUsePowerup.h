#ifndef GAME_AI_DESIRE_USE_POWERUP_H
#define GAME_AI_DESIRE_USE_POWERUP_H

#include "Game/AI/Desire.h"
#include "Game/AI/Powerups.h"

extern UnidentifiedUnsetTransition lbl_806E20B8;
class DesireUsePowerup;
class UnidentifiedFielderInput;
extern "C" DesireUpdate fn_800D2074(
    UnidentifiedFielderInput*);
extern "C" void fn_800D38D0(DesireUsePowerup*);
extern "C" void fn_800D3CBC(DesireUsePowerup*);

class DesireUsePowerup : public Desire
{
    friend void fn_800D38D0(DesireUsePowerup*);
    friend void fn_800D3CBC(DesireUsePowerup*);
public:
    DesireUsePowerup()
        : Desire(17, UnidentifiedStateTransition(lbl_806E20B8))
        , mePowerup(POWER_UP_NONE)
    {
    }

    virtual inline ~DesireUsePowerup();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

    ePowerUpType GetPowerupType() const { return mePowerup; }
    void fn_800D3968(cFielder*, ePowerUpType, bool);

private:
    void fn_800D3A50(ePowerUpType, int, cFielder*);
    inline void UnidentifiedResetPowerupState();

    cFielder* mpTarget;
    bool mbThrowingPowerup;
    ePowerUpType mePowerup;
    int mnNumPowerups;
    Timer mtPowerupEffectTime;
};

#endif // GAME_AI_DESIRE_USE_POWERUP_H
