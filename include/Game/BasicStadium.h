#ifndef GAME_BASIC_STADIUM_H
#define GAME_BASIC_STADIUM_H

#include "NL/nlMath.h"
#include "types.h"

class DrawableObject;
template <typename T> class DLListEntry;

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

    /* 0x00 */ u8 mUnidentified000[0x08];
    /* 0x08 */ DLListEntry<DrawableObject*>* mUnidentified008;
    /* 0x0C */ u8 mUnidentified00C[0x5C];
    /* 0x68 */ void* mUnidentified068;
    /* 0x6C */ void* mUnidentified06C;
    /* 0x70 */ bool mUnidentified070;
};


void UpdateHighRange();
void RenderWorldNPCs();


void fn_80276FB8(float fDeltaT);
bool fn_80277238();
void fn_80278A00(BasicStadium* stadium, int param2, int param3);

#endif // GAME_BASIC_STADIUM_H
