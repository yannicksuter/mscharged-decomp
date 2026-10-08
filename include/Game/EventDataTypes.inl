#ifndef GAME_EVENT_DATA_TYPES_INL
#define GAME_EVENT_DATA_TYPES_INL

#include "Game/EventDataTypes.h"

inline void FreePenaltyData(PenaltyData* data)
{
    g_PenaltyDataPool.Free(data);
}

#endif
