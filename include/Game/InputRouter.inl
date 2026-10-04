#ifndef GAME_INPUT_ROUTER_INL
#define GAME_INPUT_ROUTER_INL

#include "Game/InputRouter.h"

inline void FreeDetermDataEvent(DetermDataEvent* event)
{
    delete event;
}

NetMessageInputBundle::~NetMessageInputBundle()
{
}

#endif // GAME_INPUT_ROUTER_INL
