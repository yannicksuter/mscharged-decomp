#ifndef GAME_AI_DESIRE_USE_POWERUP_H
#define GAME_AI_DESIRE_USE_POWERUP_H

#include "Game/AI/Desire.h"
#include "Game/AI/Powerups.h"

extern UnidentifiedUnsetTransition lbl_806E20B8;

class DesireUsePowerup : public Desire
{
public:
    DesireUsePowerup()
        : Desire(17, UnidentifiedStateTransition(lbl_806E20B8))
        , mePowerup(POWER_UP_NONE)
    {
    }

    virtual ~DesireUsePowerup();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    ePowerUpType GetPowerupType() const { return mePowerup; }
    void fn_800D3968(cFielder*, ePowerUpType, bool);

private:
    void fn_800D3A50(ePowerUpType, int, cFielder*);

    cFielder* mpTarget;
    bool mbThrowingPowerup;
    ePowerUpType mePowerup;
    int mnNumPowerups;
    Timer mtPowerupEffectTime;
};

#endif // GAME_AI_DESIRE_USE_POWERUP_H
