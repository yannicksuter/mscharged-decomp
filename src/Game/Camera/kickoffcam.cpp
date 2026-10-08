#include "Game/Camera/kickoffcam.h"

#include "NL/gl/glMatrix.h"

nlVector3 gKickOffCameraDefaultPosition = { 0.0f, -18.0f, 8.0f };
nlVector3 gKickOffCameraDefaultTarget = { 0.0f, 0.0f, 0.0f };

void cKickOffCamera::Update(float dt)
{
    glMatrixLookAt(m_matView, m_v3Camera, m_v3Target, mUpVector);
}

cKickOffCamera::~cKickOffCamera()
{
}

cKickOffCamera::cKickOffCamera()
{
    m_v3Camera = gKickOffCameraDefaultPosition;
    m_v3Target = gKickOffCameraDefaultTarget;
}
