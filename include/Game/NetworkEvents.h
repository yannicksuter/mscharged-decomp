#ifndef GAME_NETWORK_EVENTS_H
#define GAME_NETWORK_EVENTS_H

#include "Game/DetermDataEvent.h"
#include "Game/Event.h"

void DispatchDetermDataEvents();
UnidentifiedQueuedEvent<DetermDataEvent>* GetDetermDataEventQueue();

#endif // GAME_NETWORK_EVENTS_H
