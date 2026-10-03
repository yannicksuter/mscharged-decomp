#ifndef GAME_WORLD_WORLD_ANIM_OBJECTS_H
#define GAME_WORLD_WORLD_ANIM_OBJECTS_H

#include "Game/World/WorldObject.h"
#include "Game/World/WorldDrawable.h"
#include "Game/World/WorldPhysicsDescription.h"
#include "NL/nlMath.h"
#include "types.h"

class GLView;
class glModel;
class PhysicsObject;
class WorldAnimController;
class WorldAnimObject;
struct WorldVisibilityNode;

struct WorldAnimBinding
{
    unsigned long m_uDrawableHash;
    unsigned long m_uNodeHash;
};

class WorldVisibilityDrawable : public WorldObject
{
public:
    virtual ~WorldVisibilityDrawable();
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);
    virtual void Draw();
    virtual bool IsVisible();

    glModel* GetModel() const { return m_pModel; }

    /* 0x04 */ u8 m_pad04[0x0C];
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ u8 m_pad14[0x0C];
    /* 0x20 */ glModel* m_pModel;
    /* 0x24 */ WorldVisibilityNode* m_pVisibilityNode;
    /* 0x28 */ u8 m_pad28[0x08];
};

class WorldObjectBase_803416DC : public WorldObject
{
public:
    WorldObjectBase_803416DC() { m_uObjectCreationFlags |= 4; }

    /* 0x04 */ u8 m_pad04[0x08];
    /* 0x0C */ unsigned long m_uObjectCreationFlags;
}; // size: 0x10

class WorldPhysicsDrawable : public WorldObjectBase_803416DC
{
public:
    virtual ~WorldPhysicsDrawable() { }
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);

    /* 0x10 */ u8 m_pad10[0x10];
    /* 0x20 */ WorldPhysicsDescription m_Description;
    /* 0x74 */ u8 m_pad74[0x0C];
    /* 0x80 */ PhysicsObject* m_pPhysicsObject;
    /* 0x84 */ u8 m_pad84[0x0C];
}; // size: 0x90

void BindWorldAnimObjectDrawables(WorldAnimObject* pAnimObject);
void SelectRandomWorldAnimation(WorldAnimObject* pAnimObject);
void InitializeWorldVisibilityDrawable(WorldVisibilityDrawable* pDrawable,
    WorldObjectLoadContext* pContext);
WorldVisibilityNode* FindWorldVisibilityNode(WorldVisibilityDrawable* pDrawable,
    WorldVisibilityNode* pNode);
void InitializeWorldPhysicsDrawable(WorldPhysicsDrawable* pDrawable,
    WorldObjectLoadContext* pContext);

typedef char WorldDrawable_size_check[
    sizeof(WorldDrawable) == 0x70 ? 1 : -1];
typedef char WorldPhysicsDrawable_size_check[
    sizeof(WorldPhysicsDrawable) == 0x90 ? 1 : -1];

#endif // GAME_WORLD_WORLD_ANIM_OBJECTS_H
