#ifndef GAME_AI_DESIRE_SLIDE_ATTACK_H
#define GAME_AI_DESIRE_SLIDE_ATTACK_H

#include "Game/AI/Desire.h"

extern UnidentifiedUnsetTransition lbl_806E20B8;

class DesireSlideAttack : public Desire
{
public:
    DesireSlideAttack()
        : Desire(16, UnidentifiedStateTransition(lbl_806E20B8))
    {
    }

    virtual inline ~DesireSlideAttack();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(UnidentifiedDesireUpdate*, float);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);

private:
    cFielder* mpTarget;
    int meDesireSubState;
};

#endif // GAME_AI_DESIRE_SLIDE_ATTACK_H
