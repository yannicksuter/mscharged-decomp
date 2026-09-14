#ifndef NL_INTERSECTION_H
#define NL_INTERSECTION_H

#include "NL/nlMath.h"

// Collinear lines also return true, with both parameters set to zero.
bool nlIntersectLineSegments2D(const nlVector2* start,
    const nlVector2* end, const nlVector2* edgeStart,
    const nlVector2* edgeEnd, float* segmentParameter, float* edgeParameter);

#endif // NL_INTERSECTION_H
