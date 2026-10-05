#ifndef GAME_HBM_HIDE_EVENT_H
#define GAME_HBM_HIDE_EVENT_H

#include "Game/UnidentifiedStaticEvent.h"

class HBMHideEvent
    : public UnidentifiedStaticEvent<UnidentifiedEventNoData, 8>
{
public:
    HBMHideEvent()
        : UnidentifiedStaticEvent<UnidentifiedEventNoData, 8>("HBMHide", -1)
    {
    }

    virtual ~HBMHideEvent() { }
};

#endif // GAME_HBM_HIDE_EVENT_H
