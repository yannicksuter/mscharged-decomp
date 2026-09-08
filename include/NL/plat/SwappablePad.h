#ifndef NL_PLAT_SWAPPABLE_PAD_H
#define NL_PLAT_SWAPPABLE_PAD_H

#include "Game/Event.h"

class PadBackend;
bool UpdatePadBackend(PadBackend* pad);

class SwappablePadChangedEvent : public UnidentifiedStaticEvent<void(int), 5>
{
public:
    SwappablePadChangedEvent()
        : UnidentifiedStaticEvent<void(int), 5>("SwappablePadChanged", -1)
    {
    }

    virtual ~SwappablePadChangedEvent() { }
}; // size 0xA4

extern SwappablePadChangedEvent gSwappablePadChanged;
extern bool gEnableWiiRemotePad;
extern bool gEnableWiiFreestylePad;
extern bool gEnableWiiClassicPad;

#endif // NL_PLAT_SWAPPABLE_PAD_H
