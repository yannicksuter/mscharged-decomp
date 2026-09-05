#ifndef GAME_PAD_ACTIONS_H
#define GAME_PAD_ACTIONS_H

#include "Game/Event.h"

class DeviceChangedEvent_80137B40
    : public UnidentifiedStaticEvent3<int, int, int, 5>
{
public:
    DeviceChangedEvent_80137B40()
        : UnidentifiedStaticEvent3<int, int, int, 5>("DeviceChanged", -1)
    {
    }

    virtual ~DeviceChangedEvent_80137B40() { }
};

void fn_80137824(bool useDefaultRemap);
void fn_80137890();
void fn_801379AC();
void InitPads();
void fn_80137B40();
void UpdateMonkeyState(int monkeySet);

#endif // GAME_PAD_ACTIONS_H
