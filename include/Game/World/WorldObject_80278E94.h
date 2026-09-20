#ifndef GAME_WORLD_WORLD_OBJECT_80278E94_H
#define GAME_WORLD_WORLD_OBJECT_80278E94_H

class nlMatrix4;
class World;

// Polymorphic base of the objects World creates from its object stream.
// Every derived vtable shares the slot 0x18 body at 0x80278E94, which
// World::InitializeObjects calls with the owning world. Only the slot
// layout is known; derived classes keep their own data.
class WorldObject_80278E94
{
public:
    virtual ~WorldObject_80278E94() { }
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);
    virtual void UnidentifiedVirtual18(World* world);
};

#endif // GAME_WORLD_WORLD_OBJECT_80278E94_H
