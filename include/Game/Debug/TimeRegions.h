#ifndef GAME_DEBUG_TIME_REGIONS_H
#define GAME_DEBUG_TIME_REGIONS_H

#include "NL/nlList.h"
#include "types.h"

extern const float lbl_806E6178;
extern const float lbl_806E617C;

class UnidentifiedTimeRegionData_802B9570
{
public:
    UnidentifiedTimeRegionData_802B9570(
        const char* pName, int numBins, float minValue, float binSize);
    virtual ~UnidentifiedTimeRegionData_802B9570();

    /* 0x04 */ const char* m_unk04;
    /* 0x08 */ int m_unk08;
    /* 0x0C */ float m_unk0C;
    /* 0x10 */ float m_unk10;
    /* 0x14 */ int* m_unk14;
    /* 0x18 */ int m_unk18;
};

void DestroyTimeRegions();
void InitializeTimeRegions();

class TimeRegion
{
public:
    static nlListContainer<TimeRegion*> sTimeRegionList;

    TimeRegion(const char* pName, bool (*pConditionFunc)());

    virtual ~TimeRegion();

    /* 0x04 */ const char* m_pName;
    /* 0x08 */ bool (*m_pConditionFunc)();
    /* 0x0C */ float m_fThreshold;
    /* 0x10 */ int m_unk10;
    /* 0x14 */ UnidentifiedTimeRegionData_802B9570 m_unk14;
};

#endif // GAME_DEBUG_TIME_REGIONS_H
