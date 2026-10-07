#ifndef GAME_DEBUG_TIME_REGIONS_H
#define GAME_DEBUG_TIME_REGIONS_H

#include "Game/Debug/Histogram.h"
#include "NL/nlList.h"
#include "types.h"

void DestroyTimeRegions();
void InitializeTimeRegions();

class TimeRegion
{
public:
    static nlListContainer<TimeRegion*> sTimeRegionList;

    TimeRegion(const char* pName, bool (*pConditionFunc)());

    virtual ~TimeRegion()
    {
        sTimeRegionList.RemoveEntry(this);
    }

    /* 0x04 */ const char* m_pName;
    /* 0x08 */ bool (*m_pConditionFunc)();
    /* 0x0C */ float m_fThreshold;
    /* 0x10 */ int m_unk10;
    /* 0x14 */ Histogram m_Histogram;
};

extern TimeRegion* pGamePlayTimeRegion;
extern TimeRegion* pNISTimeRegion;
extern TimeRegion* pAutoReplayTimeRegion;
extern TimeRegion* pCentreFieldTimeRegion;
extern TimeRegion* pLeftFieldTimeRegion;
extern TimeRegion* pRightFieldTimeRegion;
extern TimeRegion* pShotTimeRegion;

#endif // GAME_DEBUG_TIME_REGIONS_H
