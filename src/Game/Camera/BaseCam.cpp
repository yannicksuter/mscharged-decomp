#include "Game/Camera/BaseCam.h"
#include "Game/UnidentifiedStaticStorage.h"

// In this reconstruction, the disposal function causes MWCC to emit the weak
// destructor before GetFOV and Reactivate; the disposal function itself is
// discarded at link time. The retail destructor order does not establish an
// original disposal function, its signature or its source owner.
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
