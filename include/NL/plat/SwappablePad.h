#ifndef NL_PLAT_SWAPPABLE_PAD_H
#define NL_PLAT_SWAPPABLE_PAD_H

#include "Game/StaticEvent.h"

class PadBackend;
bool UpdatePadBackend(PadBackend* pad);

class SwappablePadChangedEvent : public StaticEvent<void(int), 5>
{
public:
    SwappablePadChangedEvent();

    virtual ~SwappablePadChangedEvent() { }
}; // size 0xA4

extern SwappablePadChangedEvent gSwappablePadChanged;
extern bool gEnableWiiRemotePad;
extern bool gEnableWiiFreestylePad;
extern bool gEnableWiiClassicPad;

#endif // NL_PLAT_SWAPPABLE_PAD_H
