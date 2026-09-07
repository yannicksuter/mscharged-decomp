#ifndef GAME_RENDER_PEACH_PHOTO_H
#define GAME_RENDER_PEACH_PHOTO_H

#include "NL/nlMath.h"
#include "types.h"

struct PeachPhotoCell
{
    nlVector3 world[4];
    nlVector2 projected[4];
    nlVector2 texture[4];
};

struct PeachPhotoState
{
    int state;
    nlVector3 centre;
    nlVector3 corners[4];
    float displacement;
    bool firstFrameSeen;
    bool textureReady;
    u8 _046[2];
    unsigned int lastFrame;
    bool projected;
    u8 _04D[3];
    nlVector2 projectedCorners[4];
    float fadeTime;
    float delay;
    PeachPhotoCell cells[3][3];
};

extern PeachPhotoState gPeachPhotoState;

void EndPeachPhoto(PeachPhotoState* photo, bool immediate);
void RenderPeachPhoto(PeachPhotoState* photo);

void UpdatePeachPhoto(PeachPhotoState* photo, float fSimulationTick, int frame);

void StartPeachPhoto(PeachPhotoState* photo, const nlVector3* centre,
    float delay, float halfWidth, float halfHeight);
void SetPeachPhotoTextureBorder(unsigned short colour, unsigned long texture);

#endif // GAME_RENDER_PEACH_PHOTO_H
