#ifndef GAME_AI_DESIRE_RUN_TO_NET_H
#define GAME_AI_DESIRE_RUN_TO_NET_H

#include "Game/AI/Desire.h"
#include "Game/AI/TransitionFunc.h"

class DesireRunToNet : public Desire
{
public:
    DesireRunToNet(const TransitionFunc& transition)
        : Desire(9, transition)
    {
    }

    virtual inline ~DesireRunToNet();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    SpaceSearch* m_pSpaceSearch;
};

#endif // GAME_AI_DESIRE_RUN_TO_NET_H
