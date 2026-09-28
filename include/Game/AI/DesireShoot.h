#ifndef GAME_AI_DESIRE_SHOOT_H
#define GAME_AI_DESIRE_SHOOT_H

#include "Game/AI/Desire.h"

class DesireWindupShot : public Desire
{
public:
    DesireWindupShot(
        int state, const TransitionFunc& transition)
        : Desire(state, transition)
    {
    }

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    bool mbShotMeterActivated;
};

class DesireShoot : public Desire
{
public:
    DesireShoot(int state, const TransitionFunc& transition)
        : Desire(state, transition)
    {
    }

    virtual bool UnidentifiedInitialize(void*);
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    bool mbLobShot;
};

#endif // GAME_AI_DESIRE_SHOOT_H
