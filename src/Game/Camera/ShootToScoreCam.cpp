#include "Game/Camera/ShootToScoreCam.h"

#include "NL/gl/glMatrix.h"

void cShootToScoreCamera::Update(float fDeltaT)
{
    glMatrixLookAt(m_matView, m_v3Camera, m_v3Target, mUpVector);
}

void cShootToScoreCamera::fn_800F93F0(float side)
{
    if (side * m_v3Camera.x < 0.0f)
    {
        m_v3Camera.x *= -1.0f;
        m_v3Target.x *= -1.0f;
    }
}

cShootToScoreCamera::~cShootToScoreCamera()
{
}

cShootToScoreCamera::cShootToScoreCamera()
{
    m_fFOV = 50.0f;
    nlVec3Set(m_v3Camera, 23.1f, 0.0f, 0.7f);
    nlVec3Set(m_v3Target, 15.0f, 0.0f, 2.2f);
    Update(0.0f);
}
