#include "Game/Camera/GoalCam.h"

#include "NL/gl/glMatrix.h"

void GoalCamera::Update(float dt)
{
    glMatrixLookAt(m_matView, m_vecCamera, m_vecTarget, mUpVector);
}

GoalCamera::~GoalCamera()
{
}

GoalCamera::GoalCamera()
{
    nlVec3Set(m_vecCamera, 0.0f, 0.0f, 0.0f);
    nlVec3Set(m_vecTarget, 1.0f, 0.0f, 0.0f);
    Update(0.0f);
}
