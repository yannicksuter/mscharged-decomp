#ifndef GAME_AI_DESIRE_RUN_TO_NET_H
#define GAME_AI_DESIRE_RUN_TO_NET_H

#include "Game/AI/Desire.h"
#include "Game/AI/UnidentifiedStringHash.h"

class DesireRunToNet : public Desire
{
public:
    DesireRunToNet()
        : Desire(9, UnidentifiedStringHash("TransDesireRunToNet"))
    {
    }

    virtual ~DesireRunToNet();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    SpaceSearch* m_pSpaceSearch;
};

#endif // GAME_AI_DESIRE_RUN_TO_NET_H
