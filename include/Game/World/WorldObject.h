#ifndef GAME_WORLD_WORLD_OBJECT_H
#define GAME_WORLD_WORLD_OBJECT_H

class nlMatrix4;
class World;
struct WorldObjectLoadContext;

// Polymorphic base of the objects World creates from its object stream.
// Every derived vtable shares the slot 0x18 body at 0x80278E94, which
// World::InitializeObjects calls with the owning world. Only the slot
// layout is known; derived classes keep their own data.
class WorldObject
{
public:
    virtual ~WorldObject() { }
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4& transform);
    virtual void UnidentifiedVirtual18(World* world);
};

// The 0x104 stream object uses the vtable retained with BasicStadium.
class WorldObject_805223B0 : public WorldObject
{
public:
    virtual ~WorldObject_805223B0();
    virtual void UnidentifiedVirtual1C(WorldObjectLoadContext* context);

    /* 0x04 */ unsigned char mUnidentified004[0x5C];
}; // size: 0x60

#endif // GAME_WORLD_WORLD_OBJECT_H
