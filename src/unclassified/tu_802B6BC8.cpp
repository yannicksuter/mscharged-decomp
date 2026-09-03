#include "Game/MathHelpers.h"

extern "C" bool fn_802B6BC8(const nlVector3* v3Start,
    const nlVector3* v3End, const nlVector3* v3A,
    const nlVector3* v3B, float* fOut1, float* fOut2)
{
    float divisor = (v3End->x - v3Start->x) * (v3B->y - v3A->y)
        - (v3End->y - v3Start->y) * (v3B->x - v3A->x);
    float numerator1 = (v3Start->y - v3A->y) * (v3B->x - v3A->x)
        - (v3Start->x - v3A->x) * (v3B->y - v3A->y);
    float numerator2 = (v3Start->y - v3A->y)
            * (v3End->x - v3Start->x)
        - (v3Start->x - v3A->x) * (v3End->y - v3Start->y);

    if (nlAbs(divisor) > 0.001f)
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
        *fOut1 = result1;
        *fOut2 = result2;
        return intersects;
    }

    bool intersects = nlAbs(numerator1) < 0.001f;
    *fOut1 = 0.0f;
    *fOut2 = 0.0f;
    return intersects;
}
