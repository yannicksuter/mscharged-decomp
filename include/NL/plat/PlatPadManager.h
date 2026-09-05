#ifndef NL_PLAT_PLATPADMANAGER_H
#define NL_PLAT_PLATPADMANAGER_H

#include "NL/plat/PlatPadStatus.h"
#include "Game/PadActions.h"

struct PlatPadManager
{
    PlatPadStatus status[WPAD_MAX_CONTROLLERS];
    bool connected[WPAD_MAX_CONTROLLERS];
    int type[WPAD_MAX_CONTROLLERS];
    bool disableFreestyle;
    bool disableClassic;
    bool dpdEnabled[WPAD_MAX_CONTROLLERS];
    bool dpdActive[WPAD_MAX_CONTROLLERS];
    bool dataFormatSet[WPAD_MAX_CONTROLLERS];
    unsigned char padding[2];
    DeviceChangedEvent_80137B40 deviceChanged;
};

#endif // NL_PLAT_PLATPADMANAGER_H
