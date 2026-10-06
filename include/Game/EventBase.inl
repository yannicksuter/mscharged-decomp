#ifndef GAME_EVENT_BASE_INL
#define GAME_EVENT_BASE_INL

#include "Game/Event.h"

inline EventBase::~EventBase()
{
}

template <typename ReturnType>
inline UnidentifiedTypedEvent0<ReturnType>::~UnidentifiedTypedEvent0()
{
}

#endif // GAME_EVENT_BASE_INL
