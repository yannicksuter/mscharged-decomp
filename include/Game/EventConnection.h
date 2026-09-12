#ifndef GAME_EVENT_CONNECTION_H
#define GAME_EVENT_CONNECTION_H

#include "Game/Event.h"

struct UnidentifiedEventConnectionOwner
{
    UnidentifiedEventConnectionOwner()
        : mConnection(0)
    {
    }

    ~UnidentifiedEventConnectionOwner()
    {
        if (mConnection != 0 && ((mConnection->mFlags >> 30) & 1) != 0)
        {
            ((UnidentifiedEventBase*)mConnection->mTarget)->Disconnect(this);
        }
    }

    UnidentifiedConnection* mConnection;
};

#endif // GAME_EVENT_CONNECTION_H
