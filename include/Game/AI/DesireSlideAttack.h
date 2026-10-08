#ifndef GAME_AI_DESIRE_SLIDE_ATTACK_H
#define GAME_AI_DESIRE_SLIDE_ATTACK_H

#include "Game/AI/Desire.h"


enum eDesireSlideAttackState
{
    DESIRE_SLIDE_APPROACH = 0,
    DESIRE_SLIDE_ATTACKING = 1,
    DESIRE_SLIDE_RECOVER = 2,
};

class DesireSlideAttack : public Desire
{
public:
    DesireSlideAttack()
        : Desire(16, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual inline ~DesireSlideAttack();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void RegisterDebugFields(void*, DebugWriteCache*);
    virtual inline void SyncLog(void*, DebugWriteCache*);

private:
    cFielder* mpTarget;
    eDesireSlideAttackState meDesireSubState;
};

#endif // GAME_AI_DESIRE_SLIDE_ATTACK_H
