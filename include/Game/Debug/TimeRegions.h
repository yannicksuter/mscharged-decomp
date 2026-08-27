#ifndef GAME_DEBUG_TIME_REGIONS_H
#define GAME_DEBUG_TIME_REGIONS_H

#include "NL/nlList.h"
#include "types.h"

struct TimeRegionData
{
    /* 0x00 */ u32 m_unk00;
    /* 0x04 */ u32 m_unk04;
    /* 0x08 */ int m_unk08;
    /* 0x0C */ float m_unk0C;
    /* 0x10 */ float m_unk10;
    /* 0x14 */ int* m_unk14;
    /* 0x18 */ int m_unk18;
};

class TimeRegion
{
public:
    static nlListContainer<TimeRegion*> sTimeRegionList;

    virtual ~TimeRegion();

    /* 0x04 */ const char* m_pName;
    /* 0x08 */ bool (*m_pConditionFunc)();
    /* 0x0C */ float m_fThreshold;
    /* 0x10 */ int m_unk10;
    /* 0x14 */ TimeRegionData m_unk14;
};

#endif // GAME_DEBUG_TIME_REGIONS_H
