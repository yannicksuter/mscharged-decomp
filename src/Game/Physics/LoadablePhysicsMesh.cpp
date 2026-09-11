#include "Game/Physics/LoadablePhysicsMesh.h"

/**
 * Offset/Address/Size: 0x0 | 0x8013A25C | size: 0x20
 */
void LoadablePhysicsMesh::Destroy()
{
    delete this;
}

// The retail body is not in the image: nothing references this override, so
// the linker dropped it together with the class vtable it keys. Type 0x10 is
// what PhysicsShell and PhysicsNPC test their collision partner for after
// GetObjectType(), and no other physics class returns it.
int LoadablePhysicsMesh::GetObjectType() const
{
    return 0x10;
}
