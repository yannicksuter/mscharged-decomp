#ifndef GAME_AI_DESIRE_USER_CONTROLLED_H
#define GAME_AI_DESIRE_USER_CONTROLLED_H

#include "Game/AI/FielderDesireTypes.h"
#include "Game/AI/Desire.h"


class DesireUserControlled : public Desire
{
public:
    DesireUserControlled()
        : Desire(FIELDER_DESIRE_USER_CONTROLLED, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual inline ~DesireUserControlled();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void SyncLog(void*, DebugWriteCache*);
    virtual inline void RegisterDebugFields(void*, DebugWriteCache*);
};

#endif // GAME_AI_DESIRE_USER_CONTROLLED_H
