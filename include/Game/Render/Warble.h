#ifndef GAME_RENDER_WARBLE_H
#define GAME_RENDER_WARBLE_H

#include "NL/nlMath.h"

class GLView;

struct WarbleConfiguration
{
    /* 0x00 */ GLView* view;
    /* 0x04 */ nlVector3 position;
    /* 0x10 */ float blobScale;
    /* 0x14 */ float duration;
    /* 0x18 */ float values18[6];
}; // size: 0x30

struct WarbleInstance : public WarbleConfiguration
{
    WarbleInstance(const WarbleConfiguration& configuration);
    bool UnidentifiedQuery() const { return elapsed >= duration; }

    /* 0x30 */ float elapsed;
    /* 0x34 */ int active;
}; // size: 0x38

struct WarbleOwner
{
    WarbleInstance* instance;
};

extern WarbleOwner gWarble;

void InitializeWarble(WarbleOwner* owner);
void ShutdownWarble(WarbleOwner* owner);
void ResetWarble(WarbleOwner* owner);
void UpdateWarble(WarbleOwner* owner, float dt);
void RenderWarble(WarbleOwner* owner);
void SetWarbleInstance(WarbleOwner* owner, WarbleInstance* instance);


extern bool gWarbleEnabled;

void LoadWarbleBlob();
void GenerateWarbleTexture(float phase, float frequency, float amplitude);
void UpdateWarbleTexture(bool* enabled);
void UpdateWarblePhase(bool* enabled, float dt);
void RenderWarbleQuad(bool* enabled);
void InitializeWarbleRendering(bool* enabled);
void ShutdownWarbleRendering(bool* enabled);

#endif // GAME_RENDER_WARBLE_H
