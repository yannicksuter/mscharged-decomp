#ifndef GAME_WORLD_TRIGGERS_H
#define GAME_WORLD_TRIGGERS_H

#include "NL/nlMath.h"

void EmitGeneric(const char* name);
void EmitCameraFlash(const nlVector3& position, void* transform = 0);

#endif // GAME_WORLD_TRIGGERS_H
