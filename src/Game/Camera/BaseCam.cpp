#include "Game/Camera/BaseCam.h"
#include "Game/UnidentifiedStaticStorage.h"

// The retail unit emits cBaseCamera's weak inline destructor ahead of this
// unit's own GetFOV/Reactivate, which MWCC only does when a function compiled
// earlier in the unit requested it. That function is not in the image: the
// linker dropped it as unreferenced (it does so for static and extern alike).
// Its real name, signature and linkage are unrecoverable from a stripped DOL;
// what retail fixes is that it destroyed a camera through a base pointer and
// that no surviving code called it.
static void UnidentifiedCameraDisposal(cBaseCamera* pCamera)
{
    delete pCamera;
}

float cBaseCamera::GetFOV() const
{
    return 27.0f;
}

void cBaseCamera::Reactivate()
{
}
