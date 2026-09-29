#ifndef GAME_AI_DESIRE_SLIDE_ATTACK_H
#define GAME_AI_DESIRE_SLIDE_ATTACK_H

#include "Game/AI/Desire.h"


class DesireSlideAttack : public Desire
{
public:
    DesireSlideAttack()
        : Desire(16, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual inline ~DesireSlideAttack();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);

private:
    cFielder* mpTarget;
    int meDesireSubState;
};

#endif // GAME_AI_DESIRE_SLIDE_ATTACK_H
