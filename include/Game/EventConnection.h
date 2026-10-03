#ifndef GAME_EVENT_CONNECTION_H
#define GAME_EVENT_CONNECTION_H

#include "Game/Event.h"

struct EventConnectionOwner
{
    EventConnectionOwner()
        : mConnection(0)
    {
    }

    ~EventConnectionOwner()
    {
        if (mConnection != 0 && ((mConnection->mFlags >> 30) & 1) != 0)
        {
            ((EventBase*)mConnection->mEvent)->Disconnect(this);
        }
    }

    EventConnection* mConnection;
};

#endif // GAME_EVENT_CONNECTION_H
