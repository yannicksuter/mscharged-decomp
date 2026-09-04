#ifndef GAME_BASIC_STADIUM_H
#define GAME_BASIC_STADIUM_H

#include "NL/nlMath.h"
#include "types.h"

struct UnidentifiedStadiumShadowData
{
    /* 0x00 */ u8 unknown00[0x8C];
    /* 0x8C */ nlVector3 unknown8C;
    /* 0x98 */ float shadowHeight;
};

class BasicStadium
{
public:
    static BasicStadium* GetCurrentStadium();

    /* 0x00 */ u8 mUnidentified000[0x68];
    /* 0x68 */ void* mUnidentified068;
    /* 0x6C */ void* mUnidentified06C;
    /* 0x70 */ bool mUnidentified070;
};


void UpdateHighRange();
void RenderWorldNPCs();

#endif // GAME_BASIC_STADIUM_H
