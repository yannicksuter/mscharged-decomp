#ifndef GAME_AI_DESIRE_SHOOT_H
#define GAME_AI_DESIRE_SHOOT_H

#include "Game/AI/Desire.h"

class DesireShoot : public Desire
{
public:
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
