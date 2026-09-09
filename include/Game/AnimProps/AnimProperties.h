#ifndef ANIM_PROPERTIES_H
#define ANIM_PROPERTIES_H

#include "Game/SAnim.h"

enum eFootPhase
{
    REGULAR_FOOT = 0,
    GOOFY_FOOT = 1,
    LEFT_FOOT_DOWN = 2,
    RIGHT_FOOT_DOWN = 3,
};

struct AnimProperties
{
    const char* enumName;
    const char* animName;
    ePlayMode playMode;
    float blendAmount;
    bool mirror;
    int ballRotationMode;
    eFootPhase endPhase;
    u8 matchCharacterSpeed;
};

#endif
