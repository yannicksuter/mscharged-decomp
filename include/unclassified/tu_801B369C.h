#ifndef UNCLASSIFIED_TU_801B369C_H
#define UNCLASSIFIED_TU_801B369C_H

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

#endif // UNCLASSIFIED_TU_801B369C_H
