#include "NL/nlIntersection.h"

#include <math.h>
#include "Game/MathHelpers.h"

bool nlIntersectLineSegments2D(const nlVector2* start,
    const nlVector2* end, const nlVector2* edgeStart,
    const nlVector2* edgeEnd, float* segmentParameter, float* edgeParameter)
{
    float x1 = start->x;
    float y1 = start->y;
    float x2 = end->x;
    float y2 = end->y;
    float x3 = edgeStart->x;
    float y3 = edgeStart->y;
    float x4 = edgeEnd->x;
    float y4 = edgeEnd->y;
    float numerator1 = (y1 - y3) * (x4 - x3)
        - (x1 - x3) * (y4 - y3);
    float numerator2 = (y1 - y3)
            * (x2 - x1)
        - (x1 - x3) * (y2 - y1);
    float divisor = (x2 - x1) * (y4 - y3)
        - (y2 - y1) * (x4 - x3);

    if (fabsf(divisor) > 0.001f)
    {
        float result1 = numerator1 / divisor;
        float result2 = numerator2 / divisor;
        bool intersects = result1
            == nlMinEquals(nlMaxEquals(result1, 0.0f), 1.0f);
        if (intersects)
        {
            intersects = result2
                == nlMinEquals(nlMaxEquals(result2, 0.0f), 1.0f);
        }
        *segmentParameter = result1;
        *edgeParameter = result2;
        return intersects;
    }

    bool intersects = fabsf(numerator1) < 0.001f;
    *segmentParameter = 0.0f;
    *edgeParameter = 0.0f;
    return intersects;
}
