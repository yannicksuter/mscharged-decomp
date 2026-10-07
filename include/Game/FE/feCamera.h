#ifndef GAME_FE_CAMERA_H
#define GAME_FE_CAMERA_H

#include "Game/Camera/CameraMan.h"

enum FEWorldLoadState
{
    FE_WORLD_LOADING_DATA = 1,
    FE_WORLD_LOADING_RESOURCES = 2,
    FE_WORLD_LOADING_EFFECTS = 3,
};

extern int gFEWorldLoadState;

void BeginLoadFEWorld();
bool FinishLoadFEWorld();
void DestroyFEWorld();

void PushPresentationCamera(const char* name, void (*callback)(eCameraMessage),
    float duration, bool deleteCurrentCamera);
void PopPresentationCamera(void (*callback)(eCameraMessage), float duration);

#endif // GAME_FE_CAMERA_H
