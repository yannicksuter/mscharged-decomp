#ifndef GAME_AI_DESIRE_USER_CONTROLLED_H
#define GAME_AI_DESIRE_USER_CONTROLLED_H

#include "Game/AI/Desire.h"


class DesireUserControlled : public Desire
{
public:
    DesireUserControlled(UnidentifiedStateTransition transition)
        : Desire(20, transition)
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
