#include "Game/Camera/AnimViewerCam.h"

cAnimViewerCamera::~cAnimViewerCamera()
{
    m_pCurrentPlayer = 0;
}

eCameraType cAnimViewerCamera::GetType()
{
    return eCameraType_AnimViewer;
}

void cAnimViewerCamera::Update(float fDeltaT)
{
    if (m_pCurrentPlayer)
    {
        m_v3OOI = m_pCurrentPlayer->m_v3Position;
    }
    cFollowCamera::Update(fDeltaT);
}

cAnimViewerCamera::cAnimViewerCamera()
    : cFollowCamera(FOLLOW_ANIM_VIEWER_CHARACTER)
{
    m_pCurrentPlayer = 0;
    m_bPitchLimits = false;
    m_aPitch = false;
}
