#ifndef GAME_EFFECTS_PHOTOFLASHEFFECT_H
#define GAME_EFFECTS_PHOTOFLASHEFFECT_H

#include "types.h"

class PhotoFlash
{
public:
    static void Render(float dt);

    static s32 sNumFramesSinceFlash;
};

#endif // GAME_EFFECTS_PHOTOFLASHEFFECT_H
