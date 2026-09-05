#ifndef GAME_EVENT_DISPATCHER_INL
#define GAME_EVENT_DISPATCHER_INL

#include "Game/Task/DispatchEventsTask.h"

inline EventDispatcherBase::EventDispatcherBase()
    : callbacks(16, 16)
    , state()
{
}

inline EventDispatcherBase::~EventDispatcherBase()
{
}

inline EventDispatcher::EventDispatcher()
{
}

#endif // GAME_EVENT_DISPATCHER_INL
