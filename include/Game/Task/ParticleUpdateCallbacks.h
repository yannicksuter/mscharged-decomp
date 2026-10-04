#ifndef GAME_TASK_PARTICLE_UPDATE_CALLBACKS_H
#define GAME_TASK_PARTICLE_UPDATE_CALLBACKS_H

#include "types.h"

extern bool g_bRenderParticles;
extern u8 lbl_806E1458;

void ParticleUpdateNoOp(u8* state);
void InitializeParticleUpdateCallbacks();

#endif // GAME_TASK_PARTICLE_UPDATE_CALLBACKS_H
