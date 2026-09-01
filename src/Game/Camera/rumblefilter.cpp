#include "Game/Camera/rumblefilter.h"

#include "NL/nlTask.h"

int cRumbleFilter::vfunc_0x14()
{
    return 0;
}

void cRumbleFilter::Update(float dt)
{
    float dx;
    float dy;
    float fHTerm;
    float fDTerm;

    if (nlTaskManager::m_pInstance->mCurrentState == 1 ||
        nlTaskManager::m_pInstance->mCurrentState == 0x20)
    {
        return;
    }

    float step = dt <= 0.02f ? dt : 0.02f;

    dy = v2Pos0.y - v2Pos1.y;
    dx = v2Pos0.x - v2Pos1.x;
    const float len = nlSqrt(dx * dx + dy * dy, true);
    fHTerm = len * Ks;

    nlVector2 _dv;
    _dv.y = v2Vel0.y - v2Vel1.y;
    _dv.x = v2Vel0.x - v2Vel1.x;
    float proj = (_dv.x * dx) + (_dv.y * dy);
    if (len == 0.0f)
    {
        fDTerm = 0.0f;
    }
    else
    {
        fDTerm = (proj * Kd) / len;
    }

    nlVector2 unit;
    if (len == 0.0f)
    {
        unit.x = 0.0f;
        unit.y = 0.0f;
    }
    else
    {
        static bool bIsBouncing = false;

        if (len < 0.01f)
        {
            if (bIsBouncing)
            {
                bIsBouncing = false;
            }
        }
        else
        {
            bIsBouncing = true;
        }

        float invLen = 1.0f / len;
        unit.x = invLen * dx;
        unit.y = invLen * dy;
    }

    nlVec2Set(v2Force0, 0.0f, 0.0f);
    nlVec2Set(v2Force1, 0.0f, 0.0f);

    float total = -(fHTerm + fDTerm);
    nlVec2Set(unit, total * unit.x, total * unit.y);
    nlVec2Set(v2Force0, unit.x + v2Force0.x, unit.y + v2Force0.y);
    nlVec2Set(v2Force1, v2Force1.x - unit.x, v2Force1.y - unit.y);

    float factor0 = 0.0f;
    nlVec2Set(v2Vel0,
        v2Vel0.x + (step * (factor0 * v2Force0.x)),
        v2Vel0.y + (step * (factor0 * v2Force0.y)));
    nlVec2Set(v2Pos0,
        v2Pos0.x + (step * v2Vel0.x),
        v2Pos0.y + (step * v2Vel0.y));

    float factor1 = 1.0f;
    nlVec2Set(v2Vel1,
        v2Vel1.x + (step * (factor1 * v2Force1.x)),
        v2Vel1.y + (step * (factor1 * v2Force1.y)));
    nlVec2Set(v2Pos1,
        v2Pos1.x + (step * v2Vel1.x),
        v2Pos1.y + (step * v2Vel1.y));
}

void cRumbleFilter::Rumble(float x, float y, float ks, float kd)
{
    nlVec2Set(v2Pos0, 0.0f, 0.0f);
    nlVec2Set(v2Pos1, x, y);
    nlVec2Set(v2Vel0, 0.0f, 0.0f);
    nlVec2Set(v2Vel1, 0.0f, 0.0f);
    Ks = ks;
    Kd = kd;
}

void cRumbleFilter::Reset()
{
    nlVec2Set(v2Pos0, 0.0f, 0.0f);
    nlVec2Set(v2Pos1, 0.0f, 0.0f);
    nlVec2Set(v2Vel0, 0.0f, 0.0f);
    nlVec2Set(v2Vel1, 0.0f, 0.0f);
    nlVec2Set(v2Force0, 0.0f, 0.0f);
    nlVec2Set(v2Force1, 0.0f, 0.0f);
}

void cRumbleFilter::Filter(const nlMatrix4& matViewIn, nlMatrix4& matViewOut)
{
    matViewOut = matViewIn;
    matViewOut.m41 += v2Pos1.x;
    matViewOut.m42 += v2Pos1.y;
}

cRumbleFilter::cRumbleFilter()
{
    nlVec2Set(v2Pos0, 0.0f, 0.0f);
    nlVec2Set(v2Pos1, 0.0f, 0.0f);
    nlVec2Set(v2Vel0, 0.0f, 0.0f);
    nlVec2Set(v2Vel1, 0.0f, 0.0f);
    nlVec2Set(v2Force0, 0.0f, 0.0f);
    nlVec2Set(v2Force1, 0.0f, 0.0f);
    Ks = 5000.0f;
    Kd = 10.0f;
}
