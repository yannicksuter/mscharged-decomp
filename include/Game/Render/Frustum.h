#ifndef GAME_RENDER_FRUSTUM_H
#define GAME_RENDER_FRUSTUM_H

#include "NL/nlMath.h"

enum FrustumResult
{
    FRUSTUM_OUTSIDE = 0,
    FRUSTUM_INSIDE = 1,
    FRUSTUM_INTERSECTING = 2
};

void ExtractFrustumPlanes(nlVector4* pPlanes, const nlMatrix4& projection,
    const nlMatrix4& view);
FrustumResult ClassifyBoxInFrustum(const nlVector4* pPlanes,
    const nlVector3* pBoundsMin, const nlVector3* pBoundsMax,
    unsigned long* pPlaneMask);
FrustumResult ClassifySphereInFrustum(const nlVector4* pPlanes,
    const nlVector3* pPosition, float fRadius);
void GetFrustumCorners(const nlVector4* pPlanes, nlVector4* pCorners);

#endif // GAME_RENDER_FRUSTUM_H
