#ifndef GAME_AI_DESIRE_USER_CONTROLLED_H
#define GAME_AI_DESIRE_USER_CONTROLLED_H

#include "Game/AI/Desire.h"

extern UnidentifiedUnsetTransition lbl_806E20B8;

class DesireUserControlled : public Desire
{
public:
    DesireUserControlled()
        : Desire(20, UnidentifiedStateTransition(lbl_806E20B8))
    {
    }

    virtual ~DesireUserControlled();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

#endif // GAME_AI_DESIRE_USER_CONTROLLED_H
