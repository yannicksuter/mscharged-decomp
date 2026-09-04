#ifndef GAME_FIXED_UPDATE_TASK_BASE_H
#define GAME_FIXED_UPDATE_TASK_BASE_H

#include "types.h"

class UnidentifiedFixedUpdateTaskBase
{
public:
    virtual int GetFrame() = 0;
    virtual float GetFixedUpdateMilliseconds() = 0;
    virtual u32 CalculateChecksum() = 0;
    virtual u32 WriteSyncLog() = 0;
    virtual void UnidentifiedVirtual10() = 0;
    virtual void UnidentifiedVirtual14() = 0;
    virtual u16 UnidentifiedVirtual18() = 0;
    virtual bool UnidentifiedVirtual1C() = 0;
};

#endif // GAME_FIXED_UPDATE_TASK_BASE_H
