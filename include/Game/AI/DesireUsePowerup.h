#ifndef GAME_AI_DESIRE_USE_POWERUP_H
#define GAME_AI_DESIRE_USE_POWERUP_H

#include "Game/AI/Desire.h"
#include "Game/AI/Powerups.h"

class DesireUsePowerup;
class AIContext;
DesireUpdate TransDesireUsePowerup(
    AIContext*);
extern "C" void fn_800D38D0(DesireUsePowerup*);
void ThrowPowerup(DesireUsePowerup*);

class DesireUsePowerup : public Desire
{
    friend void fn_800D38D0(DesireUsePowerup*);
    friend void ThrowPowerup(DesireUsePowerup*);
public:
    DesireUsePowerup()
        : Desire(17, UnsetTransitionFunc(g_UnsetTransitionFunc))
        , mePowerup(POWER_UP_NONE)
    {
    }

    virtual inline ~DesireUsePowerup();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

    ePowerUpType GetPowerupType() const { return mePowerup; }
    void fn_800D3968(cFielder*, ePowerUpType, bool);

private:
    void SetPowerup(ePowerUpType, int, cFielder*);
    inline void ResetPowerupState();

    cFielder* mpTarget;
    bool mbThrowingPowerup;
    ePowerUpType mePowerup;
    int mnNumPowerups;
    Timer mtPowerupEffectTime;
};

#endif // GAME_AI_DESIRE_USE_POWERUP_H
