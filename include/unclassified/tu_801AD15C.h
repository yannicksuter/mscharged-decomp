#ifndef UNCLASSIFIED_TU_801AD15C_H
#define UNCLASSIFIED_TU_801AD15C_H

#include "NL/nlMath.h"
#include "types.h"

struct UnidentifiedPeachPhotoCell
{
    nlVector3 world[4];
    nlVector2 projected[4];
    nlVector2 texture[4];
};

struct UnidentifiedPeachPhotoState
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
    UnidentifiedPeachPhotoCell cells[3][3];
};

extern UnidentifiedPeachPhotoState gPeachPhotoState;

void EndPeachPhoto(UnidentifiedPeachPhotoState* photo, bool immediate);
void RenderPeachPhoto(UnidentifiedPeachPhotoState* photo);

#endif // UNCLASSIFIED_TU_801AD15C_H
