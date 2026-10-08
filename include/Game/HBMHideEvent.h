#ifndef GAME_HBM_HIDE_EVENT_H
#define GAME_HBM_HIDE_EVENT_H

#include "Game/StaticEvent.h"

class HBMHideEvent
    : public StaticEvent<NoEventData, 8>
{
public:
    HBMHideEvent()
        : StaticEvent<NoEventData, 8>("HBMHide", -1)
    {
    }

    virtual ~HBMHideEvent() { }
};

#endif // GAME_HBM_HIDE_EVENT_H
