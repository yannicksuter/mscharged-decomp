#ifndef GAME_CAMERA_ANIMCAM_H
#define GAME_CAMERA_ANIMCAM_H

class cAnimCamera
{
public:
    virtual ~cAnimCamera();
    virtual int GetType();
    virtual void Update(float dt);
};

#endif // GAME_CAMERA_ANIMCAM_H
