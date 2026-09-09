#ifndef GAME_FE_CAMERA_H
#define GAME_FE_CAMERA_H

#include "Game/Camera/CameraMan.h"

void PushPresentationCamera(const char* name, void (*callback)(eCameraMessage),
    float duration, bool deleteCurrentCamera);
void PopPresentationCamera(void (*callback)(eCameraMessage), float duration);

#endif // GAME_FE_CAMERA_H
