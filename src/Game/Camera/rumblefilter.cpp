#include "Game/Camera/rumblefilter.h"

#include "NL/nlTask.h"

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

void cRumbleFilter::Filter(const nlMatrix4& matViewIn, nlMatrix4& matViewOut)
{
    matViewOut = matViewIn;
    matViewOut.m41 += v2Pos1.x;
    matViewOut.m42 += v2Pos1.y;
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

void cRumbleFilter::Rumble(float x, float y, float ks, float kd)
{
    nlVec2Set(v2Pos0, 0.0f, 0.0f);
    nlVec2Set(v2Pos1, x, y);
    nlVec2Set(v2Vel0, 0.0f, 0.0f);
    nlVec2Set(v2Vel1, 0.0f, 0.0f);
    Ks = ks;
    Kd = kd;
}

void cRumbleFilter::Update(float dt)
{
    nlVector2 d;
    float fDTerm;
    float fHTerm;

    if (nlTaskManager::m_pInstance->mCurrentState == 1 ||
        nlTaskManager::m_pInstance->mCurrentState == 0x20)
    {
        return;
    }

    float step = dt <= 0.02f ? dt : 0.02f;

    nlVec2Sub(d, v2Pos0, v2Pos1);
    const float len = nlVec2Length(d);
    fHTerm = len * Ks;

    nlVector2 _dv;
    nlVec2Sub(_dv, v2Vel0, v2Vel1);
    float proj = nlVec2DotProduct(_dv, d);
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
        nlVec2Set(unit, 0.0f, 0.0f);
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

        nlVec2Scale(unit, d, 1.0f / len);
    }

    nlVec2Scale(unit, unit, -(fHTerm + fDTerm));
    nlVec2Set(v2Force0, 0.0f, 0.0f);
    nlVec2Set(v2Force1, 0.0f, 0.0f);
    nlVec2Add(v2Force0, unit, v2Force0);
    nlVec2Sub(v2Force1, v2Force1, unit);

    nlVector2 acceleration;
    nlVec2Scale(acceleration, v2Force0, 0.0f);
    nlVec2ScaleAdd(v2Vel0, step, acceleration, v2Vel0);
    nlVec2ScaleAdd(v2Pos0, step, v2Vel0, v2Pos0);

    nlVec2Scale(acceleration, v2Force1, 1.0f);
    nlVec2ScaleAdd(v2Vel1, step, acceleration, v2Vel1);
    nlVec2ScaleAdd(v2Pos1, step, v2Vel1, v2Pos1);
}

int cRumbleFilter::GetFilterIndex()
{
    return 0;
}
