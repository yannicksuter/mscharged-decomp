#ifndef GAME_WORLD_TRIGGERS_H
#define GAME_WORLD_TRIGGERS_H

#include "NL/nlMath.h"

class cSAnim;

void CharacterAnimTriggerCallback(cSAnim* anim, unsigned int uParam);
void EmitCameraFlash(const nlVector3& position, void* flyingCamera = 0);

#endif // GAME_WORLD_TRIGGERS_H
