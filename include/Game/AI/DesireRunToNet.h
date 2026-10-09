#ifndef GAME_AI_DESIRE_RUN_TO_NET_H
#define GAME_AI_DESIRE_RUN_TO_NET_H

#include "Game/AI/FielderDesireTypes.h"
#include "Game/AI/Desire.h"
#include "Game/AI/TransitionFunc.h"

class DesireRunToNet : public Desire
{
public:
    DesireRunToNet()
        : Desire(FIELDER_DESIRE_RUN_TO_NET, ScriptTransitionFunc("TransDesireRunToNet"))
    {
    }

    virtual inline ~DesireRunToNet();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void SyncLog(void*, DebugWriteCache*);
    virtual inline void RegisterDebugFields(void*, DebugWriteCache*);

private:
    SpaceSearch* m_pSpaceSearch;
};

#endif // GAME_AI_DESIRE_RUN_TO_NET_H
