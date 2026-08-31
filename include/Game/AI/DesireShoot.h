#ifndef GAME_AI_DESIRE_SHOOT_H
#define GAME_AI_DESIRE_SHOOT_H

#include "Game/AI/Desire.h"

class DesireShoot : public Desire
{
public:
    DesireShoot(int state, const UnidentifiedStateTransition& transition)
        : Desire(state, transition)
    {
    }

    virtual ~DesireShoot();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    bool mbLobShot;
};

class DesireWindupShot : public Desire
{
public:
    DesireWindupShot(
        int state, const UnidentifiedStateTransition& transition)
        : Desire(state, transition)
    {
    }

    virtual ~DesireWindupShot();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    bool mbShotMeterActivated;
};

#endif // GAME_AI_DESIRE_SHOOT_H
