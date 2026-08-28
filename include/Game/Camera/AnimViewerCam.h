#ifndef GAME_CAMERA_ANIM_VIEWER_CAM_H
#define GAME_CAMERA_ANIM_VIEWER_CAM_H

#include "Game/Camera/FollowCam.h"
#include "Game/Player.h"

class cAnimViewerCamera : public cFollowCamera
{
public:
    cAnimViewerCamera();

    virtual eCameraType GetType();
    virtual ~cAnimViewerCamera();
    virtual void Update(float fDeltaT);

    /* 0xA4 */ cPlayer* m_pCurrentPlayer;
};

#endif // GAME_CAMERA_ANIM_VIEWER_CAM_H
