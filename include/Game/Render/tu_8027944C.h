#ifndef GAME_RENDER_TU_8027944C_H
#define GAME_RENDER_TU_8027944C_H

#include "Game/World/WorldAnimObjects.h"
#include "Game/World/WorldObject_80129EE0.h"

struct WorldObjectLoadContext;

class StadiumMarker_8027999C : public WorldObject_80129EE0
{
public:
    virtual ~StadiumMarker_8027999C();
    virtual void ReleaseResources();
    virtual void UnidentifiedVirtual1C(WorldObjectLoadContext* context);

    /* 0x60 */ unsigned char mUnidentified060[0x10];
}; // size: 0x70

class StadiumMarker_802799AC : public WorldObject_80129EE0
{
public:
    virtual ~StadiumMarker_802799AC();
    virtual void ReleaseResources();
    virtual void UnidentifiedVirtual1C(WorldObjectLoadContext* context);

    /* 0x60 */ unsigned char mUnidentified060[0x10];
}; // size: 0x70

class StadiumPhysicsObject_8027944C : public WorldPhysicsDrawable_80534448
{
public:
    virtual ~StadiumPhysicsObject_8027944C();
    virtual void ReleaseResources();
    virtual void UnidentifiedVirtual1C(WorldObjectLoadContext* context);
}; // size: 0x90

#endif
