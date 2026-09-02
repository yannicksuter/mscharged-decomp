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

    /* 0x00 */ u8 mUnidentified000[0x70];
    /* 0x70 */ bool mUnidentified070;
};

#endif // GAME_BASIC_STADIUM_H
