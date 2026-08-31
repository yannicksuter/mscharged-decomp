#ifndef GAME_AI_DESIRE_SLIDE_ATTACK_H
#define GAME_AI_DESIRE_SLIDE_ATTACK_H

#include "Game/AI/Desire.h"

extern UnidentifiedStateTransition lbl_806E20B8;

class DesireSlideAttack : public Desire
{
public:
    DesireSlideAttack()
        : Desire(16, UnidentifiedStateTransition(lbl_806E20B8))
    {
    }

    virtual ~DesireSlideAttack();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    cFielder* mpTarget;
    int meDesireSubState;
};

#endif // GAME_AI_DESIRE_SLIDE_ATTACK_H
