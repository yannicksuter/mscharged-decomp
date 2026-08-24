#include "Game/AI/Fuzzy.h"

float FGREATER(float f1, float f2)
{
    float fScore = 0.0f;
    float fDelta = f1;
    fDelta -= f2;

    if (fDelta > 0.0f)
    {
        f2 = 1.0f - f2;
        f2 = FMAX(f2, f1);
        f2 = FMIN(f2, 0.5f);

        fScore = fDelta / f2;
        fScore = FMAX(fScore, 0.0f);
        fScore = FMIN(fScore, 1.0f);
    }

    return fScore;
}

float FLESS(float f1, float f2)
{
    float fScore = 0.0f;
    float fDelta = f2;
    fDelta -= f1;

    if (fDelta > 0.0f)
    {
        f1 = 1.0f - f1;
        f1 = FMAX(f1, f2);
        f1 = FMIN(f1, 0.5f);

        fScore = fDelta / f1;
        fScore = FMAX(fScore, 0.0f);
        fScore = FMIN(fScore, 1.0f);
    }

    return fScore;
}
