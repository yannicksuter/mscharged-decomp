#ifndef GAME_WORLD_WORLD_ANIM_OBJECTS_H
#define GAME_WORLD_WORLD_ANIM_OBJECTS_H

#include "Game/World/WorldObject.h"
#include "Game/World/WorldPhysicsDescription.h"
#include "NL/nlMath.h"
#include "types.h"

class GLView;
class glModel;
class PhysicsObject;
class WorldAnimController;
struct WorldVisibilityNode;

class WorldAnimDrawable_80343A40 : public WorldObject
{
public:
    virtual ~WorldAnimDrawable_80343A40();
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);
    virtual void Draw();
    virtual bool V6(const nlVector4* planes);
    virtual void V7(glModel* model);
    virtual void V8(GLView* view);

    /* 0x04 */ u8 m_pad04[0x0C];
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ int m_nAnimNode;
    /* 0x18 */ WorldAnimController* m_pAnimController;
    /* 0x1C */ u8 m_pad1C[0x04];
    /* 0x20 */ nlMatrix4 m_transform;
    /* 0x60 */ float m_fRadius;
    /* 0x64 */ glModel* m_pModel;
    /* 0x68 */ u8 m_pad68[0x08];
}; // size: 0x70

class WorldVertexAnimDrawable_80343E3C : public WorldObject
{
public:
    virtual ~WorldVertexAnimDrawable_80343E3C();
    virtual void Draw();
    virtual bool IsVisible();

    glModel* GetModel() const { return m_pModel; }

    /* 0x04 */ u8 m_pad04[0x0C];
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ u8 m_pad14[0x0C];
    /* 0x20 */ glModel* m_pModel;
    /* 0x24 */ WorldVisibilityNode* m_pVertexAnimNode;
    /* 0x28 */ u8 m_pad28[0x08];
};

class WorldObjectBase_803416DC : public WorldObject
{
public:
    WorldObjectBase_803416DC() { m_uObjectCreationFlags |= 4; }

    /* 0x04 */ u8 m_pad04[0x08];
    /* 0x0C */ unsigned long m_uObjectCreationFlags;
}; // size: 0x10

class WorldPhysicsDrawable_80534448 : public WorldObjectBase_803416DC
{
public:
    virtual ~WorldPhysicsDrawable_80534448();
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);

    /* 0x10 */ u8 m_pad10[0x10];
    /* 0x20 */ WorldPhysicsDescription_80341EEC m_Description;
    /* 0x74 */ u8 m_pad74[0x0C];
    /* 0x80 */ PhysicsObject* m_pPhysicsObject;
    /* 0x84 */ u8 m_pad84[0x0C];
}; // size: 0x90

typedef char WorldAnimDrawable_80343A40_size_check[
    sizeof(WorldAnimDrawable_80343A40) == 0x70 ? 1 : -1];
typedef char WorldPhysicsDrawable_80534448_size_check[
    sizeof(WorldPhysicsDrawable_80534448) == 0x90 ? 1 : -1];

#endif // GAME_WORLD_WORLD_ANIM_OBJECTS_H
