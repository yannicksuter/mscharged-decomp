#ifndef GAME_RENDER_IMPOSTOR_LIGHTING_H
#define GAME_RENDER_IMPOSTOR_LIGHTING_H

#include "NL/nlColour.h"

class nlVector3;

nlColour GetImpostorLightingColour(const nlVector3* position);
void UpdateImpostorLighting();
void SetImpostorLightingTexture(unsigned int textureHandle);
void FreeImpostorLighting();

#endif // GAME_RENDER_IMPOSTOR_LIGHTING_H
