#ifndef GAME_RENDER_STADIUM_PHYSICS_OBJECT_H
#define GAME_RENDER_STADIUM_PHYSICS_OBJECT_H

#include "Game/World/WorldAnimObjects.h"
#include "Game/World/WorldObject_80129EE0.h"

struct WorldObjectLoadContext;

// Registers itself with FEModelManager; its users look it up there by the id
// at +0x60 and place models at its world matrix.
class StadiumFEModelMarker : public WorldObject_80129EE0
{
public:
    virtual ~StadiumFEModelMarker();
    virtual void ReleaseResources();
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x60 */ int mUnidentified060;
    /* 0x64 */ unsigned char mUnidentified064[0xC];
}; // size: 0x70

// Sets the current stadium's shadow height to the z of its world position.
class StadiumShadowHeightMarker : public WorldObject_80129EE0
{
public:
    virtual ~StadiumShadowHeightMarker();
    virtual void ReleaseResources();
    virtual void Initialize(WorldObjectLoadContext* context);

    /* 0x60 */ unsigned char mUnidentified060[0x10];
}; // size: 0x70

// Builds its static physics primitive when loaded and removes it on release.
class StadiumPhysicsObject : public WorldPhysicsDrawable
{
public:
    virtual ~StadiumPhysicsObject();
    virtual void ReleaseResources();
    virtual void Initialize(WorldObjectLoadContext* context);
}; // size: 0x90

#endif
