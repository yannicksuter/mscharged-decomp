#include "Game/Camera/TopDownCamera.h"

#include "Game/Ball.h"
#include "Game/Field.h"
#include "NL/gl/glMatrix.h"

static float sfCameraHeight = 21.0f;
static float sfFov = 90.0f;
static bool sbFocusOnBall = false;

float TopDownCamera::GetFOV() const
{
    return sfFov;
}

TopDownCamera::~TopDownCamera()
{
}

const nlMatrix4& TopDownCamera::GetViewMatrix() const
{
    nlVector3 up = { 0.0f, 1.0f, 0.0f };
    glMatrixLookAt((nlMatrix4&)m_View, m_Position, m_Target, up);
    return m_View;
}

void TopDownCamera::Update(float dt)
{
    if (sbFocusOnBall)
    {
        m_Target = g_pBall->m_v3Position;
    }
    else
    {
        nlVec3Set(m_Target, 0.0f, 0.0f, 0.0f);
    }

    nlVec3Set(m_Position, m_Target.x, m_Target.y - 1.0f, sfCameraHeight);

    float gx = cField::GetGoalLineX((unsigned int)1);
    if (m_Position.x > gx)
        m_Position.x = gx;

    if (m_Position.x < -gx)
        m_Position.x = -gx;
}

TopDownCamera::TopDownCamera()
{
    nlVec3Set(m_Position, 0.0f, -1.0f, 21.0f);
    nlVec3Set(m_Target, 0.0f, 0.0f, 0.0f);
}
