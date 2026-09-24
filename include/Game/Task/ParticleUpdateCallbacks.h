#ifndef GAME_TASK_PARTICLE_UPDATE_CALLBACKS_H
#define GAME_TASK_PARTICLE_UPDATE_CALLBACKS_H

#include "types.h"

extern bool g_bRenderParticles;

void ParticleUpdateNoOp(u8* state);
void InitializeParticleUpdateCallbacks();

#endif // GAME_TASK_PARTICLE_UPDATE_CALLBACKS_H
