#ifndef _TLINSTANCE_H_
#define _TLINSTANCE_H_

#include "types.h"

enum eTimeLineAssetType
{
    TLAT_UNKNOWN = -1,
    TLAT_SLIDE = 0,
    TLAT_LAYER = 1,
    TLAT_IMAGE = 2,
    TLAT_TEXT = 3,
    TLAT_COMPONENT = 4,
    TLAT_GROUP = 5,
    TLAT_MAX_TYPES = 6,
};

class TLComponent;

class TLInstance
{
public:
    eTimeLineAssetType GetType() const;

    /* 0x00 */ TLInstance* m_next;
    /* 0x04 */ TLInstance* m_prev;
    /* 0x08 */ TLInstance* pChildren;
    /* 0x0C */ TLComponent* m_component;
    /* 0x10 */ f32 m_fStartTime;
    /* 0x14 */ f32 m_fDuration;
    /* 0x18 */ char m_szName[32];
    /* 0x38 */ u32 m_hash;
    /* 0x3C */ u8 field_0x3C[0x4C];
    /* 0x88 */ eTimeLineAssetType m_type;
};

#endif // _TLINSTANCE_H_
