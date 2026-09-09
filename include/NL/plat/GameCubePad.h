#ifndef NL_PLAT_GAMECUBEPAD_H
#define NL_PLAT_GAMECUBEPAD_H

#include <revolution/pad.h>
#include "NL/platpad.h"

class GameCubePad : public PadBackend
{
public:
    virtual float GetPressure(int button, bool remap);
    virtual int GetClassID();

    /* 0x1C */ PADStatus* mCurrentStatus;
    /* 0x20 */ unsigned char mUnidentified020[0x5C];
}; // size 0x7C

extern nlArrayAllocator<GameCubePad> gGameCubePadAllocator;
extern int* gGameCubePadButtonMap;
extern int gGameCubePadClassID;

#endif // NL_PLAT_GAMECUBEPAD_H
