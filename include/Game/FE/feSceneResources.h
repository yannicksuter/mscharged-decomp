#ifndef GAME_FE_SCENE_RESOURCES_H
#define GAME_FE_SCENE_RESOURCES_H

#include "NL/gl/glMemory.h"

void CreateFEResourcePool();
void CreateLargeFEResourcePool();
void DestroyFEResourcePool();
void LoadFEMiniBundle(const char* bundleFileName);
bool UnloadFEMiniBundle();
GLResourcePool* GetFEResourcePool();

#endif // GAME_FE_SCENE_RESOURCES_H
