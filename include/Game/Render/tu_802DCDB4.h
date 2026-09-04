#ifndef GAME_RENDER_TU_802DCDB4_H
#define GAME_RENDER_TU_802DCDB4_H

#include "NL/nlMath.h"
#include "types.h"

class GLView;
class WorldNPC_802DE058;

enum FrustumResult_802DD05C
{
    FRUSTUM_OUTSIDE = 0,
    FRUSTUM_INSIDE = 1,
    FRUSTUM_INTERSECTING = 2
};

extern "C"
{
    void fn_802DCDB4(nlVector4* pPlanes, const nlMatrix4& projection,
        const nlMatrix4& view);
    FrustumResult_802DD05C fn_802DD05C(const nlVector4* pPlanes,
        const nlVector3* pBoundsMin, const nlVector3* pBoundsMax,
        unsigned long* pPlaneMask);
    FrustumResult_802DD05C fn_802DD1EC(const nlVector4* pPlanes,
        const nlVector3* pPosition, float fRadius);
    void fn_802DD2F4(const nlVector4* pPlanes, nlVector4* pCorners);
}

#endif // GAME_RENDER_TU_802DCDB4_H
