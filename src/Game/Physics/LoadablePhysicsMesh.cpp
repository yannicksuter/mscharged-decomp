#include "Game/Physics/LoadablePhysicsMesh.h"

/**
 * Offset/Address/Size: 0x0 | 0x8013A25C | size: 0x20
 */
void LoadablePhysicsMesh::Destroy()
{
    delete this;
}

// The current link does not retain this override or the class vtable.
// Its reconstructed type code remains unverified.
int LoadablePhysicsMesh::GetObjectType() const
{
    return 0x10;
}
